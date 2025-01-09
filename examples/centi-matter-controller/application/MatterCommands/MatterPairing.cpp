#include <controller/CHIPDeviceControllerFactory.h>


#include "MatterPairing.h"
#include "../MatterManager.h"
#include "../MatterClient/MatterClientFactory.h"

#include "../MatterClient/ClusterClients/DescriptorClient.h"

#include <lib/support/CodeUtils.h>

#include <setup_payload/ManualSetupPayloadParser.h>
#include <setup_payload/QRCodeSetupPayloadParser.h>

#include <iostream>
#include <iomanip>  // For std::setw and std::setfill
#include <cstdint>  // For uint8_t

using namespace chip;
using namespace chip::Controller;

struct ClusterInteractionContext
{
    MatterPairing* currentPairing;
    chip::EndpointId currentEndpoint;
};


CHIP_ERROR MatterPairing::Run(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    switch(mPairingMode)
    {
        case PairingMode::None:
            SuccessOrExit(Unpair(mNodeId));
        break;

        case PairingMode::Code:
            SuccessOrExit(PairWithCode(mNodeId));
        break;
    }

exit:
    if (err != CHIP_NO_ERROR)
    {
        notifyComplete(err);
    }

    return err;
}

CHIP_ERROR MatterPairing::PairWithCode(chip::NodeId nodeId)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "PairWithCode");

    ReturnLogErrorOnFailure(GetRandomOperationalNodeId(&nodeId));

    err = DeviceLayer::ThreadStackMgr().GetThreadProvision(mOperationalDataset);
    ReturnLogErrorOnFailure(err);

    ChipLogProgress(Controller, "ThreadDataset is %s", mOperationalDataset.IsCommissioned() ? "ready" : "not ready");

    MatterManager::MatterMgr().CurrentCommissioner().RegisterPairingDelegate(this);
    
    err = MatterManager::MatterMgr().CurrentCommissioner().PairDevice(nodeId, mSetUpCode.c_str(), GetCommissioningParameters());
    ReturnLogErrorOnFailure(err);

    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterPairing::Unpair(chip::NodeId nodeId)
{
    ChipLogProgress(chipTool, "Unpairing Matter node ID " ChipLogFormatX64, ChipLogValueX64(nodeId));

    if (! MatterManager::MatterMgr().HasMatterNode(nodeId))
    {
        ChipLogProgress(chipTool, "Matter node with ID " ChipLogFormatX64 " doesn't exist", ChipLogValueX64(nodeId))
        return CHIP_ERROR_INVALID_ARGUMENT;    
    }

    mCurrentFabricRemover = Platform::MakeUnique<Controller::CurrentFabricRemover>(& MatterManager::MatterMgr().CurrentCommissioner());
    return mCurrentFabricRemover->RemoveCurrentFabric(nodeId, &mCurrentFabricRemoveCallback);
}

void MatterPairing::OnStatusUpdate(DevicePairingDelegate::Status status)
{
    switch (status)
    {
        case DevicePairingDelegate::Status::SecurePairingSuccess:
            ChipLogProgress(Controller, "Secure Pairing Success");
            ChipLogProgress(Controller, "CASE establishment successful");
            break;
        case DevicePairingDelegate::Status::SecurePairingFailed:
            ChipLogError(Controller, "Secure Pairing Failed");
            notifyComplete(CHIP_ERROR_NOT_CONNECTED);
            break;
    }
}

void MatterPairing::OnPairingComplete(CHIP_ERROR err)
{
    if (err == CHIP_NO_ERROR)
    {
        ChipLogProgress(Controller, "Pairing Success");
        ChipLogProgress(Controller, "PASE establishment successful");
    }
    else
    {
        ChipLogProgress(Controller, "Pairing Failure: %s", ErrorStr(err));
        notifyComplete(err);
    }
}

void MatterPairing::OnPairingDeleted(CHIP_ERROR err)
{
    if (err == CHIP_NO_ERROR)
    {
        ChipLogProgress(Controller, "Pairing Deleted Success");
    }
    else
    {
        ChipLogProgress(Controller, "Pairing Deleted Failure: %s", ErrorStr(err));
        notifyComplete(err);
    }
}

void MatterPairing::OnReadCommissioningInfo(const Controller::ReadCommissioningInfo & info)
{
    ChipLogProgress(Controller, "OnReadCommissioningInfo - vendorId=0x%04X productId=0x%04X", info.basic.vendorId,
                    info.basic.productId);

    // The string in CharSpan received from the device is not null-terminated, we use std::string here for coping and
    // appending a numm-terminator at the end of the string.
    std::string userActiveModeTriggerInstruction;

    // Note: the callback doesn't own the buffer, should make a copy if it will be used it later.
    if (info.icd.userActiveModeTriggerInstruction.size() != 0)
    {
        userActiveModeTriggerInstruction =
            std::string(info.icd.userActiveModeTriggerInstruction.data(), info.icd.userActiveModeTriggerInstruction.size());
    }

    if (info.icd.userActiveModeTriggerHint.HasAny())
    {
        ChipLogProgress(Controller, "OnReadCommissioningInfo - LIT UserActiveModeTriggerHint=0x%08x",
                        info.icd.userActiveModeTriggerHint.Raw());
        ChipLogProgress(Controller, "OnReadCommissioningInfo - LIT UserActiveModeTriggerInstruction=%s",
                        userActiveModeTriggerInstruction.c_str());
    }
}

void MatterPairing::OnCommissioningComplete(NodeId nodeId, CHIP_ERROR err)
{
    if (err == CHIP_NO_ERROR)
    {
        ChipLogProgress(Controller, "Device commissioning completed with success - connecting to remote node");

        err = InitializeMatterNode(nodeId);
        LogErrorOnFailure(err);

        if (err != CHIP_NO_ERROR)
        {
            this->notifyComplete(err);
        }
    }
    else
    {
        ChipLogProgress(Controller, "Device commissioning Failure: %s", ErrorStr(err));
        notifyComplete(err);
    }
}

void MatterPairing::OnCommissioningFailure(PeerId peerId, CHIP_ERROR error, CommissioningStage stageFailed,
                                        Optional<Credentials::AttestationVerificationResult> additionalErrorInfo)
{
    ChipLogProgress(Controller, "OnCommissioningFailure - NodeId='%lu', err='%s', stage='%s' , additionalErrorInfo='%u'",
    peerId.GetNodeId(), ErrorStr(error), StageToString(stageFailed), 
    static_cast<uint16_t>(additionalErrorInfo.ValueOr(Credentials::AttestationVerificationResult::kSuccess)));

    notifyComplete(error);
}

void MatterPairing::OnCommissioningStatusUpdate(PeerId peerId, CommissioningStage stageCompleted, CHIP_ERROR error)
{
    ChipLogProgress(Controller, "OnCommissioningStatusUpdate - stageCompleted='%s' error='%s'", StageToString(stageCompleted),
                    ErrorStr(error));
}

void MatterPairing::OnDiscoveredDevice(const chip::Dnssd::CommissionNodeData & nodeData)
{
    // Ignore nodes with closed commissioning window
    VerifyOrReturn(nodeData.commissioningMode != 0);

    auto & resolutionData = nodeData;

    const uint16_t port = resolutionData.port;
    char buf[chip::Inet::IPAddress::kMaxStringLength];
    resolutionData.ipAddress[0].ToString(buf);
    ChipLogProgress(Controller, "Discovered Device: %s:%u", buf, port);

    // Stop Mdns discovery.
    auto err = MatterManager::MatterMgr().CurrentCommissioner().StopCommissionableDiscovery();

    // Some platforms does not implement a mechanism to stop mdns browse, so
    // we just ignore CHIP_ERROR_NOT_IMPLEMENTED instead of bailing out.
    if (CHIP_NO_ERROR != err && CHIP_ERROR_NOT_IMPLEMENTED != err)
    {
        return;
    }

    MatterManager::MatterMgr().CurrentCommissioner().RegisterDeviceDiscoveryDelegate(nullptr);

    ChipLogProgress(Controller, "MatterPairing, rendezvousInformation is valid");

    auto commissioningParams = GetCommissioningParameters();
    err         = MatterManager::MatterMgr().CurrentCommissioner().PairDevice(mNodeId, mSetUpCode.c_str(), commissioningParams);
    SuccessOrExit(err);

exit:
    if (err != CHIP_NO_ERROR)
    {
        notifyComplete(err);
    }
 }

void MatterPairing::OnDeviceAttestationCompleted(chip::Controller::DeviceCommissioner * deviceCommissioner,
                                                  chip::DeviceProxy * device,
                                                  const chip::Credentials::DeviceAttestationVerifier::AttestationDeviceInfo & info,
                                                  chip::Credentials::AttestationVerificationResult attestationResult)
{
    ChipLogProgress(Controller, "OnDeviceAttestationCompleted - bypassing verification");

    // Bypass attestation verification, continue with success
    //auto err =
     (void) deviceCommissioner->ContinueCommissioningAfterDeviceAttestation(
        device, chip::Credentials::AttestationVerificationResult::kSuccess);
}

chip::Optional<uint16_t> MatterPairing::FailSafeExpiryTimeoutSecs() const
{
    // We don't need to set additional failsafe timeout as we don't ask the final user if he wants to continue
    return chip::Optional<uint16_t>();
}


CHIP_ERROR MatterPairing::InitializeMatterNode(chip::NodeId nodeId)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    MatterNode * p_matterNode = nullptr;
    DescriptorClient * descriptorClient = nullptr;
    chip::EndpointId rootEndpoint = chip::kRootEndpointId;
    MatterNode matterNode(nodeId, MatterManager::MatterMgr().CurrentCommissioner().GetFabricIndex());
    this->mNodeId = nodeId;

    matterNode.SetPayload(mSetUpCode);

    MatterManager::MatterMgr().AddMatterNode(matterNode);

    p_matterNode = MatterManager::MatterMgr().GetMatterNode(nodeId);
    VerifyOrExit(p_matterNode != nullptr, err = CHIP_ERROR_BAD_REQUEST);

    /* All Matter nodes shall have a root endpoint */
    p_matterNode->GetOrAddEndpoint(rootEndpoint);

    descriptorClient = MatterClientFactory::GetInstance().GetOrCreateDescriptorClient(*p_matterNode);
    
    err = descriptorClient->ReadPartsList(chip::kRootEndpointId, this,
                                          OnReadPartsListSuccess, OnReadPartsListFailure);
    SuccessOrExit(err);

exit:
    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(Controller, "Failed to InitializeMatterNode (%s)", ErrorStr(err));
        this->notifyComplete(err);
    }

    return err;
}

chip::Controller::CommissioningParameters MatterPairing::GetCommissioningParameters()
{
    CommissioningParameters commissioningParameters;
    //const uint8_t* dataset = nullptr;
    uint64_t activeTimestamp = 0;
    uint16_t channel = 0;
    uint8_t extPanId[8];
    uint16_t panId;
    uint8_t masterKey[16];
    uint8_t meshLocalPrefix[8];
    char networkName[17];
    uint8_t pskc[16];

    commissioningParameters.SetThreadOperationalDataset(mOperationalDataset.AsByteSpan());

    ChipLogProgress(Controller, "ThreadDataset");

    for (uint8_t i = 0; i < mOperationalDataset.AsByteSpan().size(); i++)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mOperationalDataset.AsByteSpan().data()[i]);
    }

    std::cout << "\n\n" << std::endl;
    
    SuccessOrExit(mOperationalDataset.GetActiveTimestamp(activeTimestamp));
    SuccessOrExit(mOperationalDataset.GetChannel(channel));
    SuccessOrExit(mOperationalDataset.GetExtendedPanId(extPanId));
    SuccessOrExit(mOperationalDataset.GetMasterKey(masterKey));
    SuccessOrExit(mOperationalDataset.GetNetworkName(networkName));
    SuccessOrExit(mOperationalDataset.GetPSKc(pskc));
    SuccessOrExit(mOperationalDataset.GetMeshLocalPrefix(meshLocalPrefix));
    SuccessOrExit(mOperationalDataset.GetPanId(panId));

    ChipLogProgress(Controller, "Active Timestamp: %lu" , activeTimestamp);
    ChipLogProgress(Controller, "Channel: %u", channel);
    ChipLogProgress(Controller, "Ext PAN ID");

    for (uint8_t i = 0; i < 8; i++)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(extPanId[i]);
    }

    std::cout << "\n\n" << std::endl;

    ChipLogProgress(Controller, "Mesh Local Prefix");

    for (uint8_t i = 0; i < 8; i++)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(meshLocalPrefix[i]);
    }
    std::cout << "\n\n" << std::endl;
    ChipLogProgress(Controller, "MasterKey");

    for (uint8_t i = 0; i < 16; i++)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(masterKey[i]);
    }
    std::cout << "\n\n" << std::endl;

    ChipLogProgress(Controller, "Network name %s", networkName);

    ChipLogProgress(Controller, "PAN ID: 0x%04X", panId);

    ChipLogProgress(Controller, "PSKc");

    for (size_t i = 0; i < 16; i++)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(pskc[i]);
    }
    std::cout << "\n\n" << std::endl;

    commissioningParameters.SetDeviceAttestationDelegate(this);

exit:
    return commissioningParameters;
}


void MatterPairing::OnReadPartsListSuccess(void* context, const chip::app::DataModel::DecodableList<chip::EndpointId> & endpointList)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    MatterPairing* currentPairing = static_cast<MatterPairing *>(context);

    MatterNode* matterNode = MatterManager::MatterMgr().GetMatterNode(currentPairing->mNodeId);
    ChipLogProgress(Controller, "MatterNode address: %p", static_cast<void *>(matterNode));

    DescriptorClient * descriptorClient = nullptr;

    ChipLogProgress(Controller, "DescriptorReadParts: Default Success Response endpoint=root");

    auto iter = endpointList.begin();

    while (iter.Next())
    {
        const chip::EndpointId & endpointId = iter.GetValue();
        ChipLogProgress(NotSpecified, "Endpoint %u is from PartsList", endpointId);
        matterNode->AddEndpoint(endpointId);
        currentPairing->AddOrUpdateEndpointToTrack(endpointId);
        
        ClusterInteractionContext* clusterInteractionContext = new ClusterInteractionContext();

        clusterInteractionContext->currentPairing = currentPairing;
        clusterInteractionContext->currentEndpoint = endpointId;

        descriptorClient = MatterClientFactory::GetInstance().GetOrCreateDescriptorClient(*matterNode);

        if (descriptorClient != nullptr)
        {
            /* Every endpoint shall have a ServerList attribute to read the clusters contained... */
            err = descriptorClient->ReadServerList(clusterInteractionContext->currentEndpoint, clusterInteractionContext,
                                                    OnReadServerListSuccess, OnReadServerListFailure);
            
            if (err == CHIP_NO_ERROR)
            {
                /* Every endpoint shall have a DeviceTypeList attribute to read the deviceTypes contained in the endpoint... */
                err = descriptorClient->ReadDeviceTypeList(clusterInteractionContext->currentEndpoint, clusterInteractionContext,
                                                            OnReadDeviceTypeListSuccess, OnReadDeviceTypeListFailure);
                
                if (err != CHIP_NO_ERROR)
                {
                    ChipLogProgress(Controller, "Failed to ReadDeviceTypeList for endpoint %u and node %lu", endpointId, matterNode->GetNodeId());
                    currentPairing->notifyComplete(err);
                    delete clusterInteractionContext;
                }
            }
            else
            {
                ChipLogProgress(Controller, "Failed to read ServerList for endpoint %u and node %lu", endpointId, matterNode->GetNodeId());
                currentPairing->notifyComplete(err);
                delete clusterInteractionContext;
            }
        }
        else
        {
            ChipLogProgress(Controller, "descriptorClient for MatterNode %lu is null", matterNode->GetNodeId());
            delete clusterInteractionContext;
        }
    }
}

void MatterPairing::OnReadPartsListFailure(void* context, CHIP_ERROR error)
{
    ChipLogError(Controller, "Descriptor: Default Failure Response: %" CHIP_ERROR_FORMAT, error.Format());
    ClusterInteractionContext * clusterInteractionContext = static_cast<ClusterInteractionContext*>(context);
    MatterPairing* currentPairing = clusterInteractionContext->currentPairing;
    currentPairing->notifyComplete(error);

    delete clusterInteractionContext;
}

void MatterPairing::OnReadServerListSuccess(void* context, const chip::app::DataModel::DecodableList<chip::ClusterId> & clusterList)
{
    ClusterInteractionContext * clusterInteractionContext = static_cast<ClusterInteractionContext*>(context);
    VerifyOrDieWithMsg(clusterInteractionContext != nullptr, Controller, "currentEndpoint in clusterInteractionContext is null");

    chip::EndpointId currentEndpoint = clusterInteractionContext->currentEndpoint;
    MatterPairing* currentPairing = clusterInteractionContext->currentPairing;
    MatterEndpoint* matterEndpoint = nullptr;

    ChipLogProgress(Controller, "OnReadServerListSuccess: Default Success Response endpoint=%u", currentEndpoint);
    VerifyOrDieWithMsg(currentPairing != nullptr, Controller, "currentPairing in clusterInteractionContext is null");

    MatterNode* matterNode = MatterManager::MatterMgr().GetMatterNode(currentPairing->mNodeId);
    ChipLogProgress(Controller, "MatterNode address: %p", static_cast<void *>(matterNode));

    matterEndpoint = matterNode->GetEndpoint(currentEndpoint);
    VerifyOrDieWithMsg(matterEndpoint != nullptr, Controller, "matterEndpoint retrieved from clusterInteractionContext and MatterNode is null");
    ChipLogProgress(Controller, "MatterNode endpoint: %p", static_cast<void *>(matterEndpoint));

    auto iter = clusterList.begin();
    
    while (iter.Next())
    {
        chip::ClusterId clusterId = iter.GetValue();
        if (!matterEndpoint->HasCluster(clusterId))
        {
            matterEndpoint->AddCluster(clusterId);
        }
    }
}

void MatterPairing::OnReadServerListFailure(void* context, CHIP_ERROR error)
{
    ChipLogError(Controller, "OnReadServerListFailure: Default Failure Response: %" CHIP_ERROR_FORMAT, error.Format());
    ClusterInteractionContext * clusterInteractionContext = static_cast<ClusterInteractionContext*>(context);
    MatterPairing* currentPairing = clusterInteractionContext->currentPairing;

    VerifyOrDieWithMsg(currentPairing != nullptr, Controller, "currentPairing in clusterInteractionContext is null");

    currentPairing->notifyComplete(error);

    delete clusterInteractionContext;
}

void MatterPairing::OnReadDeviceTypeListSuccess(void* context, 
    const chip::app::DataModel::DecodableList<chip::app::Clusters::Descriptor::Structs::DeviceTypeStruct::DecodableType> & deviceTypeStructList)
{
    MatterEndpoint* matterEndpoint = nullptr;
    MatterNode* matterNode = nullptr;
    MatterPairing* currentPairing = nullptr;
    chip::EndpointId currentEndpoint = chip::kInvalidEndpointId;

    ChipLogProgress(Controller, "OnReadDeviceTypeListSuccess: Success Response" );

    ClusterInteractionContext * clusterInteractionContext = static_cast<ClusterInteractionContext*>(context);
    VerifyOrDieWithMsg(clusterInteractionContext != nullptr, Controller, "clusterInteractionContext is null");

    currentPairing = clusterInteractionContext->currentPairing;
    currentEndpoint = clusterInteractionContext->currentEndpoint;

    ChipLogProgress(Controller, "OnReadDeviceTypeListSuccess: Default Success Response endpoint=%u", currentEndpoint);

    VerifyOrDieWithMsg(currentPairing != nullptr, Controller, "currentPairing in clusterInteractionContext is null");

    matterNode = MatterManager::MatterMgr().GetMatterNode(currentPairing->mNodeId);
    VerifyOrDieWithMsg(matterNode != nullptr, Controller, "matterNode with nodeId %lu from currentPairing %p is null", 
                        currentPairing->mNodeId, static_cast<void *>(currentPairing));

    matterEndpoint = matterNode->GetEndpoint(currentEndpoint);
    VerifyOrDieWithMsg(matterEndpoint != nullptr, Controller, "matterEndpoint retrieved from clusterInteractionContext and MatterNode is null");

    auto iter = deviceTypeStructList.begin();

    while(iter.Next())
    {
        auto deviceTypeStruct = iter.GetValue();
        ChipLogProgress(NotSpecified, "DeviceType to add: 0x%08X", deviceTypeStruct.deviceType);
        
        if (!matterEndpoint->HasDeviceType(deviceTypeStruct.deviceType))
        {
            matterEndpoint->AddDeviceType(deviceTypeStruct.deviceType);
            currentPairing->mDeviceTypeIdList.push_back(deviceTypeStruct.deviceType);
        }
    }

    matterEndpoint->PrintInfo();

    currentPairing->MarkEndpointAsRead(currentEndpoint);

    if (currentPairing->AllEndpointsRead())
    {
        ChipLogProgress(Controller, "All endpoints read and discovered, saving now Matter Node to cache");
        
        CHIP_ERROR err = CHIP_NO_ERROR;
        err = MatterManager::MatterMgr().SaveMatterNodeToCache(currentPairing->mNodeId);
        LogErrorOnFailure(err);
        currentPairing->notifyComplete(err);
    }

    delete clusterInteractionContext;
}

void MatterPairing::OnReadDeviceTypeListFailure(void * context, CHIP_ERROR error)
{
    ChipLogError(Controller, "OnReadDeviceTypeListFailure: %" CHIP_ERROR_FORMAT, error.Format());
    ClusterInteractionContext * clusterInteractionContext = static_cast<ClusterInteractionContext*>(context);
    MatterPairing* currentPairing = clusterInteractionContext->currentPairing;
    currentPairing->notifyComplete(error);

    delete clusterInteractionContext;
}

void MatterPairing::OnCurrentFabricRemove(void * context, NodeId nodeId, CHIP_ERROR err)
{
    MatterPairing * currentPairing = reinterpret_cast<MatterPairing *>(context);
    VerifyOrReturn(currentPairing != nullptr, ChipLogError(chipTool, "OnCurrentFabricRemove: context is null"));

    if (err == CHIP_NO_ERROR)
    {
        ChipLogProgress(chipTool, "Device unpair completed with success: " ChipLogFormatX64, ChipLogValueX64(nodeId));
    }
    else
    {
        ChipLogProgress(chipTool, "Device unpair Failure: " ChipLogFormatX64 " %s", ChipLogValueX64(nodeId), ErrorStr(err));
    }

    if (err == CHIP_NO_ERROR)
    {
        MatterManager::MatterMgr().RemoveMatterNode(nodeId);
    }

    currentPairing->notifyComplete(err);
}

CHIP_ERROR MatterPairing::GetRandomOperationalNodeId(NodeId * aNodeId)
{
    for (int i = 0; i < 10; ++i)
    {
        CHIP_ERROR err = chip::Crypto::DRBG_get_bytes(reinterpret_cast<uint8_t *>(aNodeId), sizeof(*aNodeId));
        if (err != CHIP_NO_ERROR)
        {
            return err;
        }

        if (IsOperationalNodeId(*aNodeId))
        {
            return CHIP_NO_ERROR;
        }
    }

    // Terrible, universe-ending luck (chances are 1 in 2^280 or so here, if our
    // DRBG is good).
    return CHIP_ERROR_INTERNAL;
}
