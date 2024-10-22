#include "RelativeHumidityClient.h"

using namespace chip;
using namespace chip::Controller;
using namespace chip::app::Clusters;

CHIP_ERROR RelativeHumidityClient::SubscribeMeasuredValue(
            chip::EndpointId endpointId,
            void * context,
            chip::Controller::ReadResponseSuccessCallback<
            chip::app::Clusters::RelativeHumidityMeasurement::Attributes::MeasuredValue::TypeInfo::DecodableArgType> reportCb,
            chip::Controller::ReadResponseFailureCallback failureCb,
            uint16_t minIntervallFloorSeconds, uint16_t maxIntervalCeilingSeconds,
            chip::Controller::SubscriptionEstablishedCallback subscriptionEstablishedCb,
            chip::Controller::SubscriptionOnDoneCallback subscriptionDoneCb)
{
    mRelativeHumidityMeasuredValueSubscriber.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [=](Messaging::ExchangeManager & exchangeMgr,
                                    const SessionHandle & sessionHandle)
    {
        CHIP_ERROR error = CHIP_NO_ERROR;
        mRelativeHumidityMeasuredValueSubscriber.SetTargetNode(exchangeMgr, sessionHandle);

        error = mRelativeHumidityMeasuredValueSubscriber.SubscribeAttribute(context, reportCb, failureCb, 
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
