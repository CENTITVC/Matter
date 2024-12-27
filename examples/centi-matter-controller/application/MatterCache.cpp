#include "MatterCache.h"

#include <lib/core/TLV.h>
#include <platform/KeyValueStoreManager.h>
#include <system/SystemClock.h>
#include <system/SystemLayer.h>

using namespace chip;

CHIP_ERROR MatterCache::AddMatterNode(MatterNode* matterNode)
{
    ChipLogProgress(chipTool, "MatterCache: AddMatterNode");
    VerifyOrReturnError(matterNode != nullptr, CHIP_ERROR_INVALID_ARGUMENT);

    std::vector<MatterNode> cachedMatterNodes;
    ReturnLogErrorOnFailure(ReadAllMatterNodes(cachedMatterNodes));
    bool newMatterNode = true;

    for (size_t i = 0; i < cachedMatterNodes.size(); i++)
    {
        // overwrite data if these are the same nodes
        if (cachedMatterNodes[i] == *matterNode)
        {
            cachedMatterNodes[i] = *matterNode;
            newMatterNode = false;
        }
    }

    if (newMatterNode)
    {
        cachedMatterNodes.push_back(*matterNode);
        ChipLogProgress(AppServer, "MatterCache::AddMatterNode writing new matter node at position: %lu",
                        static_cast<unsigned long>(cachedMatterNodes.size() - 1));
    }

    return WriteAllMatterNodes(cachedMatterNodes);
}

CHIP_ERROR MatterCache::DeleteMatterNode(MatterNode* matterNode)
{
    ChipLogProgress(chipTool, "MatterCache: DeleteMatterNode");
    VerifyOrReturnError(matterNode != nullptr, CHIP_ERROR_INVALID_ARGUMENT);

    std::vector<MatterNode> cachedMatterNodes;
    ReturnLogErrorOnFailure(ReadAllMatterNodes(cachedMatterNodes));

    auto it = std::find_if(cachedMatterNodes.begin(), cachedMatterNodes.end(),
            [&] (const MatterNode & node) { return node.GetNodeId() == matterNode->GetNodeId(); });
    
    if (it != cachedMatterNodes.end())
    {
        cachedMatterNodes.erase(it);
        return WriteAllMatterNodes(cachedMatterNodes);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterCache::ReadAllMatterNodes(std::vector<MatterNode>& cachedMatterNodes)
{
    ChipLogProgress(chipTool, "MatterCache: ReadAllMatterNodes");
    
    uint8_t data[kCentiControllerDataMaxBytes] = {0};
    size_t data_size = 0;
    CHIP_ERROR err = CHIP_NO_ERROR;

    err = chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Get(kCentiControllerDataKey, data, kCentiControllerDataMaxBytes, &data_size);

    if (err == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND) // no error, if the key-value pair was not stored
    {
        ChipLogProgress(AppServer, "MatterCache::ReadAllMatterNodes ignoring error %" CHIP_ERROR_FORMAT, err.Format());
        return CHIP_NO_ERROR;
    }

    ChipLogProgress(chipTool, "MatterCache::ReadAllMatterNodes Read TLV(CastingData) from KVS store with size: %lu bytes",
                static_cast<unsigned long>(data_size));

    TLV::TLVReader reader;
    reader.Init(data);

    // read the envelope (and version)
    ReturnErrorOnFailure(reader.Next(TLV::kTLVType_Structure, TLV::AnonymousTag()));
    TLV::TLVType outerContainerType = TLV::kTLVType_Structure;
    ReturnErrorOnFailure(reader.EnterContainer(outerContainerType));
    ReturnErrorOnFailure(reader.Next());
    TLV::Tag outerContainerTag      = reader.GetTag();
    uint8_t outerContainerTagNum = static_cast<uint8_t> (TLV::TagNumFromTag(outerContainerTag));
    VerifyOrReturnError(outerContainerTagNum == kCurrentControllerDataVersionTag, CHIP_ERROR_INVALID_TLV_TAG);
    uint32_t version;
    ReturnErrorOnFailure(reader.Get(version));

    ChipLogProgress(AppServer, "MatterCache::ReadAllMatterNodes TLV(CentiController) version: %d", version);

    // Entering matter Nodes container
    TLV::TLVType matterNodesContainerType = TLV::kTLVType_Array;
    ReturnErrorOnFailure(reader.Next());
    ReturnErrorOnFailure(reader.EnterContainer(matterNodesContainerType));

    chip::NodeId nodeId = 0;
    chip::FabricIndex fabricIndex = 0;
    uint16_t vendorId = 0;
    uint16_t productId = 0;
    uint16_t discriminator;
    std::vector<chip::DeviceTypeId> deviceTypeIds;
    uint16_t port = 0;
    uint64_t lastDiscoveredMs = 0;
    const uint8_t QRCodeSize = 22;
    char payload_str[QRCodeSize] = {0};
    std::string payload = "";
    uint8_t nodes_read_cnt = 0;

    while((err = reader.Next()) == CHIP_NO_ERROR)
    {
        TLV::Tag matterNodesContainerTag = reader.GetTag();

        if (!TLV::IsContextTag(matterNodesContainerTag))
        {
            ChipLogError(AppServer, "Unexpected non-context TLV tag.");
            return CHIP_ERROR_INVALID_TLV_TAG;
        }

        uint8_t matterNodesContainerTagNum = static_cast<uint8_t>(TLV::TagNumFromTag(matterNodesContainerTag));

        switch(matterNodesContainerTagNum)
        {
            case kNodeIdTag:
                ReturnErrorOnFailure(reader.Get(nodeId));
                break;
            case kFabricIndexTag:
                ReturnErrorOnFailure(reader.Get(fabricIndex));
                break;
            case kMatterNodeVendorIdTag:
                ReturnErrorOnFailure(reader.Get(vendorId));
                break;
            case kMatterNodeProductIdTag:
                ReturnErrorOnFailure(reader.Get(productId));
                break;
            case kMatterNodeDiscriminatorTag:
                ReturnErrorOnFailure(reader.Get(discriminator));
                break;
            case kMatterNodePortTag:
                ReturnErrorOnFailure(reader.Get(port));
                break;
            case kMatterNodeLastDiscoveredTag:
                ReturnErrorOnFailure(reader.Get(lastDiscoveredMs));
                break;
            case kMatterNodeSetupQRCodeTag:
                ReturnErrorOnFailure(reader.GetBytes(reinterpret_cast<uint8_t *>(payload_str), QRCodeSize));
                payload = payload_str;
                break;
            case kMatterNodeEndpointsContainerTag:
            {
                // Entering Matter Node Endpoints container
                MatterNode matterNode(nodeId, fabricIndex, payload, chip::System::Clock::Milliseconds64(lastDiscoveredMs));
                
                TLV::TLVType matterNodeEndpointArrayContainerType = TLV::kTLVType_Array;
                ReturnErrorOnFailure(reader.EnterContainer(matterNodeEndpointArrayContainerType));

                MatterEndpoint* endpoint = nullptr;
                while((err = reader.Next()) == CHIP_NO_ERROR)
                {
                    TLV::Tag matterNodeContainerTag = reader.GetTag();
                    if (!TLV::IsContextTag(matterNodeContainerTag))
                    {
                        ChipLogError(AppServer, "Unexpected non-context TLV tag.");
                        return CHIP_ERROR_INVALID_TLV_TAG;
                    }

                    uint8_t matterNodeContainerTagNum = static_cast<uint8_t>(TLV::TagNumFromTag(matterNodeContainerTag));
                    if (matterNodeContainerTagNum == kEndpointIdTag)
                    {
                        chip::EndpointId endpointId;
                        ReturnErrorOnFailure(reader.Get(endpointId));
                        endpoint = matterNode.GetOrAddEndpoint(endpointId);
                        //continue;
                    }

                    if (endpoint != nullptr)
                    {
                        if (matterNodeContainerTagNum == kClusterIdsContainerTag)
                        {
                            // Entering ClusterIds container
                            TLV::TLVType clusterIdArrayContainerType = TLV::kTLVType_Array;
                            ReturnErrorOnFailure(reader.EnterContainer(clusterIdArrayContainerType));

                            while ((err = reader.Next()) == CHIP_NO_ERROR)
                            {
                                TLV::Tag clusterIdsContainerTag = reader.GetTag();
                                if (!TLV::IsContextTag(clusterIdsContainerTag))
                                {
                                    ChipLogError(AppServer, "Unexpected non-context TLV tag.");
                                    return CHIP_ERROR_INVALID_TLV_TAG;
                                }

                                uint8_t clusterIdsContainerTagNum = static_cast<uint8_t>(TLV::TagNumFromTag(clusterIdsContainerTag));
                                if (clusterIdsContainerTagNum == kClusterIdTag)
                                {
                                    chip::ClusterId clusterId;
                                    ReturnErrorOnFailure(reader.Get(clusterId));
                                    endpoint->AddCluster(clusterId);
                                    //continue;
                                }
                            }

                            if (err == CHIP_END_OF_TLV)
                            {
                                // Exiting ClusterIds container
                                ReturnErrorOnFailure(reader.ExitContainer(clusterIdArrayContainerType));
//                                break;
                            }
                        }
                        else if (matterNodeContainerTagNum == kDeviceTypeIdsContainerTag)
                        {
                            // Entering DeviceTypeIds container
                            TLV::TLVType deviceTypeIdArrayContainerType = TLV::kTLVType_Array;
                            ReturnErrorOnFailure(reader.EnterContainer(deviceTypeIdArrayContainerType));

                            while((err = reader.Next()) == CHIP_NO_ERROR)
                            {
                                TLV::Tag deviceTypeIdsContainerTag = reader.GetTag();
                                if (!TLV::IsContextTag(deviceTypeIdsContainerTag))
                                {
                                    ChipLogError(AppServer, "Unexpected non-context TLV tag.");
                                    return CHIP_ERROR_INVALID_TLV_TAG;
                                }

                                uint8_t deviceTypeIdsContainerTagNum = static_cast<uint8_t>(TLV::TagNumFromTag(deviceTypeIdsContainerTag));

                                if (deviceTypeIdsContainerTagNum == kDeviceTypeIdTag)
                                {
                                    chip::DeviceTypeId deviceTypeId;
                                    ReturnErrorOnFailure(reader.Get(deviceTypeId));
                                    endpoint->AddDeviceType(deviceTypeId);
                                }
                            }

                            if (err == CHIP_END_OF_TLV)
                            {
                                // Exiting deviceTypeIds container
                                ReturnErrorOnFailure(reader.ExitContainer(deviceTypeIdArrayContainerType));
                            }
                        }
                    }
                }

                if (err == CHIP_END_OF_TLV)
                {
                    // Exiting Matter Node Endpoints container
                    ReturnErrorOnFailure(reader.ExitContainer(matterNodeEndpointArrayContainerType));
                    cachedMatterNodes.emplace_back(matterNode);
                    nodes_read_cnt++;
                }
                break;
            }
            default:
                ChipLogProgress(chipTool, "Invalid matterNodesContainerTagNum (%u)", static_cast<uint8_t>(matterNodesContainerTagNum));
                break;
        }
    }

    VerifyOrReturnError(err == CHIP_END_OF_TLV, err);
    ReturnErrorOnFailure(reader.ExitContainer(matterNodesContainerType));
    ReturnErrorOnFailure(reader.ExitContainer(outerContainerType));

    for (auto & node : cachedMatterNodes)
    {
        ChipLogProgress(Controller, "Cached data");
        ChipLogProgress(Controller, "Node ID 0x" ChipLogFormatX64, ChipLogValueX64(node.GetNodeId()));
        ChipLogProgress(Controller, "FabricIndex: %u", node.GetFabricIndex());
        ChipLogProgress(Controller, "QRCode: %s", node.GetSetupPayloadStr().c_str());

        for (auto & endpoint : node.GetEndpoints())
        {
            endpoint.PrintInfo();
        }
    }

    ChipLogProgress(AppServer, "MatterCache::ReadAllMatterNodes -> Matter nodes read count: %lu",
                    static_cast<unsigned long>(nodes_read_cnt));
    return CHIP_NO_ERROR;
}

void MatterCache::OnFabricRemoved(const chip::FabricTable & fabricTable, chip::FabricIndex fabricIndex)
{
    ChipLogProgress(chipTool, "MatterCache::OnFabricRemoved called for fabricIndex: %d", fabricIndex);

    std::vector<MatterNode> cachedMatterNodes;

    VerifyOrReturn(ReadAllMatterNodes(cachedMatterNodes) == CHIP_NO_ERROR);

    auto it = std::find_if(cachedMatterNodes.begin(), cachedMatterNodes.end(),
            [&] (const MatterNode & node) 
                { 
                    ChipLogProgress(chipTool, "MatterCache::OnFabricRemoved removing matter Node with nodeId 0x" ChipLogFormatX64 "from cache", ChipLogValueX64(node.GetNodeId()));
                    return node.GetFabricIndex() == fabricIndex; 
                } );
    
    if (it != cachedMatterNodes.end())
    {
        cachedMatterNodes.erase(it);
        return (void) WriteAllMatterNodes(cachedMatterNodes);
    }
}

CHIP_ERROR MatterCache::PurgeMatterNodeCache(void)
{
    ChipLogProgress(chipTool, "MatterCache::PurgeMatterNodeCache");
    CHIP_ERROR err = chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Delete(kCentiControllerDataKey);
    if (err == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND) // no error, if the key-value pair was not stored
    {
        ChipLogProgress(AppServer, "MatterCache::PurgeMatterNodeCache ignoring error %" CHIP_ERROR_FORMAT, err.Format());
        return CHIP_NO_ERROR;
    }
    return err;
}

CHIP_ERROR MatterCache::WriteAllMatterNodes(std::vector<MatterNode>& matterNodes)
{
    ChipLogProgress(chipTool, "MatterCache::WriteAllMatterNodes");

    TLV::TLVWriter tlvWriter;
    uint8_t data[kCentiControllerDataMaxBytes];
    tlvWriter.Init(data, kCentiControllerDataMaxBytes);

    TLV::TLVType outerContainerType = TLV::kTLVType_Structure;
    ReturnErrorOnFailure(tlvWriter.StartContainer(TLV::AnonymousTag(), TLV::kTLVType_Structure, outerContainerType));
    ReturnErrorOnFailure(tlvWriter.Put(TLV::ContextTag(kCurrentControllerDataVersionTag), kCentiControllerDataVersion));

    TLV::TLVType matterNodesContainerType = TLV::kTLVType_Array;
    // Matter nodes container starts
    ReturnErrorOnFailure(
        tlvWriter.StartContainer(TLV::ContextTag(kMatterNodesContainerTag), TLV::kTLVType_Structure, matterNodesContainerType));

    std::vector<chip::ClusterId> clusters;
    std::vector<chip::DeviceTypeId> deviceTypeIds;
    uint16_t nodesWritten = 0;
    uint16_t discriminator;
    
    for (auto & matterNode : matterNodes )
    {
        nodesWritten++;

        ReturnErrorOnFailure(tlvWriter.Put(TLV::ContextTag(kNodeIdTag), matterNode.GetNodeId()));
        ReturnErrorOnFailure(tlvWriter.Put(TLV::ContextTag(kFabricIndexTag), matterNode.GetFabricIndex()));
        ReturnErrorOnFailure(tlvWriter.Put(TLV::ContextTag(kMatterNodeVendorIdTag), matterNode.GetVendorID()));
        ReturnErrorOnFailure(tlvWriter.Put(TLV::ContextTag(kMatterNodeProductIdTag), matterNode.GetProductID()));
        ReturnErrorOnFailure(tlvWriter.Put(TLV::ContextTag(kMatterNodePortTag), matterNode.GetPort()));
        ReturnErrorOnFailure(tlvWriter.Put(TLV::ContextTag(kMatterNodeLastDiscoveredTag), matterNode.GetLastDiscovered().count()));
        ReturnErrorOnFailure(tlvWriter.PutBytes(TLV::ContextTag(kMatterNodeSetupQRCodeTag),
                                                (const uint8_t *) matterNode.GetSetupPayloadStr().c_str(),
                                                static_cast<uint32_t>(matterNode.GetSetupPayloadStr().size())));
 
        discriminator = matterNode.GetDiscriminator().IsShortDiscriminator() ? 
                                static_cast<uint16_t>(matterNode.GetDiscriminator().GetShortValue()) :
                                matterNode.GetDiscriminator().GetLongValue();
        ReturnErrorOnFailure(tlvWriter.Put(TLV::ContextTag(kMatterNodeDiscriminatorTag), discriminator));

        std::vector<MatterEndpoint> endpoints;
        endpoints = matterNode.GetEndpoints();
        
        if (!endpoints.empty())
        {
            ChipLogProgress(chipTool, "MatterCache::WriteAllMatterNodes Caching matterNode endpoints");
            TLV::TLVType matterNodeEndpointContainerType = TLV::kTLVType_Array;

            // Matter node endpoints container starts
            ReturnErrorOnFailure(tlvWriter.StartContainer(TLV::ContextTag(kMatterNodeEndpointsContainerTag),
                                                            TLV::kTLVType_Structure, matterNodeEndpointContainerType));
            
            for(auto & endpoint : endpoints)
            {
                ReturnErrorOnFailure(tlvWriter.Put(TLV::ContextTag(kEndpointIdTag), endpoint.GetEndpointId()));

                clusters = endpoint.GetClusters();

                if (!clusters.empty())
                {
                    TLV::TLVType clusterIdsContainerType = TLV::kTLVType_Array;
                    // ClusterIds container starts
                    ReturnErrorOnFailure(tlvWriter.StartContainer(TLV::ContextTag(kClusterIdsContainerTag),
                                                                    TLV::kTLVType_Structure, clusterIdsContainerType));

                    for (auto & cluster : clusters)
                    {
                        ReturnErrorOnFailure(tlvWriter.Put(TLV::ContextTag(kClusterIdTag), cluster));
                    }

                    // ClusterIds container ends
                    ReturnErrorOnFailure(tlvWriter.EndContainer(clusterIdsContainerType));
                }

                deviceTypeIds = endpoint.GetDeviceTypes();

                if (!deviceTypeIds.empty())
                {
                    TLV::TLVType deviceTypeIdsContainerType = TLV::kTLVType_Array;

                    // DeviceTypeIds container starts
                    ReturnErrorOnFailure(tlvWriter.StartContainer(TLV::ContextTag(kDeviceTypeIdsContainerTag),
                                                                    TLV::kTLVType_Structure, deviceTypeIdsContainerType));
                    
                    for (auto & deviceTypeId : deviceTypeIds)
                    {
                        ReturnErrorOnFailure(tlvWriter.Put(TLV::ContextTag(kDeviceTypeIdTag), deviceTypeId));
                    }

                    // DeviceTypeIds container ends
                    ReturnErrorOnFailure(tlvWriter.EndContainer(deviceTypeIdsContainerType));
                }
            }

            // Matter node endpoints container ends
            ReturnErrorOnFailure(tlvWriter.EndContainer(matterNodeEndpointContainerType));
        }
    }
    
    // Matter Node container ends
    ReturnErrorOnFailure(tlvWriter.EndContainer(matterNodesContainerType));
    ReturnErrorOnFailure(tlvWriter.EndContainer(outerContainerType));
    ReturnErrorOnFailure(tlvWriter.Finalize());
        
    ChipLogProgress(chipTool,
                    "MatterCache::WriteAllMatterNodes TLV.LengthWritten: %d bytes, matter node count: %u "
                    "and version: %d",
                    tlvWriter.GetLengthWritten(), nodesWritten, kCentiControllerDataVersion);
    return chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Put(kCentiControllerDataKey, data, tlvWriter.GetLengthWritten());
}
