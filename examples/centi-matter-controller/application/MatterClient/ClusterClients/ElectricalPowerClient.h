#pragma once

#include "MatterClientBase.h"
#include "../MatterClientClusterCommandBase.h"
#include "../MatterClientClusterSubscriptionBase.h"
#include <controller/CHIPCluster.h>
#include <app-common/zap-generated/cluster-objects.h>
#include "../../MatterNode.h"

class ActivePowerSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::ElectricalPowerMeasurement::Attributes::ActivePower::TypeInfo>
{
    public:
        ActivePowerSubscriber() : MatterClientClusterSubscriptionBase(chip::app::Clusters::ElectricalPowerMeasurement::Id) {}
};

class ElectricalPowerClient : public MatterClientBase
{
    public:
        ElectricalPowerClient(MatterNode & matterNode) : MatterClientBase(matterNode) {}

        CHIP_ERROR SubscribeActivePower(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::ElectricalPowerMeasurement::Attributes::ActivePower::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb = nullptr );

    private:
        ActivePowerSubscriber mActivePowerSubscriber;
};