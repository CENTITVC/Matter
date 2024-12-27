#include "MatterEndpoint.h"

using namespace chip;

bool MatterEndpoint::HasCluster(ClusterId clusterId)
{
    if (mClusters.size() == 0)
    {
        return false;
    }
    
    for (auto & cluster : mClusters)
    {
        if (cluster == clusterId)
        {
            return true;
        }
    }

    return false;
}

std::vector<chip::ClusterId> MatterEndpoint::GetClusters()
{
    return mClusters;
}

void MatterEndpoint::AddCluster(ClusterId clusterId)
{
    mClusters.push_back(clusterId);
}

void MatterEndpoint::AddDeviceType(chip::DeviceTypeId& deviceTypeId)
{
    mDeviceTypes.push_back(deviceTypeId);

    if (GetMatterDevice(deviceTypeId) == nullptr)
    {
        switch(deviceTypeId)
        {
            case MATTER_DEVICE_ID_AIR_QUALITY_SENSOR:
                mMatterDevices.push_back(std::move(std::make_unique<AirQualitySensor>()));
                break;
            case MATTER_DEVICE_ID_CONTACT_SENSOR:
                mMatterDevices.push_back(std::move(std::make_unique<ContactSensor>()));
                break;
            case MATTER_DEVICE_ID_DOOR_LOCK:
                mMatterDevices.push_back(std::move(std::make_unique<DoorLock>()));
                break;
            case MATTER_DEVICE_ID_EXTENDED_COLOR_LIGHT:
                mMatterDevices.push_back(std::move(std::make_unique<ExtendedColorLight>()));
                break;
            case MATTER_DEVICE_ID_OCCUPANCY_SENSOR:
                mMatterDevices.push_back(std::move(std::make_unique<OccupancySensor>()));
                break;
            case MATTER_DEVICE_ID_LIGHT_SENSOR:
                mMatterDevices.push_back(std::move(std::make_unique<LightSensor>()));
                break;
            case MATTER_DEVICE_ID_HUMIDITY_SENSOR:
                mMatterDevices.push_back(std::move(std::make_unique<HumiditySensor>()));
                break;
            case MATTER_DEVICE_ID_TEMPERATURE_SENSOR:
                mMatterDevices.push_back(std::move(std::make_unique<TemperatureSensor>()));
                break;
            case MATTER_DEVICE_ID_ELECTRICAL_SENSOR:
                mMatterDevices.push_back(std::move(std::make_unique<ElectricalSensor>()));
                break;
            case MATTER_DEVICE_ID_WINDOW_COVERING:
                mMatterDevices.push_back(std::move(std::make_unique<WindowCover>()));
                break;

            default:
                ChipLogProgress(NotSpecified, "Not handling MatterDevice with deviceType=" ChipLogFormatMEI, ChipLogValueMEI(deviceTypeId));
                break;
        }
    }
}

bool MatterEndpoint::HasDeviceType(const chip::DeviceTypeId& deviceTypeId)
{
    for (chip::DeviceTypeId & type : mDeviceTypes)
    {
        if (type == deviceTypeId)
        {
            return true;
        }
    }

    return false;
}

std::vector<chip::DeviceTypeId>& MatterEndpoint::GetDeviceTypes(void)
{
    return mDeviceTypes;
}

const std::vector<std::unique_ptr<MatterDevice>>& MatterEndpoint::GetMatterDevices(void) const
{
    return mMatterDevices;
}

MatterDevice* MatterEndpoint::GetMatterDevice(chip::DeviceTypeId deviceTypeId)
{
    for (auto& p_Device : GetMatterDevices())
    {
        if (p_Device->GetType() == deviceTypeId)
        {
            return p_Device.get();
        }
    }
    
    return nullptr;
}

void MatterEndpoint::PrintInfo()
{
    ChipLogProgress(NotSpecified, "   endpoint=%u", mEndpointId);
    ChipLogProgress(NotSpecified, "     Number of clusters: %lu", mClusters.size());

    for (chip::ClusterId & cluster : mClusters)
    {
        if (cluster != kInvalidClusterId)
        {
            ChipLogProgress(NotSpecified, "      cluster=" ChipLogFormatMEI, ChipLogValueMEI(cluster));
        }
    }

    for (chip::DeviceTypeId & type : mDeviceTypes)
    {
        ChipLogProgress(NotSpecified, "         deviceType=" ChipLogFormatMEI, ChipLogValueMEI(type));
    }
}