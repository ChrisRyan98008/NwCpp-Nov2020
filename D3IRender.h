#pragma once
#include <memory>

struct Options
{
    enum Mode
    {
        Wireframe,
        DepthBuffer,
        Image,
    };
    enum Model
    {
        Up,
        Frankie,
        Mixed,
        Halfempty,
        Earth,
        Grid
    };
    enum Speed
    {
        slow    = 100,
        medium  = 50,
        fast    = 1,
    };

    bool    stats   = {};
    bool    pause   = {};
    Speed   speed   = fast;
    Mode    mode    = Wireframe;

    Model   model = Up;
    float   scale = {};
    float   offset = {};

    const char** surfaces = {};

    bool operator != (const Options& rhs) { return !operator==(rhs); }
    bool operator == (const Options& rhs)
    {
        return ((stats  == rhs.stats)   &&
                (pause  == rhs.pause)  &&
                (speed  == rhs.speed)   &&
                (mode   == rhs.mode)    &&
                (model  == rhs.model)   &&
                (scale  == rhs.scale)   &&
                (offset == rhs.offset));
    }
};

class IRender
{
public:
    static IRender* Create(HWND hWnd);

    virtual ~IRender() {}

    virtual void Timer() = 0;
    virtual void Draw(HDC hDC, Options& options) = 0;
};

