#pragma once

#include "MatterCommandBase.h"

class DummyCommand : public MatterCommandBase
{
    public:
        DummyCommand(std::function<void(DummyCommand *, CHIP_ERROR)> completeCallback, uint8_t timeoutSec)
            : timeout_seconds(timeoutSec), sOnCompleteCallback(completeCallback) {}
        CHIP_ERROR Run()
        {
            ChipLogProgress(AppServer, "DummyCommand running...");
            return CHIP_NO_ERROR;
        }

        chip::System::Clock::Timeout GetWaitDuration() const override { return chip::System::Clock::Seconds16(timeout_seconds); }
        
        void callback() override
        {
            ChipLogProgress(AppServer, "DummyCommand callback...");
            if (sOnCompleteCallback != nullptr)
            {
                ChipLogProgress(AppServer, "DummyCommand callback exec...");
                sOnCompleteCallback(this, this->GetError());
            }
        }
        uint8_t timeout_seconds;

        private:
            std::function<void(DummyCommand *, CHIP_ERROR)> sOnCompleteCallback;
       
};
