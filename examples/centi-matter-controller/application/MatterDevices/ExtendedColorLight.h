#pragma once

#include "MatterDeviceBase.h"

class ExtendedColorLight;

class ExtendedColorLightDelegate
{
    public:
        virtual ~ExtendedColorLightDelegate() = default;
        virtual void OnOnOffStateChangedHandler(ExtendedColorLight* pExtendedColorLight) = 0;
        virtual void OnCurrentLevelChangedHandler(ExtendedColorLight* pExtendedColorLight) = 0;
        virtual void OnCurrentHueChangedHandler(ExtendedColorLight* pExtendedColorLight) = 0;
        virtual void OnCurrentSaturationChangedHandler(ExtendedColorLight* pExtendedColorLight) = 0;
        virtual void OnColorTemperatureMiredsChangedHandler(ExtendedColorLight* pExtendedColorLight) = 0;
};

class ExtendedColorLight : public MatterDevice
{
    public:
        ExtendedColorLight() :  MatterDevice(MATTER_DEVICE_ID_EXTENDED_COLOR_LIGHT),
                                mIsOn(false),
                                mOnOffSubscriptionId(0),
                                mCurrentHue(0),
                                mCurrentHueSubscriptionId(0),
                                mCurrentSaturation(0),
                                mCurrentSaturationSubscriptionId(0),
                                mExtendedColorLightDelegate(nullptr) {}

        void RegisterExtendedColorLightDelegate(ExtendedColorLightDelegate* extendedColorLightDelegate) { mExtendedColorLightDelegate = extendedColorLightDelegate; }
        ExtendedColorLightDelegate* GetExtendedColorLightDelegate(void) { return mExtendedColorLightDelegate; }
        
        std::unique_ptr<MatterDevice> clone() const override { return std::make_unique<ExtendedColorLight>(*this); }

        void SetOnOff(bool on) 
        {   
            if (mIsOn != on)
            {
                mIsOn = on;
                
                if (mExtendedColorLightDelegate != nullptr)
                {
                    mExtendedColorLightDelegate->OnOnOffStateChangedHandler(this);
                }
            }
        }

        bool IsOn(void) { return mIsOn; }

		static void OnNewOnOff(void* context, bool responseObject)
        {
			ExtendedColorLight* p_Device = static_cast<ExtendedColorLight*>(context);

            p_Device->SetOnOff(responseObject);
        }

        static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
        {
            ChipLogProgress(chipTool, "ExtendedColorLight subscription read failure with %s", err.Format())
        }

        static void OnOnOffSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			ExtendedColorLight* p_device = static_cast<ExtendedColorLight*>(context);

            p_device->mOnOffSubscriptionId = subscriptionId;
        }

        chip::SubscriptionId GetOnOffSubscriptionId(void) { return mOnOffSubscriptionId; }

        void SetCurrentLevel(uint8_t currentLevel)
        {
            if (mCurrentLevel != currentLevel)
            {
                mCurrentLevel = currentLevel;

                if (mExtendedColorLightDelegate != nullptr)
                {
                    mExtendedColorLightDelegate->OnCurrentLevelChangedHandler(this);
                }
            }
        }

        uint8_t GetCurrentLevel(void) { return mCurrentLevel; }

        static void OnNewCurrentLevel(void* context, const chip::app::DataModel::Nullable<uint8_t> & responseObject)
        {
			ExtendedColorLight* p_Device = static_cast<ExtendedColorLight*>(context);

            if (!responseObject.IsNull())
            {
                p_Device->SetCurrentLevel(responseObject.Value());
            }
        }

        static void OnCurrentLevelSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			ExtendedColorLight* p_device = static_cast<ExtendedColorLight*>(context);

            p_device->mCurrentLevelSubscriptionId = subscriptionId;
        }

        void SetCurrentHue(uint8_t currentHue)
        {
            if (mCurrentHue != currentHue)
            {
                mCurrentHue = currentHue;

                if (mExtendedColorLightDelegate != nullptr)
                {
                    mExtendedColorLightDelegate->OnCurrentHueChangedHandler(this);
                }
            }
        }
        
        uint16_t GetHue(void) 
        {
            // Matter Application Clusters 1.3 - 3.2.7.2 page 214
            return static_cast<uint16_t>((static_cast<uint32_t>(mCurrentHue) * 360) / 254);
        }

        static void OnNewCurrentHue(void* context, uint8_t currentHue)
        {
			ExtendedColorLight* p_Device = static_cast<ExtendedColorLight*>(context);

            p_Device->SetCurrentHue(currentHue);
        }

        static void OnCurrentHueSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			ExtendedColorLight* p_device = static_cast<ExtendedColorLight*>(context);

            p_device->mCurrentHueSubscriptionId = subscriptionId;
        }

        void SetCurrentSaturation(uint8_t currentSaturation)
        {
            if (mCurrentSaturation != currentSaturation)
            {
                mCurrentSaturation = currentSaturation;

                if (mExtendedColorLightDelegate != nullptr)
                {
                    mExtendedColorLightDelegate->OnCurrentSaturationChangedHandler(this);
                }
            }
        }
        
        uint8_t GetSaturation(void) 
        {
            // Matter Application Clusters 1.3 - 3.2.7.3 page 214
            return static_cast<uint8_t>((static_cast<uint16_t>(mCurrentSaturation) * 100) / 254);
        }

        static void OnNewCurrentSaturation(void* context, uint8_t currentSaturation)
        {
			ExtendedColorLight* p_Device = static_cast<ExtendedColorLight*>(context);

            p_Device->SetCurrentSaturation(currentSaturation);
        }

        static void OnCurrentSaturationSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			ExtendedColorLight* p_device = static_cast<ExtendedColorLight*>(context);

            p_device->mCurrentSaturationSubscriptionId = subscriptionId;
        }

        void SetColorTemperatureMireds(uint16_t colorTemperatureMireds)
        {
            if (mColorTemperatureMireds != colorTemperatureMireds)
            {
                mColorTemperatureMireds = colorTemperatureMireds;

                if (mExtendedColorLightDelegate != nullptr)
                {
                    mExtendedColorLightDelegate->OnColorTemperatureMiredsChangedHandler(this);
                }
            }
        }
        
        uint16_t GetColorTemperatureKelvin(void) 
        {        
            return static_cast<uint16_t>(static_cast<uint32_t>(1000000) / static_cast<uint32_t>(mColorTemperatureMireds));
        }

        uint16_t GetColorTemperatureMireds(void) { return mColorTemperatureMireds; }


        static void OnNewColorTemperatureMireds(void* context, uint16_t colorTemperatureMireds)
        {
			ExtendedColorLight* p_Device = static_cast<ExtendedColorLight*>(context);

            p_Device->SetColorTemperatureMireds(colorTemperatureMireds);
        }

        static void OnColorTemperatureMiredsSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			ExtendedColorLight* p_device = static_cast<ExtendedColorLight*>(context);

            p_device->mColorTemperatureMiredsSubscriptionId = subscriptionId;
        }

    private:
        bool mIsOn;
        chip::SubscriptionId mOnOffSubscriptionId;

        uint8_t mCurrentLevel;
        chip::SubscriptionId mCurrentLevelSubscriptionId;

        uint8_t mCurrentHue;
        chip::SubscriptionId mCurrentHueSubscriptionId;

        uint8_t mCurrentSaturation;
        chip::SubscriptionId mCurrentSaturationSubscriptionId;

        uint16_t mColorTemperatureMireds;
        chip::SubscriptionId mColorTemperatureMiredsSubscriptionId;

        ExtendedColorLightDelegate* mExtendedColorLightDelegate = nullptr;
};
