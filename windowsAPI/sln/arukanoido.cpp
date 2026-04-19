// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.

#ifndef WINVER                  // Allow use of features specific to Windows XP or later.
#define WINVER 0x0501           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410   // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE               // Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600        // Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers


#include "arukanoido.h"






char forDebug[100];


void focusOrder(HWND hBlock, HWND hBall, HWND hBar) {

    if (hBar != GetActiveWindow()) {
        SetWindowPos(hBlock, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        SetWindowPos(hBall, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        SetWindowPos(hBar, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    }

}

void deleteWin(HWND hBlock, HWND hBall, HWND hBar) {
    SetWindowPos(hBall, HWND_TOP, -100, 0, 0, 0, SWP_NOSIZE);//hide
    SetWindowPos(hBar, HWND_TOP, -100, 0, 0, 0, SWP_NOSIZE);//hide
    DestroyWindow(hBlock);
    DestroyWindow(hBall);
    DestroyWindow(hBar);
}

/******************************************************************
*  Application entrypoint                                         *
******************************************************************/

int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR pszCmdLine,
    int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pszCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (SUCCEEDED(hr))
    {
        {
            
            barApp aBar;
            ballApp aBall;
            blockApp aBlock;

            hr = aBar.Initialize(hInstance);
            hr = aBall.Initialize(hInstance);
            hr = aBlock.Initialize(hInstance);
            
            chTP last = chHRC::now();
            if (SUCCEEDED(hr))
            {
                

                //make order
                
                SetWindowPos(aBlock.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
                SetWindowPos(aBall.m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
                SetWindowPos(aBar.m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
                
                BOOL fRet;
                MSG msg;
                bool ready = FALSE;
                // Main message loop:
                while ((fRet = GetMessage(&msg, NULL, 0, 0)) != 0)
                {
                    if (fRet == -1) break;

                    focusOrder(aBlock.m_hWnd, aBall.m_hWnd, aBar.m_hWnd);
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);

                    if (ready) {

                        //wait one frame
                        chTP now = chHRC::now();
                        std::chrono::duration<double> dur = now - last;
                        float deltaT = (float)dur.count();
                        if (deltaT < 1.f / FPS)continue;//restart
                        last = now;
                        
                        //move
                        aBall.move(&aBar, &aBlock, MOVE_UNIT * deltaT);
                        aBar.moveByKey(aBar.getConstKeyStats(), MOVE_UNIT * deltaT);

                        //delete
                        bool lackOfWindow = !aBar.isExist() || !aBall.isExist() || !aBlock.isExist();
                        if (aBar.getKeyStat("esc") || lackOfWindow) 
                            deleteWin(aBlock.m_hWnd, aBall.m_hWnd, aBar.m_hWnd);
                        
                    }
                    else {
                        //if bar and ball is visible, ready = true
                        WINDOWINFO pwii;
                        pwii.cbSize = sizeof(WINDOWINFO);
                        GetWindowInfo(aBar.m_hWnd, &pwii);
                        bool temp = pwii.rcWindow.left > -100;
                        GetWindowInfo(aBall.m_hWnd, &pwii);
                        ready = temp && pwii.rcWindow.left > -100;
                    }
                }
            }
        }
        CoUninitialize();
    }

    return 0;
}