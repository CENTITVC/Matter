#pragma once

#include "../MatterDevices/MatterDevicesCommon.h"
#include "../MatterNode.h"


namespace Illiance
{
    enum class IllianceDeviceType : uint8_t
    {
        Unknown = 0, 
        Sensors,
        Window,
        Occupancy,
        DoorLock,
        Contact,
        Light,
        Thermostat,
    };

    bool IsIllianceSensors(MatterNode& node);

    bool IsIllianceWindow(MatterNode& node);

    bool IsIllianceOccupancySensor(MatterNode& node);

    bool IsIllianceDoorLock(MatterNode& node);

    bool IsIllianceContactSensor(MatterNode& node);
    
    bool IsIllianceLight(MatterNode& node);

    bool IsIllianceThermostat(MatterNode& node);

    class IllianceDevice
    {
        public:
            IllianceDevice(IllianceDeviceType type) : mIllianceDeviceType(type) {}

            IllianceDeviceType GetType() { return mIllianceDeviceType; }

        private:
            IllianceDeviceType mIllianceDeviceType;
    };

    class IllianceSensors : public IllianceDevice
    {
        public:
            IllianceSensors(AirQualitySensor* airQualitySensor, LightSensor* lightSensor, ElectricalSensor* electricalSensor ) : 
                            IllianceDevice(IllianceDeviceType::Sensors)
            {
                mAirQualitySensor = airQualitySensor;
                mLightSensor = lightSensor;
                mElectricalSensor = electricalSensor;
            }
            
            AirQualitySensor* GetAirQualitySensor(void) { return mAirQualitySensor; }
            LightSensor* GetLightSensor(void) { return mLightSensor; }
            ElectricalSensor* GetElectricalSensor(void) { return mElectricalSensor; }
            
        private:
            AirQualitySensor* mAirQualitySensor;
            LightSensor* mLightSensor;
            ElectricalSensor* mElectricalSensor;
    };

    class IllianceWindow : public IllianceDevice 
    {
        public:
            IllianceWindow(WindowCover* windowCover) : 
                    IllianceDevice(IllianceDeviceType::Window) 
            {
                mWindowCover = windowCover;
            }
            
            WindowCover* GetWindowCover(void) { return mWindowCover; }

        private:
            WindowCover* mWindowCover;
    };

    class IllianceOccupancy : public IllianceDevice 
    {
        public:
            IllianceOccupancy(OccupancySensor* occupancySensor) : 
                    IllianceDevice(IllianceDeviceType::Occupancy)
            {
                mOccupancySensor = occupancySensor;
            } 
            
            OccupancySensor* GetOccupancySensor(void) { return mOccupancySensor; }

        private:
            OccupancySensor* mOccupancySensor;
    };

    class IllianceDoorLock : public IllianceDevice 
    {
        public:
            IllianceDoorLock(DoorLock* doorLock) : 
                    IllianceDevice(IllianceDeviceType::DoorLock), mDoorLock(doorLock)
            {
                mDoorLock = doorLock;
            }
            
            DoorLock* GetDoorLock(void) { return mDoorLock; }
            
        private:
            DoorLock* mDoorLock;
    };

    class IllianceThermostat : public IllianceDevice
    {
        public:
            IllianceThermostat(ThermostatDevice* thermostat) : 
                    IllianceDevice(IllianceDeviceType::Thermostat)
            {
                mThermostat = thermostat;
            }
            
            ThermostatDevice* GetThermostat(void) { return mThermostat; }
        
        private:
            ThermostatDevice* mThermostat;
    };

} //namespace Illiance
