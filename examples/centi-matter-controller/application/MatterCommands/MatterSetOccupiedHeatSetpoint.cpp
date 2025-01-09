#include "MatterSetOccupiedHeatSetpoint.h"
#include "../MatterClient/MatterClientFactory.h"

#include <lib/support/CodeUtils.h>
#include <cstdlib>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

CHIP_ERROR MatterSetOccupiedHeatSetpoint::Run(void)
{
    CHIP_ERROR error = CHIP_NO_ERROR;

    VerifyOrExit(MatterManager::MatterMgr().HasMatterNode(mNodeId), error = CHIP_ERROR_INVALID_ARGUMENT);

    mMatterNode = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDie(mMatterNode != nullptr);

    mThermostatClient = MatterClientFactory::GetInstance().GetOrCreateThermostatClient(*mMatterNode);
    VerifyOrDie(mThermostatClient != nullptr); 

    mEndpointThermostat = mMatterNode->GetEndpointWithDeviceType(MATTER_DEVICE_ID_THERMOSTAT)->GetEndpointId();
    VerifyOrExit(mEndpointThermostat != chip::kInvalidEndpointId, error = CHIP_ERROR_INVALID_ARGUMENT);

    error = mThermostatClient->WriteOccupiedHeatingSetpoint(mOccupiedHeatSetpoint, mEndpointThermostat, this, 
                                                            OnOccupiedHeatingSetpointWriteSuccess, OnOccupiedHeatingSetpointWriteFailure, 
                                                            chip::NullOptional, nullptr, chip::NullOptional);

exit:
    if (error != CHIP_NO_ERROR)
    {
        notifyComplete(error);
    }

    return error;
}

void MatterSetOccupiedHeatSetpoint::OnSubscriptionFailed(void* context, CHIP_ERROR error)
{
    MatterSetOccupiedHeatSetpoint* command = static_cast<MatterSetOccupiedHeatSetpoint *>(context);
    VerifyOrDieWithMsg(command != nullptr, chipTool, "MatterSetOccupiedHeatSetpoint context is null!");

    command->notifyComplete(error);
}

void MatterSetOccupiedHeatSetpoint::OnOccupiedHeatingSetpointWriteSuccess(void* context)
{
    MatterSetOccupiedHeatSetpoint* command = static_cast<MatterSetOccupiedHeatSetpoint *>(context);
    VerifyOrDieWithMsg(command != nullptr, chipTool, "MatterSetOccupiedHeatSetpoint context is null!");

    command->notifyComplete(CHIP_NO_ERROR);
}

void MatterSetOccupiedHeatSetpoint::OnOccupiedHeatingSetpointWriteFailure(void* context, CHIP_ERROR error)
{
    MatterSetOccupiedHeatSetpoint* command = static_cast<MatterSetOccupiedHeatSetpoint *>(context);
    VerifyOrDieWithMsg(command != nullptr, chipTool, "MatterSetOccupiedHeatSetpoint context is null!");

    command->notifyComplete(error);
}
