/************************************************************************/
// 创建时间: 081110
// 最后确定: 081125

#include "Font.h"
#include "RenderSystem.h"

namespace AnyPlay
{
	Font::Font()
	{
		m_pFont = NULL;
		pd3dDevice = NULL;
		m_pSprite = NULL;

		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		if(rs)
		{
			pd3dDevice = rs->GetD3D9Device();				// D3D9设备对象指针
			m_pSprite = rs->GetSprite();
		}
	}

	bool Font::CreateFont(
		const String&	strFontName,
		INT				Height,
		UINT			Width,
		UINT			Weight,
		UINT			MipLevels,
		BOOL			Italic,
		DWORD			CharSet,
		DWORD			OutputPrecision,
		DWORD			Quality,
		DWORD			PitchAndFamily)
	{
		if(pd3dDevice)
		{
			Free();
			if(FAILED(D3DXCreateFont(
				pd3dDevice,
				Height,
				Width,
				Weight,
				MipLevels,
				Italic,
				CharSet,
				OutputPrecision,
				Quality,
				PitchAndFamily,
				strFontName(),
				&m_pFont)))
			{
				LOG(_T("创建字体失败"), Insane);
				return false;
			}

			SetName(strFontName);

			return true;
		}
		return false;
	}

	bool Font::Free()
	{
		if(m_pFont)
		{
			SAFE_RELEASE(m_pFont);
			return true;
		}
		return false;
	}

	bool Font::OnLost()
	{
		if(m_pFont)
		{
			m_pFont->OnLostDevice();
			return true;
		}
		return false;
	}

	bool Font::OnReset()
	{
		if(m_pFont)
		{
			m_pFont->OnResetDevice();
			return true;
		}
		return false;
	}

	bool Font::DrawFont(const String& text, INT Count, const Rect* rect, DWORD Format, const Color& color)
	{
		if(pd3dDevice && m_pFont)
		{
			if(m_pSprite) m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
			if(FAILED(m_pFont->DrawText(m_pSprite, text(), Count, rect ? &rect->GetRECT() : NULL, Format, color.GetColor()))) return false;
			if(m_pSprite) m_pSprite->End();
			return true;
		}
		return false;
	}
}