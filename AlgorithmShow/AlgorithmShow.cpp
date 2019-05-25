// AlgorithmShow.cpp : 定义应用程序的入口点。
//

#include <windows.h>
#include <tchar.h>
#include "AlgorithmShow.h"
#include "AlgorithmManager.h"

// 全局变量: 
HINSTANCE hInst;								// 当前实例

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

#include <algorithm>
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  在此放置代码。
	MSG msg;

	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	AlgorithmManager::getInstance()->startAlgorithm(E_EAT_SORT);

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= _T("");
	wcex.lpszClassName = _T("AlgorithmClass");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(_T("AlgorithmClass"), _T("algorithm"), WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   // 设置屏幕大小和位置
   RECT clientrc;
   GetClientRect(hWnd, &clientrc);
   RECT windowrc;
   GetWindowRect(hWnd, &windowrc);

   int borderwidth = (windowrc.right - windowrc.left) - (clientrc.right - clientrc.left);
   int borderheight = (windowrc.bottom - windowrc.top) - (clientrc.bottom - clientrc.top);

   int cx = ::GetSystemMetrics(SM_CXSCREEN);
   int cy = ::GetSystemMetrics(SM_CYSCREEN);
   int width = 500 * 3 + 10;
   int height = 200 * 4 + 10 + 20;
   MoveWindow(hWnd, (cx - width) / 2, (cy - height) / 2 - 50, width + borderwidth, height + borderheight, TRUE);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		SetTimer(hWnd, 1, 20, nullptr);
		break;
	case WM_TIMER:
		InvalidateRect(hWnd, nullptr, true);
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...

		// 双缓冲,防止画面闪烁
		RECT rc = { 0 };
		GetClientRect(hWnd, &rc);
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hBmpMem = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
		SelectObject(hdcMem, hBmpMem);
		FillRect(hdcMem, &rc, (HBRUSH)(COLOR_WINDOW + 1));

		AlgorithmManager::getInstance()->drawV(hdcMem);

		BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
		DeleteObject(hBmpMem);
		DeleteDC(hdcMem);

		EndPaint(hWnd, &ps);
	} break;
	case WM_KEYDOWN:
		AlgorithmManager::getInstance()->keyDown(wParam);
		break;
	case WM_DESTROY:
        AlgorithmManager::getInstance()->stopAlgorithm();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}