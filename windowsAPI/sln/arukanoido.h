// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once
#include <windows.h>
#include <map>
#include <iostream>
#include <wincodec.h>
#include <commdlg.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <math.h>
#include <time.h>
#include <chrono>
#include "Resource.h"

using keyStat = std::map<std::string, bool>;
using chHRC = std::chrono::high_resolution_clock;
using chTP = std::chrono::steady_clock::time_point;


#define BARCX 40
#define BARCY 10
#define BALLCX 16
#define BALLCY 16
#define BLOCKCX 50
#define BLOCKCY  25
#define NBLOCKX 10
#define NBLOCKY 10
#define MOVE_UNIT 500
#define ROOT_2 1.41421356f
#define FPS 60.f

const FLOAT DEFAULT_DPI = 96.f;   // Default DPI that maps image resolution directly to screen resoltuion
const int G_blockx = GetSystemMetrics(SM_CXFULLSCREEN) / 2 - 300;
const int G_blocky = 100;

const int G_blockcx = 600;
const int G_blockcy = 600;

template <typename T>
inline void SafeRelease(T*& p);


void focusOrder(HWND window, HWND window2, HWND window3);
void deleteWin(HWND window, HWND window2, HWND window3);
/******************************************************************
*                                                                 *
*  DemoApp                                                        *
*                                                                 *
******************************************************************/

class DemoApp
{
public:
    DemoApp(int);
    ~DemoApp();

    HRESULT Initialize(HINSTANCE hInstance);
    void moveByKey(keyStat k, float deltaT);
    void fillTransColor(HWND hWnd, HPEN whpen);
    bool isExist();
    bool m_activate;
    int test;
    HWND m_hWnd;

protected:
    virtual HRESULT setWindow(HINSTANCE, HWND*) { return NULL; };
    HRESULT CreateD2DBitmapFromFile(HWND hWnd,WCHAR* name);
    
    HRESULT CreateDeviceResources(HWND hWnd);

    virtual LRESULT WndProc(HWND, UINT, WPARAM, LPARAM) { return NULL; };
    LRESULT OnPaint(HWND hWnd, float x, float y);

    static LRESULT CALLBACK s_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool                    m_isexist;
    HINSTANCE               m_hInst;
    IWICImagingFactory     *m_pIWICFactory;
    
    ID2D1Factory           *m_pD2DFactory;
    ID2D1HwndRenderTarget  *m_pRT;
    ID2D1Bitmap            *m_pD2DBitmap;
    IWICFormatConverter    *m_pConvertedSourceBitmap;
};
//1:bar
//2:block
//3:ball

class barApp : public DemoApp 
{
public:
    barApp();
    const keyStat getConstKeyStats();
    bool getKeyStat(std::string key);
protected:
    keyStat m_key2stat;
    HRESULT setWindow(HINSTANCE hInstance, HWND* hwndout) override;
    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

class blockApp : public DemoApp 
{
public:
    blockApp();
    bool checkBlock(int i, int j);
    void eraseBlock(int i, int j);
protected:
    int m_x, m_y;
    bool exist[NBLOCKX][NBLOCKY];
    HRESULT setWindow(HINSTANCE hInstance, HWND* hwndout) override;
    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

class ballApp : public DemoApp
{
public:
    ballApp();
    void move(barApp* aBar, blockApp* aBlock, float deltaT);
protected:

    void collisionANDmove(barApp* aBar, blockApp* aBlock, float deltaT);
    float m_rad;
    bool m_start;
    HRESULT setWindow(HINSTANCE hInstance, HWND* hwndout) override;
    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};








