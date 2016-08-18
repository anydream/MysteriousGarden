

#include "Texture.h"
#include "RenderSystem.h"

#pragma warning(disable: 4244)

namespace AnyPlay
{
	Texture::Texture()
	{
		m_CreateFlags = 0;									// 创建标记
		m_bColorKey = false;								// 是否起用关键色

		m_Width = 0;										// 纹理宽度
		m_Height = 0;										// 纹理高度

		m_pTexture = NULL;									// D3D9纹理对象指针
		memset(&m_ImageInfo, 0, sizeof(D3DXIMAGE_INFO));	// 图片文件信息

		pd3dDevice = NULL;

		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		if(rs) pd3dDevice = rs->GetD3D9Device();				// D3D9设备对象指针	
	}

	Texture::~Texture()
	{
		Free();
	}

	bool Texture::LoadFromFile(const String& filename, const Color* colorKey, dword crtFlags)
	{
		if(pd3dDevice)
		{
			// 释放
			Free();

			FileSystem* fs = FileSystem::GetSingletonPtr();

			if(File* pFile = fs->Open(filename))
			{
				if( FAILED(D3DXCreateTextureFromFileInMemoryEx(
					pd3dDevice,
					pFile->GetData(),
					pFile->GetSize(),
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					(crtFlags & APTEX_HIRES) ? D3DUSAGE_DYNAMIC : NULL,
					D3DFMT_UNKNOWN,
					(crtFlags & APTEX_HIRES) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
					(crtFlags & APTEX_FILTER) ? D3DX_FILTER_LINEAR : D3DX_FILTER_NONE,
					(crtFlags & APTEX_MIPMAP) ? D3DX_FILTER_LINEAR : D3DX_FILTER_NONE,
					colorKey ? colorKey->GetColor() : NULL,
					&m_ImageInfo,
					NULL,
					&m_pTexture
					)))
				{
					return false;
				}

				// 保存创建信息
				m_FileName = filename;
				m_CreateFlags = crtFlags;
				if(colorKey) m_ColorKey = *colorKey;
				m_bColorKey = colorKey ? true : false;

				// 计算纹理大小
				CalcTextureSize();

				fs->Close(pFile);
				return true;
			}
		}
		return false;
	}

	bool Texture::Free()
	{
		if(m_pTexture)
		{
			SAFE_RELEASE(m_pTexture);
			return true;
		}
		return false;
	}

	UINT Texture::GetOrgWidth() const
	{
		return m_ImageInfo.Width;
	}

	UINT Texture::GetOrgHeight() const
	{
		return m_ImageInfo.Height;
	}

	void Texture::CalcTextureSize()
	{
		D3DSURFACE_DESC texDesc;
		if(m_pTexture)
		{
			m_pTexture->GetLevelDesc(0, &texDesc);
			m_Width = texDesc.Width;
			m_Height = texDesc.Height;
		}
	}

	bool Texture::DrawTexture(const Quad* pDstQuad, const Quad* pSrcQuad, const Color pColor[4])
	{
		if(pd3dDevice)
		{
			Quad DstQuad;
			Quad SrcQuad;
			Color color[4];

			if(pDstQuad) DstQuad = *pDstQuad;
			else DstQuad = Rect(0, 0, m_Width, m_Height);

			if(pSrcQuad) SrcQuad = *pSrcQuad;
			else SrcQuad = Rect(0, 0, m_Width, m_Height);

			if(pColor)
			{
				for(short i = 0; i < 4; i++)
					color[i] = pColor[i];
			}

			APRS_SCREEN_VERTEX vertices[4] =
			{
				DstQuad.m_lt.x, DstQuad.m_lt.y, 0.f, 1.f, color[0].GetColor(), m_pTexture ? PIXEL_ALIGNED(SrcQuad.m_lt.x) / m_Width : 0.f, m_pTexture ? PIXEL_ALIGNED(SrcQuad.m_lt.y) / m_Height : 0.f,
				DstQuad.m_rt.x, DstQuad.m_rt.y, 0.f, 1.f, color[1].GetColor(), m_pTexture ? PIXEL_ALIGNED(SrcQuad.m_rt.x) / m_Width : 0.f, m_pTexture ? PIXEL_ALIGNED(SrcQuad.m_rt.y) / m_Height : 0.f,
				DstQuad.m_rb.x, DstQuad.m_rb.y, 0.f, 1.f, color[2].GetColor(), m_pTexture ? PIXEL_ALIGNED(SrcQuad.m_rb.x) / m_Width : 0.f, m_pTexture ? PIXEL_ALIGNED(SrcQuad.m_rb.y) / m_Height : 0.f,
				DstQuad.m_lb.x, DstQuad.m_lb.y, 0.f, 1.f, color[3].GetColor(), m_pTexture ? PIXEL_ALIGNED(SrcQuad.m_lb.x) / m_Width : 0.f, m_pTexture ? PIXEL_ALIGNED(SrcQuad.m_lb.y) / m_Height : 0.f,
			};

			pd3dDevice->SetTexture(0, m_pTexture);
			pd3dDevice->SetFVF(APRS_SCREEN_VERTEX_FVF);
			if(FAILED(pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(APRS_SCREEN_VERTEX))))
				return false;
			return true;
		}
		return false;
	}

	void Texture::OnLost()
	{
		// 只有当纹理是被创建在高速缓存时才需要重新载入
		if(m_CreateFlags & APTEX_HIRES) Free();
	}

	void Texture::OnReset()
	{
		if(m_CreateFlags & APTEX_HIRES)
		{
			LoadFromFile(m_FileName, m_bColorKey ? &m_ColorKey : NULL, m_CreateFlags);
		}
	}

}