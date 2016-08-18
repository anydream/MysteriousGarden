/************************************************************************/
// ����ʱ��: 081028
// ���ȷ��: 081119

#include "RenderSystem.h"
#include "../FrameWork/Application.h"
#include "../Utils.h"

namespace AnyPlay
{
	template<> RenderSystem* Singleton<RenderSystem>::m_Singleton = 0;

	RenderSystem::RenderSystem()
	{
		m_hWnd = 0;											// ��ͼ���ھ��
		m_StencilBuffer = false;							// �Ƿ�ʹ��ģ�滺��
		m_AntiAlias = false;								// �Ƿ�ʹ��ȫ��������
		m_DevType = D3DDEVTYPE_HAL;							// ʹ�����/Ӳ���豸
		memset(&m_Caps, 0, sizeof(D3DCAPS9));				// �豸��Ϣ

		m_pD3D = NULL;										// D3D����
		pd3dDevice = NULL;									// D3D�豸
		m_pSprite = NULL;									// �������
		memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));	// �豸��������

		m_bDeviceLost = false;								// �Ƿ�ʧ���豸
	}

	RenderSystem::~RenderSystem()
	{
		FreeRenderSystem();
	}

	bool RenderSystem::CreateRenderSystem(UINT BackBufferWidth, UINT BackBufferHeight, HWND hWnd, bool TrueColor, bool Windowed, bool VSync, bool AntiAlias)
	{
		m_hWnd = hWnd;
		m_AntiAlias = AntiAlias;

		// ��Ҫ�ͷ��豸
		FreeRenderSystem();

		// ���û���豸ָ��ſ��Դ����豸
		if(!m_pD3D)
		{
			// �����豸
			m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
			if(!m_pD3D)
			{
				LOG(_T("�޷�����Direct3D9����"), Errors);
				return false;
			}

			// ��ʾ�豸�汾
			D3DADAPTER_IDENTIFIER9 dai;
			if(!FAILED(m_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &dai)))
			{
				int Product = HIWORD(dai.DriverVersion.HighPart);
				int Version = LOWORD(dai.DriverVersion.HighPart);
				int SubVersion = HIWORD(dai.DriverVersion.LowPart);
				int Build = LOWORD(dai.DriverVersion.LowPart);
				//TCHAR str[512] = {0};
				//_stprintf(str, _T("���� %s, ���� %s, ��Ʒ %d.%d.%d.%d"), (dai.Description), (dai.Driver), Product, Version, SubVersion, Build);
				//LOG(str);
			}
			else LOG(_T("�޷�����豸�汾"), Warnings);

			// ����豸��ʾģʽ
			D3DDISPLAYMODE d3ddm;
			HRESULT hr = m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
			if(FAILED(hr))
			{
				LOG(_T("�޷�����豸��ʾģʽ"), Errors);
				return false;
			}

			// ��䴴���豸����
			ZeroMemory(&d3dpp, sizeof(d3dpp));
			d3dpp.BackBufferWidth					= BackBufferWidth;
			d3dpp.BackBufferHeight					= BackBufferHeight;
			d3dpp.BackBufferCount					= 1;
			d3dpp.EnableAutoDepthStencil			= TRUE;

			// �Ƿ�����ֱͬ��
			if(VSync) d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_ONE;
			else d3dpp.PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;

			// �Ƿ񴰿�ģʽ
			if(Windowed)
			{
				d3dpp.BackBufferFormat				= d3ddm.Format;
				d3dpp.SwapEffect					= D3DSWAPEFFECT_COPY;
				d3dpp.Windowed						= TRUE;
			}
			else
			{
				// ȫ���µĺ�̨��ɫ��ʽ
				if(TrueColor)
					d3dpp.BackBufferFormat			= D3DFMT_X8R8G8B8;
				else
					d3dpp.BackBufferFormat			= D3DFMT_R5G6B5;

				d3dpp.SwapEffect					= D3DSWAPEFFECT_FLIP;
				d3dpp.Windowed						= FALSE;
				d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
			}

			// �Ƿ���ȫ��������
			if(AntiAlias)
			{
				dword qualityLevels = 0;

				if( SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
					m_DevType, d3dpp.BackBufferFormat, Windowed ? TRUE : FALSE,
					D3DMULTISAMPLE_4_SAMPLES, &qualityLevels)))
				{
					// �򿪷�����
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
							LOG(_T("����Ӳ����֧��,�޷���ȫ��������"), Warnings);
							AntiAlias = false;
						}
					}
				}
			}

			// �Ƿ���Ҫģ�滺��
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
							LOG(_T("����Ӳ����֧��,�޷���ģ�滺��"), Warnings);
							m_StencilBuffer = false;
						}
					}
				}
				else
				{
					if( FAILED(m_pD3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, m_DevType,
						d3dpp.BackBufferFormat, d3dpp.BackBufferFormat, d3dpp.AutoDepthStencilFormat)))
					{
						LOG(_T("���ģ�滺���ʽ�ǲ�֧�ֵĸ�ʽ"), Warnings);
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
							LOG(_T("�豸��֧�ֱ������Ȼ����ʽ"), Errors);
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

		// �����豸
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
			LOG(_T("����D3D�豸ʧ��"), Errors);
			return false;
		}

		pd3dDevice->GetDeviceCaps(&m_Caps);
		if( m_StencilBuffer &&
			(!(m_Caps.StencilCaps & D3DSTENCILCAPS_DECRSAT) ||
			!(m_Caps.StencilCaps & D3DSTENCILCAPS_INCRSAT) ||
			!(m_Caps.StencilCaps & D3DSTENCILCAPS_KEEP)))
		{
			LOG(_T("�豸û������ģ�滺��.����ģ�滺��"), Warnings);
			m_StencilBuffer = false;
		}

		// ��ȫ������������
		if(m_AntiAlias) pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

		// �����������
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
		// ��Ҫ
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
					LOG(_T("D3D�������ش���"), Insane);
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
			LOG(_T("����ʧ��"), Warnings);

		if(FAILED(pd3dDevice->BeginScene()))
		{
			LOG(_T("�޷���ʼ���Ƴ���"), Warnings);
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
			LOG(_T("�޷�ֹͣ���Ƴ���"), Warnings);
			return false;
		}

		HRESULT hr;

		RECT rcDst = {0, 0, d3dpp.BackBufferWidth, d3dpp.BackBufferHeight};

		hr = pd3dDevice->Present(pSrcRC ? &pSrcRC->GetRECT() : NULL, &rcDst, NULL, NULL);

		if(hr == D3DERR_DEVICELOST)
		{
			m_bDeviceLost = true;
			LOG(_T("�豸�Ѷ�ʧ"), Information);
		}
		else
		{
			if(FAILED(hr) && hr != D3DERR_INVALIDCALL)
			{
				LOG(_T("�޷�����ǰ�󻺳�"), Warnings);
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

			// ��������������ʧ��Ϣ
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
					LOG(_T("�޷������豸"), Warnings);
				}
				else LOG(_T("�����豸û����ȫ���ͷ�,�����޷������豸"), Warnings);
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