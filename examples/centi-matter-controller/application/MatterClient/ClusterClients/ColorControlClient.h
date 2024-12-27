#include "MatterClientBase.h"
#include "../MatterClientClusterCommandBase.h"
#include "../MatterClientClusterReadBase.h"
#include "../MatterClientClusterSubscriptionBase.h"
#include <controller/CHIPCluster.h>

#include <app-common/zap-generated/cluster-objects.h>

class MoveToHueCommand : public MatterClientClusterCommandBase<chip::app::Clusters::ColorControl::Commands::MoveToHue::Type>
{
    public:
        MoveToHueCommand() : MatterClientClusterCommandBase(chip::app::Clusters::ColorControl::Id) {}
};

class MoveToSaturationCommand : public MatterClientClusterCommandBase<chip::app::Clusters::ColorControl::Commands::MoveToSaturation::Type>
{
    public:
        MoveToSaturationCommand() : MatterClientClusterCommandBase(chip::app::Clusters::ColorControl::Id) {}
};

class MoveToColorTemperatureCommand : public MatterClientClusterCommandBase<chip::app::Clusters::ColorControl::Commands::MoveToColorTemperature::Type>
{
    public:
        MoveToColorTemperatureCommand() : MatterClientClusterCommandBase(chip::app::Clusters::ColorControl::Id) {}
};

class MoveToHueAndSaturationCommand : public MatterClientClusterCommandBase<chip::app::Clusters::ColorControl::Commands::MoveToHueAndSaturation::Type>
{
    public:
        MoveToHueAndSaturationCommand() : MatterClientClusterCommandBase(chip::app::Clusters::ColorControl::Id) {}
};

class CurrentHueSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::ColorControl::Attributes::CurrentHue::TypeInfo>
{
    public:
        CurrentHueSubscriber() : MatterClientClusterSubscriptionBase(chip::app::Clusters::ColorControl::Id) {}
};

class CurrentSaturationSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::ColorControl::Attributes::CurrentSaturation::TypeInfo>
{
    public:
        CurrentSaturationSubscriber() : MatterClientClusterSubscriptionBase(chip::app::Clusters::ColorControl::Id) {}
};

class ColorTemperatureSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::ColorControl::Attributes::ColorTemperatureMireds::TypeInfo>
{
    public:
        ColorTemperatureSubscriber() : MatterClientClusterSubscriptionBase(chip::app::Clusters::ColorControl::Id) {}
};

class ColorControlClient : public MatterClientBase
{
    public:
        ColorControlClient(MatterNode & matterNode) : MatterClientBase(matterNode) {}

        CHIP_ERROR  CommandMoveToHueAndSaturation(
                uint8_t hue, uint8_t saturation,
                uint16_t transitionTime, uint8_t optionsMask, uint8_t optionsOverride,               
                chip::EndpointId endpointId,
                void * context,
                chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::ColorControl::Commands::MoveToHueAndSaturation::Type::ResponseType> successCb,
                chip::Controller::CommandResponseFailureCallback failureCb);

        CHIP_ERROR  CommandMoveToColorTemperature(
                uint16_t colorTemperatureMireds,
                uint16_t transitionTime, uint8_t optionsMask, uint8_t optionsOverride,               
                chip::EndpointId endpointId,
                void * context,
                chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::ColorControl::Commands::MoveToColorTemperature::Type::ResponseType> successCb,
                chip::Controller::CommandResponseFailureCallback failureCb);

        CHIP_ERROR SubscribeCurrentHue(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::ColorControl::Attributes::CurrentHue::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb = nullptr );

        CHIP_ERROR SubscribeCurrentSaturation(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::ColorControl::Attributes::CurrentSaturation::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb = nullptr );

        CHIP_ERROR SubscribeColorTemperature(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::ColorControl::Attributes::ColorTemperatureMireds::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb = nullptr );

    private:
        MoveToHueCommand mMoveToHueCommand;
        MoveToSaturationCommand mMoveToSaturationCommand;
        MoveToHueAndSaturationCommand mMoveToHueAndSaturationCommand;
        MoveToColorTemperatureCommand mMoveToColorTemperatureCommand;

        CurrentHueSubscriber mCurrentHueSubscriber;
        CurrentSaturationSubscriber mCurrentSaturationSubscriber;
        ColorTemperatureSubscriber mColorTemperatureSubscriber;
};