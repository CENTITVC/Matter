#include "MatterNode.h"
#include "MatterManager.h"

#include <setup_payload/ManualSetupPayloadParser.h>
#include <setup_payload/QRCodeSetupPayloadParser.h>

CHIP_ERROR MatterNode::Initialize(chip::NodeId nodeId, chip::FabricIndex fabricIndex,
                                chip::System::Clock::Timestamp lastDiscovered)
{
    mNodeId = nodeId;
    mFabricIndex = fabricIndex;
    mLastDiscovered = chip::System::SystemClock().GetMonotonicMilliseconds64();

    return CHIP_NO_ERROR;
}

MatterEndpoint * MatterNode::GetOrAddEndpoint(const chip::EndpointId & endpointId)
{
    MatterEndpoint * endpoint = GetEndpoint(endpointId);
    
    if (endpoint != nullptr)
    {
        return endpoint;
    }
    
    AddEndpoint(endpointId);

    return GetEndpoint(endpointId);
}

void MatterNode::AddEndpoint(const chip::EndpointId& endpointId)
{
    // Check if the endpoint already exists to prevent duplicates
    auto it = std::find_if(mEndpoints.begin(), mEndpoints.end(),
                           [&endpointId](const MatterEndpoint& endpoint)
                           { return endpoint.GetEndpointId() == endpointId; });
    
    if (it == mEndpoints.end())
    {
        MatterEndpoint endpoint(endpointId);
        mEndpoints.emplace_back(endpoint);
    }
    else
    {
        ChipLogProgress(NotSpecified, "Endpoint %u already exists", endpointId);
    }
}

MatterEndpoint * MatterNode::GetEndpoint(const chip::EndpointId& endpointId)
{
    if (mEndpoints.size() == 0)
    {
        return nullptr;
    }

    for (auto & endpointInfo : mEndpoints)
    {
        if (endpointInfo.GetEndpointId() == endpointId)
        {
            return &endpointInfo;
        }
    }

    return nullptr;
}

MatterEndpoint * MatterNode::GetEndpointWithCluster(const chip::ClusterId clusterId)
{
    if (mEndpoints.size() == 0)
    {
        return nullptr;
    }

    for(auto & endpoint : mEndpoints)
    {
        if (endpoint.HasCluster(clusterId))
        {
            return &endpoint;
        }
    }

    return nullptr;
}

MatterEndpoint * MatterNode::GetEndpointWithDeviceType(chip::DeviceTypeId deviceTypeId)
{
    if (mEndpoints.size() == 0)
    {
        return nullptr;
    }

    for(auto & endpoint : mEndpoints)
    {
        if (endpoint.HasDeviceType(deviceTypeId))
        {
            return &endpoint;
        }
    }

    return nullptr;
}

std::vector<MatterEndpoint>& MatterNode::GetEndpoints()
{
    return mEndpoints;
}

std::vector<chip::DeviceTypeId> MatterNode::GetDeviceTypes()
{
    std::vector<chip::DeviceTypeId> allDeviceTypesInNode;
    std::vector<chip::DeviceTypeId> deviceTypesInEndpoint;

    for(auto & endpoint : mEndpoints)
    {
        deviceTypesInEndpoint = endpoint.GetDeviceTypes();
        allDeviceTypesInNode.insert(allDeviceTypesInNode.end(), deviceTypesInEndpoint.begin(), deviceTypesInEndpoint.end());
    }
    
    return allDeviceTypesInNode;
}

bool MatterNode::HasDevice(chip::DeviceTypeId deviceTypeId)
{
    for(auto & endpoint : mEndpoints)
    {
        if (endpoint.HasDeviceType(deviceTypeId))
        {
            return true;
        }
    }

    return false;
}

CHIP_ERROR MatterNode::SetPayload(std::string payload)
{
    bool isQRCode = strncmp(payload.c_str(), chip::kQRCodePrefix, strlen(chip::kQRCodePrefix)) == 0;
    if (isQRCode)
    {
        ReturnErrorOnFailure(chip::QRCodeSetupPayloadParser(payload.c_str()).populatePayload(mSetupPayload));
        VerifyOrReturnError(mSetupPayload.isValidQRCodePayload(), CHIP_ERROR_INVALID_ARGUMENT);
    }
    else
    {
        ReturnErrorOnFailure(chip::ManualSetupPayloadParser(payload.c_str()).populatePayload(mSetupPayload));
        VerifyOrReturnError(mSetupPayload.isValidManualCode(), CHIP_ERROR_INVALID_ARGUMENT);
    }

    mSetupPayloadStr = payload;

    return CHIP_NO_ERROR;
}

uint16_t MatterNode::GetVendorID(void)
{
    return mSetupPayload.vendorID;
}

uint16_t MatterNode::GetProductID(void)
{
    return mSetupPayload.productID;
}

chip::SetupDiscriminator MatterNode::GetDiscriminator(void)
{
    return mSetupPayload.discriminator;
}