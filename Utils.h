/************************************************************************/
// 创建时间: 081117
// 最后确定: 081117

#ifndef _Utils_h_
#define _Utils_h_

#include "AnyPlay.h"

HWND	InitWindow(LPCTSTR lpszClassName, LPCTSTR lpWindowName, int nWidth, int nHeight,
					WNDPROC lpfnWndProc, HICON hIcon, HICON hIconSm, HCURSOR hCursor);
void	ResetWindow(HWND hWnd, int nWidth, int nHeight);
int		Random(int mmin, int mmax);
int		FileLen(FILE *fp);

#endif