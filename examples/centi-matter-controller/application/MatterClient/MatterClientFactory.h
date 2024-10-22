#pragma once


#include "ClusterClients/MatterClientBase.h"
#include "ClusterClients/DescriptorClient.h"
#include "ClusterClients/WindowCoveringClient.h"
#include "ClusterClients/TemperatureClient.h"
#include "ClusterClients/RelativeHumidityClient.h"
#include "ClusterClients/CarbonDioxideClient.h"
#include "ClusterClients/ElectricalPowerClient.h"
#include "ClusterClients/IlluminanceClient.h"
#include "ClusterClients/DoorLockClient.h"
#include "ClusterClients/OccupancySensingClient.h"
#include "ClusterClients/BooleanStateClient.h"
#include "ClusterClients/OnOffClient.h"
#include "ClusterClients/LevelControlClient.h"
#include "ClusterClients/ColorControlClient.h"
#include <unordered_map>
#include <memory>


class MatterClientFactory {
public:
    static MatterClientFactory& GetInstance() 
    {
        static MatterClientFactory instance;
        return instance;
    }
    
    MatterClientFactory(const MatterClientFactory&) = delete;
    MatterClientFactory& operator=(const MatterClientFactory&) = delete;

    MatterClientBase* CreateClient(chip::ClusterId clusterId, MatterNode & matterNode);
    MatterClientBase* GetClient(chip::ClusterId clusterId, MatterNode & matterNode);
    void DeleteAllClientsForNode(MatterNode & matterNode);

    BooleanStateClient* GetBooleanStateClient(MatterNode & matterNode);
    BooleanStateClient* GetOrCreateBooleanStateClient(MatterNode & matterNode);

    CarbonDioxideClient* GetCarbonDioxideClient(MatterNode & matterNode);
    CarbonDioxideClient* GetOrCreateCarbonDioxideClient(MatterNode & matterNode);

    ColorControlClient* GetColorControlClient(MatterNode & matterNode);
    ColorControlClient* GetOrCreateColorControlClient(MatterNode & matterNode);

    DescriptorClient* GetDescriptorClient(MatterNode & matterNode);
    DescriptorClient* GetOrCreateDescriptorClient(MatterNode & matterNode);

    DoorLockClient* GetDoorLockClient(MatterNode & matterNode);
    DoorLockClient* GetOrCreateDoorLockClient(MatterNode & matterNode);

    ElectricalPowerClient* GetElectricalPowerClient(MatterNode & matterNode);
    ElectricalPowerClient* GetOrCreateElectricalPowerClient(MatterNode & matterNode);

    IlluminanceClient* GetIlluminanceClient(MatterNode & matterNode);
    IlluminanceClient* GetOrCreateIlluminanceClient(MatterNode & matterNode);

    LevelControlClient* GetLevelControlClient(MatterNode & matterNode);
    LevelControlClient* GetOrCreateLevelControlClient(MatterNode & matterNode);

    OccupancySensingClient* GetOccupancySensingClient(MatterNode & matterNode);
    OccupancySensingClient* GetOrCreateOccupancySensingClient(MatterNode & matterNode);

    OnOffClient* GetOnOffClient(MatterNode & matterNode);
    OnOffClient* GetOrCreateOnOffClient(MatterNode & matterNode);

    RelativeHumidityClient* GetRelativeHumidityClient(MatterNode & matterNode);
    RelativeHumidityClient* GetOrCreateRelativeHumidityClient(MatterNode & matterNode);

    TemperatureClient* GetTemperatureClient(MatterNode & matterNode);
    TemperatureClient* GetOrCreateTemperatureClient(MatterNode & matterNode);

    WindowCoveringClient* GetWindowCoveringClient(MatterNode & matterNode);
    WindowCoveringClient* GetOrCreateWindowCoveringClient(MatterNode & matterNode);

    private:
        MatterClientFactory() = default;
        std::unordered_map<chip::NodeId, std::unordered_map<chip::ClusterId, std::unique_ptr<MatterClientBase>>> mClients;
};