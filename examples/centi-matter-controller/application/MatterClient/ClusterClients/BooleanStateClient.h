#pragma once

#include "MatterClientBase.h"
#include "../MatterClientClusterCommandBase.h"
#include "../MatterClientClusterSubscriptionBase.h"
#include "../ClusterEventSubscriptionBase.h"
#include <controller/CHIPCluster.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <app-common/zap-generated/ids/Events.h>
#include "../../MatterNode.h"

class StateValueSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::BooleanState::Attributes::StateValue::TypeInfo>
{
    public:
        StateValueSubscriber() : MatterClientClusterSubscriptionBase(chip::app::Clusters::BooleanState::Id) {}
};

class StateChangeSubscriber : public ClusterEventSubscriptionBase<chip::app::Clusters::BooleanState::Events::StateChange::DecodableType>
{
    public:
        StateChangeSubscriber() : ClusterEventSubscriptionBase(chip::app::Clusters::BooleanState::Id) {}
};

class BooleanStateClient : public MatterClientBase
{
    public:
        BooleanStateClient(MatterNode & matterNode) : MatterClientBase(matterNode) {}
        
        CHIP_ERROR SubscribeStateValue(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::BooleanState::Attributes::StateValue::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb = nullptr );

        CHIP_ERROR SubscribeStateChange(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::BooleanState::Events::StateChange::DecodableType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::ResubscriptionAttemptCallback resubscriptionAttemptCb = nullptr,
                    bool aKeepPreviousSubscriptions = false, bool aIsUrgentEvent = false );

    private:
        StateValueSubscriber mStateValueSubscriber;
        StateChangeSubscriber mStateChangeSubscriber;
};