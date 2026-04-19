#include "arukanoido.h"

ballApp::ballApp() : DemoApp(3), m_rad(0.4f), m_start() {};

void ballApp::move(barApp* aBar, blockApp* aBlock, float deltaT) {
    keyStat k = aBar->getConstKeyStats();
    if (k["z"])m_start = TRUE;
    if (m_start) {
        collisionANDmove(aBar, aBlock, deltaT);
    }
    else {
        moveByKey(k, deltaT);
    }
}
inline float reflectRad(float inrad, float axrad) {
    axrad *= inrad * axrad < 0 ? -1 : 1; //make sign of axrad same as that of inrad to return in (-1,1)
    return 2 * axrad - inrad;
}
void ballApp::collisionANDmove(barApp* aBar, blockApp* aBlock, float deltaT) {
    WINDOWINFO pwiiBar, pwiiBlock, pwiiBall;
    pwiiBar.cbSize = sizeof(WINDOWINFO);
    pwiiBlock.cbSize = sizeof(WINDOWINFO);
    pwiiBall.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(aBar->m_hWnd, &pwiiBar);
    GetWindowInfo(aBlock->m_hWnd, &pwiiBlock);
    GetWindowInfo(m_hWnd, &pwiiBall);

    int xrad = (int)floor(deltaT * cos(m_rad * 3.14));
    int yrad = -(int)floor(deltaT * sin(m_rad * 3.14));

    //collision
    int blockx = pwiiBlock.rcWindow.left;
    int blocky = pwiiBlock.rcWindow.top;
    int ballx = pwiiBall.rcWindow.left;
    int bally = pwiiBall.rcWindow.top;
    int nowx = ballx + xrad;
    int nowy = bally + yrad;

    
    bool leftbottom = (nowx + BALLCX > blockx && nowy < BLOCKCY* NBLOCKY + blocky);
    bool righttop = (nowx < BLOCKCX* NBLOCKX + blockx && nowy + BALLCY >blocky);
    bool ballOnBlockArea = leftbottom && righttop;
    //ball vs. block
     if (ballOnBlockArea) {
            int nowi, nowj;
            bool rightAttack = (xrad>0);
            if (rightAttack) {
                nowi = (int)((nowx + BALLCX - blockx) / BLOCKCX);
            }
            else {
                nowi = (int)((nowx - blockx) / BLOCKCX);
            }
            
            bool bottomAttack = (yrad>0);
            if (bottomAttack) {
                nowj = (int)((nowy + BALLCY - blocky) / BLOCKCY);
            }
            else {
                nowj = (int)((nowy - blocky) / BLOCKCY);
            }

            if (aBlock->checkBlock(nowi, nowj)) {
                aBlock->eraseBlock(nowi, nowj);
                bool ballOnTheBlock = BLOCKCY * nowj + blocky < nowy + BALLCY && nowy < BLOCKCY* (nowj + 1) + blocky && BLOCKCX * nowi + blockx < nowx + BALLCX && nowx < BLOCKCX* (nowi + 1) + blockx;
                bool yattack = !(BLOCKCY * nowj + blocky < bally + BALLCY && bally < BLOCKCY* (nowj + 1) + blocky);
                if (ballOnTheBlock) {
                    if (yattack) {
                        m_rad = reflectRad(m_rad, 0);
                    }
                    else {
                        m_rad = reflectRad(m_rad, 0.5);
                    }
                }
            }
        }

    //ball vs. bar
    if (pwiiBar.rcWindow.top + BARCY > nowy && nowy + BALLCY > pwiiBar.rcWindow.top) {
        if (pwiiBar.rcWindow.left < nowx + BALLCX && nowx < pwiiBar.rcWindow.left + BARCX) {
            //nowx in [left-BALLCX,left+BARCX]
            //nxtrad in [0.15,0.85]
            float nxtrad = -0.7f * (ballx - (pwiiBar.rcWindow.left - BALLCX)) / (BARCX - (-BALLCX)) + 0.85f;
            m_rad = m_rad > 0 ? -nxtrad : nxtrad;
        }
    }
    //window
    if (nowx < 0) {
        m_rad = reflectRad(m_rad, 0.5);
        pwiiBall.rcWindow.left = 0;
    }
    else if (nowy < -BALLCY) {
        m_rad = reflectRad(m_rad, 0);
        pwiiBall.rcWindow.top = -BALLCY;
    }
    else if (nowx + BALLCX > GetSystemMetrics(SM_CXFULLSCREEN)) {
        m_rad = reflectRad(m_rad, 0.5);
        pwiiBall.rcWindow.left = GetSystemMetrics(SM_CXFULLSCREEN) - BALLCX;
    }
    else if (nowy + BALLCY > GetSystemMetrics(SM_CYFULLSCREEN)) {
        m_rad = reflectRad(m_rad, 0);
        pwiiBall.rcWindow.top = GetSystemMetrics(SM_CYFULLSCREEN) - BALLCY;
    }
    xrad = (int)floor(deltaT * cos(m_rad * 3.14));
    yrad = -(int)floor(deltaT * sin(m_rad * 3.14));

    SetWindowPos(m_hWnd, HWND_TOP, pwiiBall.rcWindow.left + xrad, pwiiBall.rcWindow.top + yrad, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);

}


HRESULT ballApp::setWindow(HINSTANCE hInstance, HWND* hwndout) {
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
    wcex.lpszClassName = L"WICViewerD2D3";
    wcex.hIconSm = NULL;
    hr = RegisterClassEx(&wcex) ? S_OK : E_FAIL;

    HWND hWnd = NULL;
    if (SUCCEEDED(hr)) {
        hWnd = CreateWindowEx(WS_EX_LAYERED, L"WICViewerD2D3", NULL, WS_VISIBLE,
            -200, -200, 40, 70, nullptr, nullptr, hInstance, this);
    }
    hr = hWnd ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) *hwndout = hWnd;
    return hr;
}

LRESULT ballApp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HPEN hpen, whpen;
    static HBRUSH hbr, hbrPrev;
    switch (uMsg)
    {
    
    case WM_CREATE:
        //load picture
        if (!SUCCEEDED(CreateD2DBitmapFromFile(hWnd, L"./ball.png"))) {
            //fail
            m_isexist = FALSE;
            DestroyWindow(hWnd);
        }

        SetLayeredWindowAttributes(hWnd, RGB(255, 255, 255), 0, LWA_COLORKEY);
        hpen = CreatePen(PS_SOLID, 5, RGB(0x00, 0x7f, 0xff));
        hbr = CreateSolidBrush(RGB(0xff, 0xbf, 0x00));
        whpen = CreatePen(PS_SOLID, 5, RGB(0xff, 0xff, 0xff));
        SetTimer(hWnd, 1, 2, NULL);
        return 0;

    case WM_TIMER:
        //make window visible
        SetWindowPos(hWnd, HWND_TOP, G_blockx + (G_blockcx - BALLCX) / 2 - 6, G_blocky + G_blockcy * 3 / 4 - BALLCY, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
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
    {
        m_isexist = FALSE;
        DeleteObject(whpen);
        DeleteObject(hpen);
        DeleteObject(hbr);

        PostQuitMessage(0);
        return 0;
    }
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}


