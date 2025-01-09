#pragma once

#include "MatterClientBase.h"
#include "../MatterClientClusterCommandBase.h"
#include "../MatterClientClusterReadBase.h"
#include "../MatterClientClusterSubscriptionBase.h"
#include "../MatterClientClusterWriteBase.h"
#include <controller/CHIPCluster.h>

#include <app-common/zap-generated/cluster-objects.h>


class OccupiedHeatingSetpointWriter : public MatterClientWriteBase<chip::app::Clusters::Thermostat::Attributes::OccupiedHeatingSetpoint::TypeInfo>
{
    public:
        OccupiedHeatingSetpointWriter() : MatterClientWriteBase(chip::app::Clusters::Thermostat::Id) {}
};

/* Command Classes */
class SetpointRaiseLowerCommand : public MatterClientClusterCommandBase<chip::app::Clusters::Thermostat::Commands::SetpointRaiseLower::Type>
{
    public:
        SetpointRaiseLowerCommand() : MatterClientClusterCommandBase(chip::app::Clusters::Thermostat::Id) {}
};

class OccupiedHeatingSetpointSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::Thermostat::Attributes::OccupiedHeatingSetpoint::TypeInfo>
{
    public:
        OccupiedHeatingSetpointSubscriber() :  MatterClientClusterSubscriptionBase(chip::app::Clusters::Thermostat::Id) {}
};

class LocalTemperatureSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::Thermostat::Attributes::LocalTemperature::TypeInfo>
{
    public:
        LocalTemperatureSubscriber() :  MatterClientClusterSubscriptionBase(chip::app::Clusters::Thermostat::Id) {}
};

class ThermostatClient : public MatterClientBase
{
    public:
        ThermostatClient(MatterNode & matterNode) : MatterClientBase(matterNode) {}

        CHIP_ERROR WriteOccupiedHeatingSetpoint(int16_t occupiedHeatingSetpoint,
                                                chip::EndpointId endpointId,
                                                void* context,
                                                chip::Controller::WriteResponseSuccessCallback successCb,
                                                chip::Controller::WriteResponseFailureCallback failureCb,
                                                const chip::Optional<uint16_t> & aTimedWriteTimeoutMs,
                                                chip::Controller::WriteResponseDoneCallback onDoneCb,
                                                const chip::Optional<chip::DataVersion> & aDataVersion);

        CHIP_ERROR  CommandSetpointRaiseLower(
                        chip::app::Clusters::Thermostat::SetpointRaiseLowerModeEnum mode, int8_t amount,
                        chip::EndpointId endpointId,
                        void * context,
                        chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::Thermostat::Commands::SetpointRaiseLower::Type::ResponseType> successCb,
                        chip::Controller::CommandResponseFailureCallback failureCb);

        CHIP_ERROR  SubscribeOccupiedHeatingSetpoint(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::Thermostat::Attributes::OccupiedHeatingSetpoint::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback onSubscriptionEstablished = nullptr,
                    chip::Controller::ResubscriptionAttemptCallback onResubscriptionAttempt = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback onSubscriptionDone = nullptr);

        CHIP_ERROR  SubscribeLocalTemperature(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::Thermostat::Attributes::LocalTemperature::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback onSubscriptionEstablished = nullptr,
                    chip::Controller::ResubscriptionAttemptCallback onResubscriptionAttempt = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback onSubscriptionDone = nullptr);

    private:
        OccupiedHeatingSetpointWriter       mOccupiedHeatingSetpointWriter;
        SetpointRaiseLowerCommand           mSetpointRaiseLowerCommand;
        OccupiedHeatingSetpointSubscriber   mOccupiedHeatingSetpointSubscriber;
        LocalTemperatureSubscriber          mLocalTemperatureSubscriber;
};