#pragma once

#include "MatterDeviceBase.h"
//#include "../MatterManager.h"
class AirQualitySensor;

class AirQualitySensorDelegate
{
    public:
        virtual ~AirQualitySensorDelegate() = default;
        
        virtual void OnTemperatureMeasuredValueChangedHandler(AirQualitySensor* airQualitySensor) = 0;
        virtual void OnHumidityMeasuredValueChangedHandler(AirQualitySensor* airQualitySensor) = 0;
        virtual void OnCarbonDioxideConcentrationChangedHandler(AirQualitySensor* airQualitySensor) = 0;
};

class AirQualitySensor : public MatterDevice
{
    public:
        AirQualitySensor() :    MatterDevice(MATTER_DEVICE_ID_AIR_QUALITY_SENSOR), 
                                mTemperatureSensor(this),
                                mRelativeHumiditySensor(this),
                                mCarbonDioxideSensor(this),
                                mAirQualitySensorDelegate(nullptr) {}
            // Custom copy constructor
        AirQualitySensor(const AirQualitySensor &other) : MatterDevice(MATTER_DEVICE_ID_AIR_QUALITY_SENSOR),
                                                      mTemperatureSensor(this),  // Initialize with the new object's this pointer
                                                      mRelativeHumiditySensor(this),
                                                      mCarbonDioxideSensor(this),
                                                      mAirQualitySensorDelegate(other.mAirQualitySensorDelegate)
        {
            // Copy the measured values if needed
            mTemperatureSensor.SetMeasuredValue(other.mTemperatureSensor.GetMeasuredValue());
            mRelativeHumiditySensor.SetMeasuredValue(other.mRelativeHumiditySensor.GetMeasuredValue());
            mCarbonDioxideSensor.SetMeasuredValue(other.mCarbonDioxideSensor.GetMeasuredValue());
        }

        void RegisterAirQualitySensorDelegate(AirQualitySensorDelegate* airQualitySensorDelegate) { mAirQualitySensorDelegate = airQualitySensorDelegate; }
        AirQualitySensorDelegate* GetAirQualitySensorDelegate(void) const { return mAirQualitySensorDelegate; }
        std::unique_ptr<MatterDevice> clone() const override 
        {
            return std::make_unique<AirQualitySensor>(*this);
        }

        struct TemperatureSensor
        {
            public:
                TemperatureSensor(AirQualitySensor* airQualitySensor) : mTemperatureCelsius(INT16_MIN),
                                                                        mSubscriptionId(0),
                                                                        mAirQualitySensor(airQualitySensor) {}
                void SetMeasuredValue(int16_t temperatureCelsius)
                { 
                    if (mTemperatureCelsius != temperatureCelsius)
                    {
                        mTemperatureCelsius = temperatureCelsius;

                        if(mAirQualitySensor != nullptr && mAirQualitySensor->GetAirQualitySensorDelegate() != nullptr)
                        {
                            mAirQualitySensor->GetAirQualitySensorDelegate()->OnTemperatureMeasuredValueChangedHandler(mAirQualitySensor);
                        }
                    }
                }

                int16_t GetMeasuredValue(void) const { return mTemperatureCelsius; }

                static void OnNewMeasurement(void* context, const chip::app::DataModel::Nullable<int16_t> & responseObject)
                {
                    AirQualitySensor* p_Device = static_cast<AirQualitySensor*>(context);

                    if (!responseObject.IsNull())
                    {
                        p_Device->GetTemperatureSensor().SetMeasuredValue(responseObject.Value() / 100);
                    }
                }

                static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
                {
                    ChipLogProgress(chipTool, "TemperatureSensor subscription read failure with %s", err.Format())
                }

                static void OnSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
                {
                    AirQualitySensor* p_Device = static_cast<AirQualitySensor*>(context);

                    p_Device->GetTemperatureSensor().mSubscriptionId = subscriptionId;
                }
            
            private:
                int16_t mTemperatureCelsius;
                chip::SubscriptionId mSubscriptionId;
                AirQualitySensor* mAirQualitySensor;
        };
        
        struct RelativeHumiditySensor
        {
            public:
                RelativeHumiditySensor(AirQualitySensor* airQualitySensor) :    mRelativeHumidityPercent(UINT16_MAX),
                                                                                mSubscriptionId(0),
                                                                                mAirQualitySensor(airQualitySensor) {}

                void SetMeasuredValue(int16_t relativeHumidityPercent) 
                { 
                    if (mRelativeHumidityPercent != relativeHumidityPercent)
                    {
                        mRelativeHumidityPercent = relativeHumidityPercent;

                        if(mAirQualitySensor != nullptr && mAirQualitySensor->GetAirQualitySensorDelegate() != nullptr)
                        {
                            mAirQualitySensor->GetAirQualitySensorDelegate()->OnHumidityMeasuredValueChangedHandler(mAirQualitySensor);
                        }
                    }
                }

                uint16_t GetMeasuredValue(void) const { return mRelativeHumidityPercent; }

                static void OnNewMeasurement(void* context, const chip::app::DataModel::Nullable<uint16_t> & responseObject)
                {
                    AirQualitySensor* p_Device = static_cast<AirQualitySensor*>(context);

                    if (!responseObject.IsNull())
                    {
                        p_Device->GetRelativeHumiditySensor().SetMeasuredValue(responseObject.Value()/100);
                    }
                }

                static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
                {
                    ChipLogProgress(chipTool, "RelativeHumiditySensor subscription read failure with %s", err.Format())
                }
                
                static void OnSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
                {
                    AirQualitySensor* p_Device = static_cast<AirQualitySensor*>(context);
                    p_Device->GetRelativeHumiditySensor().mSubscriptionId = subscriptionId;
                }

                chip::SubscriptionId GetSubscriptionId(void) { return mSubscriptionId; }

            private:
                uint16_t mRelativeHumidityPercent;
                chip::SubscriptionId mSubscriptionId;
                AirQualitySensor* mAirQualitySensor;
        };

        struct CarbonDioxideSensor
        {
            public:
                CarbonDioxideSensor(AirQualitySensor* airQualitySensor) :   mCo2ppm(FLT_MIN),
                                                                            mSubscriptionId(0),
                                                                            mAirQualitySensor(airQualitySensor) {}

                void SetMeasuredValue(float co2ppm) 
                { 
                    if (mCo2ppm != co2ppm)
                    {
                        mCo2ppm = co2ppm; 

                        if(mAirQualitySensor != nullptr && mAirQualitySensor->GetAirQualitySensorDelegate() != nullptr)
                        {
                            mAirQualitySensor->GetAirQualitySensorDelegate()->OnCarbonDioxideConcentrationChangedHandler(mAirQualitySensor);
                        }
                        if (mAirQualitySensor->GetAirQualitySensorDelegate() == nullptr)
                        {
                            ChipLogProgress(chipTool, "co2: mAirQualitySensorDelegate addr: %p", (void*) mAirQualitySensor->GetAirQualitySensorDelegate());
                        }
                    }
                }
                
                float GetMeasuredValue(void) const { return mCo2ppm; }

                static void OnNewMeasurement(void* context, const chip::app::DataModel::Nullable<float>& responseObject)
                {
                    AirQualitySensor* p_Device = static_cast<AirQualitySensor*>(context);

                    if (!responseObject.IsNull())
                    {
                        p_Device->GetCarbonDioxideSensor().SetMeasuredValue(responseObject.Value());
                    }
                }
                
                static void OnSubscriptionReadFailure(void* context, CHIP_ERROR err)
                {
                    ChipLogProgress(chipTool, "CarbonDioxideSensor subscription read failure with %s", err.Format())
                }

                static void OnSubscriptionEstablished(void* context, chip::SubscriptionId subscriptionId)
                {
                    AirQualitySensor* p_Device = static_cast<AirQualitySensor*>(context);
                    p_Device->GetCarbonDioxideSensor().mSubscriptionId = subscriptionId;
                }

                chip::SubscriptionId GetSubscriptionId(void) { return mSubscriptionId; }

            private:
                float mCo2ppm;
                chip::SubscriptionId mSubscriptionId;
                AirQualitySensor* mAirQualitySensor;
        };

        TemperatureSensor & GetTemperatureSensor(void) { return mTemperatureSensor; }
        RelativeHumiditySensor & GetRelativeHumiditySensor(void) { return mRelativeHumiditySensor; }
        CarbonDioxideSensor & GetCarbonDioxideSensor(void) { return mCarbonDioxideSensor; }

        //We're not subscribing to air quality measurements since we don't need it

    private:
        TemperatureSensor mTemperatureSensor;
        RelativeHumiditySensor mRelativeHumiditySensor;
        CarbonDioxideSensor mCarbonDioxideSensor;
        AirQualitySensorDelegate* mAirQualitySensorDelegate;
};
