#pragma once

#include "MatterDeviceBase.h"
#include <controller/CHIPCluster.h>

class TemperatureSensor; //Forward declaration

class TemperatureSensorDelegate
{
    public:
        virtual ~TemperatureSensorDelegate() = default;

        virtual void OnMeasuredValueChangedHandler(TemperatureSensor* temperatureSensor) = 0;
};


class TemperatureSensor : public MatterDevice
{
    public:
        TemperatureSensor() : MatterDevice(MATTER_DEVICE_ID_TEMPERATURE_SENSOR),
                              mTemperatureCelsius(INT16_MIN),
                              mSubscriptionId(0),
                              mTemperatureSensorDelegate(nullptr) {}

        void RegisterTemperatureSensorDelegate(TemperatureSensorDelegate* temperatureSensorDelegate) { mTemperatureSensorDelegate = temperatureSensorDelegate; }
        TemperatureSensorDelegate* GetTemperatureSensorDelegate(void) { return mTemperatureSensorDelegate; }
        std::unique_ptr<MatterDevice> clone() const override { return std::make_unique<TemperatureSensor>(*this); }

        void SetMeasuredValue(int16_t temperatureCelsius) 
        { 
            if (mTemperatureCelsius != temperatureCelsius)
            {
                mTemperatureCelsius = temperatureCelsius;

                if (mTemperatureSensorDelegate != nullptr)
                {
                    mTemperatureSensorDelegate->OnMeasuredValueChangedHandler(this);
                }
            }
        }

        int16_t GetMeasuredValue(void) { return mTemperatureCelsius; }

		static void OnNewMeasurement(void* context, const chip::app::DataModel::Nullable<int16_t> & responseObject)
        {
			TemperatureSensor* p_Device = static_cast<TemperatureSensor*>(context);

			if (!responseObject.IsNull())
			{
				p_Device->SetMeasuredValue(responseObject.Value());
			}
        }

        static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
        {
            ChipLogProgress(chipTool, "TemperatureSensor subscription read failure with %s", err.Format())
        }

        static void OnTemperatureMeasurementSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			TemperatureSensor* p_device = static_cast<TemperatureSensor*>(context);

            p_device->mSubscriptionId = subscriptionId;
        }

        chip::SubscriptionId GetSubscriptionId(void) { return mSubscriptionId; }

    private:
        int16_t mTemperatureCelsius;
        chip::SubscriptionId mSubscriptionId;
        TemperatureSensorDelegate* mTemperatureSensorDelegate = nullptr;
};
