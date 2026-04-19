#include "arukanoido.h"

blockApp::blockApp() : DemoApp(1), m_x(G_blockx), m_y(G_blocky)
{
    for (int i = 0; i < NBLOCKX; i++) {
        for (int j = 0; j < NBLOCKY; j++) {
            exist[i][j] = TRUE;
        }
   }
}
bool blockApp::checkBlock(int i, int j) {
    if (i < NBLOCKX && j < NBLOCKY)return exist[i][j];
    return FALSE;
    
}
void blockApp::eraseBlock(int i, int j) {
    //delete flag
    exist[i][j] = FALSE;
    //erase picture
    int buffer = 10;
    RECT box;
    box.left = i * BLOCKCX - buffer;
    box.right = (i + 1) * BLOCKCX + buffer;
    box.top = j * BLOCKCY - buffer;
    box.bottom = (j + 1) * BLOCKCY + buffer;
    InvalidateRect(m_hWnd, &box, TRUE);
}

HRESULT blockApp::setWindow(HINSTANCE hInstance, HWND* hwndout) {
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
    wcex.lpszClassName = L"WICViewerD2D2";
    wcex.hIconSm = NULL;
    hr = RegisterClassEx(&wcex) ? S_OK : E_FAIL;

    HWND hWnd = NULL;
    if (SUCCEEDED(hr)) {
        hWnd = CreateWindowEx(NULL, L"WICViewerD2D2", L"move window. (press escape key to finish)", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
            G_blockx, G_blocky, G_blockcx, G_blockcy, nullptr, nullptr, hInstance, this);
    }
    hr = hWnd ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) *hwndout = hWnd;
    return hr;
}

LRESULT blockApp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    static HPEN hpen, hpenPrev, whpen, whpenPrev;
    static HBRUSH hbr, hbrPrev;

    WINDOWINFO pwii;
    pwii.cbSize = sizeof(WINDOWINFO);

    switch (uMsg)
    {
    
    case WM_CREATE:
        hpen = CreatePen(PS_SOLID, 5, RGB(0xa0, 0xa0, 0xa0));
        hbr = CreateSolidBrush(RGB(0x00, 0x00, 0x00));
        whpen = CreatePen(PS_SOLID, 5, RGB(0xff, 0xff, 0xff));
        SetTimer(hWnd, 1, 1, NULL);//This is needed to iterate main loop
        return 0;

    case WM_SIZE:
        pwii.cbSize = sizeof(WINDOWINFO);
        GetWindowInfo(hWnd, &pwii);
        MoveWindow(hWnd, pwii.rcWindow.left, pwii.rcWindow.top, G_blockcx, G_blockcy, FALSE);
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        hpenPrev = (HPEN)SelectObject(hdc, hpen);
        hbrPrev = (HBRUSH)SelectObject(hdc, hbr);
        for (int i = 0; i < NBLOCKX; i++) {
            for (int j = 0; j < NBLOCKY; j++) {
                if (exist[i][j])Rectangle(hdc, i * BLOCKCX, j * BLOCKCY, (i + 1) * BLOCKCX, (j + 1) * BLOCKCY);
            }
        }

        //restore
        SelectObject(hdc, hpenPrev);
        SelectObject(hdc, hbrPrev);
        EndPaint(hWnd, &ps);
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