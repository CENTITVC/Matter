#pragma once

#include "MatterClientBase.h"
#include "../MatterClientClusterCommandBase.h"
#include "../MatterClientClusterSubscriptionBase.h"
#include <controller/CHIPCluster.h>
#include <app-common/zap-generated/cluster-objects.h>
#include "../../MatterNode.h"

class IlluminanceMeasuredValueSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::IlluminanceMeasurement::Attributes::MeasuredValue::TypeInfo>
{
    public:
        IlluminanceMeasuredValueSubscriber() : MatterClientClusterSubscriptionBase(chip::app::Clusters::IlluminanceMeasurement::Id) {}
};

class IlluminanceClient : public MatterClientBase
{
    public:
        IlluminanceClient(MatterNode & matterNode) : MatterClientBase(matterNode) {}

        CHIP_ERROR SubscribeMeasuredValue(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::IlluminanceMeasurement::Attributes::MeasuredValue::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb = nullptr );

    private:
        IlluminanceMeasuredValueSubscriber mIlluminanceMeasuredValueSubscriber;
};