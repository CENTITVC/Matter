#pragma once

#include "MatterClientBase.h"
#include "../MatterClientClusterCommandBase.h"
#include "../MatterClientClusterReadBase.h"
#include <controller/CHIPCluster.h>
#include <app-common/zap-generated/cluster-objects.h>
#include "../../MatterNode.h"

class PartsListReader : public MatterClientReadBase<chip::app::Clusters::Descriptor::Attributes::PartsList::TypeInfo>
{
    public:
        PartsListReader() : MatterClientReadBase(chip::app::Clusters::Descriptor::Id) {}
};

class ServerListReader : public MatterClientReadBase<chip::app::Clusters::Descriptor::Attributes::ServerList::TypeInfo>
{
    public: 
        ServerListReader() : MatterClientReadBase(chip::app::Clusters::Descriptor::Id) {}
};

class DeviceTypeListReader : public MatterClientReadBase<chip::app::Clusters::Descriptor::Attributes::DeviceTypeList::TypeInfo>
{
    public:
        DeviceTypeListReader() : MatterClientReadBase(chip::app::Clusters::Descriptor::Id) {}
};

class DescriptorClient : public MatterClientBase
{
    public:
        DescriptorClient(MatterNode & matterNode) : MatterClientBase(matterNode) {}

        CHIP_ERROR ReadPartsList(chip::EndpointId endpointId, 
                                void * context, 
                                chip::Controller::ReadResponseSuccessCallback<
                                chip::app::Clusters::Descriptor::Attributes::PartsList::TypeInfo::DecodableArgType> successCb,
                                chip::Controller::ReadResponseFailureCallback failureCb);
        
        CHIP_ERROR ReadServerList(chip::EndpointId endpointId, 
                                void * context,
                                chip::Controller::ReadResponseSuccessCallback<
                                chip::app::Clusters::Descriptor::Attributes::ServerList::TypeInfo::DecodableArgType> successCb,
                                chip::Controller::ReadResponseFailureCallback failureCb);
        
        CHIP_ERROR ReadDeviceTypeList(chip::EndpointId endpointId,
                                void * context,
                                chip::Controller::ReadResponseSuccessCallback<
                                chip::app::Clusters::Descriptor::Attributes::DeviceTypeList::TypeInfo::DecodableArgType> successCb,
                                chip::Controller::ReadResponseFailureCallback failureCb);
    private:
        PartsListReader mPartsListReader;
        ServerListReader mServerListReader;
        DeviceTypeListReader mDeviceTypeListReader;

};