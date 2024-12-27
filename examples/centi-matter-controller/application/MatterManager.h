#pragma once

/* CHIP Includes */
#include <app/icd/client/CheckInHandler.h>
#include <app/icd/client/DefaultCheckInDelegate.h>
#include <app/icd/client/DefaultICDClientStorage.h>

#ifdef CONFIG_USE_LOCAL_STORAGE
#include <controller/ExamplePersistentStorage.h>
#endif // CONFIG_USE_LOCAL_STORAGE

#include <controller/CHIPDeviceControllerFactory.h>
#include <controller/CommissionerDiscoveryController.h>
#include <controller/AutoCommissioner.h>

#include <credentials/GroupDataProviderImpl.h>
#include <credentials/PersistentStorageOpCertStore.h>

#include <crypto/PersistentStorageOperationalKeystore.h>
#include <crypto/RawKeySessionKeystore.h>


#include <lib/support/BytesToHex.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/ThreadOperationalDataset.h>

#if CHIP_DEVICE_LAYER_TARGET_LINUX
#include <platform/Linux/NetworkCommissioningDriver.h>
#endif // CHIP_DEVICE_LAYER_TARGET_LINUX


#include <platform/CHIPDeviceLayer.h>
#include <platform/PlatformManager.h>

/* Custom includes */
#include <map>
#include "application/CredentialsIssuer/ExampleCredentialIssuerCommands.h"

#include "MatterNode.h"
#include "MatterCache.h"

inline constexpr char kIdentityAlpha[] = "alpha";
inline constexpr char kIdentityBeta[]  = "beta";
inline constexpr char kIdentityGamma[] = "gamma";
// The null fabric commissioner is a commissioner that isn't on a fabric.
// This is a legal configuration in which the commissioner delegates
// operational communication and invocation of the commssioning complete
// command to a separate on-fabric administrator node.
//
// The null-fabric-commissioner identity is provided here to demonstrate the
// commissioner portion of such an architecture.  The null-fabric-commissioner
// can carry a commissioning flow up until the point of operational channel
// (CASE) communcation.
inline constexpr char kIdentityNull[] = "null-fabric-commissioner";

class MatterManager
{
    public:
		static MatterManager & MatterMgr(void)
		{
			static MatterManager sMatterManager;
			return sMatterManager;
		}

        CHIP_ERROR Init(void);

		std::string GetIdentity();
		CHIP_ERROR GetIdentityNodeId(std::string identity, chip::NodeId * nodeId);
		CHIP_ERROR GetIdentityRootCertificate(std::string identity, chip::ByteSpan & span);
		void SetIdentity(const char * name);

		chip::Controller::DeviceCommissioner & CurrentCommissioner();
		chip::Controller::DeviceCommissioner & GetCommissionerForFabricIndex(chip::FabricIndex fabricIndex);
		static constexpr uint16_t kMaxGroupsPerFabric    = 50;
	    static constexpr uint16_t kMaxGroupKeysPerFabric = 25;

		CHIP_ERROR InitializeMatterNode(NodeId nodeId);

		void AddMatterNode(MatterNode& matterNode);
		MatterNode* GetMatterNode(chip::NodeId nodeId);
		MatterNode * GetMatterNode(MatterNode & matterNode);
		MatterNode* GetOrAddMatterNode(MatterNode & matterNode);
		CHIP_ERROR RemoveMatterNode(MatterNode & matterNode);
		CHIP_ERROR RemoveMatterNode(chip::NodeId nodeId);
		bool HasMatterNode(chip::NodeId nodeId);
		std::vector<MatterNode> & GetActiveMatterNodes(void) { return mMatterNodes; }

    	chip::Controller::DeviceCommissioner & GetCommissioner(std::string identity);

		CHIP_ERROR SaveMatterNodeToCache(MatterNode & matterNode);
		CHIP_ERROR SaveMatterNodeToCache(chip::NodeId nodeId);

		PersistentStorage & KeyValueStorage(void) { return mDefaultStorage; }

	private:
		const chip::Optional<char *> & GetStorageDirectory() const { return mStorageDirectory; }
		chip::NodeId GetNextNodeId(void);

	protected:
		chip::Optional<char *> mStorageDirectory;
		
		#ifdef CONFIG_USE_LOCAL_STORAGE
			PersistentStorage mDefaultStorage;
			// TODO: It's pretty weird that we re-init mCommissionerStorage for every
			// identity without shutting it down or something in between...
			PersistentStorage mCommissionerStorage;
		#endif // CONFIG_USE_LOCAL_STORAGE

		// Cached trust store so commands other than the original startup command
		// can spin up commissioners as needed.
		static const chip::Credentials::AttestationTrustStore * sTrustStore;
		chip::PersistentStorageOperationalKeystore mOperationalKeystore;
		chip::Credentials::PersistentStorageOpCertStore mOpCertStore;
		static chip::Crypto::RawKeySessionKeystore sSessionKeystore;
		static chip::Credentials::GroupDataProviderImpl sGroupDataProvider;
		static chip::app::DefaultICDClientStorage sICDClientStorage;
		static chip::app::DefaultCheckInDelegate sCheckInDelegate;
		static chip::app::CheckInHandler sCheckInHandler;
    	ExampleCredentialIssuerCommands mCredIssuerCmds;

		CHIP_ERROR EnsureCommissionerForIdentity(std::string identity);

	private:
		// Commissioners are keyed by name and local node id.
		struct CommissionerIdentity
		{
			bool operator<(const CommissionerIdentity & other) const
			{
				return mName < other.mName || (mName == other.mName && mLocalNodeId < other.mLocalNodeId);
			}
			std::string mName;
			chip::NodeId mLocalNodeId;
			uint8_t mRCAC[chip::Controller::kMaxCHIPDERCertLength] = {};
			uint8_t mICAC[chip::Controller::kMaxCHIPDERCertLength] = {};
			uint8_t mNOC[chip::Controller::kMaxCHIPDERCertLength]  = {};

			size_t mRCACLen;
			size_t mICACLen;
			size_t mNOCLen;
		};

		CHIP_ERROR InitializeCommissioner(CommissionerIdentity & identity, chip::FabricId fabricId);
	    void ShutdownCommissioner(const CommissionerIdentity & key);
		//chip::Controller::AutoCommissioner mAutoCommissioner;
    	chip::FabricId CurrentCommissionerId();
		static std::map<CommissionerIdentity, std::unique_ptr<chip::Controller::DeviceCommissioner>> mCommissioners;

		chip::Optional<char *> mCommissionerName;
		chip::Optional<chip::NodeId> mCommissionerNodeId;
		chip::Optional<chip::VendorId> mCommissionerVendorId;
		chip::Optional<uint16_t> mBleAdapterId;
		chip::Optional<char *> mPaaTrustStorePath;
		chip::Optional<char *> mCDTrustStorePath;
		chip::Optional<bool> mUseMaxSizedCerts;
		chip::Optional<bool> mOnlyAllowTrustedCdKeys;

		static void PairDeviceWork(intptr_t pSetUpCode);
		MatterCache mCentiControllerCache;		

		CHIP_ERROR LoadMatterNodesFromCache(void);
	protected:
		std::vector<MatterNode> mMatterNodes;
		/* collection of matters nodes
			each node has a collection of endpoints
			each endpoint has different clusters
			each cluster has attributes defined */
};