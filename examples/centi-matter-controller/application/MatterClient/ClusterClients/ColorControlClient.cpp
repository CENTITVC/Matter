#include "ColorControlClient.h"

using namespace chip;
using namespace chip::Controller;
using namespace chip::app::Clusters;

CHIP_ERROR  ColorControlClient::CommandMoveToHueAndSaturation(
        uint8_t hue, uint8_t saturation,
        uint16_t transitionTime, uint8_t optionsMask, uint8_t optionsOverride,
        EndpointId endpointId,
        void * context,
        CommandResponseSuccessCallback<typename ColorControl::Commands::MoveToHueAndSaturation::Type::ResponseType> successCb,
        CommandResponseFailureCallback failureCb)
{
    mMoveToHueAndSaturationCommand.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, hue, saturation, transitionTime, optionsMask, optionsOverride,
                                context, successCb, failureCb]
                                (Messaging::ExchangeManager & exchangeMgr,
                                const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        ColorControl::Commands::MoveToHueAndSaturation::Type request;

        request.hue = hue;
        request.saturation = saturation;
        request.transitionTime = transitionTime;
        request.optionsMask = chip::BitMask<chip::app::Clusters::ColorControl::OptionsBitmap>(optionsMask);
        request.optionsOverride = chip::BitMask<chip::app::Clusters::ColorControl::OptionsBitmap>(optionsOverride);
        
        mMoveToHueAndSaturationCommand.SetTargetNode(exchangeMgr, sessionHandle);

        error = mMoveToHueAndSaturationCommand.Invoke(request, context, successCb, failureCb);

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

CHIP_ERROR  ColorControlClient::CommandMoveToColorTemperature(
        uint16_t colorTemperatureKelvin,
        uint16_t transitionTime, uint8_t optionsMask, uint8_t optionsOverride,               
        EndpointId endpointId,
        void * context,
        CommandResponseSuccessCallback<typename ColorControl::Commands::MoveToColorTemperature::Type::ResponseType> successCb,
        CommandResponseFailureCallback failureCb)
{
    mMoveToColorTemperatureCommand.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, colorTemperatureKelvin, transitionTime, optionsMask, optionsOverride, 
                                context, successCb, failureCb]
                                (Messaging::ExchangeManager & exchangeMgr,
                                const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        ColorControl::Commands::MoveToColorTemperature::Type request;
        uint16_t colorTemperatureMireds;

        colorTemperatureMireds = static_cast<uint16_t>(static_cast<uint32_t>(1000000) / static_cast<uint32_t>(colorTemperatureKelvin));

        request.colorTemperatureMireds = colorTemperatureMireds;
        request.transitionTime = transitionTime;
        request.optionsMask = chip::BitMask<chip::app::Clusters::ColorControl::OptionsBitmap>(optionsMask);
        request.optionsOverride = chip::BitMask<chip::app::Clusters::ColorControl::OptionsBitmap>(optionsOverride);
        
        mMoveToColorTemperatureCommand.SetTargetNode(exchangeMgr, sessionHandle);

        error = mMoveToColorTemperatureCommand.Invoke(request, context, successCb, failureCb);

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

CHIP_ERROR ColorControlClient::SubscribeCurrentHue(
            EndpointId endpointId,
            void * context,
            ReadResponseSuccessCallback<ColorControl::Attributes::CurrentHue::TypeInfo::DecodableArgType> reportCb,
            ReadResponseFailureCallback failureCb,
            uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
            SubscriptionEstablishedCallback subscriptionEstablishedCb,
            SubscriptionOnDoneCallback subscriptionDoneCb)
{
    mCurrentHueSubscriber.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        mCurrentHueSubscriber.SetTargetNode(exchangeMgr, sessionHandle);

        error = mCurrentHueSubscriber.SubscribeAttribute(context, reportCb, failureCb, 
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

CHIP_ERROR ColorControlClient::SubscribeCurrentSaturation(
            EndpointId endpointId,
            void * context,
            ReadResponseSuccessCallback<ColorControl::Attributes::CurrentSaturation::TypeInfo::DecodableArgType> reportCb,
            ReadResponseFailureCallback failureCb,
            uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
            SubscriptionEstablishedCallback subscriptionEstablishedCb,
            SubscriptionOnDoneCallback subscriptionDoneCb)
{
    mCurrentSaturationSubscriber.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        mCurrentSaturationSubscriber.SetTargetNode(exchangeMgr, sessionHandle);

        error = mCurrentSaturationSubscriber.SubscribeAttribute(context, reportCb, failureCb, 
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

CHIP_ERROR ColorControlClient::SubscribeColorTemperature(
            chip::EndpointId endpointId,
            void * context,
            chip::Controller::ReadResponseSuccessCallback<
            chip::app::Clusters::ColorControl::Attributes::ColorTemperatureMireds::TypeInfo::DecodableArgType> reportCb,
            chip::Controller::ReadResponseFailureCallback failureCb,
            uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
            chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb,
            chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb)
{
    mColorTemperatureSubscriber.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        mColorTemperatureSubscriber.SetTargetNode(exchangeMgr, sessionHandle);

        error = mColorTemperatureSubscriber.SubscribeAttribute(context, reportCb, failureCb, 
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