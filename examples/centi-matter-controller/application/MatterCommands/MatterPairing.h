#pragma once

#include <controller/CommissioningDelegate.h>
#include <controller/CurrentFabricRemover.h>

#include <lib/support/Span.h>
#include <lib/support/ThreadOperationalDataset.h>

#include "MatterCommandBase.h"
#include "../MatterEndpoint.h"

using namespace chip;
using namespace chip::Controller;

enum class PairingMode
{
    None,
    Code
};

class MatterPairing :   public MatterCommandBase, 
                        public chip::Controller::DevicePairingDelegate,
                        public chip::Controller::DeviceDiscoveryDelegate,
                        public chip::Credentials::DeviceAttestationDelegate
{
    public:
        MatterPairing (std::function<void(MatterPairing *, CHIP_ERROR)> completeCallback, std::string setUpCode) : 
                       sOnCompleteCallback(completeCallback), mSetUpCode(setUpCode), mCurrentFabricRemoveCallback(OnCurrentFabricRemove, this) 
        {
            mPairingMode = PairingMode::Code;
        }

        MatterPairing (std::function<void(MatterPairing*, CHIP_ERROR)> completeCallback, chip::NodeId nodeId) :
                        mNodeId(nodeId), sOnCompleteCallback(completeCallback), mCurrentFabricRemoveCallback(OnCurrentFabricRemove, this) 
        {
            mPairingMode = PairingMode::None;
        }

        CHIP_ERROR Run(void) override;
        chip::System::Clock::Timeout GetWaitDuration() const override { return chip::System::Clock::Seconds16(120); }
        void callback() override
        {
            if (sOnCompleteCallback != nullptr)
            {
                sOnCompleteCallback(this, this->GetError());
            }
        }

        chip::NodeId GetNodeId(void) { return mNodeId; }
        std::string GetSetUpCode(void) {return mSetUpCode; }
        std::vector<chip::DeviceTypeId> GetDeviceTypes(void) { return mDeviceTypeIdList; };
        
        /////////// DevicePairingDelegate Interface /////////
        void OnStatusUpdate(chip::Controller::DevicePairingDelegate::Status status) override;
        void OnPairingComplete(CHIP_ERROR error) override;
        void OnPairingDeleted(CHIP_ERROR error) override;
        void OnReadCommissioningInfo(const chip::Controller::ReadCommissioningInfo & info) override;
        void OnCommissioningComplete(NodeId deviceId, CHIP_ERROR error) override;
        void OnCommissioningStatusUpdate(PeerId peerId, CommissioningStage stageCompleted, CHIP_ERROR error) override;
        void OnCommissioningFailure(PeerId peerId, CHIP_ERROR error, CommissioningStage stageFailed,
                                        Optional<Credentials::AttestationVerificationResult> additionalErrorInfo) override;

        /////////// DeviceDiscoveryDelegate Interface /////////
        void OnDiscoveredDevice(const chip::Dnssd::CommissionNodeData & nodeData) override;

        /////////// DeviceAttestationDelegate /////////
        chip::Optional<uint16_t> FailSafeExpiryTimeoutSecs() const override;
        void OnDeviceAttestationCompleted(chip::Controller::DeviceCommissioner * deviceCommissioner, chip::DeviceProxy * device,
                                        const chip::Credentials::DeviceAttestationVerifier::AttestationDeviceInfo & info,
                                        chip::Credentials::AttestationVerificationResult attestationResult) override;
    private:
        CHIP_ERROR InitializeMatterNode(chip::NodeId nodeId);
        chip::Controller::CommissioningParameters GetCommissioningParameters();
        CHIP_ERROR PairWithCode(chip::NodeId nodeId);
        CHIP_ERROR Unpair(chip::NodeId nodeId);

        static void OnReadPartsListSuccess(void* context, const chip::app::DataModel::DecodableList<chip::EndpointId> & endpointList);
        static void OnReadPartsListFailure(void* context, CHIP_ERROR error);

        static void OnReadServerListSuccess(void* context, const chip::app::DataModel::DecodableList<chip::ClusterId> & clusterList);
        static void OnReadServerListFailure(void* context, CHIP_ERROR error);

        static void OnReadDeviceTypeListSuccess(void* context, 
                                                const chip::app::DataModel::DecodableList<chip::app::Clusters::Descriptor::Structs::DeviceTypeStruct::DecodableType> & deviceTypeStructList);
        static void OnReadDeviceTypeListFailure(void * context, CHIP_ERROR error);

        void AddOrUpdateEndpointToTrack(EndpointId endpointId)
        {
            mEndpointsTracker[endpointId] = false;  // Initialize with isRead = false
        }

        bool AllEndpointsRead() const
        {
            for (const auto& [id, isRead] : mEndpointsTracker)
            {
                if (!isRead)
                {
                    return false;
                }
            }

            return true;
        }

        void MarkEndpointAsRead(chip::EndpointId endpointId)
        {
            if (mEndpointsTracker.find(endpointId) != mEndpointsTracker.end())
            {
                mEndpointsTracker[endpointId] = true;
            }
            else
            {
                ChipLogProgress(Controller, "Cannot mark endpoint %u since it doesn't exist", endpointId);
            }            
        }

        /**
         * Generate a random operational node id.
         *
         * @param[out] aNodeId where to place the generated id.
         *
         * On error no guarantees are made about the state of aNodeId.
         */
        CHIP_ERROR GetRandomOperationalNodeId(NodeId * aNodeId);
        NodeId mNodeId;
        std::vector<chip::DeviceTypeId> mDeviceTypeIdList;
        std::unordered_map< chip::EndpointId, bool > mEndpointsTracker;

        std::function<void(MatterPairing *, CHIP_ERROR)> sOnCompleteCallback;
        std::string mSetUpCode;
        chip::Thread::OperationalDataset mOperationalDataset;
        PairingMode mPairingMode;

        // For unpair
        chip::Platform::UniquePtr<chip::Controller::CurrentFabricRemover> mCurrentFabricRemover;
        chip::Callback::Callback<chip::Controller::OnCurrentFabricRemove> mCurrentFabricRemoveCallback;

        static void OnCurrentFabricRemove(void * context, NodeId remoteNodeId, CHIP_ERROR status);
};