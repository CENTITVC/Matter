#pragma once

#include "MatterCommandBase.h"
#include "../MatterManager.h"
#include "../MatterDevices/MatterDevicesCommon.h"
#include "../MatterClient/MatterClientFactory.h"

enum class LightCommandBitMap
{
    kColor = 0x01,
    kColorTemperature = 0x02,
    kAction = 0x04
};

struct LightSettings
{
    uint16_t hue;
    uint8_t saturation;
    uint8_t value;
    uint16_t colorTemperatureKelvin;
    bool onoff;
    chip::BitMask<LightCommandBitMap> commandMask;

    void PrintInfo(void)
    {
        if (commandMask.Has(LightCommandBitMap::kColor))
        {
            ChipLogProgress(chipTool, "HSV: %u - %u - %u ", hue, saturation, value);
        }

        if (commandMask.Has(LightCommandBitMap::kColorTemperature))
        {
            ChipLogProgress(chipTool, "ColorTemperature %u K", colorTemperatureKelvin);
        }

        if (commandMask.Has(LightCommandBitMap::kAction))
        {
            ChipLogProgress(chipTool, "Action: %s", (onoff) ? "on" : "off");
        }
    }
};

class MatterLightSettings : public MatterCommandBase
{
    public:
        MatterLightSettings(std::function<void(MatterLightSettings *, CHIP_ERROR)> completeCallback, 
                                chip::NodeId nodeId,
                                LightSettings lightSettings) : 
                                sOnCompleteCallback(completeCallback), 
                                mNodeId(nodeId),
                                mMatterNode(nullptr),
                                mMatterDevice(nullptr),
                                mColorControlClient(nullptr),
                                mLevelControlClient(nullptr),
                                mOnOffClient(nullptr),
                                mLightSettings(lightSettings) {}

        CHIP_ERROR Run() override;
        chip::System::Clock::Timeout GetWaitDuration() const override { return chip::System::Clock::Seconds16(600); }

        void callback() override
        {
            if (sOnCompleteCallback != nullptr)
            {
                sOnCompleteCallback(this, this->GetError());
            }
        }

        chip::NodeId GetNodeId(void) const { return mNodeId; }
        
    private:
        std::function<void(MatterLightSettings *, CHIP_ERROR)> sOnCompleteCallback;
        chip::NodeId mNodeId;
        MatterNode* mMatterNode;
        MatterDevice* mMatterDevice;
        chip::EndpointId mEndpointExtendedColorLight;
        ColorControlClient* mColorControlClient;
        LevelControlClient* mLevelControlClient;
        OnOffClient* mOnOffClient;
        LightSettings mLightSettings;
        const uint16_t mMinIntervalFloorSeconds = 1;
        const uint16_t mMaxIntervalCeilingSeconds = 5;

        static void OnSubscriptionFailed(void* context, CHIP_ERROR error);

        static void OnCommandMoveToHueAndSaturationSuccess(void* context, const chip::app::DataModel::NullObjectType& response);
        static void OnCommandMoveToLevelSuccess(void* context, const chip::app::DataModel::NullObjectType& response);
        static void OnCommandMoveToColorTemperatureSuccess(void* context, const chip::app::DataModel::NullObjectType& response);
        static void OnCommandOnOffSuccess(void* context, const chip::app::DataModel::NullObjectType& response);
        static void OnCommandFailure(void* context, CHIP_ERROR error);

        CHIP_ERROR ColorSetting(void);
        CHIP_ERROR ColorTemperatureSetting(void);
        CHIP_ERROR ActionSetting(void);
};