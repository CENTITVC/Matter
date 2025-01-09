#pragma once

#include "MatterCommandBase.h"
#include "../MatterManager.h"
#include "../MatterDevices/MatterDevicesCommon.h"
#include "../MatterClient/MatterClientFactory.h"

class MatterSetOccupiedHeatSetpoint : public MatterCommandBase
{
    public:
        MatterSetOccupiedHeatSetpoint(std::function<void(MatterSetOccupiedHeatSetpoint *, CHIP_ERROR)> completeCallback, 
                                chip::NodeId nodeId,
                                int16_t occupiedHeatSetpoint) :
                                sOnCompleteCallback(completeCallback),
                                mNodeId(nodeId),
                                mMatterNode(nullptr),
                                mMatterDevice(nullptr),
                                mEndpointThermostat(chip::kInvalidEndpointId),
                                mThermostatClient(nullptr),
                                mAmount(0),
                                mOccupiedHeatSetpoint(occupiedHeatSetpoint) {}

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
        int16_t GetOccupiedHeatSetpoint(void) const { return mOccupiedHeatSetpoint; }
    private:
        std::function<void(MatterSetOccupiedHeatSetpoint *, CHIP_ERROR)> sOnCompleteCallback;
        chip::NodeId mNodeId;
        MatterNode* mMatterNode;
        MatterDevice* mMatterDevice;
        chip::EndpointId mEndpointThermostat;
        ThermostatClient* mThermostatClient;
        int8_t mAmount;
        int16_t mOccupiedHeatSetpoint;
        const uint16_t mMinIntervalFloorSeconds = 1;
        const uint16_t mMaxIntervalCeilingSeconds = 5;

        static void OnSubscriptionFailed(void* context, CHIP_ERROR error);

        static void OnOccupiedHeatingSetpointWriteSuccess(void* context);
        static void OnOccupiedHeatingSetpointWriteFailure(void* context, CHIP_ERROR error);
};