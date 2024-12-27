#pragma once

#include <controller/CommissioningWindowOpener.h>
#include <lib/support/CHIPMem.h>
#include <setup_payload/ManualSetupPayloadGenerator.h>
#include <setup_payload/QRCodeSetupPayloadGenerator.h>

#include "MatterCommandBase.h"
#include "../MatterNode.h"

class MatterOpenCommissioningWindow : public MatterCommandBase
{

    public:
        MatterOpenCommissioningWindow(std::function<void(MatterOpenCommissioningWindow *, CHIP_ERROR)> completeCallback, chip::NodeId nodeId) : 
                                    sOnCompleteCallback(completeCallback), mNodeId(nodeId), 
                                    mOnOpenCommissioningWindowCallback(OnOpenCommissioningWindowResponse, this) {}

        CHIP_ERROR Run(void) override;
        chip::System::Clock::Timeout GetWaitDuration() const override { return chip::System::Clock::Seconds16(mCommissioningWindowTimeout); }
        
        void callback() override
        {
            if (sOnCompleteCallback != nullptr)
            {
                sOnCompleteCallback(this, this->GetError());
            }
        }

        chip::NodeId GetNodeId(void) const { return mNodeId; }
        std::string GetManualCode(void) { return mManualCode; }
        std::string GetQRCode(void) { return mQRCode; }

    private:
        std::function<void(MatterOpenCommissioningWindow *, CHIP_ERROR)> sOnCompleteCallback;
        chip::NodeId mNodeId;
        chip::Controller::CommissioningWindowOpener::CommissioningWindowOption mCommissioningWindowOption;
        uint16_t mCommissioningWindowTimeout = 180;
        uint32_t mIteration = 1000;
        uint16_t mDiscriminator;
        chip::SetupPayload mSetupPayload;
        std::string mManualCode;
        std::string mQRCode;
        
        chip::Platform::UniquePtr<chip::Controller::CommissioningWindowOpener> mWindowOpener;

        static void OnOpenCommissioningWindowResponse(void * context, chip::NodeId deviceId, CHIP_ERROR status, chip::SetupPayload payload);
        chip::Callback::Callback<chip::Controller::OnOpenCommissioningWindow> mOnOpenCommissioningWindowCallback;
};