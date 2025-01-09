#include "centi_mqtt.h"
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdint>
#include "../application/AppTask.h"

CHIP_ERROR CentiMqttClient::Init(CentiMqttClient::BrokerConfig brokerConfig,
                          CentiMqttClient::ClientConfig clientConfig,
                          std::string macAddress)
{
    mBrokerConfig = brokerConfig;
    mClientConfig = clientConfig;
    mMacAddress = macAddress;
    
    mConnOpts.set_automatic_reconnect(true);
    mConnOpts.set_clean_session(mClientConfig.clean_session);
    mConnOpts.set_keep_alive_interval(60);
    
    if (!mBrokerConfig.username.empty())
    {
        mConnOpts.set_user_name(mBrokerConfig.username);
        mConnOpts.set_password(mBrokerConfig.password);   
    }

    // last will
    if (!mClientConfig.last_will.topic.empty()) 
    {
        mqtt::will_options will(
            mClientConfig.last_will.topic, mClientConfig.last_will.message,
            mClientConfig.last_will.qos, mClientConfig.last_will.retained);
        mConnOpts.set_will(will);
    }

    // SSL/TLS
    if (mBrokerConfig.tls.enabled) {
        mqtt::ssl_options ssl;

        ssl.set_trust_store(mBrokerConfig.tls.ca_certificate);

        if (!mClientConfig.tls.certificate.empty() &&
            !mClientConfig.tls.key.empty()) 
        {
            ssl.set_key_store(mClientConfig.tls.certificate);
            ssl.set_private_key(mClientConfig.tls.key);
            if (!mClientConfig.tls.password.empty())
            {
                ssl.set_private_key_password(mClientConfig.tls.password);
            }
        }
        
        ssl.set_enable_server_cert_auth(mClientConfig.tls.enableServerCertAuth);
        ssl.set_ssl_version(mClientConfig.tls.version);
        ssl.set_verify(mClientConfig.tls.verify);
        ssl.set_error_handler(&SSL_ErrorHandler);
        mConnOpts.set_ssl(ssl);
    }

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        mClientId = "iNergyGW_";
    #else
        mClientId = "iPowerGW_";
    #endif
    
    mClientId += mMacAddress;

    mClientPtr = std::make_shared<mqtt::async_client>(mBrokerConfig.hostname, mClientId);
    
    mClientPtr->set_callback(*this);

    return CHIP_NO_ERROR;
}

void CentiMqttClient::SSL_ErrorHandler(const std::string& errMsg)
{
    std::cout << "SSL Error: " << errMsg << std::endl;
}

CHIP_ERROR CentiMqttClient::Connect(void)
{    
    ChipLogProgress(NotSpecified, "[MQTT] Connecting to %s as %s", mClientPtr->get_server_uri().c_str(),
                                                            mClientPtr->get_client_id().c_str());

    try
    {
        mqtt::token_ptr conntok = mClientPtr->connect(mConnOpts, nullptr, *this);
        conntok->wait();
        ChipLogProgress(NotSpecified, "[MQTT] Connected!");
    }
    catch(const mqtt::exception& e)
    {
        ChipLogError(NotSpecified, "[MQTT] Failed to connect to broker: %s", e.what());
		return CHIP_ERROR_NOT_CONNECTED;
    }

    return CHIP_NO_ERROR;
}

void CentiMqttClient::connected(const std::string& cause)
    {
    std::cout << "\nConnection success" << std::endl;
}

CHIP_ERROR CentiMqttClient::Publish_ClientInit(void)
{
    Json::Value json;

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Client Init");

    json["power_on"] = true;
    json["gw_mac"] = mMacAddress;
    
    std::string topic = kGatewayInitTopic;

    return (Publish_MessageToTopic(json, topic) == 0) ? CHIP_NO_ERROR : CHIP_ERROR_BAD_REQUEST;
}

int CentiMqttClient::Publish_WindowInit(uint64_t nodeId)
{
    Json::Value json;

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Window Init");

    json["power_on"] = true;
    json["id"] = nodeId;

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/window/init";
    #else
        std::string topic = mMacAddress + "/device/init";
        json["type"] = "window";
    #endif

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_SensorsInit(uint64_t nodeId)
{
    Json::Value json;

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Sensors Init");

    json["power_on"] = true;
    json["id"] = nodeId;

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/sensors/init";
    #else
        std::string topic = mMacAddress + "/device/init";
        json["type"] = "sensors";
    #endif

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_DoorLockInit(uint64_t nodeId)
{
    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME)
        Json::Value json;

        ChipLogProgress(NotSpecified, "[MQTT] Publishing Door Lock Init");

        json["power_on"] = true;
        json["id"] = nodeId;
        json["type"] = "door_lock";

        std::string topic = mMacAddress + "/device/init";

        return Publish_MessageToTopic(json, topic);

    #else
        return 0;
    #endif
}

int CentiMqttClient::Publish_LightInit(uint64_t nodeId)
{
    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME)
        ChipLogProgress(NotSpecified, "[MQTT] Publishing Light Init");
        Json::Value json;

        json["power_on"] = true;
        json["id"] = nodeId;
        json["type"] = "light";
        std::string topic = mMacAddress + "/device/init";

        return Publish_MessageToTopic(json, topic);

    #else
        return 0; // not implemented
    
    #endif
}

int CentiMqttClient::Publish_OccupancySensorInit(uint64_t nodeId)
{
    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME)
        ChipLogProgress(NotSpecified, "[MQTT] Publishing Occupancy Sensor Init");
        Json::Value json;

        json["power_on"] = true;
        json["id"] = nodeId;
        json["type"] = "occupancy";
        std::string topic = mMacAddress + "/device/init";

        return Publish_MessageToTopic(json, topic);

    #else
        return 0; // not implemented
    
    #endif
}

int CentiMqttClient::Publish_MatterCommissioningResponse(std::string setUpCode, CHIP_ERROR error)
{
    Json::Value json;
    Json::StreamWriterBuilder writer;
    std::string topic = mMacAddress + "/" + kMatterCommissioningTopic + "/resp";

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Commissioning Response");

    json["code"] = setUpCode.c_str();
    json["status"] = error.AsString();
    std::string payload = Json::writeString(writer, json);

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_MatterRemoveNodeResponse(uint64_t nodeId, CHIP_ERROR error)
{
    Json::Value json;
    Json::StreamWriterBuilder writer;
    std::string topic = mMacAddress + "/" + kMatterRemoveNodeTopic + "/resp";

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Remove Node Response");

    json["id"] = nodeId;
    json["status"] = error.AsString();

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_MatterCommissioningOpenResponse(uint64_t nodeId, CHIP_ERROR error,
                                                    std::string manualCode,
													std::string QRCode)
{
    Json::Value json;
    Json::StreamWriterBuilder writer;
    std::string topic = mMacAddress + "/" + kMatterCommissioningOpenTopic + "/resp";

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Commissioning Open Response");

    json["id"] = nodeId;
    json["status"] = error.AsString();
    json["manual_code"] = manualCode.c_str();
    json["qr_code"] = QRCode.c_str();

    std::string payload = Json::writeString(writer, json);

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_MatterWindowPositionControlResponseAck(
                            uint64_t nodeId, CHIP_ERROR error, uint8_t pos_req)
{
    Json::Value json;

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/window/" + std::to_string(nodeId) + "/position_control_ack";
    #else
        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/control_ack";
    #endif

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Window Position Control Ack");

    json["id"] = nodeId;
    json["success"] = (error == CHIP_NO_ERROR);
    json["status"] = error.AsString();
    json["pos"] = pos_req;

    return Publish_MessageToTopic(json, topic);
}


int CentiMqttClient::Publish_MatterSetOccupiedHeatSetpointResult(uint64_t nodeId,
												  		int16_t occupiedHeatSetpoint,
														CHIP_ERROR error)
{
    Json::Value json;
    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/sensors/" + std::to_string(nodeId) + "/control";
    #else
        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/control";
    #endif

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Set Occupied Heat Setpoint");

    json["id"] = nodeId;
    json["setpoint"] = occupiedHeatSetpoint;
    json["error"] = error.AsString();

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_MatterOccupancy(uint64_t nodeId, bool is_occupied)
{
    Json::Value json;
    std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Occupancy");

    json["occupancy"] = is_occupied ? 1 : 0;

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_MatterDoorLock(uint64_t nodeId, bool is_open)
{
    Json::Value json;
    std::string topic = mMacAddress + "/sensors/"  + std::to_string(nodeId) + "/measurements";

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter DoorLock State");
    
    json["door"] = is_open ? 1 : 0;

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_MatterDeviceSubscription(
        uint64_t nodeId, 
        uint16_t endpoint, 
        uint32_t deviceType,
        CHIP_ERROR error)
{
    Json::Value json;
    Json::StreamWriterBuilder writer;
    std::string topic = mMacAddress + "/matter/subscription/resp";

    char deviceTypeStr[7] = {0};
    std::snprintf(deviceTypeStr, sizeof(deviceTypeStr), "0x%04X", deviceType);

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Device Subscription");

    json["id"] = nodeId;
    json["endpoint"] = endpoint;
    json["deviceType"] = deviceTypeStr;
    json["status"] = error.AsString();

    std::string payload = Json::writeString(writer, json);

    try
    {
        mClientPtr->publish(mqtt::make_message(topic, payload))->wait();
        ChipLogProgress(NotSpecified, "[MQTT] Matter Device Subscription published");
    }
    catch (const mqtt::exception& e)
    {
        ChipLogProgress(NotSpecified, "[MQTT] Failed to publish message: %s", e.what() );
        return -1;
    }

    return 0;    
}

int CentiMqttClient::Publish_LightSettingsAck(uint64_t nodeId, CHIP_ERROR error)
{
    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME)
        Json::Value json;   
        
        json["type"] = "light";
        json["success"] = (error == CHIP_NO_ERROR) ? 1 : 0;

        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/control_ack";

        return Publish_MessageToTopic(json, topic);

    #else
        return 0;
    #endif
}

int CentiMqttClient::Publish_WindowCurrentPosition(uint64_t nodeId, uint8_t position)
{
    Json::Value json;

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/window/" + std::to_string(nodeId) + "/position";
    #else
        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";
    #endif

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Window Current Position");

    json["pos"] = position;

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_SensorsMeasurements(uint64_t nodeId,
                                                int16_t temperatureCelsius,
                                                uint16_t humidityPercent,
                                                float co2Ppm,
                                                uint16_t brightnessLux,
                                                int64_t powerConsumption_mW)
{
    Json::Value json;
    Json::StreamWriterBuilder writer;
    std::string topic = mMacAddress + "/" + "sensors/" + std::to_string(nodeId) + "/measurements";

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Sensors Measurements");

    json["temperature"] = temperatureCelsius;
    json["humidity"] = humidityPercent;
    json["co2"] = co2Ppm;
    json["brightness"] = brightnessLux;
    json["power_consumption"] = powerConsumption_mW;

    std::string payload = Json::writeString(writer, json);

    try
    {
        mClientPtr->publish(mqtt::make_message(topic, payload))->wait();
        ChipLogProgress(NotSpecified, "[MQTT] Matter Sensors Measurements published");
    }
    catch (const mqtt::exception& e)
    {
        ChipLogProgress(NotSpecified, "[MQTT] Failed to publish message: %s", e.what() );
        return -1;
    }

    return 0;    
}

int CentiMqttClient::Publish_ElectricalSensorActivePower(uint64_t nodeId, int64_t powerConsumption_mW)
{
    Json::Value json;
    
    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/sensors/" + std::to_string(nodeId) + "/measurements";
    #else
        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";
    #endif

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Electrical Sensor Active Power Measurement");

    json["power_consumption"] = powerConsumption_mW;

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_Brightness(uint64_t nodeId, uint16_t brightnessLux)
{
    Json::Value json;

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/sensors/" + std::to_string(nodeId) + "/measurements";
    #else
        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";
    #endif

    json["brightness"] = brightnessLux;

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Illuminance Measurement");

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_CarbonDioxide(uint64_t nodeId, float co2Ppm)
{
    Json::Value json;

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/sensors/" + std::to_string(nodeId) + "/measurements";
    #else
        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";
    #endif

    json["co2"] = co2Ppm;

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter CO2 Measurement");

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_ExtendedColorLight_CurrentLevel(uint64_t nodeId, uint8_t level)
{
    Json::Value json;

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/sensors/" + std::to_string(nodeId) + "/measurements";
    #else
        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";
    #endif

    json["current_level"] = level;

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter ExtendedColorLight CurrentLevel");

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_ExtendedColorLight_OnOff(uint64_t nodeId, bool isOn)
{
    Json::Value json;

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/sensors/" + std::to_string(nodeId) + "/measurements";
    #else
        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";
    #endif

    json["on-off"] = (isOn) ? 1 : 0;

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter ExtendedColorLight OnOff");

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_ExtendedColorLight_Hue(uint64_t nodeId, uint16_t hue)
{
    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME)
        Json::Value json;   
        
        json["hue"] = hue;

        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";
    
        ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter ExtendedColorLight Hue");

        return Publish_MessageToTopic(json, topic);
    #else
        return 0;
    #endif
}

int CentiMqttClient::Publish_ExtendedColorLight_Saturation(uint64_t nodeId, uint8_t saturation)
{
    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME)
        Json::Value json;   
        
        json["saturation"] = saturation;

        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";

        return Publish_MessageToTopic(json, topic);

    #else
        return 0;
    #endif
}

int CentiMqttClient::Publish_ExtendedColorLight_ColorTemperatureKelvin(uint64_t nodeId, uint16_t colorTemperatureKelvin)
{
    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME)
        Json::Value json;   
        
        json["temperatureKelvin"] = colorTemperatureKelvin;

        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";

        return Publish_MessageToTopic(json, topic);

    #else
        return 0;
    #endif
}


int CentiMqttClient::Publish_RelativeHumidity(uint64_t nodeId,
                            uint8_t humidityPercent)
{
    Json::Value json;

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/sensors/" + std::to_string(nodeId) + "/measurements";
    #else
        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";
    #endif

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Relative Humidity Measurement");

    json["humidity"] = humidityPercent;

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_Temperature(uint64_t nodeId,
                                        int16_t temperatureCelsius)
{
    Json::Value json;
    
    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" +mMacAddress + "/sensors/" + std::to_string(nodeId) + "/measurements";
    #else
        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";
    #endif

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Temperature Measurement");

    json["temperature"] = temperatureCelsius;

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_OccupiedHeatingSetpoint(uint64_t nodeId,
                                    int16_t occupiedHeatSetpoint)
{
    Json::Value json;

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/sensors/" + std::to_string(nodeId) + "/measurements";
    #else
        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";
    #endif

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Occupied Heating Setpoint");

    json["id"] = nodeId;
    json["heatSetpoint"] = occupiedHeatSetpoint;

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_LocalTemperature(uint64_t nodeId,
                            int16_t localTemperatureCelsius)
{
    Json::Value json;

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string topic = "centi/" + mMacAddress + "/sensors/" + std::to_string(nodeId) + "/measurements";
    #else
        std::string topic = mMacAddress + "/device/" + std::to_string(nodeId) + "/measurements";
    #endif

    ChipLogProgress(NotSpecified, "[MQTT] Publishing Matter Local Temperature Measurement");

    json["temperature"] = localTemperatureCelsius;

    return Publish_MessageToTopic(json, topic);
}

int CentiMqttClient::Publish_MessageToTopic(Json::Value& json, std::string topic)
{
    Json::StreamWriterBuilder writer;
    std::string payload = Json::writeString(writer, json);

    try
    {
        mClientPtr->publish(mqtt::make_message(topic, payload))->wait();
    }
    catch (const mqtt::exception& e)
    {
        ChipLogProgress(NotSpecified, "[MQTT] Failed to publish message: %s", e.what() );
        return -1;
    }

    return 0;
}

CHIP_ERROR CentiMqttClient::Subscribe_ClientTopics(void)
{
    std::vector<std::string> subscribeTopics = 
    {  
        #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_IPOWER_SENSING_HOME)
        
        mMacAddress + "/" + kMatterCommissioningTopic + "/req",
        mMacAddress + "/" + kMatterRemoveNodeTopic + "/req",
        mMacAddress + "/" + kMatterCommissioningOpenTopic + "/req",
        mMacAddress + "/device/+/control",
        #else
        "centi/" + mMacAddress + "/" + kMatterCommissioningTopic + "/req",
        "centi/" + mMacAddress + "/" + kMatterRemoveNodeTopic + "/req",
        "centi/" + mMacAddress + "/" + kMatterCommissioningOpenTopic + "/req",
        "centi/" + mMacAddress + "/window/+/position_control",
        #endif
    };

    std::vector<int> qos = 
    {
        1,1,1,1
    };

    for(auto & subTopic : subscribeTopics)
    {
        ChipLogProgress(NotSpecified, "[MQTT] Topic subscribed %s", subTopic.c_str());
    }

    // Convert subscribeTopics to mqtt::string_collection_ptr
    auto topics = std::make_shared<mqtt::string_collection>(subscribeTopics);
    
    try
    {
        mClientPtr->subscribe(topics, qos)->wait();
    }
    catch(const std::exception& e)
    {
        std::cout << "[MQTT] Failed to subscribe topics: " << e.what() << std::endl ;
        return CHIP_ERROR_BAD_REQUEST;
    }

    return CHIP_NO_ERROR;
}

void CentiMqttClient::connection_lost(const std::string& cause)
{
    ChipLogProgress(NotSpecified, "[MQTT] Connection Lost %s", !cause.empty() ? cause.c_str() : "");
}

void CentiMqttClient::message_arrived(mqtt::const_message_ptr mqtt_msg)
{
    ChipLogProgress(NotSpecified, "[MQTT] Message arrived from topic %s: %s", 
                                    mqtt_msg->get_topic().c_str(),
                                    mqtt_msg->to_string().c_str());
                                    
    std::cout << "Message arrived" << std::endl;
    std::cout << "\ttopic: '" << mqtt_msg->get_topic() << "'" << std::endl;
    std::cout << "\tpayload: '" << mqtt_msg->to_string() << "'\n" << std::endl;

    std::string topic = mqtt_msg->get_topic();
    std::string payload = mqtt_msg->get_payload_str();

    std::unique_ptr<MqttCommandBase> cmd = GetCommandFromMessage(topic, payload);

    switch(cmd->GetType())
    {
        case MqttCommandType::MatterCommissionNode:
            AppTask::Instance().AddMatterPairingCommand(static_cast<MqttCommandMatterCommission*>(cmd.get())->mSetUpCode);
            break;

        case MqttCommandType::MatterOpenCommissioning:
            AppTask::Instance().AddMatterCommissioningOpenCommand(static_cast<MqttCommandMatterCommissionOpen*>(cmd.get())->mNodeId);
            break;

        case MqttCommandType::MatterRemoveNode:
            AppTask::Instance().AddMatterRemoveNodeCommand(static_cast<MqttCommandMatterRemove*>(cmd.get())->mNodeId);
            break;
            
        case MqttCommandType::SetWindowPosition:
            AppTask::Instance().AddMatterCommandWindowPositionSet(static_cast<MqttCommandSetWindowPosition*>(cmd.get())->mNodeId, 
                                                                static_cast<MqttCommandSetWindowPosition*>(cmd.get())->mPosition);
            break;

        case MqttCommandType::LightSetting:
            AppTask::Instance().AddMatterLightSettingCommand(static_cast<MqttCommandLightSetting*>(cmd.get())->mNodeId,
                                                            static_cast<MqttCommandLightSetting*>(cmd.get())->mLightSettings);
            break;
            
        case MqttCommandType::SetThermostatOccupiedHeatSetpoint:
            AppTask::Instance().AddMatterSetOccupiedHeatSetpointCommand(static_cast<MqttCommandThermostatSetOccupiedHeatSetpoint*>(cmd.get())->mNodeId,
                                                                        static_cast<MqttCommandThermostatSetOccupiedHeatSetpoint*>(cmd.get())->mTemperature);
            break;

        case MqttCommandType::Unknown:
            break;
    }
}

void CentiMqttClient::delivery_complete(mqtt::delivery_token_ptr token) {}

void CentiMqttClient::on_success(const mqtt::token& token) 
{
    std::cout << "on_success" << std::endl;
    if (token.get_message_id() != 0)
        std::cout << " for token: [" << token.get_message_id() << "]" << std::endl;
    std::cout << std::endl;
}

void CentiMqttClient::on_failure(const mqtt::token& token)
{
    std::cout << "on_failure" << std::endl;
    if (token.get_message_id() != 0)
        std::cout << " for token: [" << token.get_message_id() << "]" << std::endl;
    auto top = token.get_topics();
    if (top && !top->empty())
        std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
    std::cout << std::endl;
}

std::unique_ptr<MqttCommandBase> CentiMqttClient::GetCommandFromMessage(std::string topic, std::string payload)
{
    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
        std::string matterCommissioningTopic = "centi/" + mMacAddress + "/" + kMatterCommissioningTopic + "/req";
        std::string matterRemoveTopic = "centi/" + mMacAddress + "/" + kMatterRemoveNodeTopic + "/req";
        std::string matterSetWindowPosition = "centi/" + mMacAddress + "/window/<id>/position_control";
        std::string matterCommissioningOpenTopic = "centi/" + mMacAddress + "/" + kMatterCommissioningOpenTopic + "/req";
    #else
        std::string matterCommissioningTopic = mMacAddress + "/" + kMatterCommissioningTopic + "/req";
        std::string matterRemoveTopic = mMacAddress + "/" + kMatterRemoveNodeTopic + "/req";
        std::string matterSetWindowPosition = mMacAddress + "/window/<id>/position_control";
        std::string matterCommissioningOpenTopic = mMacAddress + "/" + kMatterCommissioningOpenTopic + "/req";
    #endif

    Json::Value root;
    Json::String err;
    bool parsing_success = false;
    std::unique_ptr<MatterCommandBase> mqttCommand;

    parsing_success = ParsePayload(payload, root, err);
    ChipLogProgress(NotSpecified, "[MQTT] Payload parse %s", (parsing_success) ? "success" : "error");

    if (topic.compare(matterCommissioningTopic) == 0)
    {
        std::string setUpCode = root["code"].asString();

        if (!parsing_success)
        {
            (void) Publish_MatterCommissioningResponse(setUpCode, CHIP_ERROR_BAD_REQUEST); // ignoring retCode for now
            return std::make_unique<MqttCommandUnknown>();
        }

        return std::make_unique<MqttCommandMatterCommission>(setUpCode);
    }
    else if (topic.compare(matterRemoveTopic) == 0)
    {
        uint64_t nodeId = root["id"].asUInt64();

        if (!parsing_success)
        {
            (void) Publish_MatterRemoveNodeResponse(nodeId, CHIP_ERROR_BAD_REQUEST);
            return std::make_unique<MqttCommandUnknown>();
        }

        return std::make_unique<MqttCommandMatterRemove>(nodeId);
    }
    else if (topic.compare(matterCommissioningOpenTopic) == 0)
    {
        uint64_t nodeId = root["id"].asUInt64();

        if (!parsing_success)
        {
            (void) Publish_MatterCommissioningOpenResponse(nodeId, CHIP_ERROR_BAD_REQUEST); // ignoring retCode for now
            return std::make_unique<MqttCommandUnknown>();
        }

        return std::make_unique<MqttCommandMatterCommissionOpen>(nodeId);
    }
#if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
    else if ((topic.find("window") != std::string::npos) && (topic.find("position_control") != std::string::npos))
    {
        if (parsing_success)
        {
            uint64_t nodeId = FindNodeIdInTopic(topic);
            if (nodeId != UINT64_MAX)
            {
                uint64_t pos = root["pos"].asUInt64();
                pos = static_cast<uint64_t>(100) - pos;
                return std::make_unique<MqttCommandSetWindowPosition>(nodeId, static_cast<uint8_t>(pos));
            }
            else
            {
                (void) Publish_MatterWindowPositionControlResponseAck(nodeId, CHIP_ERROR_BAD_REQUEST);
                return std::make_unique<MqttCommandUnknown>();
            }
        }
        else
        {
            (void) Publish_MatterWindowPositionControlResponseAck(UINT64_MAX, CHIP_ERROR_BAD_REQUEST);
            return std::make_unique<MqttCommandUnknown>();
        }
    }
#else
    else if ((topic.find("device") != std::string::npos) && (topic.find("control") != std::string::npos))
    {
        if (parsing_success)
        {
            uint64_t nodeId = FindNodeIdInTopic(topic);
            if (nodeId != UINT64_MAX)
            {
                std::string deviceType = root["type"].asString();

                if (deviceType.compare("window") == 0)
                {
                    uint64_t pos = root["pos"].asUInt64();
                    pos = static_cast<uint64_t>(100) - pos;
                    return std::make_unique<MqttCommandSetWindowPosition>(nodeId, static_cast<uint8_t>(pos));
                }
                else if (deviceType.compare("light") == 0)
                {
                    LightSettings lightSettings;

                    if (root.isMember("hue") && root.isMember("saturation") && root.isMember("value"))
                    {
                        lightSettings.hue = static_cast<uint16_t>(root["hue"].asUInt64());
                        lightSettings.saturation = static_cast<uint8_t>(root["saturation"].asUInt64());
                        lightSettings.value = static_cast<uint8_t>(root["value"].asUInt64());
                        lightSettings.commandMask.SetField(LightCommandBitMap::kColor, 1);
                    }

                    if (root.isMember("temperatureKelvin"))
                    {
                        lightSettings.colorTemperatureKelvin = static_cast<uint16_t>(root["colorTemperatureKelvin"].asUInt64());
                        lightSettings.commandMask.SetField(LightCommandBitMap::kColorTemperature, 1);
                    }

                    if (root.isMember("action"))
                    {
                        std::string onoffStr = root["action"].asString();

                        lightSettings.onoff = (onoffStr.compare("on") == 0);
                        lightSettings.commandMask.SetField(LightCommandBitMap::kAction, 1);
                    }

                    return std::make_unique<MqttCommandLightSetting>(nodeId, lightSettings);
                }
                else if (deviceType.compare("thermoAccumulator") == 0)
                {
                    int16_t temperature;

                    if (root.isMember("heatSetpoint"))
                    {
                        temperature = 100 * (static_cast<int16_t>(root["heatSetpoint"].asInt64()));
                        return std::make_unique<MqttCommandThermostatSetOccupiedHeatSetpoint>(nodeId, temperature);
                    }
                }
            }
            else
            {
                std::cout << "Error in parsing node ID from topic" << std::endl;
                return std::make_unique<MqttCommandUnknown>();
            }
        }
    }
#endif
    return std::make_unique<MqttCommandUnknown>();
}

bool CentiMqttClient::ParsePayload(std::string payload, Json::Value & root, Json::String & err)
{
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    return reader->parse(payload.c_str(), payload.c_str() + payload.length(), &root, &err);
}

uint64_t CentiMqttClient::FindNodeIdInTopic(std::string topic)
{
    std::vector<std::string> tokens = StringSplitter(topic, '/');
    

    #if (ILLIANCE_PROJECT_VERSION == ILLIANCE_INERGY)
    if(tokens.size() < 5) // bandora topic has centi/ preffix
    {
        return UINT64_MAX;
    } 

    if ( (tokens[1].compare("window") == 0) || (tokens[1].compare("sensors") == 0) )
    #else
    if(tokens.size() < 4)
    {
        return UINT64_MAX;
    } 

    if ( (tokens[1].compare("device") == 0) && (tokens[3].compare("control") == 0))
    #endif
    {
        try
        {
            // std::stoull converts a string to an unsigned long long
            size_t pos;
            uint64_t number = std::stoull(tokens[2], &pos, 10);

            // Ensure the whole string was converted and there are no remaining characters
            if (pos != tokens[2].size())
            {
                throw std::invalid_argument("Trailing characters after number");
            }

            return number;
        }
        catch (const std::invalid_argument& ia)
        {
            std::cerr << "Invalid argument: " << ia.what() << std::endl;
            // Handle the error (e.g., return a special value or rethrow the exception)
            return UINT64_MAX; // or throw ia;
        }
        catch (const std::out_of_range& oor)
        {
            std::cerr << "Out of range: " << oor.what() << std::endl;
            return UINT64_MAX; // or throw oor;
        }
    }
    
    return UINT64_MAX;
}

std::vector<std::string> CentiMqttClient::StringSplitter(const std::string & str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;    
}
