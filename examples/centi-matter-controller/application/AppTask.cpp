#include "AppTask.h"
#include <../application/MatterManager.h>

#include <lib/support/BytesToHex.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/ThreadOperationalDataset.h>
#include <platform/ConfigurationManager.h>
#include "MatterDevices/MatterDeviceTypes.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <signal.h>
#include <fstream>

#define MQTT_BROKER_HOST_HIVE           (0)
#define MQTT_BROKER_HOST_ILLIANCE       (1)
#define MQTT_BROKER_HOST                MQTT_BROKER_HOST_ILLIANCE

CHIP_ERROR AppTask::StartApp(void)
{
    CHIP_ERROR error = CHIP_NO_ERROR;

    error = MQTT__Initialize();
    ReturnErrorOnFailure(error);

    error = Matter__Initialize();
    ReturnErrorOnFailure(error);

//    signal(SIGINT, StopSignalHandler);
//    signal(SIGTERM, StopSignalHandler);

    RunLoop();

    Shutdown();
    
    return CHIP_NO_ERROR;
}

void AppTask::AddMatterPairingCommand(std::string setUpCode)
{
    AddMatterCommandToQueue(std::make_unique<MatterPairing>(HandleMatterPairingResult, setUpCode));
}

void AppTask::AddMatterRemoveNodeCommand(chip::NodeId nodeId)
{
    AddMatterCommandToQueue(std::make_unique<MatterPairing>(HandleMatterRemoveNodeResult, nodeId));
}

void AppTask::AddMatterCommissioningOpenCommand(chip::NodeId nodeId)
{
    AddMatterCommandToQueue(std::make_unique<MatterOpenCommissioningWindow>(HandleMatterCommissioningOpenResult, nodeId));
}

void AppTask::AddMatterCommandWindowPositionSet(chip::NodeId nodeId, uint8_t pos)
{
    AddMatterCommandToQueue(std::make_unique<MatterCommandSetWindowPosition>(HandleMatterCommandWindowPositionSet, nodeId, pos));
}

void AppTask::AddMatterDeviceSubscriptionCommand(SubscriptionParameters subParams)
{
    subParams.PrintInfo();
    AddMatterCommandToQueue(std::make_unique<MatterSubscriptionDevice>(HandleMatterDeviceSubscriptionResult, subParams));
}

void AppTask::AddMatterLightSettingCommand(chip::NodeId nodeId, LightSettings lightSettings)
{
    AddMatterCommandToQueue(std::make_unique<MatterLightSettings>(HandleMatterLightSettingsResult, nodeId, lightSettings));
}

void AppTask::AddMatterDummyCommand(uint8_t timeout)
{
    AddMatterCommandToQueue(std::make_unique<DummyCommand>(HandleDummyResult, timeout));
}

void AppTask::HandleMatterCommandWindowPositionSet(MatterCommandSetWindowPosition* command, CHIP_ERROR error)
{
    ChipLogProgress(chipTool, "HandleMatterCommandWindowPositionSet");

    (void) CentiMqttClient::ClientMgr().Publish_MatterWindowPositionControlResponseAck(
                                                    command->GetNodeId(), 
                                                    error, 
                                                    command->GetTargetPosition());
}

void AppTask::HandleMatterCommissioningOpenResult(MatterOpenCommissioningWindow* command, CHIP_ERROR error)
{
    ChipLogProgress(chipTool, "HandleMatterCommissioningOpenResult");

    (void) CentiMqttClient::ClientMgr().Publish_MatterCommissioningOpenResponse(
                                    command->GetNodeId(), 
                                    error, 
                                    command->GetManualCode(), 
                                    command->GetQRCode());
}

void AppTask::HandleMatterRemoveNodeResult(MatterPairing* command, CHIP_ERROR error)
{
    ChipLogProgress(chipTool, "HandleMatterRemoveNodeResult");

    (void) CentiMqttClient::ClientMgr().Publish_MatterRemoveNodeResponse(command->GetNodeId(), error);
}

void AppTask::HandleMatterPairingResult(MatterPairing* command, CHIP_ERROR error)
{
    ChipLogProgress(chipTool, "HandleMatterPairingResult");

    (void) CentiMqttClient::ClientMgr().Publish_MatterCommissioningResponse(command->GetSetUpCode(), error);

    MatterNode * p_node = MatterManager::MatterMgr().GetMatterNode(command->GetNodeId());
    
    if (p_node == nullptr)
    {
        ChipLogProgress(chipTool, "MatterNode in HandleMatterPairingResult is null");
        return;
    }

    (void) AppTask::Instance().OnMatterNodeInit(*p_node);
}

void AppTask::HandleMatterDeviceSubscriptionResult(MatterSubscriptionDevice* command, CHIP_ERROR error)
{
    ChipLogProgress(chipTool, "HandleMatterDeviceSubscriptionResult");

    (void) CentiMqttClient::ClientMgr().Publish_MatterDeviceSubscription(
                command->GetNodeId(),
                command->GetEndpointId(),
                command->GetMatterDevice()->GetType(),
                error);
}

void AppTask::HandleMatterLightSettingsResult(MatterLightSettings* command, CHIP_ERROR error)
{
    ChipLogProgress(chipTool, "HandleMatterLightSettingsResult");
    
    (void) CentiMqttClient::ClientMgr().Publish_LightSettingsAck(command->GetNodeId(), error);
}


void AppTask::HandleDummyResult (DummyCommand * command, CHIP_ERROR error)
{
    ChipLogProgress(NotSpecified, "DummyCommandResult with timeout: %d", command->timeout_seconds);
    (void) command;
    (void) error;
}

CHIP_ERROR AppTask::Matter__Initialize(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    err = MatterManager::MatterMgr().Init();
    ReturnErrorOnFailure(err);

    /* After Matter initialization, Matter nodes are loaded from cache. 
        Signal this to MQTT broker that devices are available again */
    for (MatterNode& node : MatterManager::MatterMgr().GetActiveMatterNodes())
    {
        AppTask::Instance().OnMatterNodeInit(node);
    }

    err = DeviceLayer::PlatformMgr().StartEventLoopTask();
    ReturnErrorOnFailure(err);

    return CHIP_NO_ERROR;
}

CHIP_ERROR AppTask::OnMatterNodeInit(MatterNode& p_node)
{
    int mqtt_err = 0;
    SubscriptionParameters subParams;

    ChipLogProgress(NotSpecified, "OnMatterNodeInit");

    subParams.nodeId = p_node.GetNodeId();

    if (Illiance::IsIllianceSensors(p_node))
    {
        for (auto & endpoint : p_node.GetEndpoints())
        {
            subParams.endpointId = endpoint.GetEndpointId();

            for (auto & device : endpoint.GetMatterDevices())
            {
                subParams.deviceTypeId = device->GetType();

                switch(device->GetType())
                {
                    case MATTER_DEVICE_ID_AIR_QUALITY_SENSOR:
                    {
                        subParams.minIntervalFloorSeconds = 5;
                        subParams.maxIntervalCeilingSeconds = 30;
                        ChipLogProgress(chipTool, "MATTER_DEVICE_ID_AIR_QUALITY_SENSOR");

                        AirQualitySensor* pAirQualitySensor = nullptr;
                        pAirQualitySensor = static_cast<AirQualitySensor*>(endpoint.GetMatterDevice(MATTER_DEVICE_ID_AIR_QUALITY_SENSOR));
                        pAirQualitySensor->RegisterAirQualitySensorDelegate(&mAirQualitySensorHandler);

                        AddMatterDeviceSubscriptionCommand(subParams);
                        break;
                    }

                    case MATTER_DEVICE_ID_LIGHT_SENSOR:
                    {
                        subParams.minIntervalFloorSeconds = 5;
                        subParams.maxIntervalCeilingSeconds = 30;

                        ChipLogProgress(chipTool, "MATTER_DEVICE_ID_LIGHT_SENSOR");

                        LightSensor* pLightSensor = nullptr;
                        pLightSensor = static_cast<LightSensor*>(endpoint.GetMatterDevice(MATTER_DEVICE_ID_LIGHT_SENSOR));
                        pLightSensor->RegisterLightSensorDelegate(static_cast<LightSensorDelegate*>(&mLightSensorHandler));

                        AddMatterDeviceSubscriptionCommand(subParams);
                        break;
                    }

                    case MATTER_DEVICE_ID_ELECTRICAL_SENSOR:
                    {
                        subParams.minIntervalFloorSeconds = 5;
                        subParams.maxIntervalCeilingSeconds = 30;

                        ChipLogProgress(chipTool, "MATTER_DEVICE_ID_ELECTRICAL_SENSOR");
                        ElectricalSensor* pElectricalSensor = nullptr;
                        pElectricalSensor = static_cast<ElectricalSensor*>(endpoint.GetMatterDevice(MATTER_DEVICE_ID_ELECTRICAL_SENSOR));
                        pElectricalSensor->RegisterElectricalSensorDelegate(&mElectricalSensorHandler);
                        
                        AddMatterDeviceSubscriptionCommand(subParams);
                        break;
                    }
                    
                    default:
                        ChipLogProgress(NotSpecified, "Unknown deviceType=" ChipLogFormatMEI, ChipLogValueMEI(device->GetType()));
                        break;
                }
            }
        }

        mqtt_err = CentiMqttClient::ClientMgr().Publish_SensorsInit(p_node.GetNodeId());
    }
    else if (Illiance::IsIllianceWindow(p_node))
    {
        subParams.endpointId = p_node.GetEndpointWithDeviceType(MATTER_DEVICE_ID_WINDOW_COVERING)->GetEndpointId();
        subParams.deviceTypeId = MATTER_DEVICE_ID_WINDOW_COVERING;

        WindowCover* pWindowCover = nullptr;
        pWindowCover = static_cast<WindowCover*>(p_node.GetEndpointWithDeviceType(MATTER_DEVICE_ID_WINDOW_COVERING)->GetMatterDevice(MATTER_DEVICE_ID_WINDOW_COVERING));
        VerifyOrDieWithMsg(pWindowCover != nullptr, chipTool, "pWindowCover is null!");
        pWindowCover->RegisterWindowCoverDelegate(&mWindowCoverHandler);

        subParams.minIntervalFloorSeconds = 5;
        subParams.maxIntervalCeilingSeconds = 30;

        AddMatterDeviceSubscriptionCommand(subParams);

        mqtt_err = CentiMqttClient::ClientMgr().Publish_WindowInit(p_node.GetNodeId());
    }
    else if (Illiance::IsIllianceOccupancySensor(p_node))
    {
        subParams.endpointId = p_node.GetEndpointWithDeviceType(MATTER_DEVICE_ID_OCCUPANCY_SENSOR)->GetEndpointId();
        subParams.deviceTypeId = MATTER_DEVICE_ID_OCCUPANCY_SENSOR;
        subParams.minIntervalFloorSeconds = 1;
        subParams.maxIntervalCeilingSeconds = 5;

        OccupancySensor* pOccupancySensor = nullptr;
        pOccupancySensor = static_cast<OccupancySensor*>(p_node.GetEndpointWithDeviceType(MATTER_DEVICE_ID_OCCUPANCY_SENSOR)->GetMatterDevice(MATTER_DEVICE_ID_OCCUPANCY_SENSOR));
        pOccupancySensor->RegisterOccupancySensorDelegate(&mOccupancySensorHandler);

        AddMatterDeviceSubscriptionCommand(subParams);

        mqtt_err = CentiMqttClient::ClientMgr().Publish_OccupancySensorInit(p_node.GetNodeId());
    }
    else if (Illiance::IsIllianceDoorLock(p_node))
    {
        subParams.endpointId = p_node.GetEndpointWithDeviceType(MATTER_DEVICE_ID_DOOR_LOCK)->GetEndpointId();
        subParams.deviceTypeId = MATTER_DEVICE_ID_DOOR_LOCK;
        subParams.minIntervalFloorSeconds = 1;
        subParams.maxIntervalCeilingSeconds = 5;

        DoorLock* pDoorLock = nullptr;
        pDoorLock = static_cast<DoorLock*>(p_node.GetEndpointWithDeviceType(MATTER_DEVICE_ID_DOOR_LOCK)->GetMatterDevice(MATTER_DEVICE_ID_DOOR_LOCK));
        pDoorLock->RegisterDoorLockDelegate(&mDoorLockHandler);

        AddMatterDeviceSubscriptionCommand(subParams);

        mqtt_err = CentiMqttClient::ClientMgr().Publish_DoorLockInit(p_node.GetNodeId());
    }
    else if (Illiance::IsIllianceContactSensor(p_node))
    {
        subParams.endpointId = p_node.GetEndpointWithDeviceType(MATTER_DEVICE_ID_CONTACT_SENSOR)->GetEndpointId();
        subParams.deviceTypeId = MATTER_DEVICE_ID_CONTACT_SENSOR;
        subParams.minIntervalFloorSeconds = 1;
        subParams.maxIntervalCeilingSeconds = 5;

        ContactSensor* pContactSensor = nullptr;
        pContactSensor = static_cast<ContactSensor*>(p_node.GetEndpointWithDeviceType(MATTER_DEVICE_ID_CONTACT_SENSOR)->GetMatterDevice(MATTER_DEVICE_ID_CONTACT_SENSOR));
        pContactSensor->RegisterContactSensorDelegate(&mContactSensorHandler);

        AddMatterDeviceSubscriptionCommand(subParams);

        mqtt_err = CentiMqttClient::ClientMgr().Publish_DoorLockInit(p_node.GetNodeId());
    }
    else if (Illiance::IsIllianceLight(p_node))
    {
        subParams.endpointId = p_node.GetEndpointWithDeviceType(MATTER_DEVICE_ID_EXTENDED_COLOR_LIGHT)->GetEndpointId();
        subParams.deviceTypeId = MATTER_DEVICE_ID_EXTENDED_COLOR_LIGHT;
        subParams.minIntervalFloorSeconds = 0;
        subParams.maxIntervalCeilingSeconds = 5;

        ExtendedColorLight* pExtendedColorLight = nullptr;
        pExtendedColorLight = static_cast<ExtendedColorLight*>(p_node.GetEndpointWithDeviceType(MATTER_DEVICE_ID_EXTENDED_COLOR_LIGHT)->GetMatterDevice(MATTER_DEVICE_ID_EXTENDED_COLOR_LIGHT));
        pExtendedColorLight->RegisterExtendedColorLightDelegate(&mExtendedColorLightHandler);

        AddMatterDeviceSubscriptionCommand(subParams);

        mqtt_err = CentiMqttClient::ClientMgr().Publish_LightInit(p_node.GetNodeId());
    }
    else
    {
        ChipLogProgress(NotSpecified, "Unknown Illiance Device");
    }

    if (mqtt_err != 0)
    {
        return CHIP_ERROR_TRANSACTION_CANCELED;
    }

    return CHIP_NO_ERROR;
}

void AppTask::LoadMqttConfig(const std::string &filename, CentiMqttClient::BrokerConfig &brokerConfig, CentiMqttClient::ClientConfig &clientConfig)
{
    Json::Value root;
    std::ifstream file(filename);

    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    
    if (!parseFromStream(builder, file, &root, &errs))
    {
        std::cout << errs << std::endl;
        VerifyOrDieWithMsg(false, chipTool, "Json fileStream parsing failed");
    }

    std::cout << root << std::endl;

    // Parse BrokerConfig
    brokerConfig.hostname = root["broker_config"]["hostname"].asString();
    brokerConfig.port = root["broker_config"]["port"].asInt();
    brokerConfig.username = root["broker_config"]["username"].asString();
    brokerConfig.password = root["broker_config"]["password"].asString();
    brokerConfig.tls.enabled = root["broker_config"]["tls"]["enabled"].asBool();
    brokerConfig.tls.ca_certificate = root["broker_config"]["tls"]["ca_certificate"].asString();

    // Parse ClientConfig
    clientConfig.last_will.topic = root["client_config"]["last_will"]["topic"].asString();
    clientConfig.last_will.message = root["client_config"]["last_will"]["message"].asString();
    clientConfig.last_will.qos = root["client_config"]["last_will"]["qos"].asInt();
    clientConfig.last_will.retained = root["client_config"]["last_will"]["retained"].asBool();
    clientConfig.clean_session = root["client_config"]["clean_session"].asBool();
    clientConfig.tls.certificate = root["client_config"]["tls"]["certificate"].asString();
    clientConfig.tls.key = root["client_config"]["tls"]["key"].asString();
    clientConfig.tls.password = root["client_config"]["tls"]["password"].asString();
    clientConfig.tls.version = root["client_config"]["tls"]["version"].asInt();
    clientConfig.tls.verify = root["client_config"]["tls"]["verify"].asBool();
    clientConfig.tls.enableServerCertAuth = root["client_config"]["tls"]["enableServerCertAuth"].asBool();

    // Debug print
    std::cout << "Broker Hostname: " << brokerConfig.hostname << std::endl;
    std::cout << "Client Last Will Topic: " << clientConfig.last_will.topic << std::endl;
}


CHIP_ERROR AppTask::MQTT__Initialize(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    std::string eth_mac;
    CentiMqttClient::BrokerConfig brokerConfig;
    CentiMqttClient::ClientConfig clientConfig;
    
    err = GetMacAddress(eth_mac);
    
    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(NotSpecified, "Failed to GetMacAddress: %s ", ErrorStr(err));
        return err;
    }

    //brokerConfig.hostname = "mqtt://broker.hivemq.com";
#if  (MQTT_BROKER_HOST == MQTT_BROKER_HOST_ILLIANCE)
    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME)
        brokerConfig.hostname = "mqtts://mqtt.ilce-sensehome.ss-centi.com:1883";
        brokerConfig.port = 1883;
        brokerConfig.username = "userCenti";
        brokerConfig.password = "Qweasd123zxc";
    #else
        brokerConfig.hostname = "mqtts://devmqtt.bandora-om.com:8883";
        brokerConfig.port = 8883;
        brokerConfig.username = "centi";
        brokerConfig.password = "41c23AFa5o0w";
    #endif /* ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME */
    brokerConfig.tls.enabled = true;

    #if CONFIG_CROSS_COMPILATION
        #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME)
            brokerConfig.tls.ca_certificate = "/etc/ssl/certs/centi_broker_CA.pem";
        #else
            brokerConfig.tls.ca_certificate = "/etc/ssl/certs/bandora_broker_CA.pem";
        #endif /* ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME */
    #else
        #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME)
            brokerConfig.tls.ca_certificate = "/home/smart_systems/connectedhomeip/examples/centi-matter-controller/CeNTI_MQTT/centi_broker_CA.pem";
        #else
            brokerConfig.tls.ca_certificate = "/home/smart_systems/connectedhomeip/examples/centi-matter-controller/CeNTI_MQTT/bandora_broker_CA.pem";
        #endif /* ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME */
    #endif /* CONFIG_CROSS_COMPILATION */
    
    ChipLogProgress(chipTool, "CA Cert: %s", brokerConfig.tls.ca_certificate.c_str());


    clientConfig.tls.verify = false;
    clientConfig.tls.enableServerCertAuth = false;
    clientConfig.tls.version = MQTT_SSL_VERSION_TLS_1_2;

#elif (MQTT_BROKER_HOST == MQTT_BROKER_HOST_HIVE)

    brokerConfig.hostname = "mqtt://broker.hivemq.com:1883";
    brokerConfig.tls.enabled = false;
#endif

    clientConfig.last_will.topic = "home/will";

    clientConfig.last_will.message = R"({
        "gw_mac": ")" + eth_mac + R"(",
        "power_on": false
    })";
    clientConfig.last_will.qos = 0;
    clientConfig.last_will.retained = false;
    clientConfig.clean_session = true;

//    std::string filename = "config.json";
//    LoadMqttConfig(filename, brokerConfig, clientConfig);

    err = CentiMqttClient::ClientMgr().Init(brokerConfig, clientConfig, eth_mac);
    
    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(NotSpecified, "Failed to initialize CentiMqttClient: %s ", ErrorStr(err));
        return err;
    }

    err = CentiMqttClient::ClientMgr().Connect();

    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(NotSpecified, "MQTT connection failed: %s ", ErrorStr(err));
        return err;
    }

    err = CentiMqttClient::ClientMgr().Publish_ClientInit();

    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(NotSpecified, "MQTT Publish_ClientInit failed: %s ", ErrorStr(err));
        return err;
    }
    
    err = CentiMqttClient::ClientMgr().Subscribe_ClientTopics();
    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(NotSpecified, "MQTT Subscribe_ClientTopics failed: %s ", ErrorStr(err));
        return err;
    }

    return CHIP_NO_ERROR;
} 

CHIP_ERROR AppTask::GetMacAddress(std::string & str )
{
    uint8_t macBuffer[chip::DeviceLayer::ConfigurationManager::kPrimaryMACAddressLength];
    char macStr[] = "xx:xx:xx:xx:xx:xx:xx:xx"; // added null char
    MutableByteSpan mac(macBuffer);
    
    ReturnErrorOnFailure(DeviceLayer::ConfigurationMgr().GetPrimaryWiFiMACAddress(macBuffer));
    //ReturnErrorOnFailure(DeviceLayer::ConfigurationMgr().GetPrimary802154MACAddress(macBuffer));

   	sprintf((char *)macStr,(const char *)"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x" , 
                            macBuffer[0], 
                            macBuffer[1], 
                            macBuffer[2], 
                            macBuffer[3], 
                            macBuffer[4], 
                            macBuffer[5]);

    /*  ether 7c:10:c9:b8:c8:8b */
    ChipLogProgress(DeviceLayer, "Mac Address: %s", macStr);

    str.clear();

    str += std::string(macStr);

    return CHIP_NO_ERROR;
}

void AppTask::AddMatterCommandToQueue(std::unique_ptr<MatterCommandBase> command)
{
    {
        std::lock_guard<std::mutex> lock(mMutexQueue);
        commandQueue.push(std::move(command));
    }
    cvQueueCondition.notify_all();    
}

void AppTask::RunOnMatterQueue(intptr_t context)
{
    std::cout << "RunOnMatterQueue" << std::endl;
    MatterCommandBase* command = reinterpret_cast<MatterCommandBase *>(context);
    
    CHIP_ERROR err = command->Run();

    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Failed to run command %ld", context);
    }
}

void AppTask::RunLoop(void)
{
    while (mRunning.load()) 
    {
        std::unique_ptr<MatterCommandBase> command;
        {
            std::unique_lock<std::mutex> lock(mMutexQueue);
            cvQueueCondition.wait(lock, [this] { return !commandQueue.empty() || !mRunning.load(); });

            command = std::move(commandQueue.front());
            commandQueue.pop();
        }

        if (command) 
        {
            MatterCommandBase* commandRawPtr = command.get();
            CHIP_ERROR err = chip::DeviceLayer::PlatformMgr().ScheduleWork(RunOnMatterQueue, reinterpret_cast<intptr_t>(commandRawPtr));
            if (err == CHIP_NO_ERROR)
            {
                std::cout <<"Waiting Command... "  << std::endl;
                command->WaitForCompletionOrTimeout();
                std::cout <<"Command complete... "  << std::endl;
            }
            else
            {
                std::cout <<"Failed to schedule work: " << ErrorStr(err) << std::endl;
            }
        }
    }
}

void AppTask::StopSignalHandler(int signal)
{
    std::cout << "StopSignalHandler" << std::endl;
    AppTask::Instance().stopExecution();
}

void AppTask::Shutdown(void)
{
    std::cout << "Shutdown" << std::endl;

    sem_init (&mChipShutdownWaitSemaphore, 0, 0);
    chip::DeviceLayer::PlatformMgr().ScheduleWork(
    [](intptr_t arg) {
        chip::DeviceLayer::PlatformMgr().HandleServerShuttingDown();
        sem_t * semaphore = reinterpret_cast<sem_t *>(arg);
        sem_post(semaphore); // notify complete
    },
    reinterpret_cast<intptr_t>(&mChipShutdownWaitSemaphore));
    sem_wait(&mChipShutdownWaitSemaphore);

    // Stop the chip main loop as well. This is expected to
    // wait for the task to finish.
    std::cout << "StopEventLoop" << std::endl;

    chip::DeviceLayer::PlatformMgr().StopEventLoopTask();
}
