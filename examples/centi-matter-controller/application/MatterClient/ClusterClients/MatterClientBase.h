#pragma once

#include <controller/CHIPDeviceController.h>

#include "../../MatterManager.h"
#include "../../MatterNode.h"


class MatterClientBase
{
    public:
        MatterClientBase(MatterNode & matterNode) : mMatterNode(&matterNode),
                                                    mOnDeviceConnectedCallback(OnDeviceConnectedFn, this), 
                                                    mOnDeviceConnectionFailureCallback(OnDeviceConnectionFailureFn, this) {}
        
        CHIP_ERROR ConnectToNode(std::function<void(chip::Messaging::ExchangeManager & exchangeMgr,
                                                    const chip::SessionHandle & sessionHandle)> onConnectionSuccess,
                                 std::function<void(CHIP_ERROR)> onConnectionFailure)
        {
            mOnConnectionSuccess = onConnectionSuccess;
            mOnConnectionFailure = onConnectionFailure;

            return MatterManager::MatterMgr().GetCommissionerForFabricIndex(mMatterNode->GetFabricIndex()).GetConnectedDevice(mMatterNode->GetNodeId(),
                    &mOnDeviceConnectedCallback,
                    &mOnDeviceConnectionFailureCallback);
        }

    protected:
        MatterNode* mMatterNode = nullptr;
        
        static void OnDeviceConnectedFn(void * context, chip::Messaging::ExchangeManager & exchangeMgr,
                                        const chip::SessionHandle & sessionHandle)
        {
            MatterClientBase* ctx = static_cast<MatterClientBase *>(context);

            if (ctx->mOnConnectionSuccess != nullptr)
            {
                ctx->mOnConnectionSuccess(exchangeMgr, sessionHandle);
            }
        }

        static void OnDeviceConnectionFailureFn(void * context, const chip::ScopedNodeId & peerId, CHIP_ERROR error)
        {
            MatterClientBase* ctx = static_cast<MatterClientBase *>(context);

            if (ctx->mOnConnectionFailure != nullptr)
            {
                ctx->mOnConnectionFailure(error);
            }
        }
        
        chip::Callback::Callback<chip::OnDeviceConnected> mOnDeviceConnectedCallback;
        chip::Callback::Callback<chip::OnDeviceConnectionFailure> mOnDeviceConnectionFailureCallback;
        std::function<void(chip::Messaging::ExchangeManager & exchangeMgr,
                            const chip::SessionHandle & sessionHandle)> mOnConnectionSuccess = nullptr;
        std::function<void(CHIP_ERROR)> mOnConnectionFailure = nullptr;

};