/************************************************************************/
// ����ʱ��: 081028
// ���ȷ��: 081119

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
	// ��ͼ����
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
		HWND					m_hWnd;						// ��ͼ���ھ��
		bool					m_StencilBuffer;			// �Ƿ�ʹ��ģ�滺��
		bool					m_AntiAlias;				// �Ƿ�ʹ��ȫ��������
		D3DDEVTYPE				m_DevType;					// ʹ�����/Ӳ���豸
		D3DCAPS9				m_Caps;						// �豸��Ϣ

		IDirect3D9*				m_pD3D;						// D3D����
		IDirect3DDevice9*		pd3dDevice;					// D3D�豸
		ID3DXSprite*			m_pSprite;					// �������
		D3DPRESENT_PARAMETERS	d3dpp;						// �豸��������

		bool					m_bDeviceLost;				// �Ƿ�ʧ���豸
	};
}

#endif