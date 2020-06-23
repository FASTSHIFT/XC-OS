#include "graph.h"

void Graph::draw(Function function)
{
    ab.drawFastVLine(graphToScreenX(0), 0, height);
    ab.drawFastHLine(0, graphToScreenY(0), width);

    // float lastNotch = ;
    // for (uint8_t i = 0; i < width; i++) {
    //  ab.drawFastVLine(i, graphToScreenY(0) - 1, 3);
    // }
    // for (uint8_t i = 0; i < height; i++) {
    //  ab.drawFastHLine(graphToScreenX(0) - 1, i, 3);
    // }

    if (window.yMin < 0.0f && window.yMax > 0.0f)
    {
        for (float i = round(window.xMin); i < round(window.xMax); i += 1.0f)
        {
            ab.drawFastVLine(graphToScreenX(i), graphToScreenY(0) - 1, 2);
        }
    }
    if (window.xMin < 0.0f && window.xMax > 0.0f)
    {
        for (float i = round(window.yMin); i < round(window.yMax); i += 1.0f)
        {
            ab.drawFastHLine(graphToScreenX(0), graphToScreenY(i), 2);
        }
    }

    float a;

    int8_t prevY;
    ErrorType error = function.calculate(screenToGraphX(0), a);
    if (error != (ErrorType)0/*ErrorType::OK*/)
    {
        lazierShowError(error);
        return;
    }

    int8_t currentY = graphToScreenY(a);
    for (uint8_t i = 1; i < width; i++)
    {
        prevY = currentY;
        error = function.calculate(screenToGraphX(i), a);
        currentY = graphToScreenY(a);
        ab.drawLine(i - 1, prevY, i, currentY);
    }
}

int8_t Graph::graphToScreenX(const float x)
{
    return keepReasonable(invLerp(window.xMin, window.xMax, x) * (float)width);
}
int8_t Graph::graphToScreenY(const float y)
{
    return keepReasonable(invLerp(window.yMax, window.yMin, y) * (float)height);
}

float Graph::screenToGraphX(const int8_t x)
{
    return lerp(window.xMin, window.xMax, x / (float)(width - 1));
}
float Graph::screenToGraphY(const int8_t y)
{
    return lerp(window.yMax, window.yMin, y / (float)(height - 1));
}

// STATIC

int8_t Graph::keepReasonable(const float n)
{
    if (n > 127.0f)
        return 127;
    else if (n < -128.0f)
        return -128;
    else
        return (int8_t)round(n);
}

float Graph::lerp(const float a, const float b, const float p)
{
    return (1 - p) * a + p * b;
}
float Graph::invLerp(const float a, const float b, const float p)
{
    return (p - a) / (b - a);
}
