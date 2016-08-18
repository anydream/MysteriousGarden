/************************************************************************/
// 创建时间: 080906
// 最后确定: 081125

#ifndef _AnyPlay_h_
#define _AnyPlay_h_

#pragma warning (disable : 4786)
#pragma warning (disable : 4267)

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <list>
#include <map>

// 求大小
#define APMax(a,b)		(((a) > (b)) ? (a) : (b))
#define APMin(a,b)		(((a) < (b)) ? (a) : (b))
// 安全释放指针, COM接口
#define SAFE_DELETE(p)	{ if(p) { delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p) = NULL; } }
#define SAFE_RELEASE(p)	{ if(p) { (p)->Release(); (p) = NULL; } }
// 修正像素坐标
#define PIXEL_ALIGNED(x)((float)((x) + ((x) >= 0.0f ? 0.5f : -0.5f)))
// 退出程序
#define QUIT_PROGRAM	{ PostMessage(GetActiveWindow(), WM_DESTROY, 0, 0); }

// 重新定义的数据类型
typedef unsigned char	byte;
typedef unsigned long	dword;
typedef unsigned short	ushort;
typedef unsigned short	wchar;

#define NULL 0
// 迭代器是否在范围内
//#define IN_RANGE(_iter, _lst) ((_iter) >= (_lst).begin() && (_iter) < (_lst).end())
#define IN_RANGE(_iter, _lst) ((_iter) != (_lst).end())
// 向Logger发出记录事件消息
#define LOG(_s, _lv) { Logger::GetSingleton().LogEvent(_s, _lv); }

#endif
