#include "MatterClientBase.h"
#include "../MatterClientClusterCommandBase.h"
#include "../MatterClientClusterReadBase.h"
#include "../MatterClientClusterSubscriptionBase.h"
#include <controller/CHIPCluster.h>

#include <app-common/zap-generated/cluster-objects.h>


/* Command Classes */
class StepCommand : public MatterClientClusterCommandBase<chip::app::Clusters::LevelControl::Commands::Step::Type>
{
    public:
        StepCommand() : MatterClientClusterCommandBase(chip::app::Clusters::LevelControl::Id) {}
};

class MoveToLevelCommand : public MatterClientClusterCommandBase<chip::app::Clusters::LevelControl::Commands::MoveToLevel::Type>
{
    public:
        MoveToLevelCommand() : MatterClientClusterCommandBase(chip::app::Clusters::LevelControl::Id) {}
};

/* Subscription Classes */
class CurrentLevelSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::LevelControl::Attributes::CurrentLevel::TypeInfo>
{
    public:
        CurrentLevelSubscriber() : MatterClientClusterSubscriptionBase(chip::app::Clusters::LevelControl::Id) {}
};

class LevelControlClient : public MatterClientBase
{
    public:
        LevelControlClient(MatterNode & matterNode) : MatterClientBase(matterNode) {}

        CHIP_ERROR  CommandStep(
                        chip::app::Clusters::LevelControl::StepModeEnum stepMode, uint8_t stepSize,
                        chip::app::DataModel::Nullable<uint16_t> transitionTime,
                        chip::BitMask<chip::app::Clusters::LevelControl::OptionsBitmap> optionMask,
                        chip::BitMask<chip::app::Clusters::LevelControl::OptionsBitmap> optionOverride,                
                        chip::EndpointId endpointId,
                        void * context,
                        chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::LevelControl::Commands::Step::Type::ResponseType> successCb,
                        chip::Controller::CommandResponseFailureCallback failureCb);

        CHIP_ERROR  CommandMoveToLevel(
                        uint8_t level, chip::app::DataModel::Nullable<uint16_t> transitionTime,
                        chip::BitMask<chip::app::Clusters::LevelControl::OptionsBitmap> optionMask,
                        chip::BitMask<chip::app::Clusters::LevelControl::OptionsBitmap> optionOverride,                        
                        chip::EndpointId endpointId,
                        void * context,
                        chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::LevelControl::Commands::MoveToLevel::Type::ResponseType> successCb,
                        chip::Controller::CommandResponseFailureCallback failureCb);

        CHIP_ERROR SubscribeCurrentLevel(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::LevelControl::Attributes::CurrentLevel::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb = nullptr );
    
    private:
        StepCommand mStepCommand;
        MoveToLevelCommand mMoveToLevelCommand;
        CurrentLevelSubscriber mCurrentLevelSubscriber;
};