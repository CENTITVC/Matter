#include "BooleanStateClient.h"

using namespace chip;
using namespace chip::Controller;
using namespace chip::app::Clusters;

CHIP_ERROR BooleanStateClient::SubscribeStateValue(
            EndpointId endpointId,
            void * context,
            ReadResponseSuccessCallback<BooleanState::Attributes::StateValue::TypeInfo::DecodableArgType> reportCb,
            ReadResponseFailureCallback failureCb,
            uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
            SubscriptionEstablishedCallback subscriptionEstablishedCb,
            SubscriptionOnDoneCallback subscriptionDoneCb)
{
    mStateValueSubscriber.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        mStateValueSubscriber.SetTargetNode(exchangeMgr, sessionHandle);

        error = mStateValueSubscriber.SubscribeAttribute(context, reportCb, failureCb, 
                                                            minIntervallFloorSeconds,
                                                            maxIntervalCeilingSeconds,
                                                            subscriptionEstablishedCb,
                                                            nullptr /* ResubscriptionAttemptCallback */,
                                                            subscriptionDoneCb);

        if (failureCb != nullptr && error != CHIP_NO_ERROR)
        {
            failureCb(context, error);
        }
    };
    
    auto onConnectionFailureCb = [context, failureCb] (CHIP_ERROR error)
    {
        if (failureCb != nullptr && error != CHIP_NO_ERROR)
        {
            failureCb(context, error);
        }
    };

    return ConnectToNode(onConnectionSuccessCb, onConnectionFailureCb);
}

CHIP_ERROR BooleanStateClient::SubscribeStateChange(
            EndpointId endpointId,
            void * context,
            ReadResponseSuccessCallback<BooleanState::Events::StateChange::DecodableType> reportCb,
            ReadResponseFailureCallback failureCb,
            uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
            SubscriptionEstablishedCallback subscriptionEstablishedCb,
            ResubscriptionAttemptCallback resubscriptionAttemptCb,
            bool aKeepPreviousSubscriptions, bool aIsUrgentEvent)
{
    mStateChangeSubscriber.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        mStateChangeSubscriber.SetTargetNode(exchangeMgr, sessionHandle);

        error = mStateChangeSubscriber.SubscribeEvent(context, reportCb, failureCb, 
                                                        minIntervallFloorSeconds,
                                                        maxIntervalCeilingSeconds,
                                                        subscriptionEstablishedCb,
                                                        resubscriptionAttemptCb,
                                                        aKeepPreviousSubscriptions,
                                                        aIsUrgentEvent);

        if (failureCb != nullptr && error != CHIP_NO_ERROR)
        {
            failureCb(context, error);
        }
    };
    
    auto onConnectionFailureCb = [context, failureCb] (CHIP_ERROR error)
    {
        if (failureCb != nullptr && error != CHIP_NO_ERROR)
        {
            failureCb(context, error);
        }
    };

    return ConnectToNode(onConnectionSuccessCb, onConnectionFailureCb);
}