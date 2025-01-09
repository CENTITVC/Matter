#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstring>
#include "mqtt/async_client.h"
#include <lib/support/CodeUtils.h>
#include <json/json.h>
#include "centi_mqtt_commands.h"

class CentiMqttClient : public virtual mqtt::callback,
                   		public virtual mqtt::iaction_listener
{
	public:
		static CentiMqttClient & ClientMgr(void)
		{
			static CentiMqttClient sCentiMqttClient;
			return sCentiMqttClient;
		}

		struct BrokerConfig
		{
			std::string hostname;
			int 		port;
			std::string username;
			std::string password;
			struct 
			{
				bool enabled;  ///< whether to connect via SSL/TLS
				std::string ca_certificate;  ///< public CA certificate trusted by client
			} tls;               ///< SSL/TLS-related variables
		};

		struct ClientConfig
		{
			struct 
			{
				std::string topic;         ///< last-will topic
				std::string message;       ///< last-will message
				int qos;                   ///< last-will QoS value
				bool retained;             ///< whether last-will is retained
			} last_will;                 ///< last-will-related variables
			bool clean_session;          ///< whether client requests clean session
			struct 
			{
				std::string certificate;    ///< client certificate
				std::string key;            ///< client private keyfile
				std::string password;                 ///< decryption password for private key
				int version;                          ///< TLS version (https://github.com/eclipse/paho.mqtt.cpp/blob/master/src/mqtt/ssl_options.h#L305)
				bool verify;                          ///< Verify the client should conduct
				bool enableServerCertAuth;
														///< post-connect checks
			} tls;                   ///< SSL/TLS-related variables
		};	

		CHIP_ERROR Init(BrokerConfig 	brokerConfig,
				 		ClientConfig 	clientConfig,
				 		std::string 	macAddress);
		CHIP_ERROR Connect(void);
		int Disconnect(void);
		CHIP_ERROR Subscribe_ClientTopics(void);
		CHIP_ERROR Publish_ClientInit(void);
		int Publish_WindowInit(uint64_t nodeId);
		int Publish_SensorsInit(uint64_t nodeId);
		int Publish_WindowConsumption(uint64_t nodeId);
		int Publish_DoorLockInit(uint64_t nodeId);
		int Publish_OccupancySensorInit(uint64_t nodeId);
		int Publish_ElectricalSensorInit(uint64_t nodeId);
		int Publish_LightInit(uint64_t nodeId);
		int Publish_MatterCommissioningResponse(std::string setUpCode, CHIP_ERROR error);
		int Publish_MatterRemoveNodeResponse(uint64_t nodeId, CHIP_ERROR error);
		int Publish_MatterCommissioningOpenResponse(uint64_t nodeId, 
													CHIP_ERROR error, 
													std::string manualCode = "",
													std::string QRCode = "");
		int Publish_MatterWindowPositionControlResponseAck(uint64_t nodeId, 
														   CHIP_ERROR error, 
														   uint8_t pos_req = 0);
		int Publish_MatterSetOccupiedHeatSetpointResult(uint64_t nodeId,
												  		int16_t occupiedHeatSetpoint,
														CHIP_ERROR error);
		int Publish_MatterOccupancy(uint64_t nodeId, bool is_occupied);
		int Publish_MatterDoorLock(uint64_t nodeId, bool is_open);
		int Publish_MatterDeviceSubscription(
				uint64_t nodeId, 
				uint16_t endpoint, 
				uint32_t deviceType,
				CHIP_ERROR error);
		int Publish_WindowCurrentPosition(uint64_t nodeId, uint8_t position);
		int Publish_LightSettingsAck(uint64_t nodeId, CHIP_ERROR error);

		int Publish_SensorsMeasurements(uint64_t nodeId,
										int16_t temperatureCelsius,
										uint16_t humidityPercent,
										float co2Ppm,
										uint16_t brightnessLux,
										int64_t powerConsumption_mW);
		int Publish_ElectricalSensorActivePower(uint64_t nodeId, int64_t powerConsumption_mW);
		int Publish_Brightness(uint64_t nodeId, uint16_t brightnessLux);
		int Publish_ExtendedColorLight_CurrentLevel(uint64_t nodeId, uint8_t level);
		int Publish_ExtendedColorLight_Hue(uint64_t nodeId, uint16_t hue);
		int Publish_ExtendedColorLight_Saturation(uint64_t nodeId, uint8_t saturation);
		int Publish_ExtendedColorLight_ColorTemperatureKelvin(uint64_t nodeId, uint16_t colorTemperatureKelvin);
		int Publish_ExtendedColorLight_OnOff(uint64_t nodeId, bool isOn);
		int Publish_CarbonDioxide(uint64_t nodeId, float co2Ppm);
		int Publish_RelativeHumidity(uint64_t nodeId,
									uint8_t humidityPercent);
		int Publish_Temperature(uint64_t nodeId,
								int16_t temperatureCelsius);
		int Publish_OccupiedHeatingSetpoint(uint64_t nodeId,
											int16_t occupiedHeatSetpoint);
		int Publish_LocalTemperature(uint64_t nodeId,
									int16_t localTemperatureCelsius);								
		/**
		 * @brief Callback for when the connection is lost
		 * 
		 * @details Overrides mqtt::callback::connection_lost(const std::string& cause).
		 * 
		 * @param[in]   cause     Connection lost reason. See Paho Library.
		 */
		void connection_lost(const std::string& cause) override;

		/**
		 * @brief Callback for when the client receives a MQTT message from the
		 * broker.
		 * 
		 * @details Overrides mqtt::callback::message_arrived(mqtt::const_message_ptr).
		 * 
		 * @param   mqtt_msg     MQTT message
		 */
		void message_arrived(mqtt::const_message_ptr mqtt_msg) override;

		/**
		 * @brief Callback for when delivery for a MQTT message has been completed.
		 *
		 * @details Overrides mqtt::callback::delivery_complete(mqtt::delivery_token_ptr).
		 *
		 * @param   token        token tracking the message delivery
		 */
		void delivery_complete(mqtt::delivery_token_ptr token) override;

		// (Re)connection success
		void connected(const std::string& cause) override;


		/**
		 * @brief Callback for when a MQTT action succeeds.
		 *
		 * @details Overrides mqtt::iaction_listener::on_success(const mqtt::token&).
		 * Does nothing.
		 *
		 * @param   token        token tracking the action
		 */
		void on_success(const mqtt::token& token) override;

		/**
		 * @brief Callback for when a MQTT action fails.
		 *
		 * @details Overrides mqtt::iaction_listener::on_failure(const mqtt::token&).
		 * Logs error.
		 *
		 * @param   token        token tracking the action
		 */
		void on_failure(const mqtt::token& token) override;

		std::unique_ptr<MqttCommandBase> GetCommandFromMessage(std::string topic, std::string payload);
		bool ParsePayload(std::string payload, Json::Value & root, Json::String & err);
		uint64_t FindNodeIdInTopic(std::string topic);
		int Publish_MessageToTopic(Json::Value& json, std::string topic);
		static std::vector<std::string> StringSplitter(const std::string & str, char delimiter);
		static void SSL_ErrorHandler(const std::string& errMsg);
    protected:
		std::shared_ptr<mqtt::async_client> mClientPtr;
		std::string mMacAddress;
		BrokerConfig mBrokerConfig;
		ClientConfig mClientConfig;

        std::string mClientId;

		bool mInitialized;
		
        // Options to use if we need to reconnect
        mqtt::connect_options mConnOpts;
		int mMqttQos;

		const std::string kGatewayInitTopic {"home/init"};
		const std::string kInitTopicSuffix	{"init"};

		const std::string kSensorsInitTopic {"/sensors/init"};
		const std::string kSensorsTopic {"sensors"};
		const std::string kSensorsMeasTopicSuffix {"measurements"};

		const std::string kWindowInitTopic {"/window/init"};
        const std::string kWindowTopic {"window"};
		const std::string kWindowPosControlTopicSuffix {"position_control"};
		const std::string kWindowPosControlAckTopicSuffix {"position_control_ack"};

		const std::string kDoorLockInitTopic {"/door_lock/init"};
		const std::string kDoorLockTopic{"door_lock"};

		const std::string kOccupancySensorInitTopic {"/occupancy/init"};
		const std::string kOccupancySensorTopic{"occupancy"};

		const std::string kWindowPowerConsumptionTopicSuffix {"power_consumption"};

		const std::string kMatterCommissioningTopic {"matter/commissioning"};

		const std::string kMatterRemoveNodeTopic {"matter/remove_node"};
		const std::string kMatterCommissioningOpenTopic {"matter/commissioning_open"};
		const std::string kOpenThreadStateTopic {"ot/state"}; 
		const std::string kOpenThreadResetTopic {"ot/reset"};
};
