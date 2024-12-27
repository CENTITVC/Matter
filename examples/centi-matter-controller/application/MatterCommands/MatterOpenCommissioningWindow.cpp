#include "MatterOpenCommissioningWindow.h"

#include "../MatterManager.h"

CHIP_ERROR MatterOpenCommissioningWindow::Run(void)
{
    MatterNode* matterNode = nullptr;
    chip::SetupDiscriminator discriminator;
    chip::SetupPayload ignored;

    ChipLogProgress(chipTool, "MatterOpenCommissioningWindow::Run");
    
    if (! MatterManager::MatterMgr().HasMatterNode(mNodeId))
    {
        ChipLogProgress(chipTool, "Matter node with ID " ChipLogFormatX64 " doesn't exist", ChipLogValueX64(mNodeId));
        return CHIP_ERROR_INVALID_ARGUMENT;    
    }

    matterNode = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDieWithMsg(matterNode != nullptr, chipTool, "OpenCommissioningWindow MatterNode doesn't exist");

  //  VerifyOrDie(MatterManager::MatterMgr().CurrentCommissionerId() == matterNode->GetFabricIndex());
    mWindowOpener = chip::Platform::MakeUnique<chip::Controller::CommissioningWindowOpener>(&MatterManager::MatterMgr().GetCommissionerForFabricIndex(matterNode->GetFabricIndex()));

    discriminator = matterNode->GetDiscriminator();

    if (discriminator.IsShortDiscriminator())
    {
        mDiscriminator = static_cast<uint16_t>(discriminator.GetShortValue());
    }
    else
    {
        mDiscriminator = discriminator.GetLongValue();
    }

    return mWindowOpener->OpenCommissioningWindow(mNodeId, chip::System::Clock::Seconds16(mCommissioningWindowTimeout), mIteration,
                                                    mDiscriminator, chip::NullOptional, chip::NullOptional,
                                                    &mOnOpenCommissioningWindowCallback, ignored,
                                                    /* readVIDPIDAttributes */ true);
}

void MatterOpenCommissioningWindow::OnOpenCommissioningWindowResponse(void * context, NodeId remoteId, CHIP_ERROR err,
                                                                       chip::SetupPayload payload)
{
    LogErrorOnFailure(err);

    MatterOpenCommissioningWindow * command = reinterpret_cast<MatterOpenCommissioningWindow *>(context);
    VerifyOrReturn(command != nullptr, ChipLogError(chipTool, "MatterOpenCommissioningWindow: context is null"));
    
    if (err == CHIP_NO_ERROR)
    {
        CHIP_ERROR parsing_err = CHIP_NO_ERROR;

        command->mSetupPayload = payload;

        char payloadBuffer[chip::QRCodeBasicSetupPayloadGenerator::kMaxQRCodeBase38RepresentationLength + 1];
        chip::MutableCharSpan manualCode(payloadBuffer);  
        parsing_err = chip::ManualSetupPayloadGenerator(command->mSetupPayload).payloadDecimalStringRepresentation(manualCode);

        if (parsing_err == CHIP_NO_ERROR)
        {
            command->mManualCode = payloadBuffer;
        }

        chip::MutableCharSpan QRCode(payloadBuffer);
        parsing_err = chip::QRCodeBasicSetupPayloadGenerator(command->mSetupPayload).payloadBase38Representation(QRCode);

        if (parsing_err == CHIP_NO_ERROR)
        {
            command->mQRCode = payloadBuffer;
        }
    }

    command->notifyComplete(err);
}
