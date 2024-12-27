#include "LevelControlClient.h"

using namespace chip;
using namespace chip::Controller;
using namespace chip::app::Clusters;

CHIP_ERROR  LevelControlClient::CommandStep(
                    LevelControl::StepModeEnum stepMode, uint8_t stepSize,
                    app::DataModel::Nullable<uint16_t> transitionTime,
                    BitMask<LevelControl::OptionsBitmap> optionMask,
                    BitMask<LevelControl::OptionsBitmap> optionOverride,                
                    EndpointId endpointId,
                    void * context,
                    CommandResponseSuccessCallback<typename LevelControl::Commands::Step::Type::ResponseType> successCb,
                    CommandResponseFailureCallback failureCb)
{
    mStepCommand.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, stepMode, stepSize, transitionTime, optionMask, optionOverride, 
                                context, successCb, failureCb]
                                    (Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        LevelControl::Commands::Step::Type request;

        request.stepMode        = stepMode;
        request.stepSize        = stepSize;
        request.transitionTime  = transitionTime;
        request.optionsMask     = optionMask;
        request.optionsOverride = optionOverride;

        mStepCommand.SetTargetNode(exchangeMgr, sessionHandle);

        error = mStepCommand.Invoke(request, context, successCb, failureCb);

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

CHIP_ERROR  LevelControlClient::CommandMoveToLevel(
                uint8_t level, app::DataModel::Nullable<uint16_t> transitionTime,
                BitMask<LevelControl::OptionsBitmap> optionMask,
                BitMask<LevelControl::OptionsBitmap> optionOverride,
                EndpointId endpointId,
                void * context,
                CommandResponseSuccessCallback<typename LevelControl::Commands::MoveToLevel::Type::ResponseType> successCb,
                CommandResponseFailureCallback failureCb)
{
    mMoveToLevelCommand.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, level, transitionTime, optionMask, optionOverride, context, successCb, failureCb]
                                    (Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        LevelControl::Commands::MoveToLevel::Type request;

        request.level           = level;
        request.transitionTime  = transitionTime;
        request.optionsMask     = optionMask;
        request.optionsOverride = optionOverride;

        mMoveToLevelCommand.SetTargetNode(exchangeMgr, sessionHandle);

        error = mMoveToLevelCommand.Invoke(request, context, successCb, failureCb);

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

CHIP_ERROR LevelControlClient::SubscribeCurrentLevel(
            EndpointId endpointId,
            void * context,
            ReadResponseSuccessCallback<LevelControl::Attributes::CurrentLevel::TypeInfo::DecodableArgType> reportCb,
            ReadResponseFailureCallback failureCb,
            uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
            SubscriptionEstablishedCallback subscriptionEstablishedCb,
            SubscriptionOnDoneCallback subscriptionDoneCb)
{
    mCurrentLevelSubscriber.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        mCurrentLevelSubscriber.SetTargetNode(exchangeMgr, sessionHandle);

        error = mCurrentLevelSubscriber.SubscribeAttribute(context, reportCb, failureCb, 
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