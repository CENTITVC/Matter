#pragma once

#include <cstdlib>
#include <string>
#include "../application/MatterCommands/MatterLightSettings.h"

enum class MqttCommandType : uint8_t
{
    Unknown = 0,
    LightSetting,
    SetWindowPosition,
    MatterCommissionNode,
    MatterOpenCommissioning,
    MatterRemoveNode,
};


class MqttCommandBase
{
    public:
        MqttCommandBase() {}
        virtual ~MqttCommandBase() = default;
        virtual MqttCommandType GetType() = 0;
};

class MqttCommandLightSetting : public MqttCommandBase
{
    public:
        MqttCommandLightSetting(uint64_t nodeId, LightSettings lightSettings ) : 
                                            mNodeId(nodeId), 
                                            mLightSettings(lightSettings) {}

        MqttCommandType GetType() override { return MqttCommandType::LightSetting; }
        
        uint64_t mNodeId;
        LightSettings mLightSettings;
};


class MqttCommandSetWindowPosition : public MqttCommandBase
{
    public:
        MqttCommandSetWindowPosition(uint64_t nodeId, uint8_t pos) : mNodeId(nodeId), mPosition(pos) {} 
        MqttCommandType GetType() override { return MqttCommandType::SetWindowPosition; }
        uint64_t mNodeId;
        uint8_t  mPosition;         
};

class MqttCommandMatterCommission : public MqttCommandBase
{
    public:
        MqttCommandMatterCommission(std::string setUpCode) : mSetUpCode(setUpCode) {} 
        MqttCommandType GetType() override { return MqttCommandType::MatterCommissionNode; }
        std::string mSetUpCode;
};

class MqttCommandMatterRemove : public MqttCommandBase
{
    public:
        MqttCommandMatterRemove(uint64_t nodeId) :  mNodeId(nodeId) {} 
        MqttCommandType GetType() override { return MqttCommandType::MatterRemoveNode; }
        uint64_t mNodeId;
};

class MqttCommandMatterCommissionOpen : public MqttCommandBase
{
    public:
        MqttCommandMatterCommissionOpen(uint64_t nodeId) : mNodeId(nodeId) {}
        MqttCommandType GetType() override { return MqttCommandType::MatterOpenCommissioning; }
        uint64_t mNodeId;
};

class MqttCommandUnknown : public MqttCommandBase
{
    public:
        MqttCommandUnknown() {} 
        MqttCommandType GetType() override { return MqttCommandType::Unknown; }
};
