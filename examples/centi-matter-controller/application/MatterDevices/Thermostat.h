#pragma once

#include "MatterDeviceBase.h"
#include <controller/CHIPCluster.h>

class ThermostatDevice; //Forward declaration

class ThermostatDelegate {
    public:
        virtual ~ThermostatDelegate() = default;

        virtual void OnOccupiedHeatingSetpointChangedHandler(ThermostatDevice* thermostat) = 0;
        virtual void OnLocalTemperatureChangedHandler(ThermostatDevice* thermostat) = 0;
};

class ThermostatDevice : public MatterDevice
{
    public:
        ThermostatDevice() : MatterDevice(MATTER_DEVICE_ID_THERMOSTAT),
                            mOccupiedHeatingSetpointCelsius(INT16_MIN),
                            mOccupiedHeatingSetpointSubscriptionId(0),
                            mThermostatDelegate(nullptr) {}

        void RegisterThermostatDelegate(ThermostatDelegate* thermostatDelegate) { mThermostatDelegate = thermostatDelegate; }
        ThermostatDelegate* GetThermostatDelegate(void) { return mThermostatDelegate; }
        std::unique_ptr<MatterDevice> clone() const override { return std::make_unique<ThermostatDevice>(*this); }

        /** Occupied Heating Setpoint Attribute - Matter Application Cluster Chapter 4.3 */ 

        void SetOccupiedHeatingSetpoint(int16_t temperatureCelsius) 
        { 
            if (mOccupiedHeatingSetpointCelsius != temperatureCelsius)
            {
                mOccupiedHeatingSetpointCelsius = temperatureCelsius;

                if (mThermostatDelegate != nullptr)
                {
                    mThermostatDelegate->OnOccupiedHeatingSetpointChangedHandler(this);
                }
            }
        }

        int16_t GetOccupiedHeatingSetpoint(void) { return mOccupiedHeatingSetpointCelsius; }

		static void OnNewOccupiedHeatingSetpoint(void* context, int16_t responseObject)
        {
			ThermostatDevice* p_Device = static_cast<ThermostatDevice*>(context);

            if (p_Device != nullptr)
            {
                p_Device->SetOccupiedHeatingSetpoint(responseObject);
            }
        }

        static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
        {
            ChipLogProgress(chipTool, "TemperatureSensor subscription read failure with %s", err.Format())
        }

        static void OnOccupiedHeatingSetpointSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
			ThermostatDevice* p_device = static_cast<ThermostatDevice*>(context);

            p_device->mOccupiedHeatingSetpointSubscriptionId = subscriptionId;
        }

        chip::SubscriptionId GetOccupiedHeatingSetpointSubId(void) { return mOccupiedHeatingSetpointSubscriptionId; }

        /** Local Temperature Attribute - Matter Application Cluster Chapter 4.3 */ 

        void SetLocalTemperature(int16_t temperatureCelsius) 
        { 
            if (mLocalTemperatureCelsius != temperatureCelsius)
            {
                mLocalTemperatureCelsius = temperatureCelsius;

                if (mThermostatDelegate != nullptr)
                {
                    mThermostatDelegate->OnLocalTemperatureChangedHandler(this);
                }
            }
        }

        int16_t GetLocalTemperature(void) { return mLocalTemperatureCelsius; }

        static void OnNewLocalTemperature(void* context, const chip::app::DataModel::Nullable<int16_t> & responseObject)
        {
            ThermostatDevice* p_Device = static_cast<ThermostatDevice*>(context);

            if (p_Device != nullptr && !responseObject.IsNull())
            {
                p_Device->SetLocalTemperature(responseObject.Value());
            }
        }

        static void OnLocalTemperatureSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
        {
            ThermostatDevice* p_device = static_cast<ThermostatDevice*>(context);

            p_device->mLocalTemperatureSubscriptionId = subscriptionId;
        }

        chip::SubscriptionId GetLocalTemperatureSubId(void) { return mLocalTemperatureSubscriptionId; }

    private:
        int16_t mOccupiedHeatingSetpointCelsius;
        int16_t mLocalTemperatureCelsius;
        chip::SubscriptionId mOccupiedHeatingSetpointSubscriptionId;
        chip::SubscriptionId mLocalTemperatureSubscriptionId;
        ThermostatDelegate* mThermostatDelegate;
};