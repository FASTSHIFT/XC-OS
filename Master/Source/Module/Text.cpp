#include "Module.h"

static char GlobalTextBuff[5 * 1024];

char * TextBuf_GetBuff(uint32_t start)
{
    return (GlobalTextBuff + start);
}

uint32_t TextBuf_GetSize()
{
    return sizeof(GlobalTextBuff);
}

void TextBuf_Clear(uint8_t n)
{
    for(uint32_t i = 0; i < sizeof(GlobalTextBuff); i++)
    {
        GlobalTextBuff[i] = n;
    }
}


