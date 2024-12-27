#include "MatterClientBase.h"
#include "../MatterClientClusterCommandBase.h"
#include "../MatterClientClusterReadBase.h"
#include "../MatterClientClusterSubscriptionBase.h"
#include <controller/CHIPCluster.h>

#include <app-common/zap-generated/cluster-objects.h>


/* Command Classes */
class OnCommand : public MatterClientClusterCommandBase<chip::app::Clusters::OnOff::Commands::On::Type>
{
    public:
        OnCommand() : MatterClientClusterCommandBase(chip::app::Clusters::OnOff::Id) {}
};

class OffCommand : public MatterClientClusterCommandBase<chip::app::Clusters::OnOff::Commands::Off::Type>
{
    public:
        OffCommand() : MatterClientClusterCommandBase(chip::app::Clusters::OnOff::Id) {}
};

class ToggleCommand : public MatterClientClusterCommandBase<chip::app::Clusters::OnOff::Commands::Toggle::Type>
{
    public:
        ToggleCommand() : MatterClientClusterCommandBase(chip::app::Clusters::OnOff::Id) {}
};

/* Subscription Classes */
class OnOffSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::OnOff::Attributes::OnOff::TypeInfo>
{
    public:
        OnOffSubscriber() : MatterClientClusterSubscriptionBase(chip::app::Clusters::OnOff::Id) {}
};


class OnOffClient : public MatterClientBase
{
    public:
        OnOffClient(MatterNode & matterNode) : MatterClientBase(matterNode) {}

        CHIP_ERROR  CommandOn(
                        chip::EndpointId endpointId,
                        void * context,
                        chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::OnOff::Commands::On::Type::ResponseType> successCb,
                        chip::Controller::CommandResponseFailureCallback failureCb);

        CHIP_ERROR  CommandOff(
                        chip::EndpointId endpointId,
                        void * context,
                        chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::OnOff::Commands::Off::Type::ResponseType> successCb,
                        chip::Controller::CommandResponseFailureCallback failureCb);

        CHIP_ERROR  CommandToggle(
                        chip::EndpointId endpointId,
                        void * context,
                        chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::OnOff::Commands::Toggle::Type::ResponseType> successCb,
                        chip::Controller::CommandResponseFailureCallback failureCb);

        CHIP_ERROR SubscribeOnOff(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::OnOff::Attributes::OnOff::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb = nullptr );
    
    private:
        OnCommand mOnCommand;
        OffCommand mOffCommand;
        ToggleCommand mToggleCommand;
        OnOffSubscriber mOnOffSubscriber;
};