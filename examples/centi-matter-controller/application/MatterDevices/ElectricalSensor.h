#pragma once

#include "MatterDeviceBase.h"

class ElectricalSensor;

class ElectricalSensorDelegate
{
    public:
        virtual ~ElectricalSensorDelegate() = default;
        virtual void OnActivePowerValueChangedHandler(ElectricalSensor* pElectricalSensor) = 0;
};

class ElectricalSensor : public MatterDevice
{
    public:
        ElectricalSensor() :    MatterDevice(MATTER_DEVICE_ID_ELECTRICAL_SENSOR),
                                mActivePower_mW(INT64_MIN),
                                mActivePowerSubscriptionId(0),
                                mElectricalSensorDelegate(nullptr) {}

        void RegisterElectricalSensorDelegate(ElectricalSensorDelegate* electricalSensorDelegate) { mElectricalSensorDelegate = electricalSensorDelegate; }
        ElectricalSensorDelegate* GetElectricalSensorDelegate(void) { return mElectricalSensorDelegate; }
        
        std::unique_ptr<MatterDevice> clone() const override { return std::make_unique<ElectricalSensor>(*this); }

        void SetActivePower(int64_t activePower_mW) 
        {   
            if (mActivePower_mW != activePower_mW)
            {
                mActivePower_mW = activePower_mW; 

                if (mElectricalSensorDelegate != nullptr)
                {
                    mElectricalSensorDelegate->OnActivePowerValueChangedHandler(this);
                }
            }
        }

        int64_t GetActivePower(void) { return mActivePower_mW; }

		static void OnNewMeasurement(void* context, const chip::app::DataModel::Nullable<int64_t> & responseObject)
        {
			ElectricalSensor* p_Device = static_cast<ElectricalSensor*>(context);

			if (!responseObject.IsNull())
			{
				p_Device->SetActivePower(responseObject.Value());
			}
        }

        static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
        {
            ChipLogProgress(chipTool, "ElectricalSensor subscription read failure with %s", err.Format())
        }

        static void OnActivePowerSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			ElectricalSensor* p_device = static_cast<ElectricalSensor*>(context);

            p_device->mActivePowerSubscriptionId = subscriptionId;
        }

        chip::SubscriptionId GetSubscriptionId(void) { return mActivePowerSubscriptionId; }

    private:
        int64_t mActivePower_mW;
        chip::SubscriptionId mActivePowerSubscriptionId;
        ElectricalSensorDelegate* mElectricalSensorDelegate = nullptr;
};
