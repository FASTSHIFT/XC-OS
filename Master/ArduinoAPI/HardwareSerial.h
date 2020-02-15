/*
 * MIT License
 * Copyright (c) 2019 _VIFEXTech
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __HardwareSerial_h
#define __HardwareSerial_h

#include "Arduino.h"
#include "Print.h"
#include "Stream.h"

#define SERIAL_RX_BUFFER_SIZE             128
#define SERIAL_PreemptionPriority_Default 1
#define SERIAL_SubPriority_Default        3

typedef enum
{
    SERIAL_8N1,
    SERIAL_8N2,
    SERIAL_8E1,
    SERIAL_8E2,
    SERIAL_8O1,
    SERIAL_8O2,
    SERIAL_8N0_5,
    SERIAL_8N1_5,
    SERIAL_8E0_5,
    SERIAL_8E1_5,
    SERIAL_8O0_5,
    SERIAL_8O1_5,

    SERIAL_9N1,
    SERIAL_9N2,
    SERIAL_9E1,
    SERIAL_9E2,
    SERIAL_9O1,
    SERIAL_9O2,
    SERIAL_9N0_5,
    SERIAL_9N1_5,
    SERIAL_9E0_5,
    SERIAL_9E1_5,
    SERIAL_9O0_5,
    SERIAL_9O1_5,
    
    SERIAL_CFG_MAX
} SERIAL_Config;

class HardwareSerial : public Stream
{
    typedef uint16_t rx_buffer_index_t;
    typedef void(*Serial_CallbackFunction_t)(uint8_t);
public:
    HardwareSerial(USART_TypeDef *USARTx);
    USART_TypeDef *USARTx;
    void IRQHandler();
    void begin(
        uint32_t BaudRate, 
        SERIAL_Config Config = SERIAL_8N1, 
        uint8_t PreemptionPriority = SERIAL_PreemptionPriority_Default, 
        uint8_t SubPriority = SERIAL_SubPriority_Default
    );
    void end(void);
    void attachInterrupt(Serial_CallbackFunction_t function);
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
    Serial_CallbackFunction_t serialCallback;
    volatile rx_buffer_index_t rx_buffer_head;
    volatile rx_buffer_index_t rx_buffer_tail;
    uint8_t rx_buffer[SERIAL_RX_BUFFER_SIZE];
};

extern HardwareSerial Serial;
extern HardwareSerial Serial2;
extern HardwareSerial Serial3;

#endif
