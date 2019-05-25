// AlgorithmShow.cpp : ����Ӧ�ó������ڵ㡣
//

#include <windows.h>
#include <tchar.h>
#include "AlgorithmShow.h"
#include "AlgorithmManager.h"

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��

// �˴���ģ���а����ĺ�����ǰ������: 
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

 	// TODO:  �ڴ˷��ô��롣
	MSG msg;

	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	AlgorithmManager::getInstance()->startAlgorithm(E_EAT_SORT);

	// ����Ϣѭ��: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

//
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
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
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(_T("AlgorithmClass"), _T("algorithm"), WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   // ������Ļ��С��λ��
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
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
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
		// TODO:  �ڴ���������ͼ����...

		// ˫����,��ֹ������˸
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