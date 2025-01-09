#pragma once

#include "../MatterDevices/MatterDevicesCommon.h"

namespace Illiance
{
    class AirQualitySensorHandler : public AirQualitySensorDelegate
    {
        public:
            // AirQualitySensorDelegate
            void OnTemperatureMeasuredValueChangedHandler(AirQualitySensor* airQualitySensor) override;
            void OnHumidityMeasuredValueChangedHandler(AirQualitySensor* airQualitySensor) override;
            void OnCarbonDioxideConcentrationChangedHandler(AirQualitySensor* airQualitySensor) override;
    };

    class ContactSensorHandler : public ContactSensorDelegate
    {
        public:
            //ContactSensorDelegate
            void OnContactSensorStateChangedHandler(ContactSensor* contactSensor) override;
    };

    class DoorLockHandler : public DoorLockDelegate
    {
        public:
            // DoorLockDelegate
            void OnDoorLockStateChangedHandler(DoorLock* doorLock) override;

    };

    class ElectricalSensorHandler : public ElectricalSensorDelegate
    {
        public:
            //ElectricalSensorDelegate
            void OnActivePowerValueChangedHandler(ElectricalSensor* pElectricalSensor) override;
    };

    class ExtendedColorLightHandler : public ExtendedColorLightDelegate
    {
        public:
            void OnOnOffStateChangedHandler(ExtendedColorLight* pExtendedColorLight) override;
            void OnCurrentLevelChangedHandler(ExtendedColorLight* pExtendedColorLight) override;
            void OnCurrentHueChangedHandler(ExtendedColorLight* pExtendedColorLight) override;
            void OnCurrentSaturationChangedHandler(ExtendedColorLight* pExtendedColorLight) override;
            void OnColorTemperatureMiredsChangedHandler(ExtendedColorLight* pExtendedColorLight) override;
    };

    class HumiditySensorHandler : public HumiditySensorDelegate
    {
        public:
            //HumiditySensorDelegate
            void OnMeasuredValueChangedHandler(HumiditySensor* humiditySensor) override;
    };

    class LightSensorHandler : public LightSensorDelegate
    {
        public:
            //LightSensorDelegate
            void OnMeasuredValueChangedHandler(LightSensor* lightSensor) override;
    };

    class OccupancySensorHandler : public OccupancySensorDelegate
    {
        public:
            // Occupancy Sensor Delegate
            void OnOccupancyStateChangedHandler(OccupancySensor* occupancySensor) override;
    };

    class TemperatureSensorHandler : public TemperatureSensorDelegate
    {
        public:
            //TemperatureSensorDelegate
            void OnMeasuredValueChangedHandler(TemperatureSensor* temperatureSensor) override;
    };

    class ThermostatHandler : public ThermostatDelegate
    {
        public:
            // Thermostat Delegate
            void OnOccupiedHeatingSetpointChangedHandler(ThermostatDevice* thermostat) override;
            void OnLocalTemperatureChangedHandler(ThermostatDevice* thermostat) override;
    };

    class WindowCoverHandler : public WindowCoverDelegate
    {
        public:
            // Window Cover Delegate
            void OnCurrentLiftPositionPercent100thsChangedHandler(WindowCover* windowCover) override;
    };
} //namespace Illiance