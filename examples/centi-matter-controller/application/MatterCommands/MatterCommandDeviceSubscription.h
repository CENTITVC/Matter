#pragma once

#include "MatterCommandBase.h"
#include "../MatterManager.h"
#include "../MatterDevices/MatterDevicesCommon.h"

/* Include here the callbacks for subscription, writes and whatsoever */

struct SubscriptionParameters
{
    chip::NodeId nodeId;
    chip::EndpointId endpointId;
    chip::DeviceTypeId deviceTypeId;
    uint16_t minIntervalFloorSeconds;
    uint16_t maxIntervalCeilingSeconds;

    void PrintInfo(void)
    {
        ChipLogProgress(chipTool, "Matter subscription.......");
        ChipLogProgress(chipTool, "NodeId: " ChipLogFormatX64, ChipLogValueX64(nodeId));
        ChipLogProgress(chipTool, "Endpoint: %u", endpointId);
        ChipLogProgress(chipTool, "deviceType=" ChipLogFormatMEI,
                ChipLogValueMEI(MatterManager::MatterMgr().GetMatterNode(nodeId)->GetEndpoint(endpointId)->GetMatterDevice(deviceTypeId)->GetType()));
    }
};

class MatterSubscriptionDevice : public MatterCommandBase
{
    public:
        MatterSubscriptionDevice(std::function<void(MatterSubscriptionDevice *, CHIP_ERROR)> completeCallback, 
                                 SubscriptionParameters subscriptionParams) : 
                                    sOnCompleteCallback(completeCallback), 
                                    mNodeId(subscriptionParams.nodeId), 
                                    mEndpointId(subscriptionParams.endpointId),
                                    mDeviceTypeId(subscriptionParams.deviceTypeId),
                                    mMinIntervalFloorSeconds(subscriptionParams.minIntervalFloorSeconds),
                                    mMaxIntervalCeilingSeconds(subscriptionParams.maxIntervalCeilingSeconds) {}

        CHIP_ERROR Run() override;
        chip::System::Clock::Timeout GetWaitDuration() const override { return chip::System::Clock::Seconds16(600); }

        void callback() override
        {
            if (sOnCompleteCallback != nullptr)
            {
                sOnCompleteCallback(this, this->GetError());
            }
        }

        chip::NodeId GetNodeId(void) const { return mNodeId; }
        chip::EndpointId GetEndpointId(void) const { return mEndpointId; }
        MatterDevice* GetMatterDevice(void) { return mMatterDevice; }

    private:
        std::function<void(MatterSubscriptionDevice *, CHIP_ERROR)> sOnCompleteCallback;
        chip::NodeId mNodeId;
        chip::EndpointId mEndpointId;
        chip::DeviceTypeId mDeviceTypeId;
        MatterDevice* mMatterDevice;
        uint16_t mMinIntervalFloorSeconds;
        uint16_t mMaxIntervalCeilingSeconds;

        CHIP_ERROR SubscribeToAirQualitySensor(void);
        CHIP_ERROR SubscribeToContactSensor(void);
        CHIP_ERROR SubscribeToDoorLock(void);
        CHIP_ERROR SubscribeToElectricalSensor(void);
        CHIP_ERROR SubscribeToExtendedColorLight(void);
        CHIP_ERROR SubscribeToHumiditySensor(void);
        CHIP_ERROR SubscribeToLightSensor(void);
        CHIP_ERROR SubscribeToOccupancySensor(void);
        CHIP_ERROR SubscribeToTemperatureSensor(void);
        CHIP_ERROR SubscribeToThermostatDevice(void);
        CHIP_ERROR SubscribeToWindowCover(void);

        static void OnSubscriptionFailed(void* context, CHIP_ERROR error);
};