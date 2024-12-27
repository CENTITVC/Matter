#pragma once

#include "MatterClientClusterBase.h"
#include "../MatterNode.h"

template <typename TypeInfo>
class MatterClientReadBase : public MatterClientClusterBase
{
public:
    MatterClientReadBase(chip::ClusterId clusterId) :   MatterClientClusterBase(clusterId) {}

    CHIP_ERROR ReadAttribute(void * context,
                             chip::Controller::ReadResponseSuccessCallback<typename TypeInfo::DecodableArgType> successFn,
                             chip::Controller::ReadResponseFailureCallback failureFn)
    {
        VerifyOrDieWithMsg(this->mExchangeMgr != nullptr , AppServer, "Target unknown");
        chip::Controller::ClusterBase cluster(*(this->mExchangeMgr), this->mSecureSession.Get().Value(), this->mPeerEndpoint);
        return cluster.ReadAttribute<TypeInfo>(context, successFn, failureFn);
    }
};