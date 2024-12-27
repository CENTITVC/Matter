#pragma once

#include <app-common/zap-generated/cluster-objects.h>

#include <string.h>

#include <controller/CHIPDeviceController.h>
#include <system/SystemClock.h>
#include <system/SystemLayer.h>
#include <lib/dnssd/Types.h>
#include "MatterEndpoint.h"
#include <setup_payload/SetupPayload.h>

class MatterNode
{
    public:

        MatterNode() {} //used for cached matter Nodes

        MatterNode(chip::NodeId nodeId) : mNodeId(nodeId) 
        {
            mLastDiscovered = chip::System::SystemClock().GetMonotonicMilliseconds64();
        }

        MatterNode(chip::NodeId nodeId, chip::FabricIndex fabricIndex) : mNodeId(nodeId), mFabricIndex(fabricIndex) 
        {
            mLastDiscovered = chip::System::SystemClock().GetMonotonicMilliseconds64();
        }

        MatterNode(chip::NodeId nodeId, chip::FabricIndex fabricIndex, chip::SetupPayload setupPayload) : 
                        mNodeId(nodeId), mFabricIndex(fabricIndex), mSetupPayload(setupPayload)
        {
            mLastDiscovered = chip::System::SystemClock().GetMonotonicMilliseconds64();
        }

        MatterNode(chip::NodeId nodeId, chip::FabricIndex fabricIndex, std::string setupPayload, chip::System::Clock::Timestamp lastDiscoveredMs) :
                    mNodeId(nodeId), mFabricIndex(fabricIndex), mSetupPayloadStr(setupPayload), mLastDiscovered(lastDiscoveredMs)
        {
            (void) SetPayload(setupPayload);
        }

        ~MatterNode() = default;

        MatterNode & operator()() { return *this; }
        
        bool operator==(const MatterNode& other) 
        {
            return (this->mNodeId == other.mNodeId) && ( this->mFabricIndex == other.mFabricIndex );
        }

        bool IsInitialized() { return mInitialized; }
        void Reset();
        uint16_t GetVendorID(void);
        uint16_t GetProductID(void);
        chip::NodeId GetNodeId(void) const { return mNodeId; }
        chip::FabricIndex GetFabricIndex() const { return mFabricIndex; }
        chip::SetupDiscriminator GetDiscriminator(void);
        uint16_t GetPort() const { return mPort; }
        chip::System::Clock::Timestamp GetLastDiscovered(void) { return mLastDiscovered; }

        std::string GetSetupPayloadStr(void) { return mSetupPayloadStr; }
        CHIP_ERROR SetPayload(std::string payload);

        CHIP_ERROR Initialize(chip::NodeId nodeId, chip::FabricIndex fabricIndex,
                            chip::System::Clock::Timestamp lastDiscovered = chip::System::Clock::kZero);
 
        MatterEndpoint * GetOrAddEndpoint(const chip::EndpointId& endpointId);
        MatterEndpoint * GetEndpoint(const chip::EndpointId& endpointId);
        MatterEndpoint * GetEndpointWithCluster(const chip::ClusterId clusterId);
        MatterEndpoint * GetEndpointWithDeviceType(const chip::DeviceTypeId deviceTypeId);
        std::vector<MatterEndpoint>& GetEndpoints();

        void AddEndpoint(const chip::EndpointId& endpointId);
        bool HasEndpoint(chip::EndpointId& endpointId);
        std::vector<chip::DeviceTypeId> GetDeviceTypes(void);

        bool HasDevice(chip::DeviceTypeId deviceTypeId);
        MatterDevice* GetMatterDevice(chip::DeviceTypeId deviceTypeId);

    private:
        std::vector<MatterEndpoint> mEndpoints;
        chip::NodeId mNodeId;
        chip::FabricIndex mFabricIndex;
        char mDeviceName[chip::Dnssd::kMaxDeviceNameLen + 1] = {};
        uint16_t mPort;
        chip::CharSpan mMACAddress;
        bool mInitialized = false;
        chip::SetupPayload mSetupPayload;
        std::string mSetupPayloadStr;
        chip::System::Clock::Timestamp mLastDiscovered;
        static void HandleDeviceConnected(void * context, chip::Messaging::ExchangeManager & exchangeMgr,
                                        const chip::SessionHandle & sessionHandle);
};
