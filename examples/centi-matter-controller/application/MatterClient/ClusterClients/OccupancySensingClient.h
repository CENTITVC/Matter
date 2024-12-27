#pragma once

#include "MatterClientBase.h"
#include "../MatterClientClusterCommandBase.h"
#include "../MatterClientClusterSubscriptionBase.h"
#include <controller/CHIPCluster.h>
#include <app-common/zap-generated/cluster-objects.h>
#include "../../MatterNode.h"

class OccupancySubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::OccupancySensing::Attributes::Occupancy::TypeInfo>
{
    public:
        OccupancySubscriber() : MatterClientClusterSubscriptionBase(chip::app::Clusters::OccupancySensing::Id) {}
};

class OccupancySensingClient : public MatterClientBase
{
    public:
        OccupancySensingClient(MatterNode & matterNode) : MatterClientBase(matterNode) {}

        CHIP_ERROR SubscribeOccupancy(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::OccupancySensing::Attributes::Occupancy::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb = nullptr );

    private:
        OccupancySubscriber mOccupancySubscriber;
};
