#pragma once

#include "MatterDeviceBase.h"
#include <cmath>

class LightSensor; //Forward declaration

class LightSensorDelegate
{
    public:
        virtual ~LightSensorDelegate() = default;
        virtual void OnMeasuredValueChangedHandler(LightSensor* lightSensor) = 0;
};

class LightSensor : public MatterDevice
{
    public:
        LightSensor() : MatterDevice(MATTER_DEVICE_ID_LIGHT_SENSOR),
                        mIlluminanceLux(0),
                        mSubscriptionId(0),
                        mLightSensorDelegate(nullptr) {}
        
        ~LightSensor()  {}

        void RegisterLightSensorDelegate(LightSensorDelegate* lightSensorDelegate) 
        { 
            mLightSensorDelegate = lightSensorDelegate;
        }
        
        LightSensorDelegate* GetLightSensorDelegate(void) { return mLightSensorDelegate; }
        std::unique_ptr<MatterDevice> clone() const override { return std::make_unique<LightSensor>(*this); }

        void SetMeasuredValue(uint16_t illuminanceLux) 
        {
            if (mIlluminanceLux != illuminanceLux)
            {
                mIlluminanceLux = illuminanceLux; 

                if (mLightSensorDelegate != nullptr)
                {
                    mLightSensorDelegate->OnMeasuredValueChangedHandler(this);
                }
            }
        }

        uint16_t GetMeasuredValue(void) { return mIlluminanceLux; }

		static void OnNewMeasurement(void* context, const chip::app::DataModel::Nullable<uint16_t>& responseObject)
        {
			LightSensor* p_Device = static_cast<LightSensor*>(context);

			if (!responseObject.IsNull())
			{
                /*
                    Matter Specification 1.3 (Application Cluster Spec - section 2.2.5.1 page 152)
                    MeasuredValue = 10,000 x log10(illuminance) + 1
                */
                uint16_t illuminanceLux = static_cast<uint16_t>(std::pow(10, ((static_cast<float>(responseObject.Value())-1.0f) / 10000.0f)));

				p_Device->SetMeasuredValue(illuminanceLux);
			}
        }

        static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
        {
            ChipLogProgress(chipTool, "LightSensor subscription read failure with %s", err.Format())
        }

        static void OnIlluminanceMeasurementSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			LightSensor* p_device = static_cast<LightSensor*>(context);

            p_device->mSubscriptionId = subscriptionId;
        }

        chip::SubscriptionId GetSubscriptionId(void) { return mSubscriptionId; }

    private:
        uint16_t mIlluminanceLux;
        chip::SubscriptionId mSubscriptionId;
        LightSensorDelegate* mLightSensorDelegate;
        
};
