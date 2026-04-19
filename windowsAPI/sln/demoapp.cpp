#include "arukanoido.h"

template <typename T>
inline void SafeRelease(T*& p)
{

    if (NULL != p)
    {
        p->Release();
        p = NULL;
    }
}

DemoApp::DemoApp(int test)
    :
    m_pD2DBitmap(NULL),
    m_pConvertedSourceBitmap(NULL),
    m_pIWICFactory(NULL),
    m_pD2DFactory(NULL),
    m_pRT(NULL),
    m_hInst(NULL),
    m_activate(FALSE),
    m_hWnd(NULL),
    m_isexist(TRUE),
    test(test)
{}

DemoApp::~DemoApp()
{
    SafeRelease(m_pD2DBitmap);
    SafeRelease(m_pConvertedSourceBitmap);
    SafeRelease(m_pIWICFactory);
    SafeRelease(m_pD2DFactory);
    SafeRelease(m_pRT);
}

HRESULT DemoApp::Initialize(HINSTANCE hInstance)//to return hr, this is not constracter
{
    HRESULT hr = S_OK;

    // Create WIC factory
    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_pIWICFactory)
    );

    if (SUCCEEDED(hr))
    {
        // Create D2D factory
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
    }

    //set and create window 
    HWND hwndout = NULL;
    if (SUCCEEDED(hr)) hr = setWindow(hInstance, &hwndout);
    m_hInst = hInstance;
    m_hWnd = hwndout;
    return hr;
}

/******************************************************************
*  Load an image file and create an D2DBitmap                     *
******************************************************************/
HRESULT DemoApp::CreateD2DBitmapFromFile(HWND hWnd, WCHAR* name)
{
    HRESULT hr = S_OK;


    // Step 1: Create the open dialog box and locate the image file
 //   if (LocateImageFile(hWnd, szFileName, ARRAYSIZE(szFileName)))
 //  {
        // Step 2: Decode the source image

        // Create a decoder
    IWICBitmapDecoder* pDecoder = NULL;

    hr = m_pIWICFactory->CreateDecoderFromFilename(
        name,                      // Image to be decoded
        NULL,                            // Do not prefer a particular vendor
        GENERIC_READ,                    // Desired read access to the file
        WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
        &pDecoder                        // Pointer to the decoder
    );

    // Retrieve the first frame of the image from the decoder
    IWICBitmapFrameDecode* pFrame = NULL;

    if (SUCCEEDED(hr))
    {
        hr = pDecoder->GetFrame(0, &pFrame);

    }

    //Step 3: Format convert the frame to 32bppPBGRA
    if (SUCCEEDED(hr))
    {
        SafeRelease(m_pConvertedSourceBitmap);
        hr = m_pIWICFactory->CreateFormatConverter(&m_pConvertedSourceBitmap);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pConvertedSourceBitmap->Initialize(
            pFrame,                          // Input bitmap to convert
            GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
            WICBitmapDitherTypeNone,         // Specified dither pattern
            NULL,                            // Specify a particular palette 
            0.f,                             // Alpha threshold
            WICBitmapPaletteTypeCustom       // Palette translation type
        );
    }

    //Step 4: Create render target and D2D bitmap from IWICBitmapSource
    if (SUCCEEDED(hr))
    {

        hr = CreateDeviceResources(hWnd);
    }

    if (SUCCEEDED(hr))
    {
        // Need to release the previous D2DBitmap if there is one
        SafeRelease(m_pD2DBitmap);
        hr = m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
    }

    SafeRelease(pDecoder);
    SafeRelease(pFrame);
    // }

    return hr;
}

/******************************************************************
*  This method creates resources which are bound to a particular  *
*  D2D device. It's all centralized here, in case the resources   *
*  need to be recreated in the event of D2D device loss           *
* (e.g. display change, remoting, removal of video card, etc).    *
******************************************************************/
HRESULT DemoApp::CreateDeviceResources(HWND hWnd)
{
    HRESULT hr = S_OK;

    if (!m_pRT)
    {
        RECT rc;
        hr = GetClientRect(hWnd, &rc) ? S_OK : E_FAIL;

        if (SUCCEEDED(hr))
        {
            // Create a D2D render target properties
            D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();

            // Set the DPI to be the default system DPI to allow direct mapping
            // between image pixels and desktop pixels in different system DPI settings
            renderTargetProperties.dpiX = DEFAULT_DPI;
            renderTargetProperties.dpiY = DEFAULT_DPI;

            // Create a D2D render target
            D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

            hr = m_pD2DFactory->CreateHwndRenderTarget(
                renderTargetProperties,
                D2D1::HwndRenderTargetProperties(hWnd, size),
                &m_pRT
            );
        }
    }

    return hr;
}



/******************************************************************
*  Regsitered Window message handler                              *
******************************************************************/


LRESULT CALLBACK DemoApp::s_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DemoApp* pThis;
    LRESULT lRet = 0;

    if (uMsg == WM_NCCREATE)
    {
        LPCREATESTRUCT pcs = reinterpret_cast<LPCREATESTRUCT> (lParam);
        pThis = reinterpret_cast<DemoApp*> (pcs->lpCreateParams);

        SetWindowLongPtr(hWnd, GWLP_USERDATA, PtrToUlong(pThis));
        lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    else
    {
        pThis = reinterpret_cast<DemoApp*> (GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (pThis)  lRet = pThis->WndProc(hWnd, uMsg, wParam, lParam);
        else        lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return lRet;
}

LRESULT DemoApp::OnPaint(HWND hWnd, float x, float y)
{
    HRESULT hr = S_OK;
    PAINTSTRUCT ps;

    if (BeginPaint(hWnd, &ps))
    {
        // Create render target if not yet created
        hr = CreateDeviceResources(hWnd);

        if (SUCCEEDED(hr) && !(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
        {
            m_pRT->BeginDraw();

            m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());

            // Clear the background
            m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));

            unsigned int cx, cy;
            m_pConvertedSourceBitmap->GetSize(&cx, &cy);

            D2D1_RECT_F rectangle = D2D1::RectF(x, y, (float)cx + x, (float)cy + y);
            // D2DBitmap may have been released due to device loss. 
            // If so, re-create it from the source bitmap
            if (m_pConvertedSourceBitmap && !m_pD2DBitmap)
            {
                m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
            }

            // Draws an image and scales it to the current window size
            if (m_pD2DBitmap)
            {
                m_pRT->DrawBitmap(m_pD2DBitmap, rectangle);
            }

            hr = m_pRT->EndDraw();

            // In case of device loss, discard D2D render target and D2DBitmap
            // They will be re-create in the next rendering pass
            if (hr == D2DERR_RECREATE_TARGET)
            {
                SafeRelease(m_pD2DBitmap);
                SafeRelease(m_pRT);
                // Force a re-render
                hr = InvalidateRect(hWnd, NULL, TRUE) ? S_OK : E_FAIL;
            }
        }

        EndPaint(hWnd, &ps);
    }

    return SUCCEEDED(hr) ? 0 : 1;
}


void DemoApp::moveByKey(keyStat k, float deltaT) {
    LONG lx, ly;
    float  lcodeX = 0, lcodeY = 0;
    if (k["left"])lcodeX = -1;
    if (k["right"])lcodeX = 1;
    if (k["up"])lcodeY = -1;
    if (k["down"])lcodeY = 1;
    if (fabsf(lcodeY) + fabsf(lcodeX) > 1) {//lcodeX^2+lcodeY^2=1
        lcodeY /= (float)ROOT_2;
        lcodeX /= (float)ROOT_2;
    }
    lx = (LONG)(lcodeX * deltaT);
    ly = (LONG)(lcodeY * deltaT);

    WINDOWINFO pwii;
    pwii.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(m_hWnd, &pwii);
    SetWindowPos(m_hWnd, HWND_TOP, pwii.rcWindow.left + lx, pwii.rcWindow.top + ly, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);

}

bool DemoApp::isExist() {
    return m_isexist;
}

void DemoApp::fillTransColor(HWND hWnd, HPEN whpen) {
    WINDOWINFO pwii;
    pwii.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(hWnd, &pwii);
    HDC hdc = GetWindowDC(hWnd);
    int left = 0;
    int top = 0;
    int right = pwii.rcWindow.right - pwii.rcWindow.left;
    int bottom = pwii.rcWindow.bottom - pwii.rcWindow.top;

    //draw
    HPEN lhpen = (HPEN)SelectObject(hdc, whpen);
    Rectangle(hdc, left, top, 6, bottom);
    Rectangle(hdc, left, top, right, 29);
    Rectangle(hdc, left, bottom - 6, right, bottom);
    Rectangle(hdc, right - 6, top, right, bottom);
    SelectObject(hdc, lhpen);
    DeleteDC(hdc);
    return;
}