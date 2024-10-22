#pragma once

#include "MatterClientClusterBase.h"
#include "../MatterNode.h"

template <typename DecodableType>
class ClusterEventSubscriptionBase : public MatterClientClusterBase
{
    public:
        ClusterEventSubscriptionBase(chip::ClusterId clusterId) : MatterClientClusterBase(clusterId) {}

        CHIP_ERROR SubscribeEvent(void * context,
                                    chip::Controller::ReadResponseSuccessCallback<DecodableType> successFn,
                                    chip::Controller::ReadResponseFailureCallback failureFn, 
                                    uint16_t minInterval, uint16_t maxInterval, 
                                    chip::Controller::SubscriptionEstablishedCallback onSubscriptionEstablished = nullptr,
                                    chip::Controller::ResubscriptionAttemptCallback resubscriptionAttemptCb = nullptr,
                                    bool aKeepPreviousSubscriptions = false, bool aIsUrgentEvent = false)
        {
            VerifyOrDieWithMsg(this->mExchangeMgr != nullptr , AppServer, "Target unknown");

            chip::Controller::ClusterBase cluster(*(this->mExchangeMgr), this->mSecureSession.Get().Value(), this->mPeerEndpoint);

            return cluster.SubscribeEvent<DecodableType>(context, successFn, failureFn, minInterval, maxInterval,
                                                        onSubscriptionEstablished, resubscriptionAttemptCb, 
                                                        aKeepPreviousSubscriptions, aIsUrgentEvent);
        }
};
