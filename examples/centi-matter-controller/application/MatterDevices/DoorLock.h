#pragma once

#include "MatterDeviceBase.h"

class DoorLock; //Forward declaration

class DoorLockDelegate
{
    public:
        virtual ~DoorLockDelegate() = default;
        virtual void OnDoorLockStateChangedHandler(DoorLock* doorLock) = 0;
};

class DoorLock : public MatterDevice
{
    public:
        DoorLock() :    MatterDevice(MATTER_DEVICE_ID_DOOR_LOCK),
                        mDoorState(chip::app::Clusters::DoorLock::DoorStateEnum::kUnknownEnumValue),
                        mDoorLockStateSubscriptionId(0),
                        mDoorLockDelegate(nullptr) {}

        void RegisterDoorLockDelegate(DoorLockDelegate* doorLockDelegate) { mDoorLockDelegate = doorLockDelegate; }
        DoorLockDelegate* GetDoorLockDelegate(void) { return mDoorLockDelegate; }
        std::unique_ptr<MatterDevice> clone() const override { return std::make_unique<DoorLock>(*this); }

        void SetDoorLockState(chip::app::Clusters::DoorLock::DoorStateEnum doorState) 
        {
            if (mDoorState != doorState)
            {
                mDoorState = doorState;

                if (mDoorLockDelegate != nullptr)
                {
                    mDoorLockDelegate->OnDoorLockStateChangedHandler(this);
                }
            }
        }
        
        chip::app::Clusters::DoorLock::DoorStateEnum GetDoorLockState(void) { return mDoorState; }
        bool IsDoorOpen(void) { return mDoorState == chip::app::Clusters::DoorLock::DoorStateEnum::kDoorOpen; }
        bool IsDoorClosed(void) { return mDoorState == chip::app::Clusters::DoorLock::DoorStateEnum::kDoorClosed; }

		static void OnNewMeasurement(void* context, const chip::app::DataModel::Nullable<chip::app::Clusters::DoorLock::DoorStateEnum>& responseObject)
        {
			DoorLock* p_Device = static_cast<DoorLock*>(context);

			if (!responseObject.IsNull())
			{
				p_Device->SetDoorLockState(responseObject.Value());
			}
        }

        static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
        {
            ChipLogProgress(chipTool, "DoorLock subscription read failure with %s", err.Format())
        }

        static void OnDoorLockStateSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			DoorLock* p_device = static_cast<DoorLock*>(context);

            p_device->mDoorLockStateSubscriptionId = subscriptionId;
        }

        chip::SubscriptionId GetSubscriptionId(void) { return mDoorLockStateSubscriptionId; }

    private:
        chip::app::Clusters::DoorLock::DoorStateEnum mDoorState;
        chip::SubscriptionId mDoorLockStateSubscriptionId;
        DoorLockDelegate* mDoorLockDelegate = nullptr;
};
