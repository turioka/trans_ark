#include "arukanoido.h"

std::map<int, std::string> code2key{
    {37, "left"},
    {38, "up"},
    {39, "right"},
    {40, "down"},
    {27, "esc"},
    {90, "z"}
};
//for initialize m_key2stat
const keyStat initkey2stat{
    {"left", FALSE},
    {"up", FALSE },
    {"right", FALSE},
    {"down", FALSE},
    {"esc", FALSE},
    {"z", FALSE}
};

barApp::barApp() : 
    DemoApp(2), 
    m_key2stat(initkey2stat) {};

const keyStat barApp::getConstKeyStats(){
    return m_key2stat;
}
bool barApp::getKeyStat(std::string key) {
    return m_key2stat[key];
}
HRESULT barApp::setWindow(HINSTANCE hInstance, HWND* hwndout) {
    HRESULT hr;
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = s_WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"WICViewerD2D";
    wcex.hIconSm = NULL;
    hr = RegisterClassEx(&wcex) ? S_OK : E_FAIL;

    HWND hWnd = NULL;
    if (SUCCEEDED(hr)) {
        hWnd = CreateWindowEx(WS_EX_LAYERED, L"WICViewerD2D", NULL, WS_VISIBLE,
            -200, -200, 50, 60, nullptr, nullptr, hInstance, this);//hide window initially
    }
    hr = hWnd ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) *hwndout = hWnd;
    return hr;
}

LRESULT barApp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HPEN hpen, whpen;
    static HBRUSH hbr, hbrPrev;

    switch (uMsg)
    {

    case WM_CREATE:
        //load picture
        if (!SUCCEEDED(CreateD2DBitmapFromFile(hWnd, L"./bar.png"))) {
            //fail
            m_isexist = FALSE;
            DestroyWindow(hWnd);
        }

        SetLayeredWindowAttributes(hWnd, RGB(255, 255, 255), 0, LWA_COLORKEY);
        hpen = CreatePen(PS_SOLID, 5, RGB(0x00, 0x7f, 0xff));
        hbr = CreateSolidBrush(RGB(0xff, 0xbf, 0x00));
        whpen = CreatePen(PS_SOLID, 5, RGB(0xff, 0xff, 0xff));
        SetTimer(hWnd, 1, 2, NULL);//wait time to make window visible


        return 0;

    case WM_TIMER:
        //make window visible
        SetWindowPos(hWnd, HWND_TOP, G_blockx + (G_blockcx - BARCX) / 2 - 6, G_blocky + G_blockcy * 3 / 4, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
        KillTimer(hWnd, 1);
        return 0;

    case WM_SIZE:
        //never resize
        D2D1_SIZE_U size = D2D1::SizeU(LOWORD(lParam), HIWORD(lParam));

        if (m_pRT)
        {
            if (FAILED(m_pRT->Resize(size)))
            {
                SafeRelease(m_pRT);
                SafeRelease(m_pD2DBitmap);
            }
        }
        return 0;

    case WM_KEYDOWN:
        if(code2key.find(wParam) != code2key.end()) // chech if exist 
            m_key2stat[code2key[wParam]] = TRUE;
        return 0;

    case WM_KEYUP:
        if (code2key.find(wParam) != code2key.end()) // chech if exist 
            m_key2stat[code2key[wParam]] = FALSE;
        return 0;

    case WM_NCACTIVATE: //draw noncliant area (frame)
        fillTransColor(hWnd, whpen);
        InvalidateRect(hWnd, NULL, TRUE);//call PAINT
        return TRUE;

    case WM_NCPAINT: //draw noncliant area (back)
        fillTransColor(hWnd, whpen);
        InvalidateRect(hWnd, NULL, TRUE);//call PAINT
        return 0;

    case WM_PAINT:
        OnPaint(hWnd, 0, 0); //draw picture
        return 0;

    case WM_DESTROY:
        m_isexist = FALSE;
        DeleteObject(whpen);
        DeleteObject(hpen);
        DeleteObject(hbr);

        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}