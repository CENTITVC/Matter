#include "MatterCommandSetWindowPosition.h"
#include "../MatterClient/MatterClientFactory.h"

#include <lib/support/CodeUtils.h>
#include <cstdlib>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

CHIP_ERROR MatterCommandSetWindowPosition::Run(void)
{
    CHIP_ERROR error = CHIP_NO_ERROR;

    VerifyOrExit(MatterManager::MatterMgr().HasMatterNode(mNodeId), error = CHIP_ERROR_INVALID_ARGUMENT);

    mCurrentMatterNode = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDie(mCurrentMatterNode != nullptr);

    mWindowClient = MatterClientFactory::GetInstance().GetOrCreateWindowCoveringClient(*mCurrentMatterNode);
    VerifyOrDie(mWindowClient != nullptr); 

    mCurrentEndpoint = mCurrentMatterNode->GetEndpointWithCluster(WindowCovering::Id);
    VerifyOrExit(mCurrentEndpoint != nullptr, error = CHIP_ERROR_INVALID_ARGUMENT);

    error = mWindowClient->CommandGoToLiftPercentage(
                static_cast<chip::Percent100ths>(mSetPos) * 100,
                mCurrentEndpoint->GetEndpointId(), 
                this, 
                OnCommandGoToLiftPercentageSuccess,
                OnCommandGoToLiftPercentageFailure);

exit:
    if (error != CHIP_NO_ERROR)
    {
        notifyComplete(error);
    }

    return error;
}

void MatterCommandSetWindowPosition::OnCommandGoToLiftPercentageSuccess(void* context, const DataModel::NullObjectType& responseObject)
{
    MatterCommandSetWindowPosition* command = static_cast<MatterCommandSetWindowPosition *>(context);
    VerifyOrDieWithMsg(command != nullptr, chipTool, "MatterCommandSetWindowPosition context is null!");
    command->notifyComplete(CHIP_NO_ERROR);
}

void MatterCommandSetWindowPosition::OnCommandGoToLiftPercentageFailure(void* context, CHIP_ERROR error)
{
    MatterCommandSetWindowPosition* command = static_cast<MatterCommandSetWindowPosition *>(context);
    VerifyOrDieWithMsg(command != nullptr, chipTool, "MatterCommandSetWindowPosition context is null!");
    command->notifyComplete(error);
}
