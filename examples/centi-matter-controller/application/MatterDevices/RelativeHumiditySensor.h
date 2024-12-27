#pragma once

#include "MatterDeviceBase.h"

class HumiditySensor;

class HumiditySensorDelegate
{
    public:
        virtual ~HumiditySensorDelegate() = default;
        virtual void OnMeasuredValueChangedHandler(HumiditySensor* humiditySensor) = 0;
};

class HumiditySensor : public MatterDevice
{
    public:
        HumiditySensor() :  MatterDevice(MATTER_DEVICE_ID_HUMIDITY_SENSOR),
                            mRelativeHumidityPercent(0),
                            mSubscriptionId(0),
                            mHumiditySensorDelegate(nullptr) {}

        void RegisterHumiditySensorDelegate(HumiditySensorDelegate* humiditySensorDelegate) { mHumiditySensorDelegate = humiditySensorDelegate; }
        HumiditySensorDelegate* GetHumiditySensorDelegate(void) { return mHumiditySensorDelegate; }
        std::unique_ptr<MatterDevice> clone() const override { return std::make_unique<HumiditySensor>(*this); }

        void SetMeasuredValue(uint16_t relativeHumidityPercent) 
        { 
            if (mRelativeHumidityPercent != relativeHumidityPercent)
            {
                mRelativeHumidityPercent = relativeHumidityPercent;

                if (mHumiditySensorDelegate != nullptr)
                {
                    mHumiditySensorDelegate->OnMeasuredValueChangedHandler(this);
                }
            }
        }
        
        
        uint16_t GetMeasuredValue(void) { return mRelativeHumidityPercent; }

		static void OnNewMeasurement(void* context, const chip::app::DataModel::Nullable<uint16_t> & responseObject)
        {
			HumiditySensor* p_Device = static_cast<HumiditySensor*>(context);

			if (!responseObject.IsNull())
			{
				p_Device->SetMeasuredValue(responseObject.Value());
			}
        }

        static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
        {
            ChipLogProgress(chipTool, "HumiditySensor subscription read failure with %s", err.Format())
        }

        static void OnHumidityMeasurementSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			HumiditySensor* p_device = static_cast<HumiditySensor*>(context);

            p_device->mSubscriptionId = subscriptionId;
        }

        chip::SubscriptionId GetSubscriptionId(void) { return mSubscriptionId; }

    private:
        uint16_t mRelativeHumidityPercent;
        chip::SubscriptionId mSubscriptionId;
        HumiditySensorDelegate* mHumiditySensorDelegate = nullptr;
};
