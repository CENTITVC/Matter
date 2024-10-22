#pragma once

#include <platform/CHIPDeviceLayer.h>
#include "MatterDevices/MatterDeviceBase.h"
#include "MatterDevices/MatterDevicesCommon.h"

inline constexpr size_t kMaxNumberOfClustersPerEndpoint = 10;

class MatterEndpoint
{
public:
    MatterEndpoint(chip::EndpointId endpointId) : mEndpointId(endpointId) {}

    ~MatterEndpoint() = default;

    MatterEndpoint(MatterEndpoint const& other)
    {
        mClusters = other.mClusters;
        mDeviceTypes = other.mDeviceTypes;

        mMatterDevices.clear();
        mMatterDevices.reserve(other.mMatterDevices.size());

        for (const auto & matterDevice : other.mMatterDevices)
        {
            mMatterDevices.push_back(matterDevice->clone());
            ChipLogProgress(chipTool, "Endpoint copied: %u from addr %p", other.GetEndpointId(), (void*) this);
        }

        mEndpointId = other.mEndpointId;
    }

    MatterEndpoint operator=(const MatterEndpoint& other)
    {
        if (this != &other)
        {
            mClusters = other.mClusters;
            mDeviceTypes = other.mDeviceTypes;

            mMatterDevices.clear();
            mMatterDevices.reserve(other.mMatterDevices.size());

            for (const auto & matterDevice : other.mMatterDevices)
            {
                mMatterDevices.push_back(matterDevice->clone());
            }

            mEndpointId = other.mEndpointId;
        }

        return *this;
    }

    MatterEndpoint(MatterEndpoint&&) = default;

    chip::EndpointId GetEndpointId() const { return mEndpointId; }

    bool HasCluster(chip::ClusterId clusterId);
    std::vector<chip::ClusterId> GetClusters(void);
    void AddCluster(chip::ClusterId clusterId);

    void AddDeviceType(chip::DeviceTypeId& deviceTypeId);
    bool HasDeviceType(const chip::DeviceTypeId& deviceTypeId);
    std::vector<chip::DeviceTypeId>& GetDeviceTypes(void);
    const std::vector<std::unique_ptr<MatterDevice>>& GetMatterDevices(void) const;
    MatterDevice* GetMatterDevice(chip::DeviceTypeId deviceTypeId);
    void PrintInfo();

private:
    std::vector<chip::ClusterId> mClusters;
    std::vector<chip::DeviceTypeId> mDeviceTypes;
    std::vector<std::unique_ptr<MatterDevice>> mMatterDevices;
    chip::EndpointId mEndpointId;
};