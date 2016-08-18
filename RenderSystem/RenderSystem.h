/************************************************************************/
// 创建时间: 081028
// 最后确定: 081119

#ifndef _RenderSystem_h_
#define _RenderSystem_h_

#include "../AnyPlay.h"
#include "../FrameWork/Singleton.h"
#include "../FrameWork/Logger.h"
#include "../FrameWork/Manager.h"
#include "../Core/String.h"
#include "../Core/Color.h"
#include "../Core/Quad.h"
#include "Texture.h"
#include "Font.h"

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

namespace AnyPlay
{
	// 绘图参数
	struct APRS_SCREEN_VERTEX
	{
		float x, y, z, rhw;
		D3DCOLOR color;
		float tu, tv;
	};
#define APRS_SCREEN_VERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

	//////////////////////////////////////////////////////////////////////////
	class RenderSystem : public Singleton<RenderSystem>, public EventSet
	{
	public:
		RenderSystem();
		~RenderSystem();

		bool	CreateRenderSystem(UINT BackBufferWidth, UINT BackBufferHeight, HWND hWnd, bool TrueColor, bool Windowed, bool VSync, bool AntiAlias);
		bool	FreeRenderSystem();
		bool	Create3DDevice();
		void	Free3DDevice();

		void	NormalBlend();
		void	ResetRenderState();

		void	BeginSmooth();
		void	EndSmooth();
		bool	BeginScene(bool clrBackBuffer, bool clrZBuffer, const Color& color);
		bool	EndScene(const Rect* pSrcRC);
		bool	Reset();

		IDirect3DDevice9*	GetD3D9Device() { return pd3dDevice; }
		ID3DXSprite*		GetSprite() { return m_pSprite; }

	public:
		Manager<Texture>		TextureMgr;
		Manager<Font>			FontMgr;

	private:
		HWND					m_hWnd;						// 绘图窗口句柄
		bool					m_StencilBuffer;			// 是否使用模版缓冲
		bool					m_AntiAlias;				// 是否使用全屏反走样
		D3DDEVTYPE				m_DevType;					// 使用软件/硬件设备
		D3DCAPS9				m_Caps;						// 设备信息

		IDirect3D9*				m_pD3D;						// D3D对象
		IDirect3DDevice9*		pd3dDevice;					// D3D设备
		ID3DXSprite*			m_pSprite;					// 精灵对象
		D3DPRESENT_PARAMETERS	d3dpp;						// 设备创建参数

		bool					m_bDeviceLost;				// 是否丢失了设备
	};
}

#endif