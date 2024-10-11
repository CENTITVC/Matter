/*
 *
 *    Copyright (c) 2023 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <iostream>
#include "mqtt/async_client.h"

static void SSL_ErrorHandler(const std::string& errMsg)
{
    std::cout << "SSL Error: " << errMsg << std::endl;
}


int main(int argc, char * argv[])
{
    std::cout << "Hello Test example!" << std::endl;

    // Options to use if we need to reconnect
    mqtt::connect_options mConnOpts;

    std::unique_ptr<mqtt::async_client> mClientPtr;

    std::cout << "creating Client" << std::endl;

    mClientPtr = std::make_unique<mqtt::async_client>("mqtts://mqtt.ilce-sensehome.ss-centi.com:1883", "gnTest");

    std::cout << "Client created" << std::endl;

    mConnOpts.set_user_name("userCenti");
    mConnOpts.set_password("Qweasd123zxc");   

    mConnOpts.set_automatic_reconnect(true);
    mConnOpts.set_clean_session(true);
    mConnOpts.set_keep_alive_interval(60);

    std::cout << "Configuring SSL..." << std::endl;

    mqtt::ssl_options ssl;
    ssl.set_trust_store("centi_broker_CA.pem");
    ssl.set_enable_server_cert_auth(false);
    ssl.set_ssl_version(MQTT_SSL_VERSION_TLS_1_2);
    ssl.set_verify(MQTT_SSL_VERSION_TLS_1_2);
    ssl.set_error_handler(&SSL_ErrorHandler);
    mConnOpts.set_ssl(ssl);

    std::cout << "[MQTT] Connecting to " << mClientPtr->get_server_uri().c_str() << " as " <<
                                                            mClientPtr->get_client_id().c_str() << std::endl;

    try
    {
        mqtt::token_ptr conntok = mClientPtr->connect(mConnOpts);
        conntok->wait();
        std::cout << "[MQTT] Connected!" << std::endl;
    }
    catch(const mqtt::exception& e)
    {
        std::cout << "[MQTT] Failed to connect to broker: " << e.what() << std::endl;
		return 1;
    }

    while(1)
    {

    }

    return 0;
}
