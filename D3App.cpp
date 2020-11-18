#define VC_EXTRALEAN
#include <SDKDDKVer.h>
#include <afxwin.h>
#include <stdint.h>
#include "res\resource.h"
#include "D3IRender.h"

class C3DView : public CView
{
    DECLARE_MESSAGE_MAP()
    DECLARE_DYNCREATE(C3DView)
    ~C3DView() { delete _pRender; }

    using uint = uint32_t;
    IRender* _pRender = NULL;
    uint m_nMode    = ID_MODE_DEFAULT;
    uint m_nModel   = ID_MODEL_DEFAULT;
    uint m_nScale   = ID_SCALE_DEFAULT;
    uint m_nOffset  = ID_OFFSET_DEFAULT;
    uint m_nSpeed   = ID_SPEED_DEFALT;
    Options _options;

    const Options::Mode  Modes[3]   = { Options::Wireframe, Options::DepthBuffer,   Options::Image, };
    const Options::Model Models[6]  = { Options::Up,        Options::Frankie,       Options::Mixed,     Options::Halfempty, Options::Earth, Options::Grid, };
    const Options::Speed Speeds[4]  = { Options::fast,          Options::medium,    Options::slow, };
    const float          Scales[5]  = { 5, 10, 15, 20, 25, };
    const float          Offsets[5] = { 5, 10, 15, 20, 25, };
    const char*          surfaces[4] = { "Up.bmp", "Frankie.bmp", "Earth.bmp", "Grid.bmp", };

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs)
    {
        cs.lpszClass = AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW), CreateSolidBrush(RGB(0, 0, 0)), 0);
        return CView::PreCreateWindow(cs);
    }
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct)
    {
        _pRender = IRender::Create(*this);
        _options.surfaces = surfaces;
        _options.stats  = false;
        _options.pause  = false;
        _options.mode   = Modes[  m_nMode   - ID_MODE];
        _options.model  = Models[ m_nModel  - ID_MODEL];
        _options.scale  = Scales[ m_nScale  - ID_SCALE];
        _options.offset = Offsets[m_nOffset - ID_OFFSET];
        _options.speed  = Speeds[ m_nSpeed  - ID_SPEED];
        return CView::OnCreate(lpCreateStruct);
    }
    virtual void OnDraw(CDC* pDC) { _pRender->Draw(*pDC, _options); }

    void OnAppAbout()       { CDialog(ID_ABOUTBOX).DoModal(); }
    void OnStats()          { OnToggleHandler(ID_MODE_STATS,  _options.stats);}
    void OnPause()          { OnToggleHandler(ID_SPEED_PAUSE, _options.pause);}
    void OnMode(UINT nID)   { OnRangeHandler(nID, m_nMode,   ID_MODE,   _options.mode,   Modes  ); }
    void OnModel(UINT nID)  { OnRangeHandler(nID, m_nModel,  ID_MODEL,  _options.model,  Models ); }
    void OnScale(UINT nID)  { OnRangeHandler(nID, m_nScale,  ID_SCALE,  _options.scale,  Scales ); }
    void OnOffset(UINT nID) { OnRangeHandler(nID, m_nOffset, ID_OFFSET, _options.offset, Offsets); }
    void OnSpeed(UINT nID)  { OnRangeHandler(nID, m_nSpeed,  ID_SPEED,  _options.speed,  Speeds ); }
    void OnToggleHandler(uint id, bool& option)
    {
        CMenu* pMenu = AfxGetMainWnd()->GetMenu();
        option = !option;
        pMenu->CheckMenuItem(id, option ? MF_CHECKED : MF_UNCHECKED);
        InvalidateRect(NULL, FALSE);
    }
    template<typename T1, typename T2>
    void OnRangeHandler(UINT nID, uint& nSetting, uint idBase, T1& option, T2 values[])
    {
        CMenu* pMenu = AfxGetMainWnd()->GetMenu();
        pMenu->CheckMenuItem(nSetting, MF_UNCHECKED);
        nSetting = nID;
        option = values[nSetting - idBase];
        pMenu->CheckMenuItem(nSetting, MF_CHECKED);
        InvalidateRect(NULL, FALSE);
    }
};

IMPLEMENT_DYNCREATE(C3DView, CView)
BEGIN_MESSAGE_MAP(C3DView, CView)
    ON_WM_CREATE()
    ON_COMMAND(ID_MODE_STATS, OnStats)
    ON_COMMAND(ID_SPEED_PAUSE, OnPause)
    ON_COMMAND(ID_ABOUTBOX, OnAppAbout)
    ON_COMMAND_RANGE(ID_MODE_FIRST,   ID_MODE_LAST,   OnMode    )
    ON_COMMAND_RANGE(ID_MODEL_FIRST,  ID_MODEL_LAST,  OnModel   )
    ON_COMMAND_RANGE(ID_SCALE_FIRST,  ID_SCALE_LAST,  OnScale   )
    ON_COMMAND_RANGE(ID_OFFSET_FIRST, ID_OFFSET_LAST, OnOffset  )
    ON_COMMAND_RANGE(ID_SPEED_FIRST,  ID_SPEED_LAST,  OnSpeed   )
END_MESSAGE_MAP()

class CDoc : public CDocument
{ DECLARE_DYNCREATE(CDoc) };
IMPLEMENT_DYNCREATE(CDoc, CDocument)

class C3DApp : public CWinApp
{
    virtual BOOL InitInstance()
    {
        CWinApp::InitInstance();
        AddDocTemplate(new CSingleDocTemplate(IDR_D3, RUNTIME_CLASS(CDoc), RUNTIME_CLASS(CFrameWnd), RUNTIME_CLASS(C3DView)));
        OnFileNew();
        m_pMainWnd->ShowWindow(SW_SHOW);
        m_pMainWnd->UpdateWindow();
        m_pMainWnd->SetWindowText("");
        return TRUE;
    }
} theApp;
