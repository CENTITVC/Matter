#pragma once

#include "MatterClientClusterBase.h"
#include "../MatterNode.h"

template <typename RequestType>
class MatterClientClusterCommandBase : public MatterClientClusterBase
{
    public:
        MatterClientClusterCommandBase(chip::ClusterId clusterId) : MatterClientClusterBase(clusterId) {}

        CHIP_ERROR Invoke(RequestType request, void* context, 
                            chip::Controller::CommandResponseSuccessCallback<typename RequestType::ResponseType> successFn,
                            chip::Controller::CommandResponseFailureCallback  failureFn)
        {
            VerifyOrDieWithMsg(this->mExchangeMgr != nullptr , AppServer, "Target unknown");
            chip::Controller::ClusterBase cluster(*(this->mExchangeMgr), this->mSecureSession.Get().Value(), this->mPeerEndpoint);
            return cluster.InvokeCommand<RequestType>(request, context, successFn, failureFn);
        }
};
