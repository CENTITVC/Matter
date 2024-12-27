#pragma once

#include "MatterCommandBase.h"
#include "../MatterManager.h"
#include "../MatterClient/ClusterClients/WindowCoveringClient.h"

/* Include here the callbacks for subscription, writes and whatsoever */

class MatterCommandSetWindowPosition : public MatterCommandBase
{
    public:
        MatterCommandSetWindowPosition(std::function<void(MatterCommandSetWindowPosition *, CHIP_ERROR)> completeCallback, chip::NodeId nodeId, uint8_t pos) : 
                                         sOnCompleteCallback(completeCallback), mNodeId(nodeId), mSetPos(pos) {}

        CHIP_ERROR Run() override;
        chip::System::Clock::Timeout GetWaitDuration() const override { return chip::System::Clock::Seconds16(60); }

        void callback() override
        {
            if (sOnCompleteCallback != nullptr)
            {
                sOnCompleteCallback(this, this->GetError());
            }
        }

        chip::NodeId GetNodeId(void) const { return mNodeId; }
        uint8_t GetTargetPosition(void) const { return mSetPos; }

    private:
        std::function<void(MatterCommandSetWindowPosition *, CHIP_ERROR)> sOnCompleteCallback;
        chip::NodeId mNodeId;
        uint8_t mSetPos;

        MatterEndpoint* mCurrentEndpoint = nullptr;
        MatterNode* mCurrentMatterNode = nullptr;
        WindowCoveringClient* mWindowClient = nullptr;

        static void OnCommandGoToLiftPercentageSuccess(void* context, const chip::app::DataModel::NullObjectType& responseObject);
        static void OnCommandGoToLiftPercentageFailure(void* context, CHIP_ERROR error);

        CHIP_ERROR ShutdownCurrentSubscription(void);
};