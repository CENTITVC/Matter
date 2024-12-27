#pragma once

#include <lib/support/CodeUtils.h>
#include <controller/CHIPCluster.h>
#include "MatterNode.h"
#include "MatterDevice.h"

namespace CentiMatter
{
class MatterDeviceSubscriptionCallbacks
{
    public:
		static MatterDeviceSubscriptionCallbacks & Instance(void)
		{
			static MatterDeviceSubscriptionCallbacks sMatterDeviceSubscriptionCallbacks;
			return sMatterDeviceSubscriptionCallbacks;
		}

        CHIP_ERROR SubscribeToNodeDevices(chip::NodeId nodeId);

    private:
		static void OnNewTemperature(void* context, const chip::app::Clusters::TemperatureMeasurement::Attributes::MeasuredValue::TypeInfo::DecodableArgType responseObject)
		{
			MatterDevice* p_Device = static_cast<MatterDevice*>(context);

			if (!responseObject.IsNull())
			{
				chip::Percent100ths position = 0;
				
				p_Device->SetTemperature(responseObject.Value());
			}
		}

        static void OnSubscriptionEstablishedTemperature(void* context, chip::SubscriptionId subscriptionId);

		static void OnNewRelativeHumidity(void* context, const chip::app::Clusters::RelativeHumidityMeasurement::Attributes::MeasuredValue::TypeInfo::DecodableArgType responseObject);
        static void OnSubscriptionEstablishedRelativeHumidity(void* context, chip::SubscriptionId subscriptionId);

        static void OnNewCurrentPositionLiftPercentage100ths(void* context, const chip::app::Clusters::WindowCovering::Attributes::CurrentPositionLiftPercent100ths::TypeInfo::DecodableArgType responseObject);
        static void OnSubscriptionEstablishedCurrentPositionLiftPercentage100ths(void* context, chip::SubscriptionId subscriptionId);

};
} //namespace CentiMatter
