/************************************************************************/
// 创建时间: 081117
// 最后确定: 081117

#include "Utils.h"

//////////////////////////////////////////////////////////////////////////
// 创建窗口
HWND InitWindow(LPCTSTR lpszClassName, LPCTSTR lpWindowName, int nWidth, int nHeight,
				WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor)
{
	HWND hWnd = NULL;
	// 窗口类结构
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= lpfnWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= GetModuleHandle(NULL);
	wcex.hIcon			= hIcon;
	wcex.hCursor		= hCursor;
	wcex.hbrBackground	= CreateSolidBrush(0);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= lpszClassName;
	wcex.hIconSm		= hIconSm;

	// 注册窗口类
	RegisterClassEx(&wcex);
	// 创建主窗口
	hWnd = CreateWindowEx( 0,
		lpszClassName,
		lpWindowName,
		WS_OVERLAPPED |
		WS_CAPTION |
		WS_SYSMENU |
		WS_MINIMIZEBOX,
		0,
		0,
		nWidth,
		nHeight,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	if (!hWnd) return NULL;
	ResetWindow(hWnd, nWidth, nHeight);

	return hWnd;
}

//////////////////////////////////////////////////////////////////////////
// 重设窗口大小
void ResetWindow(HWND hWnd, int nWidth, int nHeight)
{
	MoveWindow(hWnd, 0, 0, nWidth, nHeight, true);
	RECT rc;
	GetClientRect(hWnd, &rc);
	int wl = nWidth - rc.right;
	int hl = nHeight - rc.bottom;
	MoveWindow(hWnd, 0, 0, nWidth + wl, nHeight + hl, true);

	//显示并更新窗口
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	UpdateWindow(hWnd);
}

int Random(int mmin, int mmax)
{
	return rand() % (mmax - mmin + 1) + mmin;
}

int FileLen(FILE *fp)
{
	fseek(fp, 0L, SEEK_END);
	int ret = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return ret;
}