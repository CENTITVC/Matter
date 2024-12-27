#pragma once

#include <controller/CHIPCluster.h>
#include <controller/CommissioneeDeviceProxy.h>
#include "../MatterNode.h"

class MatterClientClusterBase
{
    public:        
        MatterClientClusterBase(chip::ClusterId clusterId) { mClusterId = clusterId; }

        void SetTargetEndPoint(chip::EndpointId endpointId)
        {
            this->mPeerEndpoint = endpointId;
        }

        void SetTargetNode(chip::Messaging::ExchangeManager & exchangeManager, const chip::SessionHandle & session)
        {
            mExchangeMgr = &exchangeManager;
            mSecureSession.Grab(session);
        }

    protected:
        chip::ClusterId mClusterId;
        chip::EndpointId mPeerEndpoint;
        chip::Messaging::ExchangeManager * mExchangeMgr = nullptr;
        chip::SessionHolder mSecureSession;
};
