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

/* ���������Ŀ */
#define ShellNode_CommandCount              8
/* ���� / ���� ���������� */
#define ShellNode_TXFIFOLength              128
#define ShellNode_RXFIFOLength              128
/* ���ݰ���󳤶� */
#define ShellNode_TXPacketLength            64
#define ShellNode_RXPacketLength            64

#if( ShellNode_EnableAddr == 0 )
/* �����ݰ���С�����������ݰ����ȣ�1 Byte�������1 Byte�������ݰ�CRC8ֵ��1 Byte���� 3 �ֽ� */
#define ShellNode_EmptyValidPacketLength    3
#else
/* �����ݰ���С�����������ݰ����ȣ�1 Byte�������1 Byte�������ݰ�Դ��ַ��1 Byte�������ݰ�Ŀ���ַ��1 Byte�������ݰ�CRC8ֵ��1 Byte���� 5 �ֽ� */
#define ShellNode_EmptyValidPacketLength    5
#endif

#define ShellNode_BroadcastAddr             0

#define ShellNode_DefaultNodeName           "UnknownNode"

/* �ڵ������ַ�����󳤶� */
#define ShellNode_StringLength              16
/* ��·��֡��ʼ�ֽ� */
#define ShellNode_FrameBeginByte            '['
/* ��·��֡�����ֽ� */
#define ShellNode_FrameEndByte              ']'
/* ת��� */
#define ShellNode_EscapeByte                '\\'
/* ����������� */
#define ShellNode_Function_ParameterUnitByte    4
#define ShellNode_Function_MaxCount             1
#define ShellNode_Function_MaxParameterLength   64
#define ShellNode_Function_MaxRetvalLength      32
/* ��Ϣ������� */
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

/* ������� */
enum ShellNode_ErrorCode_t
{
    ShellNode_ErrorCode_Success,                    // ֡����ɹ�
    ShellNode_ErrorCode_InvalidFrame,               // δ�ҵ��Ϸ���֡
    ShellNode_ErrorCode_InvalidEscapeCharacter,     // �Ƿ���ת���
    ShellNode_ErrorCode_RXPacketOutOfSize,          // ���ݰ����ȹ���
    ShellNode_ErrorCode_InvalidPacketLength,        // ���ݰ����ȴ���
    ShellNode_ErrorCode_PacketCRCFailure,           // ���ݰ� CRC У��ʧ��
    ShellNode_ErrorCode_UnsupportedCommand,         // ����֧�ֵ�����
    ShellNode_ErrorCode_CommandListFull,            // �����б���
    ShellNode_ErrorCode_FunctionListFull,           // �����б���
    ShellNode_ErrorCode_MessageListFull,            // ��Ϣ�б���
    ShellNode_ErrorCode_EXE_ParameterOutOfSize,     // ִ�к���ʱ�����б����
    ShellNode_ErrorCode_EXE_RetvalOutOfSize,        // ִ�к���ʱ����ֵ����
    ShellNode_ErrorCode_ParameterOutOfSize,         // ע�ắ�������б����
    ShellNode_ErrorCode_RetvalOutOfSize,            // ע�ắ������ֵ����
    ShellNode_ErrorCode_InvalidFunctionIndex,       // ��������ֵ�Ƿ�
    ShellNode_ErrorCode_InvalidMessageIndex,        // ��Ϣ����ֵ�Ƿ�
    ShellNode_ErrorCode_FunctionNameNotExist,       // Ŀ�꺯�����Ʋ�����
    ShellNode_ErrorCode_MessageNameNotExist,        // Ŀ����Ϣ���Ʋ�����
    ShellNode_ErrorCode_MessageStackOverflow,       // ��Ϣջ���
    ShellNode_ErrorCode_MessageStackEmpty,          // �Ѵﵽ��Ϣջ��
    ShellNode_ErrorCode_MessageLengthOutOfSize,     // ��Ϣ���ȳ�����Ŀ����Ϣ�����������󳤶�
    ShellNode_ErrorCode_MessageLengthTooShort,      // ��Ϣ����С��Ŀ����Ϣ�����������С����
    ShellNode_ErrorCode_Other,                      // �������
};
class ShellNode;
/* ������������� */
typedef void (*ShellNode_CommandHandler_t)(ShellNode*, uint8_t, bool, void*, uint32_t);
/* �����ֽڷ����� */
typedef void (*ShellNode_SendData_t)(ShellNode*, void*, uint32_t);
/* ��Ϣ������ */
typedef void (*ShellNode_MessageHandler_t)(ShellNode*, void *, uint32_t);

/* ����һ�����͵���Ϣ */
typedef struct
{
    ShellNode_MessageHandler_t Handler;
    char Name[ShellNode_StringLength];
    uint32_t MessageMinLength;
    uint32_t MessageMaxLength;
} ShellNode_MessageInfo_t;

/* ������ ID ȡ"��"���Ա�ʾ��Ӧ�����Ӧ��� */
#define ShellNode_Command_ACK       0x80
/* ���õ������ö�� */
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
/* ����һ������ */
typedef struct
{
    ShellNode_CommandHandler_t Handler;     // ����ķ�����
    uint8_t IdentCode;                      // ����� ID
} ShellNode_CommandInfo_t;

/* ����һ�����ݰ�����ʼ���� */
typedef struct
{
    uint8_t PacketLength;           // ���ݰ����ܳ��ȣ���λ���ֽڣ�
    uint8_t IdentCode;              // ���ݰ������� ID
} ShellNode_PacketHeader_t;

/* CRC8 �� */
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
/* ������ 1-16 ����Ԫ���ȵĽṹ������ */
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
  * @brief  ��ȡָ���������ݵ� 8 λѭ������У��ֵ
  * @param  data: �����ݵ��׵�ַ
  * @param  length: ���ݵĳ��ȣ���λ���ֽڣ�
  * @retval uint8_t: 8 λѭ������У��ֵ
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

// Ĭ�����������������
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
    const char* NodeName;                               // ��ǰ�ڵ�����
    char ReceivedNodeName[ShellNode_StringLength];
    uint8_t NodeAddr;                                   // ��ǰ�ڵ��ַ��ͨ���ɽڵ������ַ������� CRC8 ����õ���
    uint8_t TargetAddr;                                 // Ŀ��ڵ��ַ

    uint8_t TXBuffer[ShellNode_TXFIFOLength];           // �����ֽ��� FIFO
    uint8_t RXBuffer[ShellNode_RXFIFOLength];           // �����ֽ��� FIFO
    uint8_t LastTransmittedCommand;                     // ������͵�����
    uint8_t LastReceivedCommand;                        // ������յ�����
    volatile bool AckReceived;                          // ָʾ�Ƿ����յ�Ӧ���
    ShellNode_ErrorCode_t LastErrorCode;                // ���һ��ϵͳ�������������Ĵ���
    volatile bool FrameFound;                           // ָʾ�Ƿ��ҵ��Ϸ�֡
    volatile bool FrameHeaderFound;                     // ָʾ�Ƿ��ҵ�֡��ʼ��
    uint32_t FrameHeaderPos;                            // ָʾ֡��ʼ��λ��
    uint32_t FrameTailPos;                              // ָʾ֡������λ��
    uint32_t TXBufferLength;                            // �����ֽ�������
    uint32_t RXBufferPos;                               // ���� FIFO ���λ��

    uint8_t TXPacketBuffer[ShellNode_TXPacketLength];   // �������ݰ�������
    uint32_t TXPacketBufferLength;                      // �������ݰ�����
    uint8_t RXPacketBuffer[ShellNode_RXPacketLength];   // �������ݰ�������
    uint32_t RXPacketPos;                               // �������ݰ����������λ��

    ShellNode_CommandInfo_t CommandsList[ShellNode_CommandCount];           // ע��������б�
    uint32_t CommandCount;                                                  // �����б�ĳ���

    ShellNode_Function_t FunctionList[ShellNode_Function_MaxCount];         // ���������б�
    uint32_t FunctionCount;
    ShellNode_Function_t TargetFunctionList[ShellNode_Function_MaxCount];   // Ŀ�꺯���б�
    uint32_t TargetFunctionCount;
    uint8_t FunctionRetval[ShellNode_Function_MaxRetvalLength];             // ����ֵ
    uint32_t FunctionRetvalLength;                                          // ����ֵ����

    ShellNode_MessageInfo_t MessageList[ShellNode_Message_MaxCount];        // ֧�ֵ���Ϣ�����б�
    uint32_t MessageCount;                                                  // ��Ϣ�����б���
    ShellNode_MessageInfo_t TargetMessageList[ShellNode_Message_MaxCount];  // Ŀ����Ϣ�����б�
    uint32_t TargetMessageCount;                                            // Ŀ����Ϣ�����б���
    uint8_t Message[ShellNode_Message_MaxLength];                           // ׼�����͵���Ϣ������
    uint32_t MessageSize;                                                   // ׼�����͵���Ϣ����������
    uint8_t TargetMessage[ShellNode_Message_MaxLength];                     // ���յ���Ϣ������
    uint32_t TargetMessagePos;                                              // ���յ���Ϣ�������Ĺ��
    uint32_t TargetMessageSize;                                             // ���յ���Ϣ�������ĳ���
    uint32_t MessageElementCount;                                           // ׼�����͵���Ϣ�������ڰ���Ԫ�صĸ���
    uint32_t TargetMessageElementCount;                                     // ���յ���Ϣ�������ڰ���Ԫ�صĸ���

    /**
      * @brief  ��ʼ��һ�� ShellNode ʵ����
      * @param  None
      * @retval ShellNode: ��ʼ����ϵ� ShellNode ʵ��
      */
    ShellNode()
    {
        DeInit();
        SetNodeName(ShellNode_DefaultNodeName);
    }
    /**
      * @brief  ��ָ���Ľڵ����ơ��������ݷ�����������ʼ��һ�� ShellNode ʵ����
      * @param  nodeName: ָ���Ľڵ����ơ��ڵ�����Ĭ�����ڴ����ýڵ�ĵ�ַ(ADDR)ֵ������ ShellNode_EnableAddr ��Ϊ 0 ʱ����ȷ�Ľڵ��ַ����Ϊ�ڵ�佨�����ӵı�Ҫ�������ڵ㽫���ٽ��շ�ָ����Դ�İ���
      * @param  sendData: ָ���ķ������ݷ���������ִ�����ͨѶ����ʱ��ShellNode ʵ����ֱ�ӵ��ô˷�����������֪һ�����ݷ��͵�����
      * @retval ShellNode: ��ʼ����ϵ� ShellNode ʵ��
      */
    ShellNode(const char* nodeName, ShellNode_SendData_t sendData)
    {
        DeInit();
        SetNodeName(nodeName);
        SetSendDataHandler(sendData);
    }
    /**
      * @brief  ���� ShellNode ʵ�����Խ����������ûָ�����ʼ��״̬��
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
      * @brief  ���������б���� ShellNode ʵ����ʼ����ע��������Զ�������Ӷ�������ϵͳ���
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
      * @brief  ���÷������ݵķ�������
      * @param  sendData: ָ���ķ������ݷ���������ִ�����ͨѶ����ʱ��ShellNode ʵ����ֱ�ӵ��ô˷�����������֪һ�����ݷ��͵�����
      * @retval None
      */
    void SetSendDataHandler(ShellNode_SendData_t sendData)
    {
        SendData = sendData;
    }
    /**
      * @brief  ע��һ���Զ����������͡�
      * @param  cmdCode: ָ�������� ID������ ID ������ʶ��������
      * @param  handler: ָ��������������������͸�����ʱ��ShellNode ʵ�������ô˷�������ʵ���������Ӧ��
      * @retval ShellNode_ErrorCode_t: �����ù��̿��ܳ��ֵĴ���
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
      * @brief  ��ͣ�õģ�ע��һ����������ʵ�ִ�͸�ĵ��á�
      * @param  startAddr: ָ����������ʼ��ַ��
      * @param  name: ָ�����������ơ������߽�ʹ�ø�������Ѱ�ҵ��˺�����
      * @param  retvalSize: ָ�������ķ���ֵ��С����λ���ֽڣ���
      * @param  parameterCount: ָ�������Ĳ���������
      * @param  ...: ָ��������ÿ�������Ĵ�С����λ���ֽڣ���
      * @retval ShellNode_ErrorCode_t: �����ù��̿��ܳ��ֵĴ���
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
      * @brief  ע��һ����Ϣ���͡�
      * @param  handler: ָ������Ϣ�������������յ������͵���Ϣ��ShellNode ʵ�������ô˷�������֪ͨ��Ϣ�ĵ��
      * @param  name: ָ����Ϣ�����ơ������߽�ʹ�ø�������Ѱ�ҵ�����Ϣ���͡�
      * @param  messageMinLength: ָ������������Ϣ��С����λ���ֽڣ���
      * @param  messageMaxLength: ָ����������Ϣ��С����λ���ֽڣ���ʹ��ֵΪ 0 �Խ��������Ϣ���ȹ��ˡ�
      * @retval ShellNode_ErrorCode_t: �����ù��̿��ܳ��ֵĴ���
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
      * @brief  ���ýڵ����ƣ��������������ɽڵ��ַ��
      * @param  nodeName: ָ���Ľڵ����ơ�
      * @retval None
      */
    void SetNodeName(const char* nodeName)
    {
        NodeName = nodeName;
        NodeAddr = ShellNode_GetCRC8(NodeName, strlen(NodeName));
    }
    /**
      * @brief  ����Ŀ��ڵ����ƣ���������������Ŀ��ڵ��ַ����ʵ�ֶ�����Դ�Է�Ŀ���ַ�Ĺ��ˡ�
      * @param  nodeName: ָ����Ŀ��ڵ����ơ�
      * @retval None
      */
    void SetTarget(const char* targetName)
    {
        TargetAddr = ShellNode_GetCRC8(targetName, strlen(targetName));
    }
    /**
      * @brief  ����һ���Զ������ݰ���
      * @param  cmdCode: ָ�������� ID��
      * @param  params: ָ�������������
      * @param  pLength: ָ��������������ȡ�
      * @retval None
      */
    void SendPacket(uint8_t cmdCode, void* params, uint32_t pLength)
    {
        AckReceived = 0;
        int txPacketPos = 0;
        TXPacketBuffer[txPacketPos++] = 0;          // ���ݰ���С
        TXPacketBuffer[txPacketPos++] = cmdCode;    // ����
        for (int i = 0; i < pLength; i++)           // �����б�
        {
            TXPacketBuffer[txPacketPos++] = ((uint8_t*)params)[i];
        }
#if( ShellNode_EnableAddr != 0 )
        TXPacketBuffer[txPacketPos++] = NodeAddr;       // Դ��ַ
        TXPacketBuffer[txPacketPos++] = TargetAddr;     // Ŀ���ַ
#endif
        TXPacketBuffer[0] = txPacketPos + 1;            // д�����ݰ���С
        TXPacketBuffer[txPacketPos++] = ShellNode_GetCRC8(TXPacketBuffer, txPacketPos); // CRCֵ
        TXPacketBufferLength = txPacketPos;
        /* ��װ��֡ */
        int txBufferPos = 0;
        TXBuffer[txBufferPos++] = ShellNode_FrameBeginByte; // ֡��ʼ��
        for (int i = 0; i < TXPacketBufferLength; i++)
        {
            char dat = TXPacketBuffer[i];
            if (dat == ShellNode_FrameBeginByte ||
                    dat == ShellNode_FrameEndByte ||
                    dat == ShellNode_EscapeByte
               )
            {
                TXBuffer[txBufferPos++] = ShellNode_EscapeByte;     // ����ת���
            }
            TXBuffer[txBufferPos++] = dat;  // ���Ƶ�֡������
        }
        TXBuffer[txBufferPos++] = ShellNode_FrameEndByte;   // ֡��ʼ��
        TXBufferLength = txBufferPos; // ��¼֡����
        LastTransmittedCommand = cmdCode & 0x7f;
        if (SendData)
        {
            SendData(this, TXBuffer, TXBufferLength);
        }
    }
    /**
      * @brief  ����һ��Ӧ�����
      * @param  params: ָ�������������
      * @param  pLength: ָ��������������ȡ�
      * @retval None
      */
    void AckPacket(void* params, uint32_t pLength)
    {
        SendPacket(LastReceivedCommand | ShellNode_Command_ACK, params, pLength);
    }

    /**
      * @brief  ��մ�������Ϣ��������
      * @param  None
      * @retval None
      */
    void ClearMessage()
    {
        MessageSize = 0;
        MessageElementCount = 0;
    }
    /**
      * @brief  ʹ���յ���Ϣ������ָ���� 0�����ظ���ȡ���ݡ�
      * @param  None
      * @retval None
      */
    void RewindMessage()
    {
        TargetMessagePos = 0;
    }
    /**
      * @brief  ���������Ϣ����������ָ�����ȵ����ݡ�
      * @param  msg: ���ݵ��׵�ַ��
      * @param  msgLength: ���ݵĳ��ȡ�
      * @retval ShellNode_ErrorCode_t: �����ù��̿��ܳ��ֵĴ���
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
      * @brief  �ӽ��յ���Ϣ������ȡ��ָ�����ȵ����ݡ�
      * @param  msg: ���ݵ��׵�ַ��
      * @param  msgLength: ���ݵĳ��ȡ�
      * @retval ShellNode_ErrorCode_t: �����ù��̿��ܳ��ֵĴ���
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
      * @brief  ���������Ϣ����������ֵ��
      * @param  T: ָ�����������͡�
      * @param  val: ֵ��
      * @retval ShellNode_ErrorCode_t: �����ù��̿��ܳ��ֵĴ���
      */
    template<typename T>
    ShellNode_ErrorCode_t Push(T val)
    {
        uint32_t valSize = sizeof(T);
        return Push(&val, valSize);
    }
    /**
      * @brief  �ӽ��յ���Ϣ������ȡ��ֵ��
      * @param  T: ָ�����������͡�
      * @retval T: ȡ����ֵ��ע�⣺�˹��̿��ܳ���������󣬵���ȡ����ֵ���ضϡ�
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
      * @brief  ������Ŀ��ڵ����ͨ�ԡ�ʹ�� GetAckStatus ��ѯ�ò����Ƿ�ͨ����
      * @param  None
      * @retval None
      */
    void Test()
    {
        SendPacket(ShellNode_DefaultCommands_Test, 0, 0);
    }
    /**
      * @brief  ��ͣ�õģ����Ի�ȡĿ��ڵ���õĺ�����Ŀ��ʹ�� GetAckStatus ��ѯ�Ƿ��ȡ�ɹ���ʹ�� GetTargetFunctionCount ��ȡֵ��
      * @param  None
      * @retval None
      */
    void BeginGetTargetFunctionCount()
    {
        SendPacket(ShellNode_DefaultCommands_GetFunctionCount, 0, 0);
    }
    /**
      * @brief  ��ͣ�õģ���ȡ�����ѯ����Ŀ��ڵ�Ŀ��ú�����Ŀ��
      * @param  None
      * @retval uint32_t: Ŀ��ڵ�Ŀ��ú�����Ŀ��
      */
    uint32_t GetTargetFunctionCount()
    {
        return TargetFunctionCount;
    }
    /**
      * @brief  ���Ի�ȡĿ��ڵ���õ���Ϣ������Ŀ��ʹ�� GetAckStatus ��ѯ�Ƿ��ȡ�ɹ���ʹ�� GetTargetMessageCount ��ȡֵ��
      * @param  None
      * @retval None
      */
    void BeginGetTargetMessageCount()
    {
        SendPacket(ShellNode_DefaultCommands_GetMessageCount, 0, 0);
    }
    /**
      * @brief  ��ȡ�����ѯ����Ŀ��ڵ�Ŀ�����Ϣ������Ŀ��
      * @param  None
      * @retval uint32_t: Ŀ��ڵ�Ŀ�����Ϣ������Ŀ��
      */
    uint32_t GetTargetMessageCount()
    {
        return TargetMessageCount;
    }
    /**
      * @brief  ��ͣ�õģ����Ի�ȡĿ��ڵ�ָ���ĺ�����Ϣ��ʹ�� GetAckStatus ��ѯ�Ƿ��ȡ�ɹ���
      * @param  index: ָ���ĺ�������ֵ��
      * @retval None
      */
    void BeginGetTargetFunctionInfo(uint32_t index)
    {
        SendPacket(ShellNode_DefaultCommands_GetFunctionInfo, &index, sizeof(uint32_t));
    }
    /**
      * @brief  ���Ի�ȡĿ��ڵ��ָ����Ϣ������Ϣ��ʹ�� GetAckStatus ��ѯ�Ƿ��ȡ�ɹ���
      * @param  index: ָ������Ϣ��������ֵ��
      * @retval None
      */
    void BeginGetTargetMessageInfo(uint32_t index)
    {
        SendPacket(ShellNode_DefaultCommands_GetMessageInfo, &index, sizeof(uint32_t));
    }
    /**
      * @brief  ��ȡ���յ���Ϣ��������Ԫ�صĸ�����
      * @param  None
      * @retval uint32_t: ���յ���Ϣ��������Ԫ�صĸ�����
      */
    uint32_t GetElementCount()
    {
        return TargetMessageElementCount;
    }
    /**
      * @brief  ��ȡ���һ�η������Ĵ�����롣
      * @param  None
      * @retval ShellNode_ErrorCode_t: ���һ�η������Ĵ�����롣
      */
    ShellNode_ErrorCode_t GetLastErrorCode()
    {
        return LastErrorCode;
    }
    /**
      * @brief  ��ȡָ������ֵ����Ϣ���͵����ơ�
      * @param  index: ָ��������ֵ��
      * @retval char*: ָ������ֵ����Ϣ���͵����ơ�
      */
    char* GetTargetMessageName(uint32_t index)
    {
        return TargetMessageList[index].Name;
    }
    /**
      * @brief  ��ͣ�õģ����Ե���Ŀ��ڵ�ĺ�����
      * @param  funcName: ָ���ĺ������ơ�
      * @param  ...: �����Ĳ�����
      * @retval ShellNode_ErrorCode_t: �����ù��̿��ܳ��ֵĴ���
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
                    va_start(va, funcName); // �ӵ�һ��������ʼ
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
      * @brief  ������Ŀ��ڵ㷢��ָ�����͵���Ϣ��
      * @param  targetMessageName: ָ������Ϣ�������ơ�
      * @retval ShellNode_ErrorCode_t: �����ù��̿��ܳ��ֵĴ���
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
                //��index(4 bytes)��length(2 byte)��element count(2 byte)��message(n byte)��
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
      * @brief  ��ȡ���һ�������Ӧ��״̬����Ӧ��״̬Ϊ true���򱾴β�ѯ�ὫӦ��״̬��Ϊ false.
      * @param  None
      * @retval bool: ָʾ���һ�������Ӧ��״̬��true: �ѽ��յ�Ӧ�����false: ��δ���յ�Ӧ�������
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
      * @brief  ��ս��ջ�����
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
      * @brief  ֪ͨ ShellNode ʵ���ѽ��յ����ֽڡ����豣֤���ֽڵĿɿ��ԡ�
      * @param  byteDat: ���յ����ֽڡ�
      * @retval None
      */
    void PushByte(uint8_t byteDat)
    {
        // ���д�����ĺϷ�֡���򲻽�������
        if (FrameFound)
        {
            return;
        }
        RXBuffer[RXBufferPos] = byteDat;
        // ����֡��ʼ��
        if (RXBuffer[RXBufferPos] == ShellNode_FrameBeginByte &&
                (!RXBufferPos || (RXBuffer[RXBufferPos - 1] != ShellNode_EscapeByte))
           )
        {
            FrameHeaderFound = true;
            FrameHeaderPos = RXBufferPos;
        }
        // ����֡������
        if (FrameHeaderFound)
        {
            if (RXBuffer[RXBufferPos] == ShellNode_FrameEndByte &&
                    (RXBufferPos && (RXBuffer[RXBufferPos - 1] != ShellNode_EscapeByte))
               )
            {
                // ���֡�����Ƿ�Ϸ�
                if (RXBufferPos - FrameHeaderPos - 1 >= ShellNode_EmptyValidPacketLength)
                {
                    FrameTailPos = RXBufferPos;
                    FrameFound = true;
                    return;
                }
            }
        }
        // ������������
        if (RXBufferPos >= ShellNode_RXFIFOLength - 1)
        {
            // �����������������ջ�����
            ClearRXBuffer();
        }
        else
        {
            RXBufferPos++;
        }
    }
    /**
      * @brief  ʹ ShellNode ����������������
      * @param  None
      * @retval ShellNode_ErrorCode_t: �����ù��̿��ܳ��ֵĴ���
      */
    ShellNode_ErrorCode_t Handle()
    {
        ShellNode_ErrorCode_t errCode = ShellNode_ErrorCode_Success;
        volatile bool isAck = false;
        do
        {
            // ��δ�ҵ��Ϸ�֡���򲻽������ݰ�����
            if (!FrameFound)
            {
                errCode = ShellNode_ErrorCode_InvalidFrame;
                break;
            }
            // ��ԭ���ݰ�
            RXPacketPos = 0;
//            int tmpHeader = FrameHeaderPos;
//            int tempTail = FrameTailPos;
            for (int i = FrameHeaderPos + 1; i < FrameTailPos; i++)
            {
                // �������ݰ����
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
                // ����ת���
                if (dat == ShellNode_EscapeByte)
                {
                    uint8_t nextDat = RXBuffer[i + 1];
                    if (nextDat != ShellNode_FrameBeginByte &&
                            nextDat != ShellNode_FrameEndByte &&
                            nextDat != ShellNode_EscapeByte
                       )
                    {
                        // ����֧�ֵ�ת���
                        errCode = ShellNode_ErrorCode_InvalidEscapeCharacter;
                        break;
                    }
                    else
                    {
                        dat = nextDat;
                        i++;
                    }
                }
                // �������ݰ�
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
                // CRC8 У��
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
                            // �ж��յ���Ӧ��Ӧ���
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

// Ĭ�����������
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
        // ���������
        sender->AckPacket((void*)sender->NodeName, strlen(sender->NodeName));
    }
    else
    {
        // ����Ӧ���
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
        // ���������
        sender->AckPacket(&sender->FunctionCount, sizeof(uint32_t));
    }
    else
    {
        // ����Ӧ���
        sender->TargetFunctionCount = *(uint32_t*)params;
    }
}
static void ShellNode_DefaultHandler_GetFunctionInfo(ShellNode* sender, uint8_t sourceAddr, bool ack, void* params, uint32_t pLength)
{
    if (!ack)
    {
        // �����������index(4 bytes)��
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
        // ����Ӧ�����index(4 bytes)��error code(1 byte)��functionInfo(n bytes)��
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
        // ����������� index(1 byte)��parameters(n byte) ��
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
        // ����Ӧ����� error code(1 byte)��retval length(1 byte)��retval(n byte) ��
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
        // ���������
        sender->AckPacket(&sender->MessageCount, sizeof(uint32_t));
    }
    else
    {
        // ����Ӧ���
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
        // �����������index(4 bytes)��
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
        // ����Ӧ�����index(4 bytes)��error code(1 byte)��messageInfo(n bytes)��
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
        // �����������index(4 bytes)��length(2 byte)��element count(2 byte)��message(n byte)��
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
            // ������鲻ͨ��
            *(uint16_t*)&(buffer[bufferPos]) = 0;
            bufferPos += sizeof(uint16_t);
            *(uint16_t*)&(buffer[bufferPos]) = 0;
            bufferPos += sizeof(uint16_t);
            sender->TargetMessageSize = 0;
        }
        else
        {
            // ������ȷ
            sender->TargetMessageSize = length;
            sender->TargetMessageElementCount = cnt;
            for (int i = 0; i < length; i++)
            {
                sender->TargetMessage[i] = msg[i];
            }
            targetMsg->Handler(sender, msg, length); // ֪ͨ��Ϣ������
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
        // ����Ӧ�����index(4 bytes)��error code(1 byte)��length(2 byte)��element count(2 byte)��message(n byte)��
//        uint32_t index = *(uint32_t*)params;
        uint8_t errCode = *(uint8_t*) & (((uint8_t*)params)[4]);
        uint16_t length = *(uint16_t*) & (((uint8_t*)params)[5]);
        uint16_t cnt = *(uint16_t*) & (((uint8_t*)params)[7]);
        uint8_t* msg = &(((uint8_t*)params)[9]);

        sender->TargetMessagePos = 0;
        sender->ClearMessage();
        if (errCode)
        {
            // ������鲻ͨ��
            sender->TargetMessageSize = 0;
            sender->TargetMessageElementCount = 0;
        }
        else
        {
            // ������ȷ
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
