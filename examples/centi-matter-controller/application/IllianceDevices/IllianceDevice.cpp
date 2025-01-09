#include "IllianceDevice.h"

namespace Illiance
{
bool IsIllianceSensors(MatterNode& node)
{
    return ((node.HasDevice(MATTER_DEVICE_ID_AIR_QUALITY_SENSOR)) && 
        (node.HasDevice(MATTER_DEVICE_ID_LIGHT_SENSOR))); 
        /* &&
        (node->HasDevice(MATTER_DEVICE_ID_ELETRICAL_SENSOR)))*/
}

bool IsIllianceWindow(MatterNode& node) { return (node.HasDevice(MATTER_DEVICE_ID_WINDOW_COVERING)); }

bool IsIllianceOccupancySensor(MatterNode& node) { return (node.HasDevice(MATTER_DEVICE_ID_OCCUPANCY_SENSOR)); }

bool IsIllianceDoorLock(MatterNode& node) { return (node.HasDevice(MATTER_DEVICE_ID_DOOR_LOCK)); }

bool IsIllianceContactSensor(MatterNode& node) { return (node.HasDevice(MATTER_DEVICE_ID_CONTACT_SENSOR)); }

bool IsIllianceLight(MatterNode& node) { return node.HasDevice(MATTER_DEVICE_ID_EXTENDED_COLOR_LIGHT); }

bool IsIllianceThermostat(MatterNode& node) { return node.HasDevice(MATTER_DEVICE_ID_THERMOSTAT); }

} //namespace Illiance
