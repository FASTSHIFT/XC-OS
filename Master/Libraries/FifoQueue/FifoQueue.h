#ifndef __FIFOQUEUE_H
#define __FIFOQUEUE_H

#include <stdint.h>

template <class T> class FifoQueue {
public:
    FifoQueue(uint32_t bufferSize);
    FifoQueue(T* buffer, uint32_t bufferSize);
    ~FifoQueue();
    uint32_t available()
    {
        return ((uint32_t)(BufferSize + Head - Tail)) % BufferSize;
    }
    uint32_t size()
    {
        return BufferSize;
    }
    bool write(T data);
    T read();
    void flush()
    {
        Head = Tail;
    }

private:
    T *Buffer;
    bool IsBufferMalloc;
    uint32_t Head;
    uint32_t Tail;
    uint32_t BufferSize;
};

template <class T> FifoQueue<T>::FifoQueue(T* buffer, uint32_t bufferSize)
{
    BufferSize = bufferSize;
    Buffer = buffer;
    IsBufferMalloc = false;
    Head = Tail = 0;
}

template <class T> FifoQueue<T>::FifoQueue(uint32_t bufferSize)
{
    BufferSize = bufferSize;
    Buffer = new T[BufferSize];
    IsBufferMalloc = true;
    Head = Tail = 0;
}

template <class T> FifoQueue<T>::~FifoQueue()
{
    if(IsBufferMalloc)
        delete[] Buffer;
}

template <class T> T FifoQueue<T>::read()
{
    if (Head == Tail)
        return T(0);
    else
    {
        T c = Buffer[Tail];
        Tail = (Tail + 1) % BufferSize;
        return c;
    }
}

template <class T> bool FifoQueue<T>::write(T data)
{
    uint32_t i = (uint32_t)(Head + 1) % BufferSize;
    if (i != Tail)
    {
        Buffer[Head] = data;
        Head = i;
        return true;
    }
    else
        return false;
}

#endif
