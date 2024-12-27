#pragma once

#include "MatterClientBase.h"
#include "../MatterClientClusterCommandBase.h"
#include "../MatterClientClusterSubscriptionBase.h"
#include <controller/CHIPCluster.h>
#include <app-common/zap-generated/cluster-objects.h>
#include "../../MatterNode.h"


class DoorStateSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::DoorLock::Attributes::DoorState::TypeInfo>
{
    public:
        DoorStateSubscriber() : MatterClientClusterSubscriptionBase(chip::app::Clusters::DoorLock::Id) {}
};

class DoorLockClient : public MatterClientBase
{
    public:
        DoorLockClient(MatterNode & matterNode) : MatterClientBase(matterNode) {}

        CHIP_ERROR SubscribeDoorState(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::DoorLock::Attributes::DoorState::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb = nullptr );

    private:
        DoorStateSubscriber mDoorStateSubscriber;
};
