#pragma once

#include "MatterClientClusterBase.h"
#include "../MatterNode.h"

template <typename TypeInfo>
class MatterClientClusterSubscriptionBase : public MatterClientClusterBase
{
    public:
        MatterClientClusterSubscriptionBase(chip::ClusterId clusterId) : MatterClientClusterBase(clusterId) {}

        CHIP_ERROR SubscribeAttribute(void * context,
                                    chip::Controller::ReadResponseSuccessCallback<typename TypeInfo::DecodableArgType> successFn,
                                    chip::Controller::ReadResponseFailureCallback failureFn, uint16_t minInterval,
                                    uint16_t maxInterval, chip::Controller::SubscriptionEstablishedCallback onSubscriptionEstablished = nullptr,
                                    chip::Controller::ResubscriptionAttemptCallback onResubscriptionAttempt = nullptr,
                                    chip::Controller::SubscriptionOnDoneCallback onSubscriptionDone = nullptr)
        {
            VerifyOrDieWithMsg(this->mExchangeMgr != nullptr , AppServer, "Target unknown");

            chip::Controller::ClusterBase cluster(*(this->mExchangeMgr), this->mSecureSession.Get().Value(), this->mPeerEndpoint);

            return cluster.SubscribeAttribute<TypeInfo>(context, successFn, failureFn, minInterval, maxInterval,
                                                        onSubscriptionEstablished, onResubscriptionAttempt,
                                                        true /* fabricFiltered */, true /* keepPreviousSubscriptions */,
                                                        chip::NullOptional, onSubscriptionDone);
        }
};