#pragma once

#include "MatterClientBase.h"
#include "../MatterClientClusterCommandBase.h"
#include "../MatterClientClusterReadBase.h"
#include "../MatterClientClusterSubscriptionBase.h"
#include <controller/CHIPCluster.h>

#include <app-common/zap-generated/cluster-objects.h>

/* Command Classes */
class GoToLiftPercentageCommand : public MatterClientClusterCommandBase<chip::app::Clusters::WindowCovering::Commands::GoToLiftPercentage::Type>
{
    public:
        GoToLiftPercentageCommand() : MatterClientClusterCommandBase(chip::app::Clusters::WindowCovering::Id) {}
};

class UpOrOpenCommand : public MatterClientClusterCommandBase<chip::app::Clusters::WindowCovering::Commands::UpOrOpen::Type>
{
    public:
        UpOrOpenCommand() : MatterClientClusterCommandBase(chip::app::Clusters::WindowCovering::Id) {}
};

class DownOrCloseCommand : public MatterClientClusterCommandBase<chip::app::Clusters::WindowCovering::Commands::DownOrClose::Type>
{
    public:
        DownOrCloseCommand() : MatterClientClusterCommandBase(chip::app::Clusters::WindowCovering::Id) {}
};

class StopMotionCommand : public MatterClientClusterCommandBase<chip::app::Clusters::WindowCovering::Commands::StopMotion::Type>
{
    public:
        StopMotionCommand() : MatterClientClusterCommandBase(chip::app::Clusters::WindowCovering::Id) {}
};

/* Reader Classes */

class OperationalStatusReader : public MatterClientReadBase<chip::app::Clusters::WindowCovering::Attributes::OperationalStatus::TypeInfo>
{
    public:
        OperationalStatusReader() : MatterClientReadBase(chip::app::Clusters::WindowCovering::Id) {}
};

/* Subscriber Classes */
class CurrentPositionLiftPercentage100thsSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::WindowCovering::Attributes::CurrentPositionLiftPercent100ths::TypeInfo>
{
    public:
        CurrentPositionLiftPercentage100thsSubscriber() :  MatterClientClusterSubscriptionBase(chip::app::Clusters::WindowCovering::Id) {}
};

class WindowCoveringClient : public MatterClientBase
{
    public:
        WindowCoveringClient(MatterNode & matterNode) : MatterClientBase(matterNode) {}

        CHIP_ERROR  CommandGoToLiftPercentage(
                        chip::Percent100ths percentage,
                        chip::EndpointId endpointId,
                        void * context,
                        chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::WindowCovering::Commands::GoToLiftPercentage::Type::ResponseType> successCb,
                        chip::Controller::CommandResponseFailureCallback failureCb);

        CHIP_ERROR CommandUpOrOpen(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::WindowCovering::Commands::UpOrOpen::Type::ResponseType> successCb,
                    chip::Controller::CommandResponseFailureCallback failureCb);

        CHIP_ERROR CommandDownOrClose(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::WindowCovering::Commands::DownOrClose::Type::ResponseType> successCb,
                    chip::Controller::CommandResponseFailureCallback failureCb);

        CHIP_ERROR CommandStopMotion(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::WindowCovering::Commands::StopMotion::Type::ResponseType> successCb,
                    chip::Controller::CommandResponseFailureCallback failureCb);
      
        CHIP_ERROR ReadOperationalStatus(
                        chip::EndpointId endpointId, 
                        void * context,
                        chip::Controller::ReadResponseSuccessCallback<
                        chip::app::Clusters::WindowCovering::Attributes::OperationalStatus::TypeInfo::DecodableArgType> successCb,
                        chip::Controller::ReadResponseFailureCallback failureCb);

        CHIP_ERROR SubscribeCurrentPositionLiftPercentage100ths(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::WindowCovering::Attributes::CurrentPositionLiftPercent100ths::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb = nullptr );

    private:
        GoToLiftPercentageCommand mGoToLiftPercentageCommand;
        UpOrOpenCommand mUpOrOpenCommand;
        DownOrCloseCommand mDownOrCloseCommand;
        StopMotionCommand mStopMotionCommand;
        OperationalStatusReader mOperationalStatusReader;
        CurrentPositionLiftPercentage100thsSubscriber mCurrentPositionLiftPercentage100thsSubscriber;
        const uint16_t mSubscriptionMinIntervalFloorSeconds = 0;
        const uint16_t mSubscriptionMaxIntervalCeilingSeconds = 60;
};
