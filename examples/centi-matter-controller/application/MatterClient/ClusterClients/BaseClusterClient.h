
#pragma once

#include "MatterClientBase.h"
#include <controller/CHIPCluster.h>
#include <app-common/zap-generated/cluster-objects.h>

// Base template for cluster commands
template<typename CommandType>
class ClusterCommand : public MatterClientClusterCommandBase<CommandType> {
public:
    ClusterCommand(chip::ClusterId clusterId) : 
        MatterClientClusterCommandBase<CommandType>(clusterId) {}

    template<typename RequestType>
    CHIP_ERROR Execute(
        const RequestType& request,
        chip::EndpointId endpointId,
        void* context,
        chip::Controller::CommandResponseSuccessCallback<typename CommandType::ResponseType> successCb,
        chip::Controller::CommandResponseFailureCallback failureCb)
    {
        this->SetTargetEndPoint(endpointId);

        auto onSuccess = [this, &request, context, successCb, failureCb]
            (chip::Messaging::ExchangeManager& exchangeMgr,
             const chip::SessionHandle& sessionHandle) 
        {
            CHIP_ERROR error = CHIP_NO_ERROR;
            this->SetTargetNode(exchangeMgr, sessionHandle);
            error = this->Invoke(request, context, successCb, failureCb);

            if (failureCb != nullptr && error != CHIP_NO_ERROR) {
                failureCb(context, error);
            }
        };

        auto onFailure = [context, failureCb](CHIP_ERROR error) {
            if (failureCb != nullptr && error != CHIP_NO_ERROR) {
                failureCb(context, error);
            }
        };

        return this->ConnectToNode(onSuccess, onFailure);
    }
};

// Base template for cluster subscriptions
template<typename AttributeType>
class ClusterSubscription : public MatterClientClusterSubscriptionBase<AttributeType> {
public:
    ClusterSubscription(chip::ClusterId clusterId) :
        MatterClientClusterSubscriptionBase<AttributeType>(clusterId) {}

    CHIP_ERROR Subscribe(
        chip::EndpointId endpointId,
        void* context,
        chip::Controller::ReadResponseSuccessCallback<typename AttributeType::DecodableArgType> reportCb,
        chip::Controller::ReadResponseFailureCallback failureCb,
        uint16_t minInterval,
        uint16_t maxInterval,
        chip::Controller::SubscriptionEstablishedCallback onEstablished = nullptr,
        chip::Controller::SubscriptionOnDoneCallback onDone = nullptr)
    {
        this->SetTargetEndPoint(endpointId);

        auto onSuccess = [this, context, reportCb, failureCb, minInterval, maxInterval, onEstablished, onDone]
            (chip::Messaging::ExchangeManager& exchangeMgr,
             const chip::SessionHandle& sessionHandle) 
        {
            CHIP_ERROR error = CHIP_NO_ERROR;
            this->SetTargetNode(exchangeMgr, sessionHandle);
            error = this->SubscribeAttribute(context, reportCb, failureCb, minInterval, maxInterval, onEstablished, nullptr, onDone);

            if (failureCb != nullptr && error != CHIP_NO_ERROR) {
                failureCb(context, error);
            }
        };

        auto onFailure = [context, failureCb](CHIP_ERROR error) {
            if (failureCb != nullptr && error != CHIP_NO_ERROR) {
                failureCb(context, error);
            }
        };

        return this->ConnectToNode(onSuccess, onFailure);
    }
};

// Base template for cluster clients using CRTP
template<typename Derived>
class BaseClusterClient : public MatterClientBase {
public:
    BaseClusterClient(MatterNode& matterNode) : MatterClientBase(matterNode) {}

protected:
    // Helper methods that derived classes can use
    template<typename CommandType, typename RequestType>
    CHIP_ERROR ExecuteCommand(
        ClusterCommand<CommandType>& command,
        const RequestType& request,
        chip::EndpointId endpointId,
        void* context,
        chip::Controller::CommandResponseSuccessCallback<typename CommandType::ResponseType> successCb,
        chip::Controller::CommandResponseFailureCallback failureCb)
    {
        return command.Execute(request, endpointId, context, successCb, failureCb);
    }

    template<typename AttributeType>
    CHIP_ERROR SubscribeAttribute(
        ClusterSubscription<AttributeType>& subscription,
        chip::EndpointId endpointId,
        void* context,
        chip::Controller::ReadResponseSuccessCallback<typename AttributeType::DecodableArgType> reportCb,
        chip::Controller::ReadResponseFailureCallback failureCb,
        uint16_t minInterval,
        uint16_t maxInterval,
        chip::Controller::SubscriptionEstablishedCallback onEstablished = nullptr,
        chip::Controller::SubscriptionOnDoneCallback onDone = nullptr)
    {
        return subscription.Subscribe(endpointId, context, reportCb, failureCb, minInterval, maxInterval, onEstablished, onDone);
    }
};