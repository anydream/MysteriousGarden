/************************************************************************/
// 创建时间: 081110
// 最后确定: 081125

#ifndef _Font_h_
#define _Font_h_

#include "../AnyPlay.h"
#include "../FrameWork/Manager.h"
#include "../FrameWork/Event.h"
#include "../FrameWork/Logger.h"
#include "../Core/String.h"
#include "../Core/Color.h"
#include "../Core/Quad.h"

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

namespace AnyPlay
{
	class RenderSystem;

	class Font : public Managed<Font>
	{
	public:
		Font();

		bool CreateFont(
			const String&	strFontName,
			INT				Height = -12,
			UINT			Width = 6,
			UINT			Weight = 1,
			UINT			MipLevels = 10,
			BOOL			Italic = FALSE,
			DWORD			CharSet = DEFAULT_CHARSET,
			DWORD			OutputPrecision = OUT_TT_ONLY_PRECIS,
			DWORD			Quality = ANTIALIASED_QUALITY,
			DWORD			PitchAndFamily = FIXED_PITCH);

		bool Free();
		bool OnLost();
		bool OnReset();
		bool DrawFont(const String& text, INT Count, const Rect* rect, DWORD Format, const Color& color);

	private:
		ID3DXFont*			m_pFont;		// 字体指针
		IDirect3DDevice9*	pd3dDevice;		// D3D9设备对象指针
		ID3DXSprite*		m_pSprite;		// 精灵指针
	};
}

#endif