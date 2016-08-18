/************************************************************************/
// 创建时间: 081028
// 最后确定: 081119

#ifndef _Texture_h_
#define _Texture_h_

#include "../AnyPlay.h"
#include "../FrameWork/Manager.h"
#include "../FileSystem/FileSystem.h"
#include "../Core/String.h"
#include "../Core/Color.h"
#include "../Core/Quad.h"

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define APTEX_FILTER	1	// 使用过滤器
#define APTEX_MIPMAP	2	// 使用Mipmap
#define APTEX_HIRES		4	// 使用高速缓存

namespace AnyPlay
{
	class RenderSystem;

	class Texture : public Managed<Texture>
	{
	public:
		Texture();
		virtual		~Texture();
		bool		LoadFromFile(const String& filename, const Color* colorKey, dword crtFlags);
		bool		Free();
		UINT		GetOrgWidth() const;
		UINT		GetOrgHeight() const;
		void		CalcTextureSize();
		bool		DrawTexture(const Quad* pDstQuad, const Quad* pSrcQuad, const Color pColor[4]);
		void		OnLost();
		void		OnReset();
		Texture&	operator = (const Texture& tex);

		// 设置文件名
		void			SetFileName(const String& name) { m_FileName = name; }
		// 获得文件名
		const String&	GetFileName() const { return m_FileName; }

		// 设置创建标记
		void			SetCreateFlags(dword flg) { m_CreateFlags = flg; }
		// 获得创建标记
		dword			GetCreateFlags() const { return m_CreateFlags; }

		void			SetColorKey(const Color& colKey) { m_bColorKey = true; m_ColorKey = colKey; }
		// 获得关键色
		Color*			GetColorKey() { if(m_bColorKey) return &m_ColorKey; return NULL; }

		void			UsingColorKey(bool b) { m_bColorKey = b; }
		// 获得纹理宽度
		dword			GetWidth() const { return m_Width; }
		// 获得纹理高度
		dword			GetHeight() const { return m_Height; }

		bool			ReloadTexture() { return LoadFromFile(m_FileName, m_bColorKey ? &m_ColorKey : NULL, m_CreateFlags); }

	private:
		String			m_FileName;			// 文件名
		dword			m_CreateFlags;		// 创建标记
		Color			m_ColorKey;			// 关键色
		bool			m_bColorKey;		// 是否起用关键色

		dword			m_Width;			// 纹理宽度
		dword			m_Height;			// 纹理高度

		IDirect3DTexture9*	m_pTexture;		// D3D9纹理对象指针
		D3DXIMAGE_INFO		m_ImageInfo;	// 图片文件信息
		IDirect3DDevice9*	pd3dDevice;		// D3D9设备对象指针
	};
}
#endif