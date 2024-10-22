#include "WindowCoveringClient.h"

using namespace chip;
using namespace chip::Controller;
using namespace chip::app::Clusters;

CHIP_ERROR  WindowCoveringClient::CommandGoToLiftPercentage(
                        chip::Percent100ths percentage,
                        EndpointId endpointId,
                        void * context,
                        CommandResponseSuccessCallback<typename WindowCovering::Commands::GoToLiftPercentage::Type::ResponseType> successCb,
                        CommandResponseFailureCallback failureCb)
{
    mGoToLiftPercentageCommand.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, percentage, context, successCb, failureCb](Messaging::ExchangeManager & exchangeMgr,
                                                                        const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        WindowCovering::Commands::GoToLiftPercentage::Type request;
        request.liftPercent100thsValue = percentage;

        mGoToLiftPercentageCommand.SetTargetNode(exchangeMgr, sessionHandle);

        error = mGoToLiftPercentageCommand.Invoke(request, context, successCb, failureCb);

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


CHIP_ERROR  WindowCoveringClient::CommandUpOrOpen(
                        EndpointId endpointId,
                        void * context,
                        CommandResponseSuccessCallback<typename WindowCovering::Commands::UpOrOpen::Type::ResponseType> successCb,
                        CommandResponseFailureCallback failureCb)
{
    mUpOrOpenCommand.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                        const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        WindowCovering::Commands::UpOrOpen::Type request;
        mUpOrOpenCommand.SetTargetNode(exchangeMgr, sessionHandle);

        error = mUpOrOpenCommand.Invoke(request, context, successCb, failureCb);

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

CHIP_ERROR WindowCoveringClient::CommandDownOrClose(
            chip::EndpointId endpointId,
            void * context,
            chip::Controller::CommandResponseSuccessCallback<typename WindowCovering::Commands::DownOrClose::Type::ResponseType> successCb,
            chip::Controller::CommandResponseFailureCallback failureCb)
{
    mDownOrCloseCommand.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        WindowCovering::Commands::DownOrClose::Type request;
        mDownOrCloseCommand.SetTargetNode(exchangeMgr, sessionHandle);

        error = mDownOrCloseCommand.Invoke(request, context, successCb, failureCb);

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

CHIP_ERROR WindowCoveringClient::CommandStopMotion(
            chip::EndpointId endpointId,
            void * context,
            chip::Controller::CommandResponseSuccessCallback<typename WindowCovering::Commands::StopMotion::Type::ResponseType> successCb,
            chip::Controller::CommandResponseFailureCallback failureCb)
{
    mStopMotionCommand.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, context, successCb, failureCb](Messaging::ExchangeManager & exchangeMgr,
                                                                        const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        WindowCovering::Commands::StopMotion::Type request;
        mStopMotionCommand.SetTargetNode(exchangeMgr, sessionHandle);

        error = mStopMotionCommand.Invoke(request, context, successCb, failureCb);

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

CHIP_ERROR WindowCoveringClient::ReadOperationalStatus(
                EndpointId endpointId, 
                void * context,
                ReadResponseSuccessCallback<WindowCovering::Attributes::OperationalStatus::TypeInfo::DecodableArgType> successCb,
                ReadResponseFailureCallback failureCb)
{
    mOperationalStatusReader.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, context, successCb, failureCb](chip::Messaging::ExchangeManager & exchangeMgr,
                                                        const chip::SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;

        mOperationalStatusReader.SetTargetNode(exchangeMgr, sessionHandle);

        error = mOperationalStatusReader.ReadAttribute(context, successCb, failureCb);

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

CHIP_ERROR WindowCoveringClient::SubscribeCurrentPositionLiftPercentage100ths(
            EndpointId endpointId,
            void * context,
            ReadResponseSuccessCallback<WindowCovering::Attributes::CurrentPositionLiftPercent100ths::TypeInfo::DecodableArgType> reportCb,
            ReadResponseFailureCallback failureCb,
            uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
            SubscriptionEstablishedCallback subscriptionEstablishedCb,
            SubscriptionOnDoneCallback subscriptionDoneCb)
{
    mCurrentPositionLiftPercentage100thsSubscriber.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        mCurrentPositionLiftPercentage100thsSubscriber.SetTargetNode(exchangeMgr, sessionHandle);

        error = mCurrentPositionLiftPercentage100thsSubscriber.SubscribeAttribute(context, reportCb, failureCb, 
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
