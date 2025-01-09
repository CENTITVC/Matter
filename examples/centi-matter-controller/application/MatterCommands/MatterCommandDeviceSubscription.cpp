#include "MatterCommandDeviceSubscription.h"
#include "../MatterClient/MatterClientFactory.h"

CHIP_ERROR MatterSubscriptionDevice::Run()
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    
    ChipLogProgress(chipTool, "MatterSubscriptionDevice::Run");

    mMatterDevice = MatterManager::MatterMgr().GetMatterNode(mNodeId)->GetEndpoint(mEndpointId)->GetMatterDevice(mDeviceTypeId);

    if (mMatterDevice == nullptr)
    {
        ChipLogProgress(chipTool, "mMatterDevice is null");
        return CHIP_NO_ERROR;
    }

    switch(mDeviceTypeId)
    {
        case MATTER_DEVICE_ID_AIR_QUALITY_SENSOR:
            //err = SubscribeToAirQualitySensor();
            break;
        case MATTER_DEVICE_ID_CONTACT_SENSOR:
            err = SubscribeToContactSensor();
            break;
        case MATTER_DEVICE_ID_DOOR_LOCK:
            err = SubscribeToDoorLock();
            break;
        case MATTER_DEVICE_ID_EXTENDED_COLOR_LIGHT:
            err = SubscribeToExtendedColorLight();
            break;
        case MATTER_DEVICE_ID_ELECTRICAL_SENSOR:
            err = SubscribeToElectricalSensor();
            break;
        case MATTER_DEVICE_ID_HUMIDITY_SENSOR:
            err = SubscribeToHumiditySensor();
            break;
        case MATTER_DEVICE_ID_LIGHT_SENSOR:
            err = SubscribeToLightSensor();
            break;
        case MATTER_DEVICE_ID_OCCUPANCY_SENSOR:
            //err = SubscribeToOccupancySensor();
            break;
        case MATTER_DEVICE_ID_THERMOSTAT:
            err = SubscribeToThermostatDevice();
            break;
        case MATTER_DEVICE_ID_WINDOW_COVERING:
            err = SubscribeToWindowCover();
            break;
        default:
            ChipLogProgress(NotSpecified, "Can't subscribe to MatterDevice with deviceType=" ChipLogFormatMEI, ChipLogValueMEI(mMatterDevice->GetType()));
            break;
    }

    notifyComplete(err);

    return err;
}

CHIP_ERROR MatterSubscriptionDevice::SubscribeToAirQualitySensor(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "MatterSubscriptionDevice::SubscribeToAirQualitySensor");

    MatterNode* p_node = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDieWithMsg(p_node != nullptr, chipTool, "MatterNode is null!");

    TemperatureClient* temperatureClient = MatterClientFactory::GetInstance().GetOrCreateTemperatureClient(*p_node);
    VerifyOrDieWithMsg(temperatureClient != nullptr, chipTool, "temperatureClient is null!");

    RelativeHumidityClient* relativeHumidityClient = MatterClientFactory::GetInstance().GetOrCreateRelativeHumidityClient(*p_node);
    VerifyOrDieWithMsg(relativeHumidityClient != nullptr, chipTool, "relativeHumidityClient is null!");
    
    CarbonDioxideClient* carbonDioxideClient = MatterClientFactory::GetInstance().GetOrCreateCarbonDioxideClient(*p_node);
    VerifyOrDieWithMsg(carbonDioxideClient != nullptr, chipTool, "carbonDioxideClient is null!");

    AirQualitySensor* airQualitySensor = static_cast<AirQualitySensor*>(mMatterDevice);

    err = temperatureClient->SubscribeMeasuredValue(mEndpointId, airQualitySensor,
                                                    airQualitySensor->GetTemperatureSensor().OnNewMeasurement,
                                                    airQualitySensor->GetTemperatureSensor().OnSubscriptionReadFailure,
                                                    mMinIntervalFloorSeconds,
                                                    mMaxIntervalCeilingSeconds,
                                                    airQualitySensor->GetTemperatureSensor().OnSubscriptionEstablished,
                                                    nullptr);
    ReturnLogErrorOnFailure(err);

    err = relativeHumidityClient->SubscribeMeasuredValue(mEndpointId, airQualitySensor,
                                                    &airQualitySensor->GetRelativeHumiditySensor().OnNewMeasurement,
                                                    &airQualitySensor->GetRelativeHumiditySensor().OnSubscriptionReadFailure,
                                                    mMinIntervalFloorSeconds,
                                                    mMaxIntervalCeilingSeconds,
                                                    &airQualitySensor->GetRelativeHumiditySensor().OnSubscriptionEstablished,
                                                    nullptr);
    ReturnLogErrorOnFailure(err);

    err = carbonDioxideClient->SubscribeMeasuredValue(mEndpointId, airQualitySensor,
                                                    &airQualitySensor->GetCarbonDioxideSensor().OnNewMeasurement,
                                                    &airQualitySensor->GetCarbonDioxideSensor().OnSubscriptionReadFailure,
                                                    mMinIntervalFloorSeconds,
                                                    mMaxIntervalCeilingSeconds,
                                                    &airQualitySensor->GetCarbonDioxideSensor().OnSubscriptionEstablished,
                                                    nullptr);
    ReturnLogErrorOnFailure(err);

    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterSubscriptionDevice::SubscribeToContactSensor(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "MatterSubscriptionDevice::SubscribeToContactSensor");

    MatterNode* p_node = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDieWithMsg(p_node != nullptr, chipTool, "MatterNode is null!");

    BooleanStateClient* booleanStateClient = MatterClientFactory::GetInstance().GetOrCreateBooleanStateClient(*p_node);
    VerifyOrDieWithMsg(booleanStateClient != nullptr, chipTool, "booleanStateClient is null!");

    ContactSensor* contactSensor = static_cast<ContactSensor*>(mMatterDevice);

    err = booleanStateClient->SubscribeStateValue(mEndpointId, contactSensor,
                                                    &contactSensor->OnNewStateValue,
                                                    &contactSensor->OnSubscriptionReadFailure,
                                                    mMinIntervalFloorSeconds,
                                                    mMaxIntervalCeilingSeconds,
                                                    &contactSensor->OnContactSensorStateChangeSubscriptionEstablished,
                                                    nullptr);    
    ReturnLogErrorOnFailure(err);

    return CHIP_NO_ERROR;
}


CHIP_ERROR MatterSubscriptionDevice::SubscribeToDoorLock(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "MatterSubscriptionDevice::SubscribeToDoorLock");

    MatterNode* p_node = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDieWithMsg(p_node != nullptr, chipTool, "MatterNode is null!");

    DoorLockClient* doorLockClient = MatterClientFactory::GetInstance().GetOrCreateDoorLockClient(*p_node);
    VerifyOrDieWithMsg(doorLockClient != nullptr, chipTool, "doorLockClient is null!");

    DoorLock* doorLock = static_cast<DoorLock*>(mMatterDevice);

    err = doorLockClient->SubscribeDoorState(mEndpointId, doorLock,
                                            &doorLock->OnNewMeasurement,
                                            &doorLock->OnSubscriptionReadFailure,
                                            mMinIntervalFloorSeconds,
                                            mMaxIntervalCeilingSeconds,
                                            &doorLock->OnDoorLockStateSubscriptionEstablished,
                                            nullptr);
    ReturnLogErrorOnFailure(err);

    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterSubscriptionDevice::SubscribeToElectricalSensor(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "MatterSubscriptionDevice::SubscribeToElectricalSensor");

    MatterNode* p_node = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDieWithMsg(p_node != nullptr, chipTool, "MatterNode is null!");

    ElectricalPowerClient* electricalPowerClient = MatterClientFactory::GetInstance().GetOrCreateElectricalPowerClient(*p_node);
    VerifyOrDieWithMsg(electricalPowerClient != nullptr, chipTool, "electricalPowerClient is null!");

    ElectricalSensor* electricalSensor = static_cast<ElectricalSensor*>(mMatterDevice);

    err = electricalPowerClient->SubscribeActivePower(  mEndpointId, electricalSensor,
                                                        &electricalSensor->OnNewMeasurement,
                                                        &electricalSensor->OnSubscriptionReadFailure,
                                                        mMinIntervalFloorSeconds,
                                                        mMaxIntervalCeilingSeconds,
                                                        &electricalSensor->OnActivePowerSubscriptionEstablished,
                                                        nullptr);
    ReturnLogErrorOnFailure(err);

    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterSubscriptionDevice::SubscribeToExtendedColorLight(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "MatterSubscriptionDevice::SubscribeToExtendedColorLight");

    MatterNode* p_node = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDieWithMsg(p_node != nullptr, chipTool, "MatterNode is null!");

    LevelControlClient* levelControlClient = MatterClientFactory::GetInstance().GetOrCreateLevelControlClient(*p_node);
    VerifyOrDieWithMsg(levelControlClient != nullptr, chipTool, "levelControlClient is null!");

    ColorControlClient* colorControlClient = MatterClientFactory::GetInstance().GetOrCreateColorControlClient(*p_node);
    VerifyOrDieWithMsg(colorControlClient != nullptr, chipTool, "colorControlClient is null!");

    OnOffClient* onOffClient = MatterClientFactory::GetInstance().GetOrCreateOnOffClient(*p_node);
    VerifyOrDieWithMsg(onOffClient != nullptr, chipTool, "onOffClient is null!");

    ExtendedColorLight* extendedColorLight = static_cast<ExtendedColorLight*>(mMatterDevice);

    err = levelControlClient->SubscribeCurrentLevel(mEndpointId, extendedColorLight,
                                                    &extendedColorLight->OnNewCurrentLevel,
                                                    &extendedColorLight->OnSubscriptionReadFailure,
                                                    mMinIntervalFloorSeconds,
                                                    mMaxIntervalCeilingSeconds,
                                                    &extendedColorLight->OnCurrentLevelSubscriptionEstablished,
                                                    nullptr);
    ReturnLogErrorOnFailure(err);

    err = colorControlClient->SubscribeCurrentHue(mEndpointId, extendedColorLight,
                                                &extendedColorLight->OnNewCurrentHue,
                                                &extendedColorLight->OnSubscriptionReadFailure,
                                                mMinIntervalFloorSeconds,
                                                mMaxIntervalCeilingSeconds,
                                                &extendedColorLight->OnCurrentHueSubscriptionEstablished,
                                                nullptr);
    ReturnLogErrorOnFailure(err);

    err = colorControlClient->SubscribeCurrentSaturation(mEndpointId, extendedColorLight,
                                                        &extendedColorLight->OnNewCurrentSaturation,
                                                        &extendedColorLight->OnSubscriptionReadFailure,
                                                        mMinIntervalFloorSeconds,
                                                        mMaxIntervalCeilingSeconds,
                                                        &extendedColorLight->OnCurrentSaturationSubscriptionEstablished,
                                                        nullptr);
    ReturnLogErrorOnFailure(err);

    err = onOffClient->SubscribeOnOff(mEndpointId, extendedColorLight,
                                      &extendedColorLight->OnNewOnOff,
                                      &extendedColorLight->OnSubscriptionReadFailure,
                                      mMinIntervalFloorSeconds,
                                      mMaxIntervalCeilingSeconds,
                                      &extendedColorLight->OnOnOffSubscriptionEstablished,
                                      nullptr);
    ReturnLogErrorOnFailure(err);

    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterSubscriptionDevice::SubscribeToHumiditySensor(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "MatterSubscriptionDevice::SubscribeToHumiditySensor");

    MatterNode* p_node = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDieWithMsg(p_node != nullptr, chipTool, "MatterNode is null!");

    RelativeHumidityClient* relativeHumidityClient = MatterClientFactory::GetInstance().GetOrCreateRelativeHumidityClient(*p_node);
    VerifyOrDieWithMsg(relativeHumidityClient != nullptr, chipTool, "relativeHumidityClient is null!");

    HumiditySensor* humiditySensor = static_cast<HumiditySensor*>(mMatterDevice);

    err = relativeHumidityClient->SubscribeMeasuredValue(mEndpointId, humiditySensor,
                                                    &humiditySensor->OnNewMeasurement,
                                                    &humiditySensor->OnSubscriptionReadFailure,
                                                    mMinIntervalFloorSeconds,
                                                    mMaxIntervalCeilingSeconds,
                                                    &humiditySensor->OnHumidityMeasurementSubscriptionEstablished,
                                                    nullptr);
    ReturnLogErrorOnFailure(err);

    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterSubscriptionDevice::SubscribeToLightSensor(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "MatterSubscriptionDevice::SubscribeToLightSensor");

    MatterNode* p_node = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDieWithMsg(p_node != nullptr, chipTool, "MatterNode is null!");

    IlluminanceClient* illuminanceClient = MatterClientFactory::GetInstance().GetOrCreateIlluminanceClient(*p_node);
    VerifyOrDieWithMsg(illuminanceClient != nullptr, chipTool, "illuminanceClient is null!");

    LightSensor* lightSensor = static_cast<LightSensor*>(mMatterDevice);

    err = illuminanceClient->SubscribeMeasuredValue(mEndpointId, lightSensor,
                                                    &lightSensor->OnNewMeasurement,
                                                    &lightSensor->OnSubscriptionReadFailure,
                                                    mMinIntervalFloorSeconds,
                                                    mMaxIntervalCeilingSeconds,
                                                    &lightSensor->OnIlluminanceMeasurementSubscriptionEstablished,
                                                    nullptr);
    ReturnLogErrorOnFailure(err);

    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterSubscriptionDevice::SubscribeToOccupancySensor(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "MatterSubscriptionDevice::SubscribeToOccupancySensor");

    MatterNode* p_node = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDieWithMsg(p_node != nullptr, chipTool, "MatterNode is null!");

    OccupancySensingClient* occupancySensingClient = MatterClientFactory::GetInstance().GetOrCreateOccupancySensingClient(*p_node);
    VerifyOrDieWithMsg(occupancySensingClient != nullptr, chipTool, "occupancySensingClient is null!");

    OccupancySensor* occupancySensor = static_cast<OccupancySensor*>(mMatterDevice);

    err = occupancySensingClient->SubscribeOccupancy(mEndpointId, occupancySensor,
                                                    &occupancySensor->OnNewMeasurement,
                                                    &occupancySensor->OnSubscriptionReadFailure,
                                                    mMinIntervalFloorSeconds,
                                                    mMaxIntervalCeilingSeconds,
                                                    &occupancySensor->OnOccupancyStateSubscriptionEstablished,
                                                    nullptr);
    ReturnLogErrorOnFailure(err);

    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterSubscriptionDevice::SubscribeToTemperatureSensor(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "MatterSubscriptionDevice::SubscribeToTemperatureSensor");

    MatterNode* p_node = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDieWithMsg(p_node != nullptr, chipTool, "MatterNode is null!");

    TemperatureClient* temperatureClient = MatterClientFactory::GetInstance().GetOrCreateTemperatureClient(*p_node);
    VerifyOrDieWithMsg(temperatureClient != nullptr, chipTool, "temperatureClient is null!");

    TemperatureSensor* temperatureSensor = static_cast<TemperatureSensor*>(mMatterDevice);

    err = temperatureClient->SubscribeMeasuredValue(mEndpointId, temperatureSensor,
                                                    &temperatureSensor->OnNewMeasurement,
                                                    &temperatureSensor->OnSubscriptionReadFailure,
                                                    mMinIntervalFloorSeconds,
                                                    mMaxIntervalCeilingSeconds,
                                                    &temperatureSensor->OnTemperatureMeasurementSubscriptionEstablished,
                                                    nullptr);
    ReturnLogErrorOnFailure(err);

    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterSubscriptionDevice::SubscribeToThermostatDevice(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "MatterSubscriptionDevice::SubscribeToThermostatDevice");

    MatterNode* p_node = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDieWithMsg(p_node != nullptr, chipTool, "MatterNode is null!");

    ThermostatClient* thermostatClient = MatterClientFactory::GetInstance().GetOrCreateThermostatClient(*p_node);
    VerifyOrDieWithMsg(thermostatClient != nullptr, chipTool, "thermostatClient is null!");

    ThermostatDevice* thermostatDevice = static_cast<ThermostatDevice*>(mMatterDevice);

    err = thermostatClient->SubscribeOccupiedHeatingSetpoint(
                                            mEndpointId, thermostatDevice,
                                            &thermostatDevice->OnNewOccupiedHeatingSetpoint,
                                            &thermostatDevice->OnSubscriptionReadFailure,
                                            mMinIntervalFloorSeconds,
                                            mMaxIntervalCeilingSeconds,
                                            &thermostatDevice->OnOccupiedHeatingSetpointSubscriptionEstablished,
                                            nullptr);
    ReturnLogErrorOnFailure(err);

    err = thermostatClient->SubscribeLocalTemperature(
                                            mEndpointId, thermostatDevice,
                                            &thermostatDevice->OnNewLocalTemperature,
                                            &thermostatDevice->OnSubscriptionReadFailure,
                                            mMinIntervalFloorSeconds,
                                            mMaxIntervalCeilingSeconds,
                                            &thermostatDevice->OnLocalTemperatureSubscriptionEstablished,
                                            nullptr);
    ReturnLogErrorOnFailure(err);

    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterSubscriptionDevice::SubscribeToWindowCover(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    ChipLogProgress(chipTool, "MatterSubscriptionDevice::SubscribeToWindowCover");

    MatterNode* p_node = MatterManager::MatterMgr().GetMatterNode(mNodeId);
    VerifyOrDieWithMsg(p_node != nullptr, chipTool, "MatterNode is null!");

    WindowCoveringClient* windowCoveringClient = MatterClientFactory::GetInstance().GetOrCreateWindowCoveringClient(*p_node);
    VerifyOrDieWithMsg(windowCoveringClient != nullptr, chipTool, "windowCoveringClient is null!");

    WindowCover* windowCover = static_cast<WindowCover*>(mMatterDevice);

    err = windowCoveringClient->SubscribeCurrentPositionLiftPercentage100ths(
                                            mEndpointId, windowCover,
                                            &windowCover->OnNewCurrentLiftPositionPercent100ths,
                                            &windowCover->OnSubscriptionReadFailure,
                                            mMinIntervalFloorSeconds,
                                            mMaxIntervalCeilingSeconds,
                                            &windowCover->OnCurrentPositionPercent100thsSubscriptionEstablished,
                                            nullptr);
    ReturnLogErrorOnFailure(err);

    return CHIP_NO_ERROR;
}
