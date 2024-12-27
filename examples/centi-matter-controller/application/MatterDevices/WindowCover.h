#pragma once

#include "MatterDeviceBase.h"
#include <controller/CHIPCluster.h>

class WindowCover; //Forward declaration

class WindowCoverDelegate
{
    public:
        virtual ~WindowCoverDelegate() = default;

        virtual void OnCurrentLiftPositionPercent100thsChangedHandler(WindowCover* windowCover) = 0;
};

class WindowCover : public MatterDevice
{
    public: 
        WindowCover() : MatterDevice(MATTER_DEVICE_ID_WINDOW_COVERING),
                        mCurrentPositionLiftPercent100ths(0),
                        mCurrentPositionLiftPercent100thsSubscriptionId(0),
                        mWindowCoverDelegate(nullptr) {}

        void RegisterWindowCoverDelegate(WindowCoverDelegate* windowCoverDelegate) { mWindowCoverDelegate = windowCoverDelegate; }
        WindowCoverDelegate* GetWindowCoverDelegate(void) { return mWindowCoverDelegate; }
        std::unique_ptr<MatterDevice> clone() const override { return std::make_unique<WindowCover>(*this); }

        void SetCurrentLiftPositionPercent100ths(chip::Percent100ths currentLiftPositionPercent100ths) 
        {
            if (mCurrentPositionLiftPercent100ths != currentLiftPositionPercent100ths)
            {
                mCurrentPositionLiftPercent100ths = currentLiftPositionPercent100ths; 

                if (mWindowCoverDelegate != nullptr)
                {
                    mWindowCoverDelegate->OnCurrentLiftPositionPercent100thsChangedHandler(this);
                }
            }
        }

        chip::Percent100ths GetCurrentLiftPositionPercent100ths(void) { return mCurrentPositionLiftPercent100ths; }

        bool IsWindowOpen(void) { return mCurrentPositionLiftPercent100ths != 0; }
        bool IsWindowClosed(void) { return mCurrentPositionLiftPercent100ths == 10000; }

		static void OnNewCurrentLiftPositionPercent100ths(void* context, const chip::app::DataModel::Nullable<chip::Percent100ths>& responseObject)
        {
			WindowCover* p_Device = static_cast<WindowCover*>(context);

			if (!responseObject.IsNull())
			{
				p_Device->SetCurrentLiftPositionPercent100ths(responseObject.Value());
			}
        }

        static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
        {
            ChipLogProgress(chipTool, "WindowCover subscription read failure with %s", err.Format())
        }

        static void OnCurrentPositionPercent100thsSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			WindowCover* p_device = static_cast<WindowCover*>(context);

            p_device->mCurrentPositionLiftPercent100thsSubscriptionId = subscriptionId;
        }

        chip::SubscriptionId GetSubscriptionId(void) { return mCurrentPositionLiftPercent100thsSubscriptionId; }

    private:
        chip::Percent100ths mCurrentPositionLiftPercent100ths;
        chip::SubscriptionId mCurrentPositionLiftPercent100thsSubscriptionId;
        WindowCoverDelegate* mWindowCoverDelegate = nullptr;
};
