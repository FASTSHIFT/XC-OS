#ifndef HardwareSerial_h
#define HardwareSerial_h

#include "Arduino.h"
#include "Print.h"
#include "Stream.h"

#define SERIAL_RX_BUFFER_SIZE 128
#define SERIAL_Config_Default SERIAL_8N1
#define USART_ChannelPriority_Default 2

typedef uint16_t rx_buffer_index_t;
typedef void(*USART_CallbackFunction_t)(void);

#define Get_USART_WordLength_x(SERIAL_x)    ((uint16_t)(SERIAL_x&0xF000))
#define Get_USART_Parity_x(SERIAL_x)        ((uint16_t)(SERIAL_x&0x0F00))
#define Get_USART_StopBits_x(SERIAL_x)      ((uint16_t)((SERIAL_x&0x00F0)<<8))

typedef enum
{
    SERIAL_8N1 = USART_WordLength_8b | USART_Parity_No | (USART_StopBits_1 >> 8),
    SERIAL_8N2 = USART_WordLength_8b | USART_Parity_No | (USART_StopBits_2 >> 8),
    SERIAL_8E1 = USART_WordLength_8b | USART_Parity_Even | (USART_StopBits_1 >> 8),
    SERIAL_8E2 = USART_WordLength_8b | USART_Parity_Even | (USART_StopBits_2 >> 8),
    SERIAL_8O1 = USART_WordLength_8b | USART_Parity_Odd | (USART_StopBits_1 >> 8),
    SERIAL_8O2 = USART_WordLength_8b | USART_Parity_Odd | (USART_StopBits_2 >> 8),
    SERIAL_8N1_5 = USART_WordLength_8b | USART_Parity_No | (USART_StopBits_1_5 >> 8),
    SERIAL_8E1_5 = USART_WordLength_8b | USART_Parity_Even | (USART_StopBits_1_5 >> 8),
    SERIAL_8O1_5 = USART_WordLength_8b | USART_Parity_Odd | (USART_StopBits_1_5 >> 8),

    SERIAL_9N1 = USART_WordLength_9b | USART_Parity_No | (USART_StopBits_1 >> 8),
    SERIAL_9N2 = USART_WordLength_9b | USART_Parity_No | (USART_StopBits_2 >> 8),
    SERIAL_9E1 = USART_WordLength_9b | USART_Parity_Even | (USART_StopBits_1 >> 8),
    SERIAL_9E2 = USART_WordLength_9b | USART_Parity_Even | (USART_StopBits_2 >> 8),
    SERIAL_9O1 = USART_WordLength_9b | USART_Parity_Odd | (USART_StopBits_1 >> 8),
    SERIAL_9O2 = USART_WordLength_9b | USART_Parity_Odd | (USART_StopBits_2 >> 8),
    SERIAL_9N1_5 = USART_WordLength_9b | USART_Parity_No | (USART_StopBits_1_5 >> 8),
    SERIAL_9E1_5 = USART_WordLength_9b | USART_Parity_Even | (USART_StopBits_1_5 >> 8),
    SERIAL_9O1_5 = USART_WordLength_9b | USART_Parity_Odd | (USART_StopBits_1_5 >> 8),
} SERIAL_Config;

class HardwareSerial : public Stream
{
public:
    HardwareSerial(USART_TypeDef *USARTx);
    void IRQHandler();
    void begin(uint32_t BaudRate);
    void begin(uint32_t BaudRate, SERIAL_Config Config);
    void begin(uint32_t BaudRate, SERIAL_Config Config, uint8_t ChannelPriority);
    void end(void);
    void attachInterrupt(USART_CallbackFunction_t Function);
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);

    virtual size_t write(uint8_t n);
    inline size_t write(unsigned long n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(long n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(unsigned int n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(int n)
    {
        return write((uint8_t)n);
    }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool()
    {
        return true;
    }

private:
    USART_TypeDef *USARTx;
    USART_CallbackFunction_t USART_Function;
    volatile uint16_t _rx_buffer_head;
    volatile uint16_t _rx_buffer_tail;
    uint8_t _rx_buffer[SERIAL_RX_BUFFER_SIZE];
};

extern HardwareSerial Serial;
extern HardwareSerial Serial2;

#endif
