#pragma once

#include <lib/support/CodeUtils.h>
#include <system/SystemClock.h>

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <iostream>

class MatterCommandBase
{   
    public:
        MatterCommandBase() {}
        virtual ~MatterCommandBase() = default;

        virtual CHIP_ERROR Run() = 0;
        // Get the wait duration, in seconds, before the command times out.
        virtual chip::System::Clock::Timeout GetWaitDuration() const = 0;

        virtual void callback() = 0;

        void WaitForCompletionOrTimeout() 
        {
            std::unique_lock<std::mutex> lock(mMutex);
            auto timeout = std::chrono::system_clock::now() + std::chrono::duration_cast<std::chrono::seconds>(this->GetWaitDuration());

            if (!mCondition.wait_until(lock, timeout, [this]() { return this->mIsComplete; })) 
            {
                std::cout << "Condition timeout" << std::endl;
                notifyComplete(CHIP_ERROR_TIMEOUT);
            }

            callback();
        }

    protected:

        void notifyComplete(CHIP_ERROR error)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mCommandError = error;
            mIsComplete = true;
            mCondition.notify_all();
        }

        CHIP_ERROR GetError(void)
        {
            return mCommandError;
        }
    private:
        CHIP_ERROR mCommandError;
        std::mutex mMutex;
        std::condition_variable mCondition;
        bool mIsComplete = false;
};
