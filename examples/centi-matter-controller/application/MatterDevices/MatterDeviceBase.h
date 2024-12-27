#pragma once

#include <platform/CHIPDeviceLayer.h>
#include "MatterDeviceTypes.h"
#include <controller/CHIPCluster.h>

class MatterDevice
{
    public:
        MatterDevice(chip::DeviceTypeId deviceTypeId) : mNodeId(chip::kUndefinedNodeId),
                                                        mEndpointId(chip::kInvalidEndpointId),
                                                        mDeviceTypeId(deviceTypeId) {}

        MatterDevice(chip::NodeId nodeId, chip::EndpointId endpointId, chip::DeviceTypeId deviceTypeId) : 
            mNodeId(nodeId), mEndpointId(endpointId), mDeviceTypeId(deviceTypeId) {}

        virtual ~MatterDevice() = default;

        chip::NodeId GetNodeId(void) { return mNodeId; }
        chip::EndpointId GetEndpointId (void) { return mEndpointId; }
        chip::DeviceTypeId GetType(void) { return mDeviceTypeId; } 
        
        MatterDevice & operator()() { return *this; }
        
        bool operator==(const MatterDevice& other) 
        {
            return (this->mEndpointId == other.mEndpointId) && ( this->mDeviceTypeId == other.mDeviceTypeId );
        }

        virtual std::unique_ptr<MatterDevice> clone() const 
        {
                return std::make_unique<MatterDevice>(*this);
        }

    private:
        chip::NodeId mNodeId;
        chip::EndpointId mEndpointId;
        chip::DeviceTypeId mDeviceTypeId;
};

/*  
    Acabar de preencher para cada ficheiro e MatterDevice
    Adicionar ao MatterEndpoint um vetor de MatterDevice
    Adicionar comando de subscrição a cada MatterDevice encontrado no AppTask
    Adicionar IllianceDevicesBase no AppTask para criar um timer para cada um deles e publicar no MQTT
*/
