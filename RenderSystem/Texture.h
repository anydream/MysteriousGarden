/************************************************************************/
// ����ʱ��: 081028
// ���ȷ��: 081119

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

#define APTEX_FILTER	1	// ʹ�ù�����
#define APTEX_MIPMAP	2	// ʹ��Mipmap
#define APTEX_HIRES		4	// ʹ�ø��ٻ���

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

		// �����ļ���
		void			SetFileName(const String& name) { m_FileName = name; }
		// ����ļ���
		const String&	GetFileName() const { return m_FileName; }

		// ���ô������
		void			SetCreateFlags(dword flg) { m_CreateFlags = flg; }
		// ��ô������
		dword			GetCreateFlags() const { return m_CreateFlags; }

		void			SetColorKey(const Color& colKey) { m_bColorKey = true; m_ColorKey = colKey; }
		// ��ùؼ�ɫ
		Color*			GetColorKey() { if(m_bColorKey) return &m_ColorKey; return NULL; }

		void			UsingColorKey(bool b) { m_bColorKey = b; }
		// ���������
		dword			GetWidth() const { return m_Width; }
		// �������߶�
		dword			GetHeight() const { return m_Height; }

		bool			ReloadTexture() { return LoadFromFile(m_FileName, m_bColorKey ? &m_ColorKey : NULL, m_CreateFlags); }

	private:
		String			m_FileName;			// �ļ���
		dword			m_CreateFlags;		// �������
		Color			m_ColorKey;			// �ؼ�ɫ
		bool			m_bColorKey;		// �Ƿ����ùؼ�ɫ

		dword			m_Width;			// ������
		dword			m_Height;			// ����߶�

		IDirect3DTexture9*	m_pTexture;		// D3D9�������ָ��
		D3DXIMAGE_INFO		m_ImageInfo;	// ͼƬ�ļ���Ϣ
		IDirect3DDevice9*	pd3dDevice;		// D3D9�豸����ָ��
	};
}
#endif