#include <windows.h>
#include <functional>
#include <memory>
#include <map>

#include "D3.h"
#include "D3IRender.h"
#include "res/resource.h"

using namespace D3;

class Render : public IRender
{
    friend IRender;

    float   _angle = {};
    HWND    _hWnd;
    Rect    _rectImage = {};
    uint64_t _nPixels = {};
    uint    _nFrames = {};
    uint    _nStart = {};
    Options _options = {};

    using Shared = std::shared_ptr<D3::Model>;
    using Map = std::map<Options::Model, Shared>;
    using PQuad = std::shared_ptr<RGBQUAD[]>;
    using PUint = std::shared_ptr<uint[]>;

    Map     _mapModels;
    PQuad   _image;
    PUint   _depth;

    struct SurfaceInfo
    {
        LONG    id = {};
        PQuad   surface;
        int     width = {};
        int     height = {};
    };

    using MapSurfaces = std::map<LONG, SurfaceInfo>;
    MapSurfaces _mapSurfaces;

    Render(HWND hWnd) : _hWnd(hWnd), _nStart(GetTickCount()) { ::SetTimer(_hWnd, (UINT_PTR)this, 1, TimerProc); }
    static void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR event, DWORD dwTime) { ((IRender*)event)->Timer(); }
    virtual void Timer() { _angle += 1; InvalidateRect(_hWnd, NULL, FALSE); }
    virtual void Draw(HDC hDC, Options& options);
private:
    Shared  GetModel(Options::Model model);
    Screen  CreateWorld(Rect& rect, Shared& model, float angle, float fScale, float fOffset);
    PQuad   GetSurface(LONG id, int& sHeight, int& sWidth);
    void    RenderWireFrame(Mesh& mesh, HDC hDepth);
    void    RenderBitmaps(const Mesh& mesh, uint* depth, RGBQUAD* image, uint& min, uint& max);
    void    DrawStats(HDC hdc, COLORREF color, bool doMPixels);
};

IRender* IRender::Create(HWND hWnd)
{
    return new Render(hWnd);
}

Render::Shared Render::GetModel(Options::Model model)
{
    Shared ret;

    auto it = _mapModels.find(model);
    if (it != _mapModels.end())
    {
        ret = it->second;
    }
    else
    {
        switch (model)
        {
        default:
        case Options::Up:
            ret = std::make_shared<Model>(Model({
                { { { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 } }, IDB_UP, { {    0,    0 }, {    0,  179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1, -1,  1 }, { -1, -1,  1 } }, IDB_UP, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 } }, IDB_UP, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1, -1 }, {  1, -1, -1 }, { -1, -1, -1 } }, IDB_UP, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1,  1, -1 }, { -1,  1,  1 }, {  1,  1,  1 } }, IDB_UP, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, { -1,  1, -1 } }, IDB_UP, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, {  1, -1,  1 } }, IDB_UP, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1, -1,  1 }, {  1, -1, -1 }, { -1, -1, -1 } }, IDB_UP, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { {  1, -1, -1 }, {  1, -1,  1 }, {  1,  1,  1 } }, IDB_UP, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, {  1, -1, -1 } }, IDB_UP, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, { -1,  1,  1 } }, IDB_UP, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1,  1,  1 }, { -1,  1, -1 }, { -1, -1, -1 } }, IDB_UP, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                }));
            break;
        case Options::Frankie:
            ret = std::make_shared<Model>(Model({
                { { { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 } }, IDB_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1, -1,  1 }, { -1, -1,  1 } }, IDB_FRANKIE, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 } }, IDB_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1, -1 }, {  1, -1, -1 }, { -1, -1, -1 } }, IDB_FRANKIE, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1,  1, -1 }, { -1,  1,  1 }, {  1,  1,  1 } }, IDB_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, { -1,  1, -1 } }, IDB_FRANKIE, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, {  1, -1,  1 } }, IDB_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1, -1,  1 }, {  1, -1, -1 }, { -1, -1, -1 } }, IDB_FRANKIE, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { {  1, -1, -1 }, {  1, -1,  1 }, {  1,  1,  1 } }, IDB_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, {  1, -1, -1 } }, IDB_FRANKIE, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, { -1,  1,  1 } }, IDB_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1,  1,  1 }, { -1,  1, -1 }, { -1, -1, -1 } }, IDB_FRANKIE, { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                }));
            break;
        case Options::Mixed:
            ret = std::make_shared<Model>(Model({
                { { { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 } }, IDB_FRANKIE,  { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1, -1,  1 }, { -1, -1,  1 } }, IDB_UP,       { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 } }, IDB_FRANKIE,  { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1, -1 }, {  1, -1, -1 }, { -1, -1, -1 } }, IDB_UP,       { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1,  1, -1 }, { -1,  1,  1 }, {  1,  1,  1 } }, IDB_FRANKIE,  { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, { -1,  1, -1 } }, IDB_UP,       { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, {  1, -1,  1 } }, IDB_FRANKIE,  { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1, -1,  1 }, {  1, -1, -1 }, { -1, -1, -1 } }, IDB_UP,       { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { {  1, -1, -1 }, {  1, -1,  1 }, {  1,  1,  1 } }, IDB_FRANKIE,  { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, {  1, -1, -1 } }, IDB_UP,       { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, { -1,  1,  1 } }, IDB_FRANKIE,  { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1,  1,  1 }, { -1,  1, -1 }, { -1, -1, -1 } }, IDB_UP,       { {  179,  179 }, {  179,   0 }, {    0,    0 } } },
                }));
            break;
        case Options::Halfempty:
            ret = std::make_shared<Model>(Model({
                { { { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 } }, IDB_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 } }, IDB_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1,  1, -1 }, { -1,  1,  1 }, {  1,  1,  1 } }, IDB_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, {  1, -1,  1 } }, IDB_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { {  1, -1, -1 }, {  1, -1,  1 }, {  1,  1,  1 } }, IDB_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, { -1,  1,  1 } }, IDB_FRANKIE, { {    0,    0 }, {    0, 179 }, {  179,  179 } } },
                }));
            break;
        case Options::Earth:
            ret = std::make_shared<Model>(Model({
                { { {  1,  1,  1 }, { -1,  1,  1 }, { -1,  1, -1 } }, IDB_EARTH, { { 200, 400 }, { 200, 600 }, { 400, 600 } } },
                { { { -1,  1, -1 }, {  1,  1, -1 }, {  1,  1,  1 } }, IDB_EARTH, { { 400, 600 }, { 400, 400 }, { 200, 400 } } },
                { { { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 } }, IDB_EARTH, { {   0, 200 }, {   0, 400 }, { 200, 400 } } },
                { { {  1,  1,  1 }, {  1, -1,  1 }, { -1, -1,  1 } }, IDB_EARTH, { { 200, 400 }, { 200, 200 }, {   0, 200 } } },
                { { {  1, -1, -1 }, {  1, -1,  1 }, {  1,  1,  1 } }, IDB_EARTH, { { 400, 200 }, { 200, 200 }, { 200, 400 } } },
                { { {  1,  1,  1 }, {  1,  1, -1 }, {  1, -1, -1 } }, IDB_EARTH, { { 200, 400 }, { 400, 400 }, { 400, 200 } } },
                { { {  1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 } }, IDB_EARTH, { { 400, 200 }, { 600, 400 }, { 400, 400 } } },
                { { { -1,  1, -1 }, { -1, -1, -1 }, {  1, -1, -1 } }, IDB_EARTH, { { 600, 400 }, { 600, 200 }, { 400, 200 } } },
                { { { -1, -1,  1 }, {  1, -1,  1 }, {  1, -1, -1 } }, IDB_EARTH, { { 200,   0 }, { 200, 200 }, { 400, 200 } } },
                { { {  1, -1, -1 }, { -1, -1, -1 }, { -1, -1,  1 } }, IDB_EARTH, { { 400, 200 }, { 400,   0 }, { 200,   0 } } },
                { { { -1, -1, -1 }, { -1, -1,  1 }, { -1,  1,  1 } }, IDB_EARTH, { { 600, 200 }, { 800, 200 }, { 800, 400 } } },
                { { { -1,  1,  1 }, { -1,  1, -1 }, { -1, -1, -1 } }, IDB_EARTH, { { 800, 400 }, { 600, 400 }, { 600, 200 } } },
                }));
            break;
        case Options::Grid:
            ret = std::make_shared<Model>(Model({
                { { { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 } }, IDB_GRID, { {   0,   0 }, {  0, 200 }, { 200, 200 } } },
                { { {  1,  1,  1 }, {  1, -1,  1 }, { -1, -1,  1 } }, IDB_GRID, { { 200, 200 }, { 200,  0 }, {   0,   0 } } },
                }));
            break;
        }
        _mapModels[model] = ret;
    }
    return ret;
}

Screen Render::CreateWorld(Rect& rect, Shared& model, float angle, float fScale, float fOffset)
{
    Model modelX = *model * Scale(fScale, fScale, fScale);
    Model modelY = modelX * RotateZ(90);
    Model modelZ = modelX * RotateY(90);

    World world;
    world += modelX * (RotateX(angle) *                                   Translate(-fOffset, -fOffset, -20));
    world += modelY * (RotateY(angle) *                                   Translate(-fOffset, fOffset, -40));
    world += modelZ * (RotateZ(angle) *                                   Translate(fOffset, -fOffset, -60));
    world += modelX * (RotateX(angle) * RotateY(angle) * RotateZ(angle) * Translate(fOffset, fOffset, 0));

    Matrix pov = PointOfView({ 0, 0, 100 }, { 0, 0, 0 }, { 0, 1, 0 });
    Matrix fov = FieldOfView(45, rect.AspectRatio(), 1, 100);
    Matrix view = Viewport(rect, 0, 100);

    Screen screen = world * (pov * fov * view);
    screen.PerspectiveDivide();

    return screen;
}

Render::PQuad Render::GetSurface(LONG id, int& height, int&width)
{
    auto it = _mapSurfaces.find(id);
    if (it != _mapSurfaces.end())
    {
        SurfaceInfo& si = it->second;
        width = si.width;
        height = si.height;
        return  si.surface;;
    }
    else
    {
        struct
        {
            BITMAPINFO bmi;
            RGBQUAD rgb[2];
        } info = { sizeof(BITMAPINFOHEADER) };

        HDC hdc = CreateCompatibleDC(nullptr);
        HBITMAP hBmp = (HBITMAP)LoadImageA(GetModuleHandle(NULL), _options.surfaces[id - IDB_SURFACES], IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_SHARED | LR_LOADFROMFILE);

        int rc = GetDIBits(hdc, hBmp, 0, 0, nullptr, &info.bmi, BI_RGB);
        width = info.bmi.bmiHeader.biWidth;
        height = info.bmi.bmiHeader.biHeight;

        PQuad surface(new RGBQUAD[info.bmi.bmiHeader.biSizeImage / sizeof(RGBQUAD)]);
        rc = GetDIBits(hdc, hBmp, 0, height, surface.get(), &info.bmi, BI_RGB);
        _mapSurfaces[id] = { id, surface, width, height };

        DeleteObject(hBmp);
        DeleteDC(hdc);

        return surface;
    }
}

void Render::RenderWireFrame(Mesh& mesh, HDC hDepth)
{
    PolyPoly polyPoly;
    mesh.ExportPolyPoly(polyPoly);
    polyPoly.Draw(hDepth);
}

void Render::RenderBitmaps(const Mesh& mesh, uint* depth, RGBQUAD* image, uint& min, uint& max)
{
    int count = mesh.Count();
    for (int i = 0; i < count; i++)
    {
        D3::Polygon polygon = mesh[i];

        int sHeight = {};
        int sWidth = {};
        PQuad surface = GetSurface(polygon.id, sHeight, sWidth);

        Point3& p0 = polygon.tripple3.p0;
        Point3& p1 = polygon.tripple3.p1;
        Point3& p2 = polygon.tripple3.p2;

        Point2& s0 = polygon.tripple2.p0;
        Point2& s1 = polygon.tripple2.p1;
        Point2& s2 = polygon.tripple2.p2;

        if (p0.Y() > p1.Y()) { std::swap(p0, p1); std::swap(s0, s1); }
        if (p0.Y() > p2.Y()) { std::swap(p0, p2); std::swap(s0, s2); }
        if (p1.Y() > p2.Y()) { std::swap(p1, p2); std::swap(s1, s2); }

        uint64_t& nPixels = _nPixels;
        auto Rasterize = [&nPixels, &min, &max, rect=_rectImage, depth, image, sWidth, sHeight, surface=surface.get()](Point3& p0, Point3& p1, Point3& p2, Point3& p3, Point2& s0, Point2& s1, Point2& s2, Point2& s3)
        {
            int width = rect.Width();
            int den0 = int(p2.Y() - p0.Y());
            int den1 = int(p3.Y() - p1.Y());
            for (int y = int(p0.Y()); y < int(p2.Y()); y++)
            {
                if ((y < rect.top) || (y >= rect.bottom)) continue;

                int num0  = int(y - p0.Y());
                int num1  = int(y - p1.Y());
                int h0    = int(p0.X());
                int h1    = int(p1.X());
                float d0  = p0.Z();
                float d1  = p1.Z();
                float s0x = s0.x;
                float s0y = s0.y;
                float s1x = s1.x;
                float s1y = s1.y;

                if (den0 != 0)
                {
                    h0 += int((p2.X() - p0.X()) * num0 / den0);
                    d0 +=    ((p2.Z() - p0.Z()) * num0 / den0);
                    s0x +=   ((s2.x   - s0.x)   * num0 / den0);
                    s0y +=   ((s2.y   - s0.y)   * num0 / den0);
                }
                if (den1 != 0)
                {
                    h1 += int((p3.X() - p1.X()) * num1 / den1);
                    d1 +=    ((p3.Z() - p1.Z()) * num1 / den1);
                    s1x +=   ((s3.x   - s1.x)   * num1 / den1);
                    s1y +=   ((s3.y   - s1.y)   * num1 / den1);
                }
                if (h0 > h1)
                {
                    std::swap(h0,  h1);
                    std::swap(d0,  d1);
                    std::swap(s0x, s1x);
                    std::swap(s0y, s1y);
                }

                int den = h1 - h0;
                for (int x = h0; x < h1; x++)
                {
                    if ((x < rect.left) || (x >= rect.right)) continue;

                    int num  =  x - h0;
                    float d  = d0;
                    float sx = s0x;
                    float sy = s0y;

                    if (den != 0)
                    {
                        d  += (d1  - d0)  * num / den;
                        sx += (s1x - s0x) * num / den;
                        sy += (s1y - s0y) * num / den;
                    }

                    uint dd = uint(d * 10000);
                    uint ndex = y * width + x;
                    if (dd < (depth[ndex]))
                    {
                        if (min > dd) min = dd;
                        if (max < dd) max = dd;
                        depth[ndex] = dd;
                        if (image && (int(sy)<sHeight))
                        {
                            image[ndex] = surface[int(sy) * sWidth + int(sx)];
                        }
                    }
                    nPixels++;
                }
            }
        };

        Rasterize(p0, p0, p1, p2, s0, s0, s1, s2);
        Rasterize(p1, p0, p2, p2, s1, s0, s2, s2);
    }
}

void Render::DrawStats(HDC hdc, COLORREF color, bool doMPixels)
{
    if (_options.stats)
    {
        _nFrames++;
        char sz[30] = {};
        SetTextColor(hdc, color);
        SetBkColor(hdc, 0xffffff - color);
        if (!_options.pause)
        {
            uint delta = GetTickCount() - _nStart + 1;
            int len = sprintf(sz, "Frames/S = %f", double(_nFrames) * 1000 / delta);
            TextOut(hdc, 0, 0, sz, len);
            if (doMPixels)
            {
                len = sprintf(sz, "MPixels/S = %f", double(_nPixels) / 1000 / delta);
                TextOut(hdc, 0, 20, sz, len);
            }
        }
        else
        {
            TextOutA(hdc, 0, 0, "Paused", 6);
        }
    }
}

void Render::Draw(HDC hdcScreen, Options& options)
{
    if (_options != options)
    {
        _options = options;
        _nPixels = _nFrames = 0;
        _nStart = GetTickCount();

        if (_options.pause) { ::KillTimer(_hWnd, (UINT_PTR)this); } 
        else                { ::SetTimer( _hWnd, (UINT_PTR)this, _options.speed, TimerProc); }
    }

    Rect rect;
    GetClientRect(_hWnd, &rect);
    if ((_rectImage != rect) || !_depth || !_image)
    {
        _rectImage = rect;
        PUint depth(new uint[rect.Width()*rect.Height()]);
        PQuad image(new RGBQUAD[rect.Width()*rect.Height()]);
        _depth = depth;
        _image = image;
    }

    Shared model = GetModel(_options.model);
    Screen screen = CreateWorld(rect, model, float(_angle), float(_options.scale), float(_options.offset));
    uint min = UINT_MAX;
    uint max = 0;

    HDC hdc = CreateCompatibleDC(nullptr);
    switch(_options.mode)
    {
    default:
    case Options::Wireframe:
        {
            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.Width(), rect.Height());
            SelectObject(hdc, hBitmap);
            BitBlt(hdc, 0, 0, rect.Width(), rect.Height(), hdc, 0, 0, BLACKNESS);

            RenderWireFrame(screen, hdc);

            DrawStats(hdc, RGB(255,255,255), false);
            BitBlt(hdcScreen, 0, 0, rect.Width(), rect.Height(), hdc, 0, 0, SRCCOPY);
            DeleteObject(hBitmap);
            break;
        }
    case Options::DepthBuffer:
        {
            uint* depth = _depth.get();
            uint size = _rectImage.Width() * _rectImage.Height();
            memset(depth, 0xff, size * sizeof(*depth));

            RenderBitmaps(screen, depth, nullptr, min, max);

            int diff = max - min;
            if (diff)
            {
                for (uint i = 0; i < size; i++)
                {
                    if (depth[i] <= max)
                    {
                        depth[i] = (depth[i] - min) * 255 / diff * 0x010101;
                    }
                }
            }
            HBITMAP hBitmap = CreateBitmap(rect.Width(), rect.Height(), 1, 32, depth);
            SelectObject(hdc, hBitmap);
            DrawStats(hdc, RGB(0, 0, 0), true);
            BitBlt(hdcScreen, 0, 0, rect.Width(), rect.Height(), hdc, 0, 0, SRCCOPY);
            DeleteObject(hBitmap);
            break;
        }
    case Options::Image:
        {
            uint* depth = _depth.get();
            RGBQUAD* image = _image.get();
            uint size = _rectImage.Width() * _rectImage.Height();
            memset(depth, 0xff, size * sizeof(*depth));
            memset(image, 0x00, size * sizeof(*image));

            RenderBitmaps(screen, depth, image, min, max);

            HBITMAP hBitmap =  CreateBitmap(rect.Width(), rect.Height(), 1, 32, image);
            SelectObject(hdc, hBitmap);
            DrawStats(hdc, RGB(255, 255, 255), true);
            BitBlt(hdcScreen, 0, 0, rect.Width(), rect.Height(), hdc, 0, 0, SRCCOPY);
            DeleteObject(hBitmap);
            break;
        }
    }
    DeleteDC(hdc);
}
