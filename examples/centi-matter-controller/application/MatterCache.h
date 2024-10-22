#include <lib/support/CodeUtils.h>
#include "MatterNode.h"

class MatterCache  : public chip::FabricTable::Delegate
{
    public:
        CHIP_ERROR AddMatterNode(MatterNode* matterNode);

        CHIP_ERROR DeleteMatterNode(MatterNode* matterNode);

        CHIP_ERROR ReadAllMatterNodes(std::vector<MatterNode>& cachedMatterNodes);

        void OnFabricRemoved(const chip::FabricTable & fabricTable, chip::FabricIndex fabricIndex);

        CHIP_ERROR PurgeMatterNodeCache(void);

    private:
        CHIP_ERROR WriteAllMatterNodes(std::vector<MatterNode>& matterNodes);

        enum MatterNodeTLVTag
        {
            kNodeIdTag = 1,
            kFabricIndexTag,
            kMatterNodesContainerTag,
            kMatterNodeEndpointsContainerTag,
            kEndpointIdTag,
            kClusterIdsContainerTag,
            kClusterIdTag,
            kCurrentControllerDataVersionTag,
            kDeviceTypeIdsContainerTag,
            kDeviceTypeIdTag,
            kMatterNodeVendorIdTag,
            kMatterNodeProductIdTag,
            kMatterNodeDiscriminatorTag,
            kMatterNodePortTag,
            kMatterNodeLastDiscoveredTag,
            kMatterNodeSetupQRCodeTag,

            kContextTagMaxNum = UINT8_MAX
        };

        constexpr static size_t kCentiControllerDataMaxBytes            = 1024 * 100; // 100 KBs
        constexpr static char * kCentiControllerDataKey            = (char *) "com.centi.matter.controller";
        constexpr static uint32_t kCentiControllerDataVersion       = 1;
        constexpr static uint32_t kSupportedCastingDataVersions[1] = { 1 };
};