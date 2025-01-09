#pragma once

#include <queue>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <memory>
#include <iostream>
#include <semaphore.h>

#include "../CeNTI_MQTT/centi_mqtt.h"

#include "MatterCommands/MatterCommandBase.h"
#include <platform/PlatformManager.h>

#include "MatterCommands/MatterPairing.h"
#include "MatterCommands/MatterOpenCommissioningWindow.h"
#include "MatterCommands/MatterCommandSetWindowPosition.h"
#include "MatterCommands/MatterCommandDeviceSubscription.h"
#include "MatterCommands/MatterLightSettings.h"
#include "MatterCommands/MatterSetOccupiedHeatSetpoint.h"
#include "MatterCommands/DummyCommand.h"

#include "IllianceDevices/IllianceDevice.h"
#include "IllianceDevices/MatterDevicesCallbacks.h"

#define ILLIANCE_INERGY                 (0)
#define ILLIANCE_IPOWER_SENSING_HOME    (1)
#define ILLIANCE_PROJECT_VERSION        ILLIANCE_IPOWER_SENSING_HOME

class AppTask {
public:
    AppTask() { mRunning.store(true); }

    static AppTask & Instance(void)
    {
        static AppTask sAppTask;
        return sAppTask;
    };

    // Destructor to ensure thread cleanup
    ~AppTask() {
        stopExecution();
    }

    // Method to stop the execution loop
    void stopExecution() {
        {
            std::lock_guard<std::mutex> lock(mMutexQueue);
            mRunning.store(false);
        }
        cvQueueCondition.notify_all();
    }

    CHIP_ERROR StartApp();

    void RunLoop(); 

    void AddMatterPairingCommand(std::string setUpCode);
    void AddMatterRemoveNodeCommand(chip::NodeId nodeId);
    void AddMatterCommissioningOpenCommand(chip::NodeId nodeId);
    void AddMatterCommandWindowPositionSet(chip::NodeId nodeId, uint8_t pos);
    void AddMatterDeviceSubscriptionCommand(SubscriptionParameters subParams);
    void AddMatterLightSettingCommand(chip::NodeId nodeId, LightSettings lightSettings);
    void AddMatterSetOccupiedHeatSetpointCommand(chip::NodeId nodeId, int16_t occupiedHeatSetpoint);
    void AddMatterDummyCommand(uint8_t timeout);

private:
    std::queue<std::unique_ptr<MatterCommandBase>> commandQueue;
    std::mutex mMutexQueue;
    std::condition_variable cvQueueCondition;
    std::atomic<bool> mRunning;
    std::condition_variable cvWaitingForResponse;
    std::mutex cvWaitingForResponseMutex;
    sem_t mChipShutdownWaitSemaphore;
    bool mWaitingForResponse{ true };
    
    const uint8_t MAX_WINDOW_POS_DELTA = 5;

    void LoadMqttConfig(const std::string &filename, CentiMqttClient::BrokerConfig &brokerConfig, CentiMqttClient::ClientConfig &clientConfig);
    static CHIP_ERROR MQTT__Initialize(void);
    static CHIP_ERROR Matter__Initialize(void);
    static CHIP_ERROR GetMacAddress(std::string & str);
    
    static void StopSignalHandler(int signal);
    void Shutdown();
    void AddMatterCommandToQueue(std::unique_ptr<MatterCommandBase> command);

    CHIP_ERROR OnMatterNodeInit(MatterNode& p_node);

    std::vector<Illiance::IllianceDevice> mIllianceDevices;

    static void RunOnMatterQueue(intptr_t context);

    static void HandleMatterCommandWindowPositionSet(MatterCommandSetWindowPosition* command, CHIP_ERROR error);
    static void HandleMatterCommissioningOpenResult(MatterOpenCommissioningWindow* command, CHIP_ERROR error);
    static void HandleMatterRemoveNodeResult(MatterPairing* command, CHIP_ERROR error);
    static void HandleMatterPairingResult(MatterPairing* command, CHIP_ERROR error);
    static void HandleMatterDeviceSubscriptionResult(MatterSubscriptionDevice* command, CHIP_ERROR error);
    static void HandleMatterLightSettingsResult(MatterLightSettings* command, CHIP_ERROR error);
    static void HandleMatterSetOccupiedHeatSetpointResult(MatterSetOccupiedHeatSetpoint* command, CHIP_ERROR error);
    static void HandleDummyResult (DummyCommand * command, CHIP_ERROR error);

    Illiance::AirQualitySensorHandler mAirQualitySensorHandler;
    Illiance::ContactSensorHandler mContactSensorHandler;
    Illiance::DoorLockHandler mDoorLockHandler;
    Illiance::ElectricalSensorHandler mElectricalSensorHandler;
    Illiance::ExtendedColorLightHandler mExtendedColorLightHandler;
    Illiance::HumiditySensorHandler mHumiditySensorHandler;
    Illiance::LightSensorHandler mLightSensorHandler;
    Illiance::OccupancySensorHandler mOccupancySensorHandler;
    Illiance::TemperatureSensorHandler mTemperatureSensorHandler;
    Illiance::ThermostatHandler mThermostatHandler;
    Illiance::WindowCoverHandler mWindowCoverHandler;
};
