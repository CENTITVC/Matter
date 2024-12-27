#include "OnOffClient.h"

using namespace chip;
using namespace chip::Controller;
using namespace chip::app::Clusters;

CHIP_ERROR  OnOffClient::CommandOn(
                EndpointId endpointId,
                void * context,
                CommandResponseSuccessCallback<typename OnOff::Commands::On::Type::ResponseType> successCb,
                CommandResponseFailureCallback failureCb)
{
    mOnCommand.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, context, successCb, failureCb](Messaging::ExchangeManager & exchangeMgr,
                                                                        const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        OnOff::Commands::On::Type request;

        mOnCommand.SetTargetNode(exchangeMgr, sessionHandle);

        error = mOnCommand.Invoke(request, context, successCb, failureCb);

        if (failureCb != nullptr && error != CHIP_NO_ERROR)
        {
            failureCb(context, error);
        }
    };

    auto onConnectionFailureCb = [this, context, successCb, failureCb] (CHIP_ERROR error)
    {
        if (failureCb != nullptr && error != CHIP_NO_ERROR)
        {
            failureCb(context, error);
        }
    };

    return ConnectToNode(onConnectionSuccessCb, onConnectionFailureCb);
}

CHIP_ERROR  OnOffClient::CommandOff(
                EndpointId endpointId,
                void * context,
                CommandResponseSuccessCallback<typename OnOff::Commands::Off::Type::ResponseType> successCb,
                CommandResponseFailureCallback failureCb)
{
    mOffCommand.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, context, successCb, failureCb](Messaging::ExchangeManager & exchangeMgr,
                                                                        const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        OnOff::Commands::Off::Type request;

        mOffCommand.SetTargetNode(exchangeMgr, sessionHandle);

        error = mOffCommand.Invoke(request, context, successCb, failureCb);

        if (failureCb != nullptr && error != CHIP_NO_ERROR)
        {
            failureCb(context, error);
        }
    };

    auto onConnectionFailureCb = [this, context, successCb, failureCb] (CHIP_ERROR error)
    {
        if (failureCb != nullptr && error != CHIP_NO_ERROR)
        {
            failureCb(context, error);
        }
    };

    return ConnectToNode(onConnectionSuccessCb, onConnectionFailureCb);
}

CHIP_ERROR  OnOffClient::CommandToggle(
                EndpointId endpointId,
                void * context,
                CommandResponseSuccessCallback<typename OnOff::Commands::Toggle::Type::ResponseType> successCb,
                CommandResponseFailureCallback failureCb)
{
    mToggleCommand.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, context, successCb, failureCb](Messaging::ExchangeManager & exchangeMgr,
                                                                        const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        OnOff::Commands::Toggle::Type request;

        mToggleCommand.SetTargetNode(exchangeMgr, sessionHandle);

        error = mToggleCommand.Invoke(request, context, successCb, failureCb);

        if (failureCb != nullptr && error != CHIP_NO_ERROR)
        {
            failureCb(context, error);
        }
    };

    auto onConnectionFailureCb = [this, context, successCb, failureCb] (CHIP_ERROR error)
    {
        if (failureCb != nullptr && error != CHIP_NO_ERROR)
        {
            failureCb(context, error);
        }
    };

    return ConnectToNode(onConnectionSuccessCb, onConnectionFailureCb);
}

CHIP_ERROR OnOffClient::SubscribeOnOff(
            EndpointId endpointId,
            void * context,
            ReadResponseSuccessCallback<OnOff::Attributes::OnOff::TypeInfo::DecodableArgType> reportCb,
            ReadResponseFailureCallback failureCb,
            uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
            SubscriptionEstablishedCallback subscriptionEstablishedCb,
            SubscriptionOnDoneCallback subscriptionDoneCb)
{
    mOnOffSubscriber.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        mOnOffSubscriber.SetTargetNode(exchangeMgr, sessionHandle);

        error = mOnOffSubscriber.SubscribeAttribute(context, reportCb, failureCb, 
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
