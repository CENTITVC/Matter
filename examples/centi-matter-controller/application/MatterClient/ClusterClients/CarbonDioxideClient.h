#pragma once

#include "MatterClientBase.h"
#include "../MatterClientClusterCommandBase.h"
#include "../MatterClientClusterSubscriptionBase.h"
#include <controller/CHIPCluster.h>
#include <app-common/zap-generated/cluster-objects.h>
#include "../../MatterNode.h"

class CarbonDioxideMeasuredValueSubscriber : public MatterClientClusterSubscriptionBase<chip::app::Clusters::CarbonDioxideConcentrationMeasurement::Attributes::MeasuredValue::TypeInfo>
{
    public:
        CarbonDioxideMeasuredValueSubscriber() : MatterClientClusterSubscriptionBase(chip::app::Clusters::CarbonDioxideConcentrationMeasurement::Id) {}
};

class CarbonDioxideClient : public MatterClientBase
{
    public:
        CarbonDioxideClient(MatterNode & matterNode) : MatterClientBase(matterNode) {}

        CHIP_ERROR SubscribeMeasuredValue(
                    chip::EndpointId endpointId,
                    void * context,
                    chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::CarbonDioxideConcentrationMeasurement::Attributes::MeasuredValue::TypeInfo::DecodableArgType> reportCb,
                    chip::Controller::ReadResponseFailureCallback failureCb,
                    uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
                    chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb = nullptr,
                    chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb = nullptr );

    private:
        CarbonDioxideMeasuredValueSubscriber mCarbonDioxideMeasuredValueSubscriber;
};