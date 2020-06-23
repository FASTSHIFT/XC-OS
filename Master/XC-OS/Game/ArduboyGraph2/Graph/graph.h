#pragma once

struct Window
{
    float xMin;
    float xMax;
    float yMin;
    float yMax;
};

class Graph
{
public:
    Graph()
    {
        width = 63;
        height = 63;
        window.xMin = -10.0f;
        window.xMax = 10.0f;
        window.yMin = -10.0f;
        window.yMax = 10.0f;
    }
    void draw(Function function);

    int8_t graphToScreenX(const float x);
    int8_t graphToScreenY(const float y);

    float screenToGraphX(const int8_t x);
    float screenToGraphY(const int8_t y);

    Window window;// = {-10.0f, 10.0f, -10.0f, 10.0f};
private:
    // float data[GRAPH_WIDTH];

    uint8_t width;
    uint8_t height;

    static int8_t keepReasonable(const float n);
    static float lerp(const float a, const float b, const float p);
    static float invLerp(const float a, const float b, const float p);
};
