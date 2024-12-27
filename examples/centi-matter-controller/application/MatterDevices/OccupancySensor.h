#pragma once

#include "MatterDeviceBase.h"

class OccupancySensor; //Forward declaration

class OccupancySensorDelegate
{
    public:
        virtual ~OccupancySensorDelegate() = default;
        virtual void OnOccupancyStateChangedHandler(OccupancySensor* occupancySensor) = 0;

    private:
        chip::EndpointId mEndpoint;
};

class OccupancySensor : public MatterDevice
{
    public: 
        OccupancySensor() : MatterDevice(MATTER_DEVICE_ID_OCCUPANCY_SENSOR),
                            mIsOccupied(false),
                            mOccupancyStateSubscriptionId(0),
                            mOccupancySensorDelegate(nullptr) {}

        void RegisterOccupancySensorDelegate(OccupancySensorDelegate* occupancySensorDelegate) { mOccupancySensorDelegate = occupancySensorDelegate; }
        OccupancySensorDelegate* GetOccupancySensorDelegate(void) { return mOccupancySensorDelegate; }
        std::unique_ptr<MatterDevice> clone() const override { return std::make_unique<OccupancySensor>(*this); }

        void SetOccupancyState(bool isOccupied) 
        { 
            if (mIsOccupied != isOccupied)
            {
                if (mOccupancySensorDelegate != nullptr)
                {
                    mOccupancySensorDelegate->OnOccupancyStateChangedHandler(this);
                }
            }

            mIsOccupied = isOccupied; 
        }

        bool GetOccupancyState(void) { return mIsOccupied; }

		static void OnNewMeasurement(void* context, chip::BitMask<chip::app::Clusters::OccupancySensing::OccupancyBitmap> responseObject)
        {
            ChipLogProgress(chipTool, "OccupancySensor OnNewMeasurement");
			OccupancySensor* p_device = static_cast<OccupancySensor*>(context);
            bool isOccupied = !responseObject.Has(chip::app::Clusters::OccupancySensing::OccupancyBitmap::kOccupied);
            p_device->SetOccupancyState(isOccupied);
        }

        static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
        {
            ChipLogProgress(chipTool, "OccupancySensor subscription read failure with %s", err.Format())
        }

        static void OnOccupancyStateSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			OccupancySensor* p_device = static_cast<OccupancySensor*>(context);
            
            p_device->mOccupancyStateSubscriptionId = subscriptionId;
        }

        chip::SubscriptionId GetSubscriptionId(void) { return mOccupancyStateSubscriptionId; }

    private:
        bool mIsOccupied;
        chip::SubscriptionId mOccupancyStateSubscriptionId;
        OccupancySensorDelegate* mOccupancySensorDelegate;
};
