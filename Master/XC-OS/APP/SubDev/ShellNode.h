#ifndef __SHELLNODE_H__
#define __SHELLNODE_H__

#ifdef _WIN32
#  define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"

#define ShellNode_EnableAddr                1

/* 最大命令数目 */
#define ShellNode_CommandCount              8
/* 发送 / 接收 缓冲区长度 */
#define ShellNode_TXFIFOLength              128
#define ShellNode_RXFIFOLength              128
/* 数据包最大长度 */
#define ShellNode_TXPacketLength            64
#define ShellNode_RXPacketLength            64

#if( ShellNode_EnableAddr == 0 )
/* 空数据包大小，包括：数据包长度（1 Byte）、命令（1 Byte）、数据包CRC8值（1 Byte）共 3 字节 */
#define ShellNode_EmptyValidPacketLength    3
#else
/* 空数据包大小，包括：数据包长度（1 Byte）、命令（1 Byte）、数据包源地址（1 Byte）、数据包目标地址（1 Byte）、数据包CRC8值（1 Byte）共 5 字节 */
#define ShellNode_EmptyValidPacketLength    5
#endif

#define ShellNode_BroadcastAddr             0

#define ShellNode_DefaultNodeName           "UnknownNode"

/* 节点名称字符串最大长度 */
#define ShellNode_StringLength              16
/* 链路层帧起始字节 */
#define ShellNode_FrameBeginByte            '['
/* 链路层帧结束字节 */
#define ShellNode_FrameEndByte              ']'
/* 转义符 */
#define ShellNode_EscapeByte                '\\'
/* 函数调用相关 */
#define ShellNode_Function_ParameterUnitByte    4
#define ShellNode_Function_MaxCount             1
#define ShellNode_Function_MaxParameterLength   64
#define ShellNode_Function_MaxRetvalLength      32
/* 消息传递相关 */
#define ShellNode_Message_MaxCount              8
#define ShellNode_Message_MaxLength             32

typedef struct
{
    void* StartAddr;
    uint8_t ParameterCount;
    uint32_t ParameterLength;
    uint32_t RetvalLength;
    uint8_t ParameterLengthUnitTimes[ShellNode_Function_MaxParameterLength / ShellNode_Function_ParameterUnitByte];
    char Name[ShellNode_StringLength];
} ShellNode_Function_t;

/* 错误代码 */
enum ShellNode_ErrorCode_t
{
    ShellNode_ErrorCode_Success,                    // 帧处理成功
    ShellNode_ErrorCode_InvalidFrame,               // 未找到合法的帧
    ShellNode_ErrorCode_InvalidEscapeCharacter,     // 非法的转义符
    ShellNode_ErrorCode_RXPacketOutOfSize,          // 数据包长度过大
    ShellNode_ErrorCode_InvalidPacketLength,        // 数据包长度错误
    ShellNode_ErrorCode_PacketCRCFailure,           // 数据包 CRC 校验失败
    ShellNode_ErrorCode_UnsupportedCommand,         // 不受支持的命令
    ShellNode_ErrorCode_CommandListFull,            // 命令列表满
    ShellNode_ErrorCode_FunctionListFull,           // 函数列表满
    ShellNode_ErrorCode_MessageListFull,            // 消息列表满
    ShellNode_ErrorCode_EXE_ParameterOutOfSize,     // 执行函数时参数列表过长
    ShellNode_ErrorCode_EXE_RetvalOutOfSize,        // 执行函数时返回值过长
    ShellNode_ErrorCode_ParameterOutOfSize,         // 注册函数参数列表过长
    ShellNode_ErrorCode_RetvalOutOfSize,            // 注册函数返回值过长
    ShellNode_ErrorCode_InvalidFunctionIndex,       // 函数索引值非法
    ShellNode_ErrorCode_InvalidMessageIndex,        // 消息索引值非法
    ShellNode_ErrorCode_FunctionNameNotExist,       // 目标函数名称不存在
    ShellNode_ErrorCode_MessageNameNotExist,        // 目标消息名称不存在
    ShellNode_ErrorCode_MessageStackOverflow,       // 消息栈溢出
    ShellNode_ErrorCode_MessageStackEmpty,          // 已达到消息栈底
    ShellNode_ErrorCode_MessageLengthOutOfSize,     // 消息长度超过了目标消息类型允许的最大长度
    ShellNode_ErrorCode_MessageLengthTooShort,      // 消息长度小于目标消息类型允许的最小长度
    ShellNode_ErrorCode_Other,                      // 特殊错误
};
class ShellNode;
/* 命令服务函数类型 */
typedef void (*ShellNode_CommandHandler_t)(ShellNode*, uint8_t, bool, void*, uint32_t);
/* 发送字节服务函数 */
typedef void (*ShellNode_SendData_t)(ShellNode*, void*, uint32_t);
/* 消息服务函数 */
typedef void (*ShellNode_MessageHandler_t)(ShellNode*, void *, uint32_t);

/* 描述一种类型的消息 */
typedef struct
{
    ShellNode_MessageHandler_t Handler;
    char Name[ShellNode_StringLength];
    uint32_t MessageMinLength;
    uint32_t MessageMaxLength;
} ShellNode_MessageInfo_t;

/* 与命令 ID 取"或"，以表示对应命令的应答包 */
#define ShellNode_Command_ACK       0x80
/* 可用的命令的枚举 */
enum ShellNode_CommandCode_t
{
    ShellNode_DefaultCommands_Test,
    ShellNode_DefaultCommands_GetName,
    ShellNode_DefaultCommands_GetFunctionCount,
    ShellNode_DefaultCommands_GetFunctionInfo,
    ShellNode_DefaultCommands_CallFunction,
    ShellNode_DefaultCommands_GetMessageCount,
    ShellNode_DefaultCommands_GetMessageInfo,
    ShellNode_DefaultCommands_Messaging,
    ShellNode_CustomCommands_BEGIN_VAL
};
/* 描述一种命令 */
typedef struct
{
    ShellNode_CommandHandler_t Handler;     // 命令的服务函数
    uint8_t IdentCode;                      // 命令的 ID
} ShellNode_CommandInfo_t;

/* 描述一个数据包的起始部分 */
typedef struct
{
    uint8_t PacketLength;           // 数据包的总长度（单位：字节）
    uint8_t IdentCode;              // 数据包的命令 ID
} ShellNode_PacketHeader_t;

/* CRC8 表 */
/* CRC8 implementation with polynom = x^8+x^7+x^6+x^4+x^2+1 (0xD5) */
static const uint8_t crc8tab[256] =
{
    0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54,
    0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
    0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06,
    0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
    0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0,
    0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
    0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2,
    0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
    0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9,
    0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
    0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B,
    0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
    0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D,
    0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
    0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F,
    0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
    0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB,
    0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
    0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9,
    0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
    0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F,
    0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
    0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D,
    0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
    0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26,
    0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
    0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74,
    0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
    0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82,
    0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
    0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0,
    0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9
};
/* 定义了 1-16 个单元长度的结构体类型 */
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 1];
} ShellNode_FunctionInterface_1times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 2];
} ShellNode_FunctionInterface_2times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 3];
} ShellNode_FunctionInterface_3times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 4];
} ShellNode_FunctionInterface_4times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 5];
} ShellNode_FunctionInterface_5times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 6];
} ShellNode_FunctionInterface_6times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 7];
} ShellNode_FunctionInterface_7times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 8];
} ShellNode_FunctionInterface_8times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 9];
} ShellNode_FunctionInterface_9times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 10];
} ShellNode_FunctionInterface_10times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 11];
} ShellNode_FunctionInterface_11times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 12];
} ShellNode_FunctionInterface_12times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 13];
} ShellNode_FunctionInterface_13times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 14];
} ShellNode_FunctionInterface_14times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 15];
} ShellNode_FunctionInterface_15times;
typedef struct
{
    uint8_t DATA[ShellNode_Function_ParameterUnitByte * 16];
} ShellNode_FunctionInterface_16times;

/**
  * @brief  获取指定长度数据的 8 位循环冗余校验值
  * @param  data: 该数据的首地址
  * @param  length: 数据的长度（单位：字节）
  * @retval uint8_t: 8 位循环冗余校验值
  */
static uint8_t ShellNode_GetCRC8(const void* data, uint32_t length)
{
    uint8_t crc = 0;
    uint8_t* ptr = (uint8_t*)data;
    for (uint32_t i = 0; i < length; i++)
    {
        crc = crc8tab[crc ^ (*ptr++)];
    }
    return crc;
}
static void ShellNode_Memcpy(void* des, void* src, uint32_t len)
{
    for (int i = 0; i < len; i++)
    {
        ((uint8_t*)des)[i] = ((uint8_t*)src)[i];
    }
}
uint32_t ShellNode_GetRetvalLength(uint32_t pSize)
{
    uint32_t length = 0;
    while (pSize % ShellNode_Function_ParameterUnitByte)
    {
        pSize++;
    }
    length += pSize;
    return length;
}
uint32_t ShellNode_GetParameterLength(int pCount, ...)
{
    uint32_t length = 0;
    if (pCount)
    {
        va_list va;
        va_start(va, pCount);
        for (int i = 0; i < pCount; i++)
        {
            uint32_t pSize = va_arg(va, uint32_t);
            length += ShellNode_GetRetvalLength(pSize);
        }
        va_end(va);
    }
    return length;
}

// 默认命令服务函数的声明
static void ShellNode_DefaultHandler_Test(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength);
static void ShellNode_DefaultHandler_GetName(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength);
static void ShellNode_DefaultHandler_GetFunctionCount(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength);
static void ShellNode_DefaultHandler_CallFunction(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength);
static void ShellNode_DefaultHandler_GetFunctionInfo(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength);
static void ShellNode_DefaultHandler_GetMessageCount(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength);
static void ShellNode_DefaultHandler_GetMessageInfo(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength);
static void ShellNode_DefaultHandler_Messaging(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength);
class ShellNode
{
private:
public:
    ShellNode_SendData_t SendData;
    const char* NodeName;                               // 当前节点名称
    char ReceivedNodeName[ShellNode_StringLength];
    uint8_t NodeAddr;                                   // 当前节点地址（通常由节点名称字符串进行 CRC8 计算得到）
    uint8_t TargetAddr;                                 // 目标节点地址

    uint8_t TXBuffer[ShellNode_TXFIFOLength];           // 发送字节流 FIFO
    uint8_t RXBuffer[ShellNode_RXFIFOLength];           // 接受字节流 FIFO
    uint8_t LastTransmittedCommand;                     // 最近发送的命令
    uint8_t LastReceivedCommand;                        // 最近接收的命令
    volatile bool AckReceived;                          // 指示是否已收到应答包
    ShellNode_ErrorCode_t LastErrorCode;                // 最近一次系统命令服务函数报告的错误
    volatile bool FrameFound;                           // 指示是否找到合法帧
    volatile bool FrameHeaderFound;                     // 指示是否找到帧起始符
    uint32_t FrameHeaderPos;                            // 指示帧起始符位置
    uint32_t FrameTailPos;                              // 指示帧结束符位置
    uint32_t TXBufferLength;                            // 发送字节流长度
    uint32_t RXBufferPos;                               // 接收 FIFO 光标位置

    uint8_t TXPacketBuffer[ShellNode_TXPacketLength];   // 发送数据包缓冲区
    uint32_t TXPacketBufferLength;                      // 发送数据包长度
    uint8_t RXPacketBuffer[ShellNode_RXPacketLength];   // 接收数据包缓冲区
    uint32_t RXPacketPos;                               // 接收数据包缓冲区光标位置

    ShellNode_CommandInfo_t CommandsList[ShellNode_CommandCount];           // 注册的命令列表
    uint32_t CommandCount;                                                  // 命令列表的长度

    ShellNode_Function_t FunctionList[ShellNode_Function_MaxCount];         // 本机函数列表
    uint32_t FunctionCount;
    ShellNode_Function_t TargetFunctionList[ShellNode_Function_MaxCount];   // 目标函数列表
    uint32_t TargetFunctionCount;
    uint8_t FunctionRetval[ShellNode_Function_MaxRetvalLength];             // 返回值
    uint32_t FunctionRetvalLength;                                          // 返回值长度

    ShellNode_MessageInfo_t MessageList[ShellNode_Message_MaxCount];        // 支持的消息类型列表
    uint32_t MessageCount;                                                  // 消息类型列表长度
    ShellNode_MessageInfo_t TargetMessageList[ShellNode_Message_MaxCount];  // 目标消息类型列表
    uint32_t TargetMessageCount;                                            // 目标消息类型列表长度
    uint8_t Message[ShellNode_Message_MaxLength];                           // 准备发送的消息缓冲区
    uint32_t MessageSize;                                                   // 准备发送的消息缓冲区长度
    uint8_t TargetMessage[ShellNode_Message_MaxLength];                     // 接收的消息缓冲区
    uint32_t TargetMessagePos;                                              // 接收的消息缓冲区的光标
    uint32_t TargetMessageSize;                                             // 接收的消息缓冲区的长度
    uint32_t MessageElementCount;                                           // 准备发送的消息缓冲区内包含元素的个数
    uint32_t TargetMessageElementCount;                                     // 接收的消息缓冲区内包含元素的个数

    /**
      * @brief  初始化一个 ShellNode 实例。
      * @param  None
      * @retval ShellNode: 初始化完毕的 ShellNode 实例
      */
    ShellNode()
    {
        DeInit();
        SetNodeName(ShellNode_DefaultNodeName);
    }
    /**
      * @brief  以指定的节点名称、发送数据服务函数，来初始化一个 ShellNode 实例。
      * @param  nodeName: 指定的节点名称。节点名称默认用于创建该节点的地址(ADDR)值，当宏 ShellNode_EnableAddr 不为 0 时，正确的节点地址将成为节点间建立连接的必要条件，节点将不再接收非指定来源的包。
      * @param  sendData: 指定的发送数据服务函数。当执行相关通讯操作时，ShellNode 实例将直接调用此服务函数用来告知一个数据发送的需求。
      * @retval ShellNode: 初始化完毕的 ShellNode 实例
      */
    ShellNode(const char* nodeName, ShellNode_SendData_t sendData)
    {
        DeInit();
        SetNodeName(nodeName);
        SetSendDataHandler(sendData);
    }
    /**
      * @brief  重置 ShellNode 实例，以将其所有设置恢复到初始的状态。
      * @param  None
      * @retval None
      */
    void DeInit()
    {
        SendData = 0;
        TargetAddr = ShellNode_BroadcastAddr;
        ClearRXBuffer();
        RXPacketPos = 0;
        DeInitCommandList();
        FunctionCount = 0;
        TargetFunctionCount = 0;
        MessageCount = TargetMessageCount = 0;
        MessageSize = TargetMessagePos = TargetMessagePos = 0;
        ClearMessage();
    }
    /**
      * @brief  重置命令列表，清空 ShellNode 实例初始化后注册的所有自定义命令，从而仅保留系统命令。
      * @param  None
      * @retval None
      */
    void DeInitCommandList()
    {
        CommandCount = 0;
        RegisterCommand(ShellNode_DefaultCommands_Test, ShellNode_DefaultHandler_Test);
        RegisterCommand(ShellNode_DefaultCommands_GetName, ShellNode_DefaultHandler_GetName);
        RegisterCommand(ShellNode_DefaultCommands_GetFunctionCount, ShellNode_DefaultHandler_GetFunctionCount);
        RegisterCommand(ShellNode_DefaultCommands_CallFunction, ShellNode_DefaultHandler_CallFunction);
        RegisterCommand(ShellNode_DefaultCommands_GetFunctionInfo, ShellNode_DefaultHandler_GetFunctionInfo);
        RegisterCommand(ShellNode_DefaultCommands_GetMessageCount, ShellNode_DefaultHandler_GetMessageCount);
        RegisterCommand(ShellNode_DefaultCommands_GetMessageInfo, ShellNode_DefaultHandler_GetMessageInfo);
        RegisterCommand(ShellNode_DefaultCommands_Messaging, ShellNode_DefaultHandler_Messaging);
    }
    /**
      * @brief  设置发送数据的服务函数。
      * @param  sendData: 指定的发送数据服务函数。当执行相关通讯操作时，ShellNode 实例将直接调用此服务函数用来告知一个数据发送的需求。
      * @retval None
      */
    void SetSendDataHandler(ShellNode_SendData_t sendData)
    {
        SendData = sendData;
    }
    /**
      * @brief  注册一个自定义命令类型。
      * @param  cmdCode: 指定的命令 ID。命令 ID 将用于识别你的命令。
      * @param  handler: 指定的命令服务函数。当发送该命令时，ShellNode 实例将调用此服务函数来实现命令的响应。
      * @retval ShellNode_ErrorCode_t: 描述该过程可能出现的错误。
      */
    ShellNode_ErrorCode_t RegisterCommand(uint8_t cmdCode, ShellNode_CommandHandler_t handler)
    {
        ShellNode_ErrorCode_t errCode = ShellNode_ErrorCode_Success;
        if (CommandCount < ShellNode_CommandCount)
        {
            CommandsList[CommandCount].Handler = handler;
            CommandsList[CommandCount].IdentCode = cmdCode;
            CommandCount++;
        }
        else
        {
            errCode = ShellNode_ErrorCode_CommandListFull;
        }
        return errCode;
    }
    /**
      * @brief  （停用的）注册一个函数。以实现穿透的调用。
      * @param  startAddr: 指定函数的起始地址。
      * @param  name: 指定函数的名称。调用者将使用该名称来寻找到此函数。
      * @param  retvalSize: 指定函数的返回值大小（单位：字节）。
      * @param  parameterCount: 指定函数的参数个数。
      * @param  ...: 指定函数的每个参数的大小（单位：字节）。
      * @retval ShellNode_ErrorCode_t: 描述该过程可能出现的错误。
      */
    ShellNode_ErrorCode_t RegisterFunction(void* startAddr, const char* name, uint32_t retvalSize, uint32_t parameterCount, ...)
    {
        ShellNode_ErrorCode_t errCode = ShellNode_ErrorCode_Success;
        do
        {
            uint32_t paramsLength = 0;
            uint32_t retvalLength = ShellNode_GetRetvalLength(retvalSize);
            if (retvalLength > ShellNode_Function_MaxRetvalLength)
            {
                errCode = ShellNode_ErrorCode_RetvalOutOfSize;
            }
            if (FunctionCount < ShellNode_Function_MaxCount)
            {
                va_list va;
                va_start(va, parameterCount);
                for (int i = 0; i < parameterCount; i++)
                {
                    int plen = ShellNode_GetRetvalLength(va_arg(va, int));
                    paramsLength += plen;
                    FunctionList[FunctionCount].ParameterLengthUnitTimes[i] =
                        plen / ShellNode_Function_ParameterUnitByte;
                }
                va_end(va);
                if (paramsLength > ShellNode_Function_MaxParameterLength)
                {
                    errCode = ShellNode_ErrorCode_ParameterOutOfSize;
                    break;
                }
                strcpy(FunctionList[FunctionCount].Name, name);
                FunctionList[FunctionCount].StartAddr = startAddr;
                FunctionList[FunctionCount].ParameterLength = paramsLength;
                FunctionList[FunctionCount].ParameterCount = parameterCount;
                FunctionList[FunctionCount].RetvalLength = retvalLength;
                FunctionCount++;
            }
            else
            {
                errCode = ShellNode_ErrorCode_FunctionListFull;
            }
        }
        while (false);
        return errCode;
    }
    /**
      * @brief  注册一个消息类型。
      * @param  handler: 指定的消息服务函数。当接收到该类型的消息后，ShellNode 实例将调用此服务函数以通知消息的到达。
      * @param  name: 指定消息的名称。发送者将使用该名称来寻找到此消息类型。
      * @param  messageMinLength: 指定允许的最短消息大小（单位：字节）。
      * @param  messageMaxLength: 指定允许的最长消息大小（单位：字节）。使此值为 0 以禁用最大消息长度过滤。
      * @retval ShellNode_ErrorCode_t: 描述该过程可能出现的错误。
      */
    ShellNode_ErrorCode_t RegisterMessage(ShellNode_MessageHandler_t handler, const char *name, uint32_t messageMinLength, uint32_t messageMaxLength)
    {
        ShellNode_ErrorCode_t errCode = ShellNode_ErrorCode_Success;
        if (MessageCount < ShellNode_Message_MaxCount)
        {
            MessageList[MessageCount].Handler = handler;
            strcpy(MessageList[MessageCount].Name, name);
            MessageList[MessageCount].MessageMaxLength = messageMaxLength;
            MessageList[MessageCount].MessageMinLength = messageMinLength;
            MessageCount++;
        }
        else
        {
            errCode = ShellNode_ErrorCode_MessageListFull;
        }
        return errCode;
    }
    /**
      * @brief  设置节点名称，并根据名称生成节点地址。
      * @param  nodeName: 指定的节点名称。
      * @retval None
      */
    void SetNodeName(const char* nodeName)
    {
        NodeName = nodeName;
        NodeAddr = ShellNode_GetCRC8(NodeName, strlen(NodeName));
    }
    /**
      * @brief  设置目标节点名称，并根据名称生成目标节点地址。以实现对其他源自非目标地址的过滤。
      * @param  nodeName: 指定的目标节点名称。
      * @retval None
      */
    void SetTarget(const char* targetName)
    {
        TargetAddr = ShellNode_GetCRC8(targetName, strlen(targetName));
    }
    /**
      * @brief  发送一个自定义数据包。
      * @param  cmdCode: 指定的命令 ID。
      * @param  params: 指定的命令参数。
      * @param  pLength: 指定的命令参数长度。
      * @retval None
      */
    void SendPacket(uint8_t cmdCode, void* params, uint32_t pLength)
    {
        AckReceived = 0;
        int txPacketPos = 0;
        TXPacketBuffer[txPacketPos++] = 0;          // 数据包大小
        TXPacketBuffer[txPacketPos++] = cmdCode;    // 命令
        for (int i = 0; i < pLength; i++)           // 参数列表
        {
            TXPacketBuffer[txPacketPos++] = ((uint8_t*)params)[i];
        }
#if( ShellNode_EnableAddr != 0 )
        TXPacketBuffer[txPacketPos++] = NodeAddr;       // 源地址
        TXPacketBuffer[txPacketPos++] = TargetAddr;     // 目标地址
#endif
        TXPacketBuffer[0] = txPacketPos + 1;            // 写入数据包大小
        TXPacketBuffer[txPacketPos++] = ShellNode_GetCRC8(TXPacketBuffer, txPacketPos); // CRC值
        TXPacketBufferLength = txPacketPos;
        /* 封装成帧 */
        int txBufferPos = 0;
        TXBuffer[txBufferPos++] = ShellNode_FrameBeginByte; // 帧起始符
        for (int i = 0; i < TXPacketBufferLength; i++)
        {
            char dat = TXPacketBuffer[i];
            if (dat == ShellNode_FrameBeginByte ||
                    dat == ShellNode_FrameEndByte ||
                    dat == ShellNode_EscapeByte
               )
            {
                TXBuffer[txBufferPos++] = ShellNode_EscapeByte;     // 插入转义符
            }
            TXBuffer[txBufferPos++] = dat;  // 复制到帧缓冲区
        }
        TXBuffer[txBufferPos++] = ShellNode_FrameEndByte;   // 帧起始符
        TXBufferLength = txBufferPos; // 记录帧长度
        LastTransmittedCommand = cmdCode & 0x7f;
        if (SendData)
        {
            SendData(this, TXBuffer, TXBufferLength);
        }
    }
    /**
      * @brief  发送一个应答包。
      * @param  params: 指定的命令参数。
      * @param  pLength: 指定的命令参数长度。
      * @retval None
      */
    void AckPacket(void* params, uint32_t pLength)
    {
        SendPacket(LastReceivedCommand | ShellNode_Command_ACK, params, pLength);
    }

    /**
      * @brief  清空待发送消息缓冲区。
      * @param  None
      * @retval None
      */
    void ClearMessage()
    {
        MessageSize = 0;
        MessageElementCount = 0;
    }
    /**
      * @brief  使接收的消息缓冲区指针置 0，以重复读取数据。
      * @param  None
      * @retval None
      */
    void RewindMessage()
    {
        TargetMessagePos = 0;
    }
    /**
      * @brief  向待发送消息缓冲区推入指定长度的数据。
      * @param  msg: 数据的首地址。
      * @param  msgLength: 数据的长度。
      * @retval ShellNode_ErrorCode_t: 描述该过程可能出现的错误。
      */
    ShellNode_ErrorCode_t Push(void* msg, uint32_t msgLength)
    {
        ShellNode_ErrorCode_t errCode = ShellNode_ErrorCode_Success;
        for (int i = 0; i < msgLength; i++)
        {
            if (MessageSize < ShellNode_Message_MaxLength)
            {
                Message[MessageSize] = ((uint8_t*)msg)[i];
                MessageSize++;
            }
            else
            {
                errCode = ShellNode_ErrorCode_MessageStackOverflow;
                break;
            }
        }
        if (!errCode)
        {
            MessageElementCount++;
        }
        return errCode;
    }
    /**
      * @brief  从接收的消息缓冲区取出指定长度的数据。
      * @param  msg: 数据的首地址。
      * @param  msgLength: 数据的长度。
      * @retval ShellNode_ErrorCode_t: 描述该过程可能出现的错误。
      */
    ShellNode_ErrorCode_t Pop(void* msg, uint32_t msgLength)
    {
        ShellNode_ErrorCode_t errCode = ShellNode_ErrorCode_Success;
        for (int i = 0; i < msgLength; i++)
        {
            if (TargetMessagePos < TargetMessageSize)
            {
                ((uint8_t*)msg)[i] = TargetMessage[TargetMessagePos];
                TargetMessagePos++;
            }
            else
            {
                errCode = ShellNode_ErrorCode_MessageStackOverflow;
                break;
            }
        }
        return errCode;
    }
    /**
      * @brief  向待发送消息缓冲区推入值。
      * @param  T: 指定的数据类型。
      * @param  val: 值。
      * @retval ShellNode_ErrorCode_t: 描述该过程可能出现的错误。
      */
    template<typename T>
    ShellNode_ErrorCode_t Push(T val)
    {
        uint32_t valSize = sizeof(T);
        return Push(&val, valSize);
    }
    /**
      * @brief  从接收的消息缓冲区取出值。
      * @param  T: 指定的数据类型。
      * @retval T: 取出的值。注意：此过程可能出现溢出错误，导致取出的值被截断。
      */
    template<typename T>
    T Pop()
    {
        T val;
        uint32_t valSize = sizeof(T);
        Pop(&val, valSize);
        return val;
    }

    /**
      * @brief  测试与目标节点的连通性。使用 GetAckStatus 查询该测试是否通过。
      * @param  None
      * @retval None
      */
    void Test()
    {
        SendPacket(ShellNode_DefaultCommands_Test, 0, 0);
    }
    /**
      * @brief  （停用的）尝试获取目标节点可用的函数数目。使用 GetAckStatus 查询是否获取成功，使用 GetTargetFunctionCount 获取值。
      * @param  None
      * @retval None
      */
    void BeginGetTargetFunctionCount()
    {
        SendPacket(ShellNode_DefaultCommands_GetFunctionCount, 0, 0);
    }
    /**
      * @brief  （停用的）获取最近查询到的目标节点的可用函数数目。
      * @param  None
      * @retval uint32_t: 目标节点的可用函数数目。
      */
    uint32_t GetTargetFunctionCount()
    {
        return TargetFunctionCount;
    }
    /**
      * @brief  尝试获取目标节点可用的消息类型数目。使用 GetAckStatus 查询是否获取成功，使用 GetTargetMessageCount 获取值。
      * @param  None
      * @retval None
      */
    void BeginGetTargetMessageCount()
    {
        SendPacket(ShellNode_DefaultCommands_GetMessageCount, 0, 0);
    }
    /**
      * @brief  获取最近查询到的目标节点的可用消息类型数目。
      * @param  None
      * @retval uint32_t: 目标节点的可用消息类型数目。
      */
    uint32_t GetTargetMessageCount()
    {
        return TargetMessageCount;
    }
    /**
      * @brief  （停用的）尝试获取目标节点指定的函数信息。使用 GetAckStatus 查询是否获取成功。
      * @param  index: 指定的函数索引值。
      * @retval None
      */
    void BeginGetTargetFunctionInfo(uint32_t index)
    {
        SendPacket(ShellNode_DefaultCommands_GetFunctionInfo, &index, sizeof(uint32_t));
    }
    /**
      * @brief  尝试获取目标节点的指定消息类型信息。使用 GetAckStatus 查询是否获取成功。
      * @param  index: 指定的消息类型索引值。
      * @retval None
      */
    void BeginGetTargetMessageInfo(uint32_t index)
    {
        SendPacket(ShellNode_DefaultCommands_GetMessageInfo, &index, sizeof(uint32_t));
    }
    /**
      * @brief  获取接收的消息缓冲区内元素的个数。
      * @param  None
      * @retval uint32_t: 接收的消息缓冲区内元素的个数。
      */
    uint32_t GetElementCount()
    {
        return TargetMessageElementCount;
    }
    /**
      * @brief  获取最近一次服务函数的错误代码。
      * @param  None
      * @retval ShellNode_ErrorCode_t: 最近一次服务函数的错误代码。
      */
    ShellNode_ErrorCode_t GetLastErrorCode()
    {
        return LastErrorCode;
    }
    /**
      * @brief  获取指定索引值的消息类型的名称。
      * @param  index: 指定的索引值。
      * @retval char*: 指定索引值的消息类型的名称。
      */
    char* GetTargetMessageName(uint32_t index)
    {
        return TargetMessageList[index].Name;
    }
    /**
      * @brief  （停用的）尝试调用目标节点的函数。
      * @param  funcName: 指定的函数名称。
      * @param  ...: 函数的参数。
      * @retval ShellNode_ErrorCode_t: 描述该过程可能出现的错误。
      */
    ShellNode_ErrorCode_t BeginCall(const char* funcName, ...)
    {
        ShellNode_ErrorCode_t errCode = ShellNode_ErrorCode_Success;
        do
        {
            uint8_t index = 0;
            bool found = false;
            for (int i = 0; i < TargetFunctionCount; i++)
            {
                if (!strcmp(TargetFunctionList[i].Name, funcName))
                {
                    index = i;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                errCode = ShellNode_ErrorCode_FunctionNameNotExist;
                break;
            }
            uint32_t buffPos = 0;
            uint8_t pbuffer[ShellNode_Function_MaxParameterLength];
            pbuffer[buffPos++] = (uint8_t)index;
            if (index < TargetFunctionCount)
            {
                uint32_t pLength = TargetFunctionList[index].ParameterLength;
                uint8_t pCount = TargetFunctionList[index].ParameterCount;
                if (pLength > ShellNode_Function_MaxParameterLength)
                {
                    errCode = ShellNode_ErrorCode_ParameterOutOfSize;
                    break;
                }
                else
                {
                    va_list va;
                    va_start(va, funcName); // 从第一个参数开始
                    for (int a = 0; a < pCount; a++)
                    {
                        int pTimes = TargetFunctionList[index].ParameterLengthUnitTimes[a];
                        switch (pTimes)
                        {
                        case 1:
                        {
                            ShellNode_FunctionInterface_1times data =
                                va_arg(va, ShellNode_FunctionInterface_1times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_1times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 2:
                        {
                            ShellNode_FunctionInterface_2times data =
                                va_arg(va, ShellNode_FunctionInterface_2times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_2times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 3:
                        {
                            ShellNode_FunctionInterface_3times data =
                                va_arg(va, ShellNode_FunctionInterface_3times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_3times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 4:
                        {
                            ShellNode_FunctionInterface_4times data =
                                va_arg(va, ShellNode_FunctionInterface_4times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_4times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 5:
                        {
                            ShellNode_FunctionInterface_5times data =
                                va_arg(va, ShellNode_FunctionInterface_5times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_5times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 6:
                        {
                            ShellNode_FunctionInterface_6times data =
                                va_arg(va, ShellNode_FunctionInterface_6times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_6times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 7:
                        {
                            ShellNode_FunctionInterface_7times data =
                                va_arg(va, ShellNode_FunctionInterface_7times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_7times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 8:
                        {
                            ShellNode_FunctionInterface_8times data =
                                va_arg(va, ShellNode_FunctionInterface_8times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_8times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 9:
                        {
                            ShellNode_FunctionInterface_9times data =
                                va_arg(va, ShellNode_FunctionInterface_9times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_9times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 10:
                        {
                            ShellNode_FunctionInterface_10times data =
                                va_arg(va, ShellNode_FunctionInterface_10times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_10times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 11:
                        {
                            ShellNode_FunctionInterface_11times data =
                                va_arg(va, ShellNode_FunctionInterface_11times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_11times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 12:
                        {
                            ShellNode_FunctionInterface_12times data =
                                va_arg(va, ShellNode_FunctionInterface_12times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_12times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 13:
                        {
                            ShellNode_FunctionInterface_13times data =
                                va_arg(va, ShellNode_FunctionInterface_13times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_13times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 14:
                        {
                            ShellNode_FunctionInterface_14times data =
                                va_arg(va, ShellNode_FunctionInterface_14times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_14times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 15:
                        {
                            ShellNode_FunctionInterface_15times data =
                                va_arg(va, ShellNode_FunctionInterface_15times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_15times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        case 16:
                        {
                            ShellNode_FunctionInterface_16times data =
                                va_arg(va, ShellNode_FunctionInterface_16times);
                            for (int i = 0; i < sizeof(ShellNode_FunctionInterface_16times); i++)
                            {
                                pbuffer[buffPos++] = ((uint8_t*)&data)[i];
                            }
                        }
                        break;
                        }
                    }
                    va_end(va);
                    SendPacket(ShellNode_DefaultCommands_CallFunction, pbuffer, pLength + 1);
                }
            }
            else
            {
                errCode = ShellNode_ErrorCode_InvalidFunctionIndex;
            }
        }
        while (false);
        return errCode;
    }
    /**
      * @brief  尝试向目标节点发送指定类型的消息。
      * @param  targetMessageName: 指定的消息类型名称。
      * @retval ShellNode_ErrorCode_t: 描述该过程可能出现的错误。
      */
    ShellNode_ErrorCode_t SendMessage(const char* targetMessageName)
    {
        ShellNode_ErrorCode_t errCode = ShellNode_ErrorCode_Success;
        bool found = false;
        uint32_t index = 0;
        for (int i = 0; i < TargetMessageCount; i++)
        {
            if (!strcmp(TargetMessageList[i].Name, targetMessageName))
            {
                found = true;
                index = i;
                break;
            }
        }
        do
        {
            if (found)
            {
                //（index(4 bytes)、length(2 byte)、element count(2 byte)、message(n byte)）
                uint8_t buffer[ShellNode_Message_MaxLength + sizeof(uint32_t) + sizeof(uint16_t)];
                uint32_t bufferPos = 0;
                *(uint32_t*)&(buffer[bufferPos]) = index;
                bufferPos += sizeof(uint32_t);
                *(uint16_t*)&(buffer[bufferPos]) = MessageSize;
                bufferPos += sizeof(uint16_t);
                *(uint16_t*)&(buffer[bufferPos]) = MessageElementCount;
                bufferPos += sizeof(uint16_t);
                for (int i = 0; i < MessageSize; i++)
                {
                    buffer[bufferPos++] = Message[i];
                }
                SendPacket(ShellNode_DefaultCommands_Messaging, buffer, bufferPos);
            }
            else
            {
                errCode = ShellNode_ErrorCode_MessageNameNotExist;
                break;
            }
        }
        while (false);
        ClearMessage();
        return errCode;
    }

    /**
      * @brief  获取最近一次命令的应答状态。若应答状态为 true，则本次查询会将应答状态置为 false.
      * @param  None
      * @retval bool: 指示最近一次命令的应答状态（true: 已接收到应答包，false: 还未接收到应答包）。
      */
    bool GetAckStatus()
    {
        if (AckReceived)
        {
            AckReceived = false;
            return true;
        }
        return false;
    }
    /**
      * @brief  清空接收缓冲区
      * @param  None
      * @retval None
      */
    void ClearRXBuffer()
    {
        for (int i = 0; i < ShellNode_RXFIFOLength; i++)
        {
            RXBuffer[i] = 0;
        }
        FrameHeaderFound = FrameTailPos = FrameHeaderPos = RXBufferPos = 0;
        FrameFound = 0;
        /*CommandCount = 0;
        for (int i = 0; i < ShellNode_RXPacketLength; i++)
        {
            RXPacketBuffer[i] = 0;
        }*/
    }
    /**
      * @brief  通知 ShellNode 实例已接收到的字节。无需保证该字节的可靠性。
      * @param  byteDat: 接收到的字节。
      * @retval None
      */
    void PushByte(uint8_t byteDat)
    {
        // 若有待处理的合法帧，则不接受数据
        if (FrameFound)
        {
            return;
        }
        RXBuffer[RXBufferPos] = byteDat;
        // 搜索帧起始符
        if (RXBuffer[RXBufferPos] == ShellNode_FrameBeginByte &&
                (!RXBufferPos || (RXBuffer[RXBufferPos - 1] != ShellNode_EscapeByte))
           )
        {
            FrameHeaderFound = true;
            FrameHeaderPos = RXBufferPos;
        }
        // 搜索帧结束符
        if (FrameHeaderFound)
        {
            if (RXBuffer[RXBufferPos] == ShellNode_FrameEndByte &&
                    (RXBufferPos && (RXBuffer[RXBufferPos - 1] != ShellNode_EscapeByte))
               )
            {
                // 检查帧长度是否合法
                if (RXBufferPos - FrameHeaderPos - 1 >= ShellNode_EmptyValidPacketLength)
                {
                    FrameTailPos = RXBufferPos;
                    FrameFound = true;
                    return;
                }
            }
        }
        // 缓冲区光标后移
        if (RXBufferPos >= ShellNode_RXFIFOLength - 1)
        {
            // 若缓冲区溢出，则清空缓冲区
            ClearRXBuffer();
        }
        else
        {
            RXBufferPos++;
        }
    }
    /**
      * @brief  使 ShellNode 处理最近挂起的命令
      * @param  None
      * @retval ShellNode_ErrorCode_t: 描述该过程可能出现的错误。
      */
    ShellNode_ErrorCode_t Handle()
    {
        ShellNode_ErrorCode_t errCode = ShellNode_ErrorCode_Success;
        volatile bool isAck = false;
        do
        {
            // 若未找到合法帧，则不进行数据包解析
            if (!FrameFound)
            {
                errCode = ShellNode_ErrorCode_InvalidFrame;
                break;
            }
            // 还原数据包
            RXPacketPos = 0;
//            int tmpHeader = FrameHeaderPos;
//            int tempTail = FrameTailPos;
            for (int i = FrameHeaderPos + 1; i < FrameTailPos; i++)
            {
                // 处理数据包光标
                if (i != FrameHeaderPos + 1)
                {
                    if (RXPacketPos >= ShellNode_RXPacketLength - 1)
                    {
                        errCode = ShellNode_ErrorCode_RXPacketOutOfSize;
                        break;
                    }
                    else
                    {
                        RXPacketPos++;
                    }
                }
                uint8_t dat = RXBuffer[i];
                // 处理转义符
                if (dat == ShellNode_EscapeByte)
                {
                    uint8_t nextDat = RXBuffer[i + 1];
                    if (nextDat != ShellNode_FrameBeginByte &&
                            nextDat != ShellNode_FrameEndByte &&
                            nextDat != ShellNode_EscapeByte
                       )
                    {
                        // 不受支持的转义符
                        errCode = ShellNode_ErrorCode_InvalidEscapeCharacter;
                        break;
                    }
                    else
                    {
                        dat = nextDat;
                        i++;
                    }
                }
                // 复制数据包
                RXPacketBuffer[RXPacketPos] = dat;
            }
            if (RXPacketPos)
            {
                ClearRXBuffer();
            }
            if (errCode)
            {
                break;
            }
            int packetLength = RXPacketPos + 1;
            ShellNode_PacketHeader_t* header = (ShellNode_PacketHeader_t*)RXPacketBuffer;
            if (header->PacketLength == packetLength)
            {
                // CRC8 校验
                uint8_t crc8 = ShellNode_GetCRC8(header, packetLength - 1);
                if (RXPacketBuffer[packetLength - 1] == crc8)
                {
                    bool cmdFound = false;
                    int cmdIndex = 0;
                    for (int i = 0; i < CommandCount; i++)
                    {
                        uint8_t cmdType = (header->IdentCode & 0x7f);
                        if (cmdType == CommandsList[i].IdentCode)
                        {
                            cmdIndex = i;
                            cmdFound = true;
                            LastReceivedCommand = cmdType;
                            // 判断收到对应的应答包
                            isAck = (!!(header->IdentCode & 0x80)) &&
                                    (LastTransmittedCommand == cmdType);
                            break;
                        }
                    }
                    if (cmdFound)
                    {
#if( ShellNode_EnableAddr == 0 )
                        CommandsList[cmdIndex].Handler(this, 0, AckReceived, &RXPacketBuffer[1], packetLength - ShellNode_EmptyValidPacketLength);
#else
                        uint8_t sourceAddr = RXPacketBuffer[packetLength - 3];
//                        uint8_t desAddr = RXPacketBuffer[packetLength - 2];
                        if (sourceAddr == TargetAddr)
                        {
                            CommandsList[cmdIndex].Handler(this, sourceAddr, isAck, &RXPacketBuffer[2], packetLength - ShellNode_EmptyValidPacketLength);
                        }
#endif
                    }
                    else
                    {
                        errCode = ShellNode_ErrorCode_UnsupportedCommand;
                    }
                    AckReceived = isAck;
                }
                else
                {
                    errCode = ShellNode_ErrorCode_PacketCRCFailure;
                    break;
                }
            }
            else
            {
                errCode = ShellNode_ErrorCode_InvalidPacketLength;
                break;
            }
        }
        while (false);
        return errCode;
    }
};

// 默认命令服务函数
static void ShellNode_DefaultHandler_Test(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength)
{
    if (!ack)
    {
        sender->AckPacket(0, 0);
    }
}
static void ShellNode_DefaultHandler_GetName(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength)
{
    if (!ack)
    {
        // 处理请求包
        sender->AckPacket((void*)sender->NodeName, strlen(sender->NodeName));
    }
    else
    {
        // 处理应答包
        if (pLength <= ShellNode_StringLength)
        {
            strcpy(sender->ReceivedNodeName, (char*)params);
        }
    }
}
static void ShellNode_DefaultHandler_GetFunctionCount(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength)
{
    if (!ack)
    {
        // 处理请求包
        sender->AckPacket(&sender->FunctionCount, sizeof(uint32_t));
    }
    else
    {
        // 处理应答包
        sender->TargetFunctionCount = *(uint32_t*)params;
    }
}
static void ShellNode_DefaultHandler_GetFunctionInfo(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength)
{
    if (!ack)
    {
        // 处理请求包（index(4 bytes)）
        uint32_t index = *(uint32_t*)params;
        uint8_t buff[sizeof(ShellNode_Function_t) + sizeof(uint32_t) + sizeof(uint8_t)];
        *(uint32_t*)buff = index;
        if (index < sender->FunctionCount)
        {
            buff[4] = (uint8_t)ShellNode_ErrorCode_Success;
            ShellNode_Memcpy(&buff[5], &(sender->FunctionList[index]), sizeof(ShellNode_Function_t));
            sender->AckPacket(&buff, sizeof(buff));
        }
        else
        {
            buff[4] = (uint8_t)ShellNode_ErrorCode_InvalidFunctionIndex;
            sender->AckPacket(&buff, sizeof(buff));
        }
    }
    else
    {
        // 处理应答包（index(4 bytes)、error code(1 byte)、functionInfo(n bytes)）
        uint32_t index = *(uint32_t*)params;
        uint8_t errCode = ((uint8_t*)params)[4];
        if (!errCode)
        {
            ShellNode_Memcpy(&sender->TargetFunctionList[index], &(((uint8_t*)params)[5]), sizeof(ShellNode_Function_t));
        }
        sender->LastErrorCode = (ShellNode_ErrorCode_t)(errCode & 0xff);
    }
}
typedef void(*ShellNode_Function_Execute_t)(void* startAddr, void* params, void* retval);
//extern const ShellNode_Function_Execute_t ShellNode_Function_ExecuteList[][ShellNode_Function_MaxParameterLength / ShellNode_Function_ParameterUnitByte + 1];
static ShellNode_ErrorCode_t ShellNode_Function_Execute(void* startAddr, void* params, uint32_t pLength, void *retval, uint32_t rLength)
{
    ShellNode_ErrorCode_t errCode = ShellNode_ErrorCode_Success;
//    uint32_t pTimes = pLength / ShellNode_Function_ParameterUnitByte;
//    uint32_t rTimes = rLength / ShellNode_Function_ParameterUnitByte;
//    if (pLength > ShellNode_Function_MaxParameterLength)
//    {
//        errCode = ShellNode_ErrorCode_EXE_ParameterOutOfSize;
//    }
//    else if(pLength > ShellNode_Function_MaxRetvalLength)
//    {
//        errCode = ShellNode_ErrorCode_EXE_RetvalOutOfSize;
//    }
//    else
//    {
//        ShellNode_Function_ExecuteList[rTimes][pTimes](startAddr, params, retval);
//    }
    return errCode;
}
static void ShellNode_DefaultHandler_CallFunction(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength)
{
    if (!ack)
    {
        // 处理请求包（ index(1 byte)、parameters(n byte) ）
        uint8_t function_index = *(uint8_t*)params;
        uint8_t* function_params = ((uint8_t*)params) + 1;
        uint32_t function_paramsLength = pLength - sizeof(uint8_t); // length - indexLen
        if (function_index < sender->FunctionCount)
        {
            ShellNode_Function_t* targetFunction = &(sender->FunctionList[function_index]);
            if (targetFunction->ParameterLength == function_paramsLength)
            {
                uint8_t errCode = ShellNode_Function_Execute(
                                      targetFunction->StartAddr,
                                      function_params,
                                      targetFunction->ParameterLength,
                                      sender->FunctionRetval,
                                      targetFunction->RetvalLength
                                  );
                uint32_t pos = 0;
                uint8_t buff[ShellNode_Function_MaxRetvalLength + 2];
                buff[pos++] = errCode;
                buff[pos++] = targetFunction->RetvalLength;
                for (int i = 0; i < targetFunction->RetvalLength; i++)
                {
                    buff[pos++] = sender->FunctionRetval[i];
                }
                sender->AckPacket(buff, pos);
            }
        }
    }
    else
    {
        // 处理应答包（ error code(1 byte)、retval length(1 byte)、retval(n byte) ）
        sender->LastErrorCode = (ShellNode_ErrorCode_t)((*(uint8_t*)params) & 0xff);
        uint8_t retvalLength = *(uint8_t*)(((uint8_t*)params) + 1);
        uint8_t *retval = (uint8_t*)(((uint8_t*)params) + 2);
        if (retvalLength <= ShellNode_Function_MaxRetvalLength)
        {
            for (int i = 0; i < retvalLength; i++)
            {
                sender->FunctionRetval[i] = retval[i];
            }
            sender->FunctionRetvalLength = retvalLength;
        }
    }
}
static void ShellNode_DefaultHandler_GetMessageCount(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength)
{
    if (!ack)
    {
        // 处理请求包
        sender->AckPacket(&sender->MessageCount, sizeof(uint32_t));
    }
    else
    {
        // 处理应答包
        uint32_t count = *(uint32_t*)params;
        if (count < ShellNode_Message_MaxCount)
        {
            sender->TargetMessageCount = count;
        }
    }
}
static void ShellNode_DefaultHandler_GetMessageInfo(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength)
{
    if (!ack)
    {
        // 处理请求包（index(4 bytes)）
        uint32_t index = *(uint32_t*)params;
        uint8_t buff[sizeof(ShellNode_MessageInfo_t) + sizeof(uint32_t) + sizeof(uint8_t)];
        *(uint32_t*)buff = index;
        if (index < sender->MessageCount)
        {
            buff[4] = (uint8_t)ShellNode_ErrorCode_Success;
            ShellNode_Memcpy(&buff[5], &(sender->MessageList[index]), sizeof(ShellNode_MessageInfo_t));
            sender->AckPacket(&buff, sizeof(buff));
        }
        else
        {
            buff[4] = (uint8_t)ShellNode_ErrorCode_InvalidMessageIndex;
            sender->AckPacket(&buff, sizeof(buff));
        }
    }
    else
    {
        // 处理应答包（index(4 bytes)、error code(1 byte)、messageInfo(n bytes)）
        uint32_t index = *(uint32_t*)params;
        uint8_t errCode = ((uint8_t*)params)[4];
        if (!errCode)
        {
            ShellNode_Memcpy(&sender->TargetMessageList[index], &(((uint8_t*)params)[5]), sizeof(ShellNode_MessageInfo_t));
        }
        sender->LastErrorCode = (ShellNode_ErrorCode_t)(errCode & 0xff);
    }
}
static void ShellNode_DefaultHandler_Messaging(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength)
{
    if (!ack)
    {
        // 处理请求包（index(4 bytes)、length(2 byte)、element count(2 byte)、message(n byte)）
        uint8_t buffer[ShellNode_Message_MaxLength + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint16_t)];
        uint32_t bufferPos = 0;
        uint8_t errCode = 0;
        uint32_t index = *(uint32_t*)params;
        uint16_t length = *(uint16_t*) & (((uint8_t*)params)[4]);
        uint16_t cnt = *(uint16_t*) & (((uint8_t*)params)[6]);
        uint8_t *msg = &(((uint8_t*)params)[8]);

        *(uint32_t*)(&buffer[0]) = index;
        bufferPos += sizeof(uint32_t);

        ShellNode_MessageInfo_t* targetMsg = 0;
        do
        {
            if (index >= sender->MessageCount)
            {
                errCode = (uint8_t)ShellNode_ErrorCode_InvalidMessageIndex;
                break;
            }
            targetMsg = &(sender->MessageList[index]);
            if (targetMsg->MessageMaxLength && (length > targetMsg->MessageMaxLength))
            {
                errCode = ShellNode_ErrorCode_MessageLengthOutOfSize;
                break;
            }
            else if (length < targetMsg->MessageMinLength)
            {
                errCode = ShellNode_ErrorCode_MessageLengthTooShort;
                break;
            }
            else if (length > ShellNode_Message_MaxLength)
            {
                errCode = ShellNode_ErrorCode_MessageLengthOutOfSize;
                break;
            }
        }
        while (false);
        buffer[bufferPos++] = errCode;
        sender->TargetMessagePos = 0;
        sender->ClearMessage();
        if (errCode)
        {
            // 参数检查不通过
            *(uint16_t*)&(buffer[bufferPos]) = 0;
            bufferPos += sizeof(uint16_t);
            *(uint16_t*)&(buffer[bufferPos]) = 0;
            bufferPos += sizeof(uint16_t);
            sender->TargetMessageSize = 0;
        }
        else
        {
            // 参数正确
            sender->TargetMessageSize = length;
            sender->TargetMessageElementCount = cnt;
            for (int i = 0; i < length; i++)
            {
                sender->TargetMessage[i] = msg[i];
            }
            targetMsg->Handler(sender, msg, length); // 通知消息服务函数
            *(uint16_t*)&(buffer[bufferPos]) = sender->MessageSize;
            bufferPos += sizeof(uint16_t);
            *(uint16_t*)&(buffer[bufferPos]) = sender->MessageElementCount;
            bufferPos += sizeof(uint16_t);
            for (int i = 0; i < sender->MessageSize; i++)
            {
                buffer[bufferPos++] = sender->Message[i];
            }
        }
        sender->LastErrorCode = (ShellNode_ErrorCode_t)(errCode & 0xff);
        sender->AckPacket(buffer, bufferPos);
    }
    else
    {
        // 处理应答包（index(4 bytes)、error code(1 byte)、length(2 byte)、element count(2 byte)、message(n byte)）
//        uint32_t index = *(uint32_t*)params;
        uint8_t errCode = *(uint8_t*) & (((uint8_t*)params)[4]);
        uint16_t length = *(uint16_t*) & (((uint8_t*)params)[5]);
        uint16_t cnt = *(uint16_t*) & (((uint8_t*)params)[7]);
        uint8_t* msg = &(((uint8_t*)params)[9]);

        sender->TargetMessagePos = 0;
        sender->ClearMessage();
        if (errCode)
        {
            // 参数检查不通过
            sender->TargetMessageSize = 0;
            sender->TargetMessageElementCount = 0;
        }
        else
        {
            // 参数正确
            sender->TargetMessageSize = length;
            sender->TargetMessageElementCount = cnt;
            for (int i = 0; i < length; i++)
            {
                sender->TargetMessage[i] = msg[i];
            }
        }
        sender->LastErrorCode = (ShellNode_ErrorCode_t)(errCode & 0xff);
    }
}

#endif
