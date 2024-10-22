#include "OccupancySensingClient.h"

using namespace chip;
using namespace chip::Controller;
using namespace chip::app::Clusters;

CHIP_ERROR OccupancySensingClient::SubscribeOccupancy(
        chip::EndpointId endpointId,
        void * context,
        chip::Controller::ReadResponseSuccessCallback<
        chip::app::Clusters::OccupancySensing::Attributes::Occupancy::TypeInfo::DecodableArgType> reportCb,
        chip::Controller::ReadResponseFailureCallback failureCb,
        uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
        chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb,
        chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb)

{
    mOccupancySubscriber.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        mOccupancySubscriber.SetTargetNode(exchangeMgr, sessionHandle);

        error = mOccupancySubscriber.SubscribeAttribute(context, reportCb, failureCb, 
                                                        minIntervallFloorSeconds,
                                                        maxIntervalCeilingSeconds,
                                                        subscriptionEstablishedCb,
                                                        nullptr /* ResubscriptionAttemptCallback */,
                                                        subscriptionDoneCb);

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
