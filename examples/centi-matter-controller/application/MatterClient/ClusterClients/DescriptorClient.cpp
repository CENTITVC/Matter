#include "DescriptorClient.h"


CHIP_ERROR DescriptorClient::ReadPartsList(chip::EndpointId endpointId, 
                void * context, 
                chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::Descriptor::Attributes::PartsList::TypeInfo::DecodableArgType>
                        successCb,
                chip::Controller::ReadResponseFailureCallback failureCb)
{
    mPartsListReader.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, context, successCb, failureCb](chip::Messaging::ExchangeManager & exchangeMgr,
                                                        const chip::SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;

        mPartsListReader.SetTargetNode(exchangeMgr, sessionHandle);

        error = mPartsListReader.ReadAttribute(context, successCb, failureCb);

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

CHIP_ERROR DescriptorClient::ReadServerList(chip::EndpointId endpointId, 
                void * context, 
                chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::Descriptor::Attributes::ServerList::TypeInfo::DecodableArgType>
                        successCb,
                chip::Controller::ReadResponseFailureCallback failureCb)
{
    mServerListReader.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, context, successCb, failureCb](chip::Messaging::ExchangeManager & exchangeMgr,
                                                        const chip::SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;

        mServerListReader.SetTargetNode(exchangeMgr, sessionHandle);

        error = mServerListReader.ReadAttribute(context, successCb, failureCb);

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

CHIP_ERROR DescriptorClient::ReadDeviceTypeList(chip::EndpointId endpointId,
                void * context,
                chip::Controller::ReadResponseSuccessCallback<
                    chip::app::Clusters::Descriptor::Attributes::DeviceTypeList::TypeInfo::DecodableArgType>
                        successCb,
                chip::Controller::ReadResponseFailureCallback failureCb)
{
    mDeviceTypeListReader.SetTargetEndPoint(endpointId);

    auto onConnectionSuccessCb = [this, context, successCb, failureCb](chip::Messaging::ExchangeManager & exchangeMgr,
                                                        const chip::SessionHandle & sessionHandle) 
    {
        CHIP_ERROR error = CHIP_NO_ERROR;

        mDeviceTypeListReader.SetTargetNode(exchangeMgr, sessionHandle);
        
        error = mDeviceTypeListReader.ReadAttribute(context, successCb, failureCb);

        if (error != CHIP_NO_ERROR)
        {
            ChipLogProgress(Controller, "Error in mDeviceTypeListReader.ReadAttribute");
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
