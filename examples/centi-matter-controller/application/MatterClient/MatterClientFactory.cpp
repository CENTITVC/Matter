// MatterClientFactory.cpp

#include <lib/support/CodeUtils.h>

#include "MatterClientFactory.h"

MatterClientBase* MatterClientFactory::CreateClient(chip::ClusterId clusterId, MatterNode & matterNode) 
{
    auto& clusterMap = mClients[matterNode.GetNodeId()];

    if (clusterMap.find(clusterId) == clusterMap.end()) 
    {
        std::unique_ptr<MatterClientBase> client;
        switch (clusterId)
        {
            case chip::app::Clusters::Descriptor::Id:
                ChipLogProgress(chipTool, "Creating Descriptor Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<DescriptorClient>(matterNode);
                break;
            case chip::app::Clusters::WindowCovering::Id:
                ChipLogProgress(chipTool, "Creating WindowCovering Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<WindowCoveringClient>(matterNode);
                break;
            case chip::app::Clusters::TemperatureMeasurement::Id:
                ChipLogProgress(chipTool, "Creating TemperatureMeasurement Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<TemperatureClient>(matterNode);
                break;
            case chip::app::Clusters::RelativeHumidityMeasurement::Id:
                ChipLogProgress(chipTool, "Creating RelativeHumidityMeasurement Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<RelativeHumidityClient>(matterNode);
                break;
            case chip::app::Clusters::IlluminanceMeasurement::Id:
                ChipLogProgress(chipTool, "Creating IlluminanceMeasurement Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<IlluminanceClient>(matterNode);
                break;
            case chip::app::Clusters::CarbonDioxideConcentrationMeasurement::Id:
                ChipLogProgress(chipTool, "Creating CarbonDioxideConcentrationMeasurement Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<CarbonDioxideClient>(matterNode);
                break;
            case chip::app::Clusters::ElectricalPowerMeasurement::Id:
                ChipLogProgress(chipTool, "Creating ElectricalPowerMeasurement Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<ElectricalPowerClient>(matterNode);
                break;
            case chip::app::Clusters::OccupancySensing::Id:
                ChipLogProgress(chipTool, "Creating OccupancySensingClient Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<OccupancySensingClient>(matterNode);
                break;            
            case chip::app::Clusters::DoorLock::Id:
                ChipLogProgress(chipTool, "Creating DoorLockClient Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<DoorLockClient>(matterNode);
                break;
            case chip::app::Clusters::BooleanState::Id:
                ChipLogProgress(chipTool, "Creating BooleanState Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<BooleanStateClient>(matterNode);
                break;
            case chip::app::Clusters::LevelControl::Id:
                ChipLogProgress(chipTool, "Creating LevelControl Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<LevelControlClient>(matterNode);
                break;
            case chip::app::Clusters::OnOff::Id:
                ChipLogProgress(chipTool, "Creating OnOff Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<OnOffClient>(matterNode);
                break;
            case chip::app::Clusters::ColorControl::Id:
                ChipLogProgress(chipTool, "Creating ColorControl Client for node ID " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));
                client = std::make_unique<ColorControlClient>(matterNode);
                break;
                
            default:
                // Handle unsupported cluster ID or throw an exception
                ChipLogProgress(chipTool, "Unhandled Client with cluster ID " ChipLogFormatMEI, ChipLogValueMEI(clusterId));
                return nullptr;
        }
        clusterMap[clusterId] = std::move(client);
    }
    return clusterMap[clusterId].get();
}

MatterClientBase* MatterClientFactory::GetClient(chip::ClusterId clusterId, MatterNode & matterNode) 
{
    auto nodeIt = mClients.find(matterNode.GetNodeId());
    if (nodeIt != mClients.end()) {
        auto clusterIt = nodeIt->second.find(clusterId);
        if (clusterIt != nodeIt->second.end()) 
        {
            ChipLogProgress(chipTool, "Found Client with cluster ID " ChipLogFormatMEI " for nodeId " ChipLogFormatX64, ChipLogValueMEI(clusterId), ChipLogValueX64(matterNode.GetNodeId()));
            return clusterIt->second.get();
        }
    }
    
    return nullptr;
}

DescriptorClient* MatterClientFactory::GetDescriptorClient(MatterNode & matterNode)
{
    return static_cast<DescriptorClient*>(GetClient(chip::app::Clusters::Descriptor::Id, matterNode));
}

DescriptorClient* MatterClientFactory::GetOrCreateDescriptorClient(MatterNode & matterNode)
{
    DescriptorClient * p_client = nullptr;

    p_client = GetDescriptorClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<DescriptorClient*>(CreateClient(chip::app::Clusters::Descriptor::Id, matterNode));
    }

    return p_client;
}

BooleanStateClient* MatterClientFactory::GetBooleanStateClient(MatterNode & matterNode)
{
    return static_cast<BooleanStateClient*>(GetClient(chip::app::Clusters::BooleanState::Id, matterNode));
}

BooleanStateClient* MatterClientFactory::GetOrCreateBooleanStateClient(MatterNode & matterNode)
{
    BooleanStateClient* p_client = nullptr;

    p_client = GetBooleanStateClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<BooleanStateClient*>(CreateClient(chip::app::Clusters::BooleanState::Id, matterNode));
    }

    return p_client;
}

WindowCoveringClient* MatterClientFactory::GetWindowCoveringClient(MatterNode & matterNode)
{
    return static_cast<WindowCoveringClient*>(GetClient(chip::app::Clusters::WindowCovering::Id, matterNode));
}

WindowCoveringClient* MatterClientFactory::GetOrCreateWindowCoveringClient(MatterNode & matterNode)
{
    WindowCoveringClient* p_client = nullptr;

    p_client = GetWindowCoveringClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<WindowCoveringClient*>(CreateClient(chip::app::Clusters::WindowCovering::Id, matterNode));
    }

    return p_client;
}

TemperatureClient* MatterClientFactory::GetTemperatureClient(MatterNode & matterNode)
{
    return static_cast<TemperatureClient*>(GetClient(chip::app::Clusters::TemperatureMeasurement::Id, matterNode));
}

TemperatureClient* MatterClientFactory::GetOrCreateTemperatureClient(MatterNode & matterNode)
{
    TemperatureClient* p_client = nullptr;

    p_client = GetTemperatureClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<TemperatureClient*>(CreateClient(chip::app::Clusters::TemperatureMeasurement::Id, matterNode));
    }

    return p_client;
}

RelativeHumidityClient* MatterClientFactory::GetRelativeHumidityClient(MatterNode & matterNode)
{
    return static_cast<RelativeHumidityClient*>(GetClient(chip::app::Clusters::RelativeHumidityMeasurement::Id, matterNode));
}

RelativeHumidityClient* MatterClientFactory::GetOrCreateRelativeHumidityClient(MatterNode & matterNode)
{
    RelativeHumidityClient* p_client = nullptr;

    p_client = GetRelativeHumidityClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<RelativeHumidityClient*>(CreateClient(chip::app::Clusters::RelativeHumidityMeasurement::Id, matterNode));
    }

    return p_client;
}

CarbonDioxideClient* MatterClientFactory::GetCarbonDioxideClient(MatterNode & matterNode)
{
    return static_cast<CarbonDioxideClient*>(GetClient(chip::app::Clusters::CarbonDioxideConcentrationMeasurement::Id, matterNode));
}

CarbonDioxideClient* MatterClientFactory::GetOrCreateCarbonDioxideClient(MatterNode & matterNode)
{
    CarbonDioxideClient* p_client = nullptr;

    p_client = GetCarbonDioxideClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<CarbonDioxideClient*>(CreateClient(chip::app::Clusters::CarbonDioxideConcentrationMeasurement::Id, matterNode));
    }

    return p_client;
}

ColorControlClient* MatterClientFactory::GetColorControlClient(MatterNode & matterNode)
{
    return static_cast<ColorControlClient*>(GetClient(chip::app::Clusters::ColorControl::Id, matterNode));
}

ColorControlClient* MatterClientFactory::GetOrCreateColorControlClient(MatterNode & matterNode)
{
    ColorControlClient* p_client = nullptr;

    p_client = GetColorControlClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<ColorControlClient*>(CreateClient(chip::app::Clusters::ColorControl::Id, matterNode));
    }

    return p_client;
}

IlluminanceClient* MatterClientFactory::GetIlluminanceClient(MatterNode & matterNode)
{
    return static_cast<IlluminanceClient*>(GetClient(chip::app::Clusters::IlluminanceMeasurement::Id, matterNode));
}

IlluminanceClient* MatterClientFactory::GetOrCreateIlluminanceClient(MatterNode & matterNode)
{
    IlluminanceClient* p_client = nullptr;

    p_client = GetIlluminanceClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<IlluminanceClient*>(CreateClient(chip::app::Clusters::IlluminanceMeasurement::Id, matterNode));
    }

    return p_client;
}

ElectricalPowerClient* MatterClientFactory::GetElectricalPowerClient(MatterNode & matterNode)
{
    return static_cast<ElectricalPowerClient*>(GetClient(chip::app::Clusters::ElectricalPowerMeasurement::Id, matterNode));
}

ElectricalPowerClient* MatterClientFactory::GetOrCreateElectricalPowerClient(MatterNode & matterNode)
{
    ElectricalPowerClient* p_client = nullptr;

    p_client = GetElectricalPowerClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<ElectricalPowerClient*>(CreateClient(chip::app::Clusters::ElectricalPowerMeasurement::Id, matterNode));
    }

    return p_client;
}

DoorLockClient* MatterClientFactory::GetDoorLockClient(MatterNode & matterNode)
{
    return static_cast<DoorLockClient*>(GetClient(chip::app::Clusters::DoorLock::Id, matterNode));
}

DoorLockClient* MatterClientFactory::GetOrCreateDoorLockClient(MatterNode & matterNode)
{
    DoorLockClient* p_client = nullptr;

    p_client = GetDoorLockClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<DoorLockClient*>(CreateClient(chip::app::Clusters::DoorLock::Id, matterNode));
    }

    return p_client;
}

LevelControlClient* MatterClientFactory::GetLevelControlClient(MatterNode & matterNode)
{
    return static_cast<LevelControlClient*>(GetClient(chip::app::Clusters::LevelControl::Id, matterNode));
}

LevelControlClient* MatterClientFactory::GetOrCreateLevelControlClient(MatterNode & matterNode)
{
    LevelControlClient* p_client = nullptr;

    p_client = GetLevelControlClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<LevelControlClient*>(CreateClient(chip::app::Clusters::LevelControl::Id, matterNode));
    }

    return p_client;
}

OccupancySensingClient* MatterClientFactory::GetOccupancySensingClient(MatterNode & matterNode)
{
    return static_cast<OccupancySensingClient*>(GetClient(chip::app::Clusters::OccupancySensing::Id, matterNode));
}

OccupancySensingClient* MatterClientFactory::GetOrCreateOccupancySensingClient(MatterNode & matterNode)
{
    OccupancySensingClient* p_client = nullptr;

    p_client = GetOccupancySensingClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<OccupancySensingClient*>(CreateClient(chip::app::Clusters::OccupancySensing::Id, matterNode));
    }

    return p_client;
}

OnOffClient* MatterClientFactory::GetOnOffClient(MatterNode & matterNode)
{
    return static_cast<OnOffClient*>(GetClient(chip::app::Clusters::OnOff::Id, matterNode));
}

OnOffClient* MatterClientFactory::GetOrCreateOnOffClient(MatterNode & matterNode)
{
    OnOffClient* p_client = nullptr;

    p_client = GetOnOffClient(matterNode);

    if (p_client == nullptr)
    {
        p_client = static_cast<OnOffClient*>(CreateClient(chip::app::Clusters::OnOff::Id, matterNode));
    }

    return p_client;
}

void MatterClientFactory::DeleteAllClientsForNode(MatterNode & matterNode)
{
    ChipLogProgress(chipTool, "Deleting all clients for nodeId " ChipLogFormatX64, ChipLogValueX64(matterNode.GetNodeId()));

    mClients.erase(matterNode.GetNodeId());
}
