#include "MatterDevicesCallbacks.h"
#include "../../CeNTI_MQTT/centi_mqtt.h"
#include "../MatterManager.h"

namespace Illiance
{
    void AirQualitySensorHandler::OnTemperatureMeasuredValueChangedHandler(AirQualitySensor* airQualitySensor)
    {
        ChipLogProgress(chipTool, "AirQualitySensor - OnTemperatureMeasuredValueChangedHandler");
        int16_t temperatureCelsius = INT16_MIN;

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(airQualitySensor->GetType()) == airQualitySensor)
                {
                    ChipLogProgress(chipTool, "Found airQualitySensor");
                    temperatureCelsius = airQualitySensor->GetTemperatureSensor().GetMeasuredValue();
                    (void) CentiMqttClient::ClientMgr().Publish_Temperature(node.GetNodeId(), temperatureCelsius);
                    break;
                }
            }
        }
    }

    void AirQualitySensorHandler::OnHumidityMeasuredValueChangedHandler(AirQualitySensor* airQualitySensor)
    {
        uint8_t humidityPercentage = 0;

        ChipLogProgress(chipTool, "AirQualitySensor - OnHumidityMeasuredValueChangedHandler");

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(airQualitySensor->GetType()) == airQualitySensor)
                {
                    ChipLogProgress(chipTool, "Found airQualitySensor");
                    humidityPercentage = static_cast<uint8_t> (airQualitySensor->GetRelativeHumiditySensor().GetMeasuredValue());
                    (void) CentiMqttClient::ClientMgr().Publish_RelativeHumidity(node.GetNodeId(), humidityPercentage);
                    break;
                }
            }
        }
    }

    void AirQualitySensorHandler::OnCarbonDioxideConcentrationChangedHandler(AirQualitySensor* airQualitySensor)
    {
        float co2ppm = FLT_MIN;

        ChipLogProgress(chipTool, "AirQualitySensor - OnCarbonDioxideConcentrationChangedHandler");

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(airQualitySensor->GetType()) == airQualitySensor)
                {
                    ChipLogProgress(chipTool, "Found airQualitySensor");
                    co2ppm = airQualitySensor->GetCarbonDioxideSensor().GetMeasuredValue();
                    (void) CentiMqttClient::ClientMgr().Publish_CarbonDioxide(node.GetNodeId(), co2ppm);
                    break;
                }
            }
        }
    }

    void ContactSensorHandler::OnContactSensorStateChangedHandler(ContactSensor* contactSensor)
    {
        ChipLogProgress(chipTool, "OnContactSensorStateChangedHandler");

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(contactSensor->GetType()) == contactSensor)
                {
                    ChipLogProgress(chipTool, "Found contactSensor");

                    (void) CentiMqttClient::ClientMgr().Publish_MatterDoorLock(node.GetNodeId(), !contactSensor->IsContactClosed());
                }
            }
        }
    }

    void DoorLockHandler::OnDoorLockStateChangedHandler(DoorLock* doorLock)
    {
        ChipLogProgress(chipTool, "OnDoorLockStateChangedHandler");

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(doorLock->GetType()) == doorLock)
                {
                    ChipLogProgress(chipTool, "Found doorLock");

                    chip::app::Clusters::DoorLock::DoorStateEnum doorState = doorLock->GetDoorLockState();

                    bool isOpen = ((doorState == chip::app::Clusters::DoorLock::DoorStateEnum::kDoorForcedOpen) ||
                                   (doorState == chip::app::Clusters::DoorLock::DoorStateEnum::kDoorOpen));

                    (void) CentiMqttClient::ClientMgr().Publish_MatterDoorLock(node.GetNodeId(), isOpen);
                }
            }
        }
    }

    void ElectricalSensorHandler::OnActivePowerValueChangedHandler(ElectricalSensor* pElectricalSensor)
    {
        ChipLogProgress(chipTool, "ElectricalSensor - OnActivePowerValueChangedHandler");
        int64_t powerConsumption_mW = INT64_MIN;

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(pElectricalSensor->GetType()) == pElectricalSensor)
                {
                    ChipLogProgress(chipTool, "Found electricalSensor");
                    powerConsumption_mW = pElectricalSensor->GetActivePower();
                    (void) CentiMqttClient::ClientMgr().Publish_ElectricalSensorActivePower(node.GetNodeId(), powerConsumption_mW);
                }
            }
        }
    }

    void ExtendedColorLightHandler::OnOnOffStateChangedHandler(ExtendedColorLight* pExtendedColorLight)
    {
        ChipLogProgress(chipTool, "ExtendedColorLight - OnOnOffStateChangedHandler");
        bool isOn = false;

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(pExtendedColorLight->GetType()) == pExtendedColorLight)
                {
                    ChipLogProgress(chipTool, "Found pExtendedColorLight");
                    isOn = pExtendedColorLight->IsOn();
                    (void) CentiMqttClient::ClientMgr().Publish_ExtendedColorLight_OnOff(node.GetNodeId(), isOn);
                }
            }
        }
    }


    void ExtendedColorLightHandler::OnCurrentLevelChangedHandler(ExtendedColorLight* pExtendedColorLight)
    {
        ChipLogProgress(chipTool, "ExtendedColorLight - OnCurrentLevelChangedHandler");
        uint8_t level = 0;

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(pExtendedColorLight->GetType()) == pExtendedColorLight)
                {
                    ChipLogProgress(chipTool, "Found pExtendedColorLight");
                    level = pExtendedColorLight->GetCurrentLevel();
                    (void) CentiMqttClient::ClientMgr().Publish_ExtendedColorLight_CurrentLevel(node.GetNodeId(), level);
                }
            }
        }
    }

    void ExtendedColorLightHandler::OnCurrentHueChangedHandler(ExtendedColorLight* pExtendedColorLight)
    {
        ChipLogProgress(chipTool, "ExtendedColorLight - OnCurrentHueChangedHandler");
        uint16_t hue = 0;

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(pExtendedColorLight->GetType()) == pExtendedColorLight)
                {
                    ChipLogProgress(chipTool, "Found pExtendedColorLight");
                    hue = pExtendedColorLight->GetHue();
                    (void) CentiMqttClient::ClientMgr().Publish_ExtendedColorLight_Hue(node.GetNodeId(), hue);
                }
            }
        }
    }

    void ExtendedColorLightHandler::OnCurrentSaturationChangedHandler(ExtendedColorLight* pExtendedColorLight)
    {
        ChipLogProgress(chipTool, "ExtendedColorLight - OnCurrentSaturationChangedHandler");
        uint8_t saturation = 0;

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(pExtendedColorLight->GetType()) == pExtendedColorLight)
                {
                    ChipLogProgress(chipTool, "Found pExtendedColorLight");
                    saturation = pExtendedColorLight->GetSaturation();
                    (void) CentiMqttClient::ClientMgr().Publish_ExtendedColorLight_Saturation(node.GetNodeId(), saturation);
                }
            }
        }
    }

    void ExtendedColorLightHandler::OnColorTemperatureMiredsChangedHandler(ExtendedColorLight* pExtendedColorLight)
    {
        ChipLogProgress(chipTool, "ExtendedColorLight - OnColorTemperatureMiredsChangedHandler");
        uint16_t colorTemperatureKelvin = 0;

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(pExtendedColorLight->GetType()) == pExtendedColorLight)
                {
                    ChipLogProgress(chipTool, "Found pExtendedColorLight");
                    colorTemperatureKelvin = pExtendedColorLight->GetColorTemperatureKelvin();
                    (void) CentiMqttClient::ClientMgr().Publish_ExtendedColorLight_ColorTemperatureKelvin(node.GetNodeId(), colorTemperatureKelvin);
                }
            }
        }
    }

    void HumiditySensorHandler::OnMeasuredValueChangedHandler(HumiditySensor* humiditySensor)
    {
        ChipLogProgress(chipTool, "HumiditySensor - OnMeasuredValueChangedHandler");

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(humiditySensor->GetType()) == humiditySensor)
                {
                    ChipLogProgress(chipTool, "Found humiditySensor");
                    uint8_t humidityPercentage = static_cast<uint8_t>(humiditySensor->GetMeasuredValue() / 100);
                    (void) CentiMqttClient::ClientMgr().Publish_RelativeHumidity(node.GetNodeId(), humidityPercentage);
                }
            }
        }
    }

    void LightSensorHandler::OnMeasuredValueChangedHandler(LightSensor* lightSensor)
    {
        ChipLogProgress(chipTool, "LightSensor - OnMeasuredValueChangedHandler");
        uint16_t brightnessLux = UINT16_MAX;

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(lightSensor->GetType()) == lightSensor)
                {
                    ChipLogProgress(chipTool, "Found lightSensor");
                    brightnessLux = lightSensor->GetMeasuredValue();
                    (void) CentiMqttClient::ClientMgr().Publish_Brightness(node.GetNodeId(), brightnessLux);
                }
            }
        }
    }

    void OccupancySensorHandler::OnOccupancyStateChangedHandler(OccupancySensor* occupancySensor)
    {
        ChipLogProgress(chipTool, "OnOccupancyStateChangedHandler");

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(occupancySensor->GetType()) == occupancySensor)
                {
                    ChipLogProgress(chipTool, "Found occupancySensor");
                    bool isOccupied = occupancySensor->GetOccupancyState();
                    (void) CentiMqttClient::ClientMgr().Publish_MatterOccupancy(node.GetNodeId(), isOccupied);
                }
            }
        }
    }

    void TemperatureSensorHandler::OnMeasuredValueChangedHandler(TemperatureSensor* temperatureSensor)
    {
        ChipLogProgress(chipTool, "TemperatureSensor - OnMeasuredValueChangedHandler");

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(temperatureSensor->GetType()) == temperatureSensor)
                {
                    ChipLogProgress(chipTool, "Found temperatureSensor");
                    int16_t temperatureCelsius = temperatureSensor->GetMeasuredValue() / 100;
                    (void) CentiMqttClient::ClientMgr().Publish_Temperature(node.GetNodeId(), temperatureCelsius);
                }
            }
        }
    }

    void WindowCoverHandler::OnCurrentLiftPositionPercent100thsChangedHandler(WindowCover* windowCover)
    {
        ChipLogProgress(chipTool, "OnCurrentLiftPositionPercent100thsChangedHandler");

        for(auto & node : MatterManager::MatterMgr().GetActiveMatterNodes())
        {
            for (auto & endpoint : node.GetEndpoints())
            {
                /* comparison by memory address.  review later */
                if (endpoint.GetMatterDevice(MATTER_DEVICE_ID_WINDOW_COVERING) == windowCover)
                {
                    ChipLogProgress(chipTool, "Found WindowCover");
                    uint8_t pos = static_cast<uint8_t>(windowCover->GetCurrentLiftPositionPercent100ths() / 100);
                    (void) CentiMqttClient::ClientMgr().Publish_WindowCurrentPosition(node.GetNodeId(), pos);
                }
            }
        }
    }
} //namespace Illiance
