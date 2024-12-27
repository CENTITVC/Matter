#pragma once

#include "MatterDeviceBase.h"
#include <controller/CHIPCluster.h>
#include <controller/CHIPCluster.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <app-common/zap-generated/ids/Events.h>

class ContactSensor; //Forward declaration

class ContactSensorDelegate
{
    public:
        virtual ~ContactSensorDelegate() = default;
        virtual void OnContactSensorStateChangedHandler(ContactSensor* contactSensor) = 0;
};

class ContactSensor : public MatterDevice
{
    public:
        ContactSensor() :   MatterDevice(MATTER_DEVICE_ID_CONTACT_SENSOR),
                            mContactSensorState(false),
                            mContactSensorStateValueSubscriptionId(0),
                            mContactSensorStateChangeSubscriptionId(0),
                            mContactSensorDelegate(nullptr) {}

        void RegisterContactSensorDelegate(ContactSensorDelegate* contactSensorDelegate) { mContactSensorDelegate = contactSensorDelegate; }
        ContactSensorDelegate* GetContactSensorDelegate(void) { return mContactSensorDelegate; }
        std::unique_ptr<MatterDevice> clone() const override { return std::make_unique<ContactSensor>(*this); }

        void SetContactSensorState(bool contactSensorState) 
        {
            if (mContactSensorState != contactSensorState)
            {
                mContactSensorState = contactSensorState;

                if (mContactSensorDelegate != nullptr)
                {
                    mContactSensorDelegate->OnContactSensorStateChangedHandler(this);
                }
            }
        }
        
        bool IsContactClosed(void) { return mContactSensorState; }

		static void OnNewStateValue(void* context, bool stateValue)
        {
			ContactSensor* p_Device = static_cast<ContactSensor*>(context);

            p_Device->SetContactSensorState(stateValue);
        }

        static void OnStateChange(void* context, chip::app::Clusters::BooleanState::Events::StateChange::DecodableType stateChangeEvent)
        {
			ContactSensor* p_Device = static_cast<ContactSensor*>(context);
            
            p_Device->SetContactSensorState(stateChangeEvent.stateValue);
        }

        static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
        {
            ChipLogProgress(chipTool, "ContactSensor subscription read failure with %s", err.Format())
        }

        static void OnContactSensorStateValueSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			ContactSensor* p_device = static_cast<ContactSensor*>(context);

            p_device->mContactSensorStateValueSubscriptionId = subscriptionId;
        }

        static void OnContactSensorStateChangeSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			ContactSensor* p_device = static_cast<ContactSensor*>(context);

            p_device->mContactSensorStateChangeSubscriptionId = subscriptionId;
        }

        chip::SubscriptionId GetSubscriptionId(void) { return mContactSensorStateValueSubscriptionId; }

    private:
        bool mContactSensorState;
        chip::SubscriptionId mContactSensorStateValueSubscriptionId;
        chip::SubscriptionId mContactSensorStateChangeSubscriptionId;
        ContactSensorDelegate* mContactSensorDelegate;
};
