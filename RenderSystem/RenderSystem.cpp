/************************************************************************/
// 创建时间: 081028
// 最后确定: 081119

#include "RenderSystem.h"
#include "../FrameWork/Application.h"
#include "../Utils.h"

namespace AnyPlay
{
	template<> RenderSystem* Singleton<RenderSystem>::m_Singleton = 0;

	RenderSystem::RenderSystem()
	{
		m_hWnd = 0;											// 绘图窗口句柄
		m_StencilBuffer = false;							// 是否使用模版缓冲
		m_AntiAlias = false;								// 是否使用全屏反走样
		m_DevType = D3DDEVTYPE_HAL;							// 使用软件/硬件设备
		memset(&m_Caps, 0, sizeof(D3DCAPS9));				// 设备信息

		m_pD3D = NULL;										// D3D对象
		pd3dDevice = NULL;									// D3D设备
		m_pSprite = NULL;									// 精灵对象
		memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));	// 设备创建参数

		m_bDeviceLost = false;								// 是否丢失了设备
	}

	RenderSystem::~RenderSystem()
	{
		FreeRenderSystem();
	}

	bool RenderSystem::CreateRenderSystem(UINT BackBufferWidth, UINT BackBufferHeight, HWND hWnd, bool TrueColor, bool Windowed, bool VSync, bool AntiAlias)
	{
		m_hWnd = hWnd;
		m_AntiAlias = AntiAlias;

		// 先要释放设备
		FreeRenderSystem();

		// 如果没有设备指针才可以创建设备
		if(!m_pD3D)
		{
			// 创建设备
			m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
			if(!m_pD3D)
			{
				LOG(_T("无法创建Direct3D9对象"), Errors);
				return false;
			}

			// 显示设备版本
			D3DADAPTER_IDENTIFIER9 dai;
			if(!FAILED(m_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &dai)))
			{
				int Product = HIWORD(dai.DriverVersion.HighPart);
				int Version = LOWORD(dai.DriverVersion.HighPart);
				int SubVersion = HIWORD(dai.DriverVersion.LowPart);
				int Build = LOWORD(dai.DriverVersion.LowPart);
				//TCHAR str[512] = {0};
				//_stprintf(str, _T("描述 %s, 驱动 %s, 产品 %d.%d.%d.%d"), (dai.Description), (dai.Driver), Product, Version, SubVersion, Build);
				//LOG(str);
			}
			else LOG(_T("无法获得设备版本"), Warnings);

			// 获得设备显示模式
			D3DDISPLAYMODE d3ddm;
			HRESULT hr = m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
			if(FAILED(hr))
			{
				LOG(_T("无法获得设备显示模式"), Errors);
				return false;
			}

			// 填充创建设备参数
			ZeroMemory(&d3dpp, sizeof(d3dpp));
			d3dpp.BackBufferWidth					= BackBufferWidth;
			d3dpp.BackBufferHeight					= BackBufferHeight;
			d3dpp.BackBufferCount					= 1;
			d3dpp.EnableAutoDepthStencil			= TRUE;

			// 是否开启垂直同步
			if(VSync) d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_ONE;
			else d3dpp.PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;

			// 是否窗口模式
			if(Windowed)
			{
				d3dpp.BackBufferFormat				= d3ddm.Format;
				d3dpp.SwapEffect					= D3DSWAPEFFECT_COPY;
				d3dpp.Windowed						= TRUE;
			}
			else
			{
				// 全屏下的后台颜色格式
				if(TrueColor)
					d3dpp.BackBufferFormat			= D3DFMT_X8R8G8B8;
				else
					d3dpp.BackBufferFormat			= D3DFMT_R5G6B5;

				d3dpp.SwapEffect					= D3DSWAPEFFECT_FLIP;
				d3dpp.Windowed						= FALSE;
				d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
			}

			// 是否开启全屏反走样
			if(AntiAlias)
			{
				dword qualityLevels = 0;

				if( SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
					m_DevType, d3dpp.BackBufferFormat, Windowed ? TRUE : FALSE,
					D3DMULTISAMPLE_4_SAMPLES, &qualityLevels)))
				{
					// 打开反走样
					d3dpp.MultiSampleType			= D3DMULTISAMPLE_4_SAMPLES;
					d3dpp.MultiSampleQuality		= qualityLevels - 1;
					d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;
				}
				else
				{
					if( SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
						m_DevType, d3dpp.BackBufferFormat, Windowed ? TRUE : FALSE,
						D3DMULTISAMPLE_2_SAMPLES, &qualityLevels)))
					{
						d3dpp.MultiSampleType		= D3DMULTISAMPLE_2_SAMPLES;
						d3dpp.MultiSampleQuality	= qualityLevels - 1;
						d3dpp.SwapEffect			= D3DSWAPEFFECT_DISCARD;
					}
					else
					{
						if( SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
							m_DevType, d3dpp.BackBufferFormat, Windowed ? TRUE : FALSE,
							D3DMULTISAMPLE_NONMASKABLE, &qualityLevels)))
						{
							d3dpp.SwapEffect         = D3DSWAPEFFECT_DISCARD;
							d3dpp.MultiSampleType    = D3DMULTISAMPLE_NONMASKABLE;
							d3dpp.MultiSampleQuality = qualityLevels - 1;
						}
						else
						{
							LOG(_T("由于硬件不支持,无法打开全屏反走样"), Warnings);
							AntiAlias = false;
						}
					}
				}
			}

			// 是否需要模版缓存
			if(m_StencilBuffer)
			{
				d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
				if( FAILED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, m_DevType,
					d3dpp.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
					D3DRTYPE_SURFACE, d3dpp.AutoDepthStencilFormat)))
				{
					d3dpp.AutoDepthStencilFormat = D3DFMT_D24X4S4;
					if( FAILED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, m_DevType,
						d3dpp.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
						D3DRTYPE_SURFACE, d3dpp.AutoDepthStencilFormat)))
					{
						d3dpp.AutoDepthStencilFormat = D3DFMT_D15S1;
						if( FAILED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, m_DevType,
							d3dpp.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
							D3DRTYPE_SURFACE, d3dpp.AutoDepthStencilFormat)))
						{
							LOG(_T("由于硬件不支持,无法打开模版缓存"), Warnings);
							m_StencilBuffer = false;
						}
					}
				}
				else
				{
					if( FAILED(m_pD3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, m_DevType,
						d3dpp.BackBufferFormat, d3dpp.BackBufferFormat, d3dpp.AutoDepthStencilFormat)))
					{
						LOG(_T("深度模版缓存格式是不支持的格式"), Warnings);
						m_StencilBuffer = false;
					}
				}
			}
			if(!m_StencilBuffer)
			{
				d3dpp.AutoDepthStencilFormat = D3DFMT_D32;
				if( FAILED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, m_DevType,
					d3dpp.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
					D3DRTYPE_SURFACE, d3dpp.AutoDepthStencilFormat)))
				{
					d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
					if( FAILED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, m_DevType,
						d3dpp.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
						D3DRTYPE_SURFACE, d3dpp.AutoDepthStencilFormat)))
					{
						d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
						if( FAILED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, m_DevType,
							d3dpp.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
							D3DRTYPE_SURFACE, d3dpp.AutoDepthStencilFormat)))
						{
							LOG(_T("设备不支持必须的深度缓存格式"), Errors);
							return false;
						}
					}
				}
			}
		}
		return Create3DDevice();
	}

	bool RenderSystem::FreeRenderSystem()
	{
		Free3DDevice();
		SAFE_RELEASE(m_pD3D);
		return !(m_pD3D || pd3dDevice);
	}

	bool RenderSystem::Create3DDevice()
	{
		if(!m_pD3D) return false;
		Free3DDevice();

		// 创建设备
		HRESULT hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, m_DevType, m_hWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);

		if( FAILED(hr))
			hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, m_DevType, m_hWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING , &d3dpp, &pd3dDevice);

		if( FAILED(hr))
			hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, m_DevType, m_hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);

		if( FAILED(hr))
		{
			LOG(_T("创建D3D设备失败"), Errors);
			return false;
		}

		pd3dDevice->GetDeviceCaps(&m_Caps);
		if( m_StencilBuffer &&
			(!(m_Caps.StencilCaps & D3DSTENCILCAPS_DECRSAT) ||
			!(m_Caps.StencilCaps & D3DSTENCILCAPS_INCRSAT) ||
			!(m_Caps.StencilCaps & D3DSTENCILCAPS_KEEP)))
		{
			LOG(_T("设备没有启用模版缓存.禁用模版缓存"), Warnings);
			m_StencilBuffer = false;
		}

		// 打开全屏反走样开关
		if(m_AntiAlias) pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

		// 创建精灵对象
		if(FAILED((D3DXCreateSprite(pd3dDevice, &m_pSprite)))) return false;
		ResetRenderState();

		return true;

	}

	void RenderSystem::Free3DDevice()
	{
		TextureMgr.RemoveAll();
		FontMgr.RemoveAll();
		SAFE_RELEASE(m_pSprite);
		SAFE_RELEASE(pd3dDevice);
	}

	void RenderSystem::NormalBlend()
	{
		if(!pd3dDevice) return;
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	}

	void RenderSystem::ResetRenderState()
	{
		if(!pd3dDevice) return;
		NormalBlend();
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED );
		pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );//
		pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );		//
		pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		// 重要
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	}

	void RenderSystem::BeginSmooth()
	{
		if(!pd3dDevice) return;
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	}

	void RenderSystem::EndSmooth()
	{
		if(!pd3dDevice) return;
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	}

	bool RenderSystem::BeginScene(bool clrBackBuffer, bool clrZBuffer, const Color& color)
	{
		if(!pd3dDevice) return false;

		if(m_bDeviceLost)
		{
			HRESULT hr;
			if(FAILED(hr = pd3dDevice->TestCooperativeLevel()))
			{
				if(hr == D3DERR_DEVICELOST)
					return false;
				else if(hr == D3DERR_DEVICENOTRESET)
					Reset();
				else if(hr == D3DERR_DRIVERINTERNALERROR)
				{
					LOG(_T("D3D发生严重错误"), Insane);
					QUIT_PROGRAM;
					return false;
				}
			}
		}

		dword flags = 0;
		if(clrBackBuffer) flags |= D3DCLEAR_TARGET;
		if(clrZBuffer) flags |= D3DCLEAR_ZBUFFER;
		if(m_StencilBuffer) flags |= D3DCLEAR_STENCIL;

		if(FAILED(pd3dDevice->Clear(0, NULL, flags, color.GetColor(), 1.0f, 0)))
			LOG(_T("清屏失败"), Warnings);

		if(FAILED(pd3dDevice->BeginScene()))
		{
			LOG(_T("无法开始绘制场景"), Warnings);
			return true;
		}

		return true;
	}

	bool RenderSystem::EndScene(const Rect* pSrcRC)
	{
		if(!pd3dDevice) return false;
		if(m_bDeviceLost) return false;

		if(FAILED(pd3dDevice->EndScene()))
		{
			LOG(_T("无法停止绘制场景"), Warnings);
			return false;
		}

		HRESULT hr;

		RECT rcDst = {0, 0, d3dpp.BackBufferWidth, d3dpp.BackBufferHeight};

		hr = pd3dDevice->Present(pSrcRC ? &pSrcRC->GetRECT() : NULL, &rcDst, NULL, NULL);

		if(hr == D3DERR_DEVICELOST)
		{
			m_bDeviceLost = true;
			LOG(_T("设备已丢失"), Information);
		}
		else
		{
			if(FAILED(hr) && hr != D3DERR_INVALIDCALL)
			{
				LOG(_T("无法交换前后缓冲"), Warnings);
				return false;
			}
		}

		return true;
	}


	bool RenderSystem::Reset()
	{
		if(pd3dDevice)
		{
			Manager<Texture>::ManagedMap::iterator i;
			Manager<Font>::ManagedMap::iterator j;

			// 向所有纹理发出丢失消息
			for(i = TextureMgr.m_ManagedMap.begin(); i != TextureMgr.m_ManagedMap.end(); i++)
				(i->second)->OnLost();
			//LostAllFonts
			for(j = FontMgr.m_ManagedMap.begin(); j != FontMgr.m_ManagedMap.end(); j++)
				(j->second)->OnLost();

			if(m_pSprite) m_pSprite->OnLostDevice();

			HRESULT hr;
			if(FAILED(hr = pd3dDevice->Reset(&d3dpp)))
			{
				if(hr == D3DERR_DEVICELOST)
				{
					m_bDeviceLost = true;
					LOG(_T("无法重置设备"), Warnings);
				}
				else LOG(_T("由于设备没有完全被释放,导致无法重置设备"), Warnings);
				return false;
			}

			ResetRenderState();
			if(m_pSprite) m_pSprite->OnResetDevice();

			for(j = FontMgr.m_ManagedMap.begin(); j != FontMgr.m_ManagedMap.end(); j++)
				(j->second)->OnReset();
			for(i = TextureMgr.m_ManagedMap.begin(); i != TextureMgr.m_ManagedMap.end(); i++)
				(i->second)->OnReset();

			m_bDeviceLost = false;

			ResetWindow(m_hWnd, Application::GetSingleton().GetWindowWidth(), Application::GetSingleton().GetWindowHeight());

			return true;
		}
		return false;
	}
}