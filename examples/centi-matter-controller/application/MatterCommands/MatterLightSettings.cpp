#include "MatterLightSettings.h"
#include "../MatterClient/MatterClientFactory.h"

#include <lib/support/CodeUtils.h>
#include <cstdlib>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;


CHIP_ERROR MatterLightSettings::Run(void)
{
    CHIP_ERROR error = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "MatterLightSettings::Run");

    VerifyOrExit(MatterManager::MatterMgr().HasMatterNode(mNodeId), error = CHIP_ERROR_INVALID_ARGUMENT);

    mMatterNode = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDie(mMatterNode != nullptr);

    mColorControlClient = MatterClientFactory::GetInstance().GetOrCreateColorControlClient(*mMatterNode);
    VerifyOrDie(mColorControlClient != nullptr); 

    mLevelControlClient = MatterClientFactory::GetInstance().GetOrCreateLevelControlClient(*mMatterNode);
    VerifyOrDie(mLevelControlClient != nullptr); 

    mOnOffClient = MatterClientFactory::GetInstance().GetOrCreateOnOffClient(*mMatterNode);
    VerifyOrDie(mOnOffClient != nullptr); 

    mEndpointExtendedColorLight = mMatterNode->GetEndpointWithDeviceType(MATTER_DEVICE_ID_EXTENDED_COLOR_LIGHT)->GetEndpointId();

    if (mLightSettings.commandMask.Has(LightCommandBitMap::kColor))
    {
        error = ColorSetting();
        VerifyOrExit(error == CHIP_NO_ERROR, error = error);
    }

    if (mLightSettings.commandMask.Has(LightCommandBitMap::kColorTemperature))
    {
        error = ColorTemperatureSetting();
        VerifyOrExit(error == CHIP_NO_ERROR, error = error);
    }

    if (mLightSettings.commandMask.Has(LightCommandBitMap::kAction))
    {
        error = ActionSetting();
        VerifyOrExit(error == CHIP_NO_ERROR, error = error);
    }

exit:
    if (error != CHIP_NO_ERROR)
    {
        notifyComplete(error);
    }

    return error;

}

CHIP_ERROR MatterLightSettings::ColorSetting(void)
{
    CHIP_ERROR error = CHIP_NO_ERROR;
    uint8_t hue = 0, saturation = 0, value = 0 ;
    DataModel::Nullable<uint16_t> transitionTime = DataModel::MakeNullable((uint16_t)0);
    BitMask<LevelControl::OptionsBitmap> optionsMask = BitMask<LevelControl::OptionsBitmap>();
    BitMask<LevelControl::OptionsBitmap> optionsOverride = BitMask<LevelControl::OptionsBitmap>();

    hue = static_cast<uint8_t>((static_cast<uint32_t>(mLightSettings.hue) * 254) / 360);
    saturation = static_cast<uint8_t>((static_cast<uint16_t>(mLightSettings.saturation) * 254) / 100);

    error = mColorControlClient->CommandMoveToHueAndSaturation(
                    hue, saturation, 0, 0, 0,
                    mEndpointExtendedColorLight, this, OnCommandMoveToHueAndSaturationSuccess, OnCommandFailure);
    VerifyOrReturnLogError(error == CHIP_NO_ERROR, error);

    ChipLogProgress(chipTool, "MatterLightSettings::ColorSetting");

    value = static_cast<uint8_t>((static_cast<uint32_t>(mLightSettings.value) * 254 / 100));
    ChipLogProgress(chipTool, "HSV: %u - %u - %u ", hue, saturation, value);

    return mLevelControlClient->CommandMoveToLevel(
                    value,
                    transitionTime,
                    optionsMask,
                    optionsOverride,
                    mEndpointExtendedColorLight,
                    this,
                    OnCommandMoveToLevelSuccess,
                    OnCommandFailure);
}

CHIP_ERROR MatterLightSettings::ColorTemperatureSetting(void)
{
    mColorControlClient = MatterClientFactory::GetInstance().GetOrCreateColorControlClient(*mMatterNode);
    VerifyOrDie(mColorControlClient != nullptr); 

    return mColorControlClient->CommandMoveToColorTemperature(
                    mLightSettings.colorTemperatureKelvin,
                    static_cast<uint16_t>(0),
                    static_cast<uint8_t>(0),
                    static_cast<uint8_t>(0),
                    mEndpointExtendedColorLight,
                    this,
                    OnCommandMoveToLevelSuccess,
                    OnCommandFailure);
}

CHIP_ERROR MatterLightSettings::ActionSetting(void)
{
    if (mLightSettings.onoff)
    {
        return mOnOffClient->CommandOn(mEndpointExtendedColorLight, 
                                        this, 
                                        OnCommandOnOffSuccess,
                                        OnCommandFailure);
    }
    else
    {
        return mOnOffClient->CommandOff(mEndpointExtendedColorLight, 
                                        this, 
                                        OnCommandOnOffSuccess, 
                                        OnCommandFailure);
    }
}

void MatterLightSettings::OnCommandMoveToHueAndSaturationSuccess(void* context, const DataModel::NullObjectType& response)
{
    ChipLogProgress(chipTool, "MatterLightSettings::OnCommandMoveToHueAndSaturationSuccess");
}

void MatterLightSettings::OnCommandMoveToLevelSuccess(void* context, const DataModel::NullObjectType& response)
{
    ChipLogProgress(chipTool, "MatterLightSettings::OnCommandMoveToLevelSuccess");
    MatterLightSettings* command = static_cast<MatterLightSettings *>(context);
    VerifyOrDieWithMsg(command != nullptr, chipTool, "MatterLightSettings context is null!");

    command->mLightSettings.commandMask.Clear(LightCommandBitMap::kColor);

    if (!command->mLightSettings.commandMask.HasAny())
    {
        command->notifyComplete(CHIP_NO_ERROR);
    }
}

void MatterLightSettings::OnCommandMoveToColorTemperatureSuccess(void* context, const chip::app::DataModel::NullObjectType& response)
{
    MatterLightSettings* command = static_cast<MatterLightSettings *>(context);
    VerifyOrDieWithMsg(command != nullptr, chipTool, "MatterLightSettings context is null!");

    command->mLightSettings.commandMask.Clear(LightCommandBitMap::kColorTemperature);

    if (!command->mLightSettings.commandMask.HasAny())
    {
        command->notifyComplete(CHIP_NO_ERROR);
    }
}

void MatterLightSettings::OnCommandOnOffSuccess(void* context, const chip::app::DataModel::NullObjectType& response)
{
    MatterLightSettings* command = static_cast<MatterLightSettings *>(context);
    VerifyOrDieWithMsg(command != nullptr, chipTool, "MatterLightSettings context is null!");

    command->mLightSettings.commandMask.Clear(LightCommandBitMap::kAction);

    if (!command->mLightSettings.commandMask.HasAny())
    {
        command->notifyComplete(CHIP_NO_ERROR);
    }
}


void MatterLightSettings::OnCommandFailure(void* context, CHIP_ERROR error)
{
    MatterLightSettings* command = static_cast<MatterLightSettings *>(context);
    VerifyOrDieWithMsg(command != nullptr, chipTool, "MatterLightSettings context is null!");
    command->notifyComplete(error);
}
