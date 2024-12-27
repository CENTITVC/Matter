#include <application/MatterManager.h>
#include "MatterClient/MatterClientFactory.h"
#include <include/CHIPProjectAppConfig.h>

#include <controller/CHIPDeviceControllerFactory.h>

#include <credentials/attestation_verifier/FileAttestationTrustStore.h>
#include <lib/core/CHIPConfig.h>
#include <lib/core/CHIPVendorIdentifiers.hpp>
#include <lib/core/CHIPVendorIdentifiers.hpp>

#include <lib/support/CodeUtils.h>
#include <lib/support/ScopedBuffer.h>
#include <lib/support/TestGroupData.h>

using namespace chip;
using namespace chip::DeviceLayer;

std::map<MatterManager::CommissionerIdentity, std::unique_ptr<chip::Controller::DeviceCommissioner>> MatterManager::mCommissioners;

constexpr chip::FabricId kIdentityNullFabricId  = chip::kUndefinedFabricId;
constexpr chip::FabricId kIdentityAlphaFabricId = 1;
constexpr chip::FabricId kIdentityBetaFabricId  = 2;
constexpr chip::FabricId kIdentityGammaFabricId = 3;
constexpr chip::FabricId kIdentityOtherFabricId = 4;
constexpr char kPAATrustStorePathVariable[]     = "AppServer_PAA_TRUST_STORE_PATH";
constexpr char kCDTrustStorePathVariable[]      = "AppServer_CD_TRUST_STORE_PATH";
static char kStorageDirectory[] = "/var/matter";
const chip::Credentials::AttestationTrustStore * MatterManager::sTrustStore = nullptr;
chip::Credentials::GroupDataProviderImpl MatterManager::sGroupDataProvider{ kMaxGroupsPerFabric, kMaxGroupKeysPerFabric };
// All fabrics share the same ICD client storage.
chip::app::DefaultICDClientStorage MatterManager::sICDClientStorage;
chip::Crypto::RawKeySessionKeystore MatterManager::sSessionKeystore;
chip::app::CheckInHandler MatterManager::sCheckInHandler;

namespace 
{
    CHIP_ERROR GetAttestationTrustStore(const char * paaTrustStorePath, const chip::Credentials::AttestationTrustStore ** trustStore)
    {
        if (paaTrustStorePath == nullptr)
        {
            paaTrustStorePath = getenv(kPAATrustStorePathVariable);
        }

        if (paaTrustStorePath == nullptr)
        {
            *trustStore = chip::Credentials::GetTestAttestationTrustStore();
            return CHIP_NO_ERROR;
        }

        static chip::Credentials::FileAttestationTrustStore attestationTrustStore{ paaTrustStorePath };

        if (paaTrustStorePath != nullptr && attestationTrustStore.paaCount() == 0)
        {
            ChipLogError(AppServer, "No PAAs found in path: %s", paaTrustStorePath);
            ChipLogError(AppServer,
                        "Please specify a valid path containing trusted PAA certificates using "
                        "the argument [--paa-trust-store-path paa/file/path] "
                        "or environment variable [%s=paa/file/path]",
                        kPAATrustStorePathVariable);
            return CHIP_ERROR_INVALID_ARGUMENT;
        }

        *trustStore = &attestationTrustStore;
        return CHIP_NO_ERROR;
    }

} // namespace

CHIP_ERROR MatterManager::Init(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    uint16_t pan_id = 0;

    err = Platform::MemoryInit();
    ReturnLogErrorOnFailure(err);

    mStorageDirectory.SetValue(kStorageDirectory);

    // By default, Linux device is configured as a BLE peripheral while the controller needs a BLE central.
    ReturnLogErrorOnFailure(chip::DeviceLayer::Internal::BLEMgrImpl().ConfigureBle(mBleAdapterId.ValueOr(0), true));
    ReturnLogErrorOnFailure(mDefaultStorage.Init(nullptr, GetStorageDirectory().ValueOr(nullptr)));
    //chip::DeviceLayer::PersistedStorage::KeyValueStoreMgrImpl().Init("/var/matter/centi_kvs");
    ReturnLogErrorOnFailure(mDefaultStorage.Init(nullptr, GetStorageDirectory().ValueOr(nullptr)));
    ReturnLogErrorOnFailure(mOperationalKeystore.Init(&mDefaultStorage));
    ReturnLogErrorOnFailure(mOpCertStore.Init(&mDefaultStorage));

    // chip-tool uses a non-persistent keystore.
    // ICD storage lifetime is currently tied to the chip-tool's lifetime. Since chip-tool interactive mode is currently used for
    // ICD commissioning and check-in validation, this temporary storage meets the test requirements.
    // TODO: Implement persistent ICD storage for the chip-tool.
    ReturnLogErrorOnFailure(sICDClientStorage.Init(&mDefaultStorage, &sSessionKeystore));

    chip::Controller::FactoryInitParams factoryInitParams;

    factoryInitParams.fabricIndependentStorage = &mDefaultStorage;
    factoryInitParams.operationalKeystore      = &mOperationalKeystore;
    factoryInitParams.opCertStore              = &mOpCertStore;
    factoryInitParams.enableServerInteractions = true;
    factoryInitParams.sessionKeystore          = &sSessionKeystore;

    sGroupDataProvider.SetStorageDelegate(&mDefaultStorage);
    sGroupDataProvider.SetSessionKeystore(factoryInitParams.sessionKeystore);
    ReturnLogErrorOnFailure(sGroupDataProvider.Init());
    chip::Credentials::SetGroupDataProvider(&sGroupDataProvider);
    factoryInitParams.groupDataProvider = &sGroupDataProvider;

    uint16_t port = mDefaultStorage.GetListenPort();
    if (port != 0)
    {
        // Make sure different commissioners run on different ports.
        port = static_cast<uint16_t>(port + CurrentCommissionerId());
    }
    factoryInitParams.listenPort = port;
    
    ReturnLogErrorOnFailure(chip::Controller::DeviceControllerFactory::GetInstance().Init(factoryInitParams));

    auto systemState = chip::Controller::DeviceControllerFactory::GetInstance().GetSystemState();
    VerifyOrReturnError(nullptr != systemState, CHIP_ERROR_INCORRECT_STATE);

/*    
    auto server = systemState->BDXTransferServer();
    VerifyOrReturnError(nullptr != server, CHIP_ERROR_INCORRECT_STATE);
    server->SetDelegate(&BDXDiagnosticLogsServerDelegate::GetInstance());
*/

    ReturnErrorOnFailure(GetAttestationTrustStore(mPaaTrustStorePath.ValueOr(nullptr), &sTrustStore));
/*
    auto engine = chip::app::InteractionModelEngine::GetInstance();
    VerifyOrReturnError(engine != nullptr, CHIP_ERROR_INCORRECT_STATE);
    ReturnLogErrorOnFailure(AppServerCheckInDelegate()->Init(&sICDClientStorage, engine));
    ReturnLogErrorOnFailure(sCheckInHandler.Init(chip::Controller::DeviceControllerFactory::GetInstance().GetSystemState()->ExchangeMgr(),
                                                 &sICDClientStorage, AppServerCheckInDelegate(), engine));
*/
    
    CommissionerIdentity nullIdentity{ kIdentityNull, chip::kUndefinedNodeId };
    ReturnLogErrorOnFailure(InitializeCommissioner(nullIdentity, kIdentityNullFabricId));

//    SetIdentity(kIdentityAlpha);
//    CommissionerIdentity alphaIdentify{ kIdentityAlpha, 1 };
//    ReturnLogErrorOnFailure(InitializeCommissioner(alphaIdentify, kIdentityAlphaFabricId));

    // After initializing first commissioner, add the additional CD certs once
    {
        const char * cdTrustStorePath = mCDTrustStorePath.ValueOr(nullptr);
        if (cdTrustStorePath == nullptr)
        {
            cdTrustStorePath = getenv(kCDTrustStorePathVariable);
        }

        auto additionalCdCerts =
            chip::Credentials::LoadAllX509DerCerts(cdTrustStorePath, chip::Credentials::CertificateValidationMode::kPublicKeyOnly);
        if (cdTrustStorePath != nullptr && additionalCdCerts.size() == 0)
        {
            ChipLogError(AppServer, "Warning: no CD signing certs found in path: %s, only defaults will be used", cdTrustStorePath);
            ChipLogError(AppServer,
                         "Please specify a path containing trusted CD verifying key certificates using "
                         "the argument [--cd-trust-store-path cd/file/path] "
                         "or environment variable [%s=cd/file/path]",
                         kCDTrustStorePathVariable);
        }
        ReturnErrorOnFailure(mCredIssuerCmds.AddAdditionalCDVerifyingCerts(additionalCdCerts));
    }
    bool allowTestCdSigningKey = !mOnlyAllowTrustedCdKeys.ValueOr(false);
    mCredIssuerCmds.SetCredentialIssuerOption(CredentialIssuerCommands::CredentialIssuerOptions::kAllowTestCdSigningKey,
                                               allowTestCdSigningKey);

    err = DeviceLayer::ThreadStackMgrImpl().InitThreadStack();
    ReturnLogErrorOnFailure(err);

    ChipLogProgress(NotSpecified, "Thread is %s", 
                        DeviceLayer::ConnectivityMgr().IsThreadEnabled() ? "enabled" : "disabled");

    if (DeviceLayer::ConnectivityMgr().IsThreadEnabled())
    {
        Thread::OperationalDataset newDataset;

        err = DeviceLayer::ThreadStackMgr().GetThreadProvision(newDataset);
        ReturnLogErrorOnFailure(err);

        err = newDataset.GetPanId(pan_id);
        ReturnLogErrorOnFailure(err);
        ChipLogProgress(NotSpecified, "Thread PanId 0x%04X", pan_id);

        ChipLogProgress(NotSpecified, "Thread is %s", 
                            DeviceLayer::ConnectivityMgr().IsThreadEnabled() ? "enabled" : "disabled");
    }

    ReturnLogErrorOnFailure(LoadMatterNodesFromCache());

    return CHIP_NO_ERROR;
}

void MatterManager::SetIdentity(const char * identity)
{
    std::string name = std::string(identity);
    if (name.compare(kIdentityAlpha) != 0 && name.compare(kIdentityBeta) != 0 && name.compare(kIdentityGamma) != 0 &&
        name.compare(kIdentityNull) != 0 && strtoull(name.c_str(), nullptr, 0) < kIdentityOtherFabricId)
    {
        ChipLogError(AppServer, "Unknown commissioner name: %s. Supported names are [%s, %s, %s, 4, 5...]", name.c_str(),
                     kIdentityAlpha, kIdentityBeta, kIdentityGamma);
        chipDie();
    }

    mCommissionerName.SetValue(const_cast<char *>(identity));
}

std::string MatterManager::GetIdentity()
{
    std::string name = mCommissionerName.HasValue() ? mCommissionerName.Value() : kIdentityAlpha;

    ChipLogProgress(chipTool, "Identity initial for commissioner %s", name.c_str());

    if (name.compare(kIdentityAlpha) != 0 && name.compare(kIdentityBeta) != 0 && name.compare(kIdentityGamma) != 0 &&
        name.compare(kIdentityNull) != 0)
    {
        chip::FabricId fabricId = strtoull(name.c_str(), nullptr, 0);
        if (fabricId >= kIdentityOtherFabricId)
        {
            // normalize name since it is used in persistent storage

            char s[24];
            sprintf(s, "%" PRIu64, fabricId);

            name = s;
        }
        else
        {
            ChipLogError(AppServer, "Unknown commissioner name: %s. Supported names are [%s, %s, %s, 4, 5...]", name.c_str(),
                         kIdentityAlpha, kIdentityBeta, kIdentityGamma);
            chipDie();
        }
    }

    return name;
}

CHIP_ERROR MatterManager::GetIdentityNodeId(std::string identity, chip::NodeId * nodeId)
{
    if (mCommissionerNodeId.HasValue())
    {
        *nodeId = mCommissionerNodeId.Value();
        return CHIP_NO_ERROR;
    }

    if (identity == kIdentityNull)
    {
        *nodeId = chip::kUndefinedNodeId;
        return CHIP_NO_ERROR;
    }
    
    ReturnLogErrorOnFailure(mCommissionerStorage.Init(identity.c_str(), GetStorageDirectory().ValueOr(nullptr)));

    *nodeId = mCommissionerStorage.GetLocalNodeId();

    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterManager::GetIdentityRootCertificate(std::string identity, chip::ByteSpan & span)
{
    if (identity == kIdentityNull)
    {
        return CHIP_ERROR_NOT_FOUND;
    }

    chip::NodeId nodeId;
    VerifyOrDie(GetIdentityNodeId(identity, &nodeId) == CHIP_NO_ERROR);
    CommissionerIdentity lookupKey{ identity, nodeId };
    auto item = mCommissioners.find(lookupKey);

    span = chip::ByteSpan(item->first.mRCAC, item->first.mRCACLen);
    return CHIP_NO_ERROR;
}

chip::FabricId MatterManager::CurrentCommissionerId()
{
    chip::FabricId id;

    std::string name = GetIdentity();
    if (name.compare(kIdentityAlpha) == 0)
    {
        id = kIdentityAlphaFabricId;
    }
    else if (name.compare(kIdentityBeta) == 0)
    {
        id = kIdentityBetaFabricId;
    }
    else if (name.compare(kIdentityGamma) == 0)
    {
        id = kIdentityGammaFabricId;
    }
    else if (name.compare(kIdentityNull) == 0)
    {
        id = kIdentityNullFabricId;
    }
    else if ((id = strtoull(name.c_str(), nullptr, 0)) < kIdentityOtherFabricId)
    {
        VerifyOrDieWithMsg(false, AppServer, "Unknown commissioner name: %s. Supported names are [%s, %s, %s, 4, 5...]",
                           name.c_str(), kIdentityAlpha, kIdentityBeta, kIdentityGamma);
    }

    return id;
}

chip::Controller::DeviceCommissioner & MatterManager::GetCommissionerForFabricIndex(chip::FabricIndex fabricIndex)
{
    std::string identity;

    switch(fabricIndex)
    {
        case kIdentityAlphaFabricId:
            identity = kIdentityAlpha;
            break;
        case kIdentityBetaFabricId:
            identity = kIdentityBeta;
            break;
        case kIdentityGammaFabricId:
            identity = kIdentityGamma;
            break;
        default:
            VerifyOrDie(false);
            break;
    }

    return GetCommissioner(identity);
}    

chip::Controller::DeviceCommissioner & MatterManager::CurrentCommissioner()
{
    return GetCommissioner(GetIdentity());
}

chip::Controller::DeviceCommissioner & MatterManager::GetCommissioner(std::string identity)
{
    // We don't have a great way to handle commissioner setup failures here.
    // This only matters for commands (like TestCommand) that involve multiple
    // identities.
    VerifyOrDie(EnsureCommissionerForIdentity(identity) == CHIP_NO_ERROR);

    chip::NodeId nodeId;
    VerifyOrDie(GetIdentityNodeId(identity, &nodeId) == CHIP_NO_ERROR);
    CommissionerIdentity lookupKey{ identity, nodeId };
    auto item = mCommissioners.find(lookupKey);
    VerifyOrDie(item != mCommissioners.end());
    return *item->second;
}

CHIP_ERROR MatterManager::EnsureCommissionerForIdentity(std::string identity)
{
    chip::NodeId nodeId;
    ReturnErrorOnFailure(GetIdentityNodeId(identity, &nodeId));
    CommissionerIdentity lookupKey{ identity, nodeId };
    if (mCommissioners.find(lookupKey) != mCommissioners.end())
    {
        return CHIP_NO_ERROR;
    }

    // Need to initialize the commissioner.
    chip::FabricId fabricId;
    if (identity == kIdentityAlpha)
    {
        fabricId = kIdentityAlphaFabricId;
    }
    else if (identity == kIdentityBeta)
    {
        fabricId = kIdentityBetaFabricId;
    }
    else if (identity == kIdentityGamma)
    {
        fabricId = kIdentityGammaFabricId;
    }
    else
    {
        fabricId = strtoull(identity.c_str(), nullptr, 0);
        if (fabricId < kIdentityOtherFabricId)
        {
            ChipLogError(chipTool, "Invalid identity: %s", identity.c_str());
            return CHIP_ERROR_INVALID_ARGUMENT;
        }
    }

    return InitializeCommissioner(lookupKey, fabricId);
}

void MatterManager::ShutdownCommissioner(const CommissionerIdentity & key)
{
    mCommissioners[key].get()->Shutdown();
}

CHIP_ERROR MatterManager::InitializeCommissioner(CommissionerIdentity & identity, chip::FabricId fabricId)
{
    std::unique_ptr<chip::Controller::DeviceCommissioner> commissioner = std::make_unique<chip::Controller::DeviceCommissioner>();
    chip::Controller::SetupParams commissionerParams;

    ReturnLogErrorOnFailure(mCredIssuerCmds.SetupDeviceAttestation(commissionerParams, sTrustStore));

    chip::Crypto::P256Keypair ephemeralKey;

    if (fabricId != chip::kUndefinedFabricId)
    {
        // TODO - OpCreds should only be generated for pairing command
        //        store the credentials in persistent storage, and
        //        generate when not available in the storage.
        ReturnLogErrorOnFailure(mCommissionerStorage.Init(identity.mName.c_str(), GetStorageDirectory().ValueOr(nullptr)));
        if (mUseMaxSizedCerts.HasValue())
        {
            auto option = CredentialIssuerCommands::CredentialIssuerOptions::kMaximizeCertificateSizes;
            mCredIssuerCmds.SetCredentialIssuerOption(option, mUseMaxSizedCerts.Value());
        }

        ReturnLogErrorOnFailure(mCredIssuerCmds.InitializeCredentialsIssuer(mCommissionerStorage));

        chip::MutableByteSpan nocSpan(identity.mNOC);
        chip::MutableByteSpan icacSpan(identity.mICAC);
        chip::MutableByteSpan rcacSpan(identity.mRCAC);

        ReturnLogErrorOnFailure(ephemeralKey.Initialize(chip::Crypto::ECPKeyTarget::ECDSA));

        ReturnLogErrorOnFailure(mCredIssuerCmds.GenerateControllerNOCChain(identity.mLocalNodeId, fabricId,
                                                                            mCommissionerStorage.GetCommissionerCATs(),
                                                                            ephemeralKey, rcacSpan, icacSpan, nocSpan));

        identity.mRCACLen = rcacSpan.size();
        identity.mICACLen = icacSpan.size();
        identity.mNOCLen  = nocSpan.size();

        commissionerParams.operationalKeypair           = &ephemeralKey;
        commissionerParams.controllerRCAC               = rcacSpan;
        commissionerParams.controllerICAC               = icacSpan;
        commissionerParams.controllerNOC                = nocSpan;
        commissionerParams.permitMultiControllerFabrics = true;
        commissionerParams.enableServerInteractions     = true;
    }

    // TODO: Initialize IPK epoch key in ExampleOperationalCredentials issuer rather than relying on DefaultIpkValue
    commissionerParams.operationalCredentialsDelegate = mCredIssuerCmds.GetCredentialIssuer();
    commissionerParams.controllerVendorId             = mCommissionerVendorId.ValueOr(chip::VendorId::TestVendor1);

    ReturnLogErrorOnFailure(chip::Controller::DeviceControllerFactory::GetInstance().SetupCommissioner(commissionerParams, *(commissioner.get())));

    if (identity.mName != kIdentityNull)
    {
        // Initialize Group Data, including IPK
        chip::FabricIndex fabricIndex = commissioner->GetFabricIndex();
        uint8_t compressed_fabric_id[sizeof(uint64_t)];
        chip::MutableByteSpan compressed_fabric_id_span(compressed_fabric_id);
        ReturnLogErrorOnFailure(commissioner->GetCompressedFabricIdBytes(compressed_fabric_id_span));

        ReturnLogErrorOnFailure(chip::GroupTesting::InitData(&sGroupDataProvider, fabricIndex, compressed_fabric_id_span));

        // Configure the default IPK for all fabrics used by CHIP-tool. The epoch
        // key is the same, but the derived keys will be different for each fabric.
        chip::ByteSpan defaultIpk = chip::GroupTesting::DefaultIpkValue::GetDefaultIpk();
        ReturnLogErrorOnFailure(
            chip::Credentials::SetSingleIpkEpochKey(&sGroupDataProvider, fabricIndex, defaultIpk, compressed_fabric_id_span));
    }

    MatterManager::sICDClientStorage.UpdateFabricList(commissioner->GetFabricIndex());

    mCommissioners[identity] = std::move(commissioner);

    return CHIP_NO_ERROR;
}

/* Client Interface */

MatterNode * MatterManager::GetMatterNode(MatterNode & matterNode)
{
    auto it = std::find_if(mMatterNodes.begin(), mMatterNodes.end(),
            [&] (MatterNode & node) { return node == matterNode; });
    
    if (it != mMatterNodes.end())
    {
        return &(*it);
    }

    return nullptr;
}

MatterNode* MatterManager::GetMatterNode(chip::NodeId nodeId)
{
    auto it = std::find_if(mMatterNodes.begin(), mMatterNodes.end(),
            [&] (const MatterNode & node) { return node.GetNodeId() == nodeId; });
    
    if (it != mMatterNodes.end())
    {
        return &(*it);
    }

    return nullptr;
}

void MatterManager::AddMatterNode(MatterNode& matterNode)
{
    if (GetMatterNode(matterNode) == nullptr)
    {
        mMatterNodes.push_back(matterNode);
    }
}

MatterNode* MatterManager::GetOrAddMatterNode(MatterNode & matterNode)
{
    MatterNode* p_node = nullptr;

    p_node = GetMatterNode(matterNode);

    if (p_node == nullptr)
    {
        AddMatterNode(matterNode);
        p_node = GetMatterNode(matterNode);
    }

    return p_node;
}

CHIP_ERROR MatterManager::RemoveMatterNode(MatterNode & matterNode)
{
    auto it = std::find_if(mMatterNodes.begin(), mMatterNodes.end(),
                [&] (MatterNode & node) 
                    { 
                        return node.GetNodeId() == matterNode.GetNodeId(); 
                    }   
                );
    
    if (it != mMatterNodes.end())
    {
        MatterClientFactory::GetInstance().DeleteAllClientsForNode((*it));
        LogErrorOnFailure(mCentiControllerCache.DeleteMatterNode(&(*it)));
        mMatterNodes.erase(it);
        return CHIP_NO_ERROR;
    }

    return CHIP_ERROR_INVALID_ARGUMENT;
}

CHIP_ERROR MatterManager::RemoveMatterNode(chip::NodeId nodeId)
{
    auto it = std::find_if(mMatterNodes.begin(), mMatterNodes.end(),
                [&] (MatterNode & node) 
                    { 
                        return node.GetNodeId() == nodeId; 
                    }   );


    if (it != mMatterNodes.end())
    {
        LogErrorOnFailure(mCentiControllerCache.DeleteMatterNode(&(*it)));
        mMatterNodes.erase(it);
        return CHIP_NO_ERROR;
    }

    return CHIP_ERROR_INVALID_ARGUMENT;
}

bool MatterManager::HasMatterNode(chip::NodeId nodeId)
{
    MatterNode * matterNode = nullptr;

    matterNode = GetMatterNode(nodeId);

    return (matterNode != nullptr);
}

CHIP_ERROR MatterManager::SaveMatterNodeToCache(MatterNode & matterNode)
{
    MatterNode* p_node = nullptr;

    p_node = GetMatterNode(matterNode);
    VerifyOrReturnError(p_node != nullptr, CHIP_ERROR_INVALID_ARGUMENT);

    return mCentiControllerCache.AddMatterNode(p_node);
}

CHIP_ERROR MatterManager::SaveMatterNodeToCache(chip::NodeId nodeId)
{
    MatterNode* p_node = nullptr;

    p_node = GetMatterNode(nodeId);
    VerifyOrReturnError(p_node != nullptr, CHIP_ERROR_INVALID_ARGUMENT);

    return mCentiControllerCache.AddMatterNode(p_node);
}

CHIP_ERROR MatterManager::LoadMatterNodesFromCache(void)
{
    std::vector<MatterNode> cachedMatterNodes = {};

    ReturnLogErrorOnFailure(mCentiControllerCache.ReadAllMatterNodes(cachedMatterNodes));

    ChipLogProgress(chipTool, "MatterManager::LoadMatterNodesFromCache");

    for (MatterNode& node : cachedMatterNodes)
    {
        ChipLogProgress(chipTool, "node cached addr: %p", (void *) &node);
       AddMatterNode(node);
    }

    return CHIP_NO_ERROR;    
}
