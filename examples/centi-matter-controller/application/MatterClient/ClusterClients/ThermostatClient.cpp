#include "ThermostatClient.h"

using namespace chip;
using namespace chip::Controller;
using namespace chip::app::Clusters;

CHIP_ERROR ThermostatClient::WriteOccupiedHeatingSetpoint(int16_t occupiedHeatingSetpoint,
                                        chip::EndpointId endpointId,
                                        void* context,
                                        chip::Controller::WriteResponseSuccessCallback successCb,
                                        chip::Controller::WriteResponseFailureCallback failureCb,
                                        const chip::Optional<uint16_t> & aTimedWriteTimeoutMs,
                                        chip::Controller::WriteResponseDoneCallback onDoneCb,
                                        const chip::Optional<DataVersion> & aDataVersion)
{
    // "Ligeira" trafulhice para alterar o system mode. 
    // Altera-se atributo system mode quando controlador recebe um comando SetThermostatOccupiedHeatSetpoint com valor específico
    if(occupiedHeatingSetpoint >= 10000)
    {
        mSystemModeWriter.SetTargetEndPoint(endpointId);
    }else{
        mOccupiedHeatingSetpointWriter.SetTargetEndPoint(endpointId);
    }

    auto onConnectionSuccessCb = [this, context, occupiedHeatingSetpoint, successCb, failureCb](Messaging::ExchangeManager & exchangeMgr,
                                                                        const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;

        ChipLogProgress(NotSpecified, "[DEBUG] OCCUPIEDHEATINGSETPOINT: %d", occupiedHeatingSetpoint);

        if(occupiedHeatingSetpoint >= 10000)
        {
            Thermostat::Attributes::SystemMode::TypeInfo::Type request;

            // Se occupiedHeatingSetpoint recebido == 10000, System Mode = 0
            // Se occupiedHeatingSetpoint recebido == 10100, System Mode = 1
            request = (occupiedHeatingSetpoint == 10000) ? Thermostat::SystemModeEnum::kOff : Thermostat::SystemModeEnum::kHeat;

            ChipLogProgress(NotSpecified, "[DEBUG] a escrever para system mode:");

            mSystemModeWriter.SetTargetNode(exchangeMgr, sessionHandle);
            error = mSystemModeWriter.WriteAttribute(request, context, successCb, failureCb, chip::NullOptional, nullptr, chip::NullOptional);
        }
        else
        {
            ChipLogProgress(NotSpecified, "[DEBUG] a escrever para occupiedheatingsetpoint");

            Thermostat::Attributes::OccupiedHeatingSetpoint::TypeInfo::Type request = occupiedHeatingSetpoint;
            mOccupiedHeatingSetpointWriter.SetTargetNode(exchangeMgr, sessionHandle);
            error = mOccupiedHeatingSetpointWriter.WriteAttribute(request, context, successCb, failureCb, chip::NullOptional, nullptr, chip::NullOptional);
        }

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

CHIP_ERROR  ThermostatClient::CommandSetpointRaiseLower(
                chip::app::Clusters::Thermostat::SetpointRaiseLowerModeEnum mode, int8_t amount,
                chip::EndpointId endpointId,
                void * context,
                chip::Controller::CommandResponseSuccessCallback<typename chip::app::Clusters::Thermostat::Commands::SetpointRaiseLower::Type::ResponseType> successCb,
                chip::Controller::CommandResponseFailureCallback failureCb)
{
    mSetpointRaiseLowerCommand.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [mode, amount, this, context, successCb, failureCb](Messaging::ExchangeManager & exchangeMgr,
                                                                        const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        Thermostat::Commands::SetpointRaiseLower::Type request = 
        {
            .mode = mode,
            .amount = amount
        };

        mSetpointRaiseLowerCommand.SetTargetNode(exchangeMgr, sessionHandle);

        error = mSetpointRaiseLowerCommand.Invoke(request, context, successCb, failureCb);

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

CHIP_ERROR  ThermostatClient::SubscribeOccupiedHeatingSetpoint(
            chip::EndpointId endpointId,
            void * context,
            chip::Controller::ReadResponseSuccessCallback<
            chip::app::Clusters::Thermostat::Attributes::OccupiedHeatingSetpoint::TypeInfo::DecodableArgType> reportCb,
            chip::Controller::ReadResponseFailureCallback failureCb,
            uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
            chip::Controller::SubscriptionEstablishedCallback onSubscriptionEstablished,
            chip::Controller::ResubscriptionAttemptCallback onResubscriptionAttempt,
            chip::Controller::SubscriptionOnDoneCallback onSubscriptionDone)
{
    mOccupiedHeatingSetpointSubscriber.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        mOccupiedHeatingSetpointSubscriber.SetTargetNode(exchangeMgr, sessionHandle);

        error = mOccupiedHeatingSetpointSubscriber.SubscribeAttribute(context, reportCb, failureCb, 
                                                    minIntervallFloorSeconds,
                                                    maxIntervalCeilingSeconds,
                                                    onSubscriptionEstablished,
                                                    nullptr /* ResubscriptionAttemptCallback */,
                                                    onSubscriptionDone);

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

CHIP_ERROR  ThermostatClient::SubscribeLocalTemperature(
            chip::EndpointId endpointId,
            void * context,
            chip::Controller::ReadResponseSuccessCallback<
            chip::app::Clusters::Thermostat::Attributes::LocalTemperature::TypeInfo::DecodableArgType> reportCb,
            chip::Controller::ReadResponseFailureCallback failureCb,
            uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
            chip::Controller::SubscriptionEstablishedCallback onSubscriptionEstablished,
            chip::Controller::ResubscriptionAttemptCallback onResubscriptionAttempt,
            chip::Controller::SubscriptionOnDoneCallback onSubscriptionDone)
{
    mLocalTemperatureSubscriber.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        mLocalTemperatureSubscriber.SetTargetNode(exchangeMgr, sessionHandle);

        error = mLocalTemperatureSubscriber.SubscribeAttribute(context, reportCb, failureCb, 
                                                    minIntervallFloorSeconds,
                                                    maxIntervalCeilingSeconds,
                                                    onSubscriptionEstablished,
                                                    nullptr /* ResubscriptionAttemptCallback */,
                                                    onSubscriptionDone);

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