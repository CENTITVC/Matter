#pragma once

#include "MatterClientClusterBase.h"
#include "../MatterNode.h"

template <typename AttributeInfo>
class MatterClientWriteBase : public MatterClientClusterBase
{
public:
    MatterClientWriteBase(chip::ClusterId clusterId) :   MatterClientClusterBase(clusterId) {}

    CHIP_ERROR WriteAttribute(const typename AttributeInfo::Type & requestData,
                             void * context,
                             chip::Controller::WriteResponseSuccessCallback successFn,
                             chip::Controller::WriteResponseFailureCallback failureFn,
                             const chip::Optional<uint16_t> & aTimedWriteTimeoutMs,
                             chip::Controller::WriteResponseDoneCallback onDoneCb,
                             const chip::Optional<chip::DataVersion> & aDataVersion)
    {
        VerifyOrDieWithMsg(this->mExchangeMgr != nullptr , AppServer, "Target unknown");
        chip::Controller::ClusterBase cluster(*(this->mExchangeMgr), this->mSecureSession.Get().Value(), this->mPeerEndpoint);
        return cluster.WriteAttribute<AttributeInfo>(requestData, context, successFn, failureFn, aTimedWriteTimeoutMs, onDoneCb, aDataVersion);
    }
};