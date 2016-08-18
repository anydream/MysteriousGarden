/************************************************************************/
// 创建时间: 080906
// 最后确定: 081012

#include "Color.h"

namespace AnyPlay
{
	void Color::SetColor(float a, float r, float g, float b)
	{
		m_Alpha = a;
		m_Red = r;
		m_Green = g;
		m_Blue = b;
	}
	
	void Color::SetColor(byte a, byte r, byte g, byte b)
	{
		m_Alpha = static_cast<float>(a) / 255.0f;
		m_Red = static_cast<float>(r) / 255.0f;
		m_Green = static_cast<float>(g) / 255.0f;
		m_Blue = static_cast<float>(b) / 255.0f;
	}
	
	dword Color::GetColor() const
	{
		return (
			static_cast<dword>(m_Alpha * 255) << 24 |
			static_cast<dword>(m_Red * 255) << 16 |
			static_cast<dword>(m_Green * 255) << 8 |
			static_cast<dword>(m_Blue * 255)
			);
	}
	
	bool Color::GetColor(float argb[4]) const
	{
		if(argb)
		{
			argb[0] = m_Alpha;
			argb[1] = m_Red;
			argb[2] = m_Green;
			argb[3] = m_Blue;
			return true;
		}
		return false;
	}
	
	void Color::GetColor(float& a, float& r, float& g, float& b) const
	{
		a = m_Alpha;
		r = m_Red;
		g = m_Green;
		b = m_Blue;
	}

	float Color::GetHue() const
	{
		float pRed = m_Red;
		float pGreen = m_Green;
		float pBlue = m_Blue;
		
		float pMax = APMax(APMax(m_Red, m_Green), m_Blue);
		float pMin = APMin(APMin(m_Red, m_Green), m_Blue);
		
		float pHue;
		
		if( pMax == pMin ) pHue = 0.0f;
		else
		{
			if( pMax == pRed ) pHue = (pGreen - pBlue) / (pMax - pMin);
			else if( pMax == pGreen ) pHue = 2 + (pBlue - pRed) / (pMax - pMin);
			else pHue = 4 + (pRed - pGreen) / (pMax - pMin);
		}
		
		float Hue = pHue / 6.0f;
		if( Hue < 0.0f )
			Hue += 1.0f;
		
		return Hue;
	}
	
	float Color::GetSaturation() const
	{
		float pMax = APMax(APMax(m_Red, m_Green), m_Blue);
		float pMin = APMin(APMin(m_Red, m_Green), m_Blue);
		
		float pLum = (pMax + pMin) / 2.0f;
		float pSat;
		
		if( pMax == pMin ) pSat = 0.0f;
		else
		{
			if( pLum < 0.5f ) pSat = (pMax - pMin) / (pMax + pMin);
			else pSat = (pMax - pMin) / (2.0f - pMax - pMin);
		}
		
		return pSat;
	}
	
	float Color::GetLumination() const
	{
		float pMax = APMax(APMax(m_Red, m_Green), m_Blue);
		float pMin = APMin(APMin(m_Red, m_Green), m_Blue);
		
		float pLum = (pMax + pMin) / 2.0f;
		return pLum;
	}
	
	void Color::SetHSLA(float hue, float saturation, float luminance, float alpha)
	{
		m_Alpha = alpha;
		
		float temp3[3];
		
		float pHue = hue;
		float pSat = saturation;
		float pLum = luminance;
		
		if( pSat == 0 )
		{
			m_Red = pLum;
			m_Green = pLum;
			m_Blue = pLum;
		}
		else
		{
			float temp2;
			if( pLum < 0.5f ) temp2 = pLum * (1.0f + pSat);
			else temp2 = pLum + pSat - pLum * pSat;
			
			float temp1 = 2.0f * pLum - temp2;
			
			temp3[0] = pHue + (1.0f / 3.0f);
			temp3[1] = pHue;
			temp3[2] = pHue - (1.0f / 3.0f);
			
			for(int n = 0; n < 3; n++)
			{
				if( temp3[n] < 0 )
					temp3[n] ++;
				if( temp3[n] > 1 )
					temp3[n] --;
				
				if( (temp3[n] * 6.0f) < 1.0f)
				{
					temp3[n] = temp1 + (temp2 - temp1) * 6.0f * temp3[n];
				}
				else
				{
					if((temp3[n] * 2.0f) < 1)
					{
						temp3[n] = temp2;
					}
					else
					{
						if((temp3[n] * 3.0f) < 2.0f)
						{
							temp3[n] = temp1 + (temp2 - temp1) * ((2.0f / 3.0f) - temp3[n]) * 6.0f;
						}
						else
						{
							temp3[n] = temp1;
						}
					}
				}
			}
			
			m_Red = temp3[0];
			m_Green = temp3[1];
			m_Blue = temp3[2];
		}
		
	}
	
	void Color::InvertColor()
	{
		m_Red	= 1.0f - m_Red;
		m_Green	= 1.0f - m_Green;
		m_Blue	= 1.0f - m_Blue;
	}
	
	void Color::InvertColorWithAlpha()
	{
		m_Alpha	= 1.0f - m_Alpha;
		m_Red	= 1.0f - m_Red;
		m_Green	= 1.0f - m_Green;
		m_Blue	= 1.0f - m_Blue;
	}

	Color& Color::operator = (const Color& col)
	{
		if(&col == this) return *this;
		m_Alpha = col.m_Alpha;
		m_Red = col.m_Red;
		m_Green = col.m_Green;
		m_Blue = col.m_Blue;

		return *this;
	}

	Color& Color::operator = (const float argb[4])
	{
		if(argb)
		{
			m_Alpha = argb[0];
			m_Red = argb[1];
			m_Green = argb[2];
			m_Blue = argb[3];
		}
		return *this;
	}
	
	Color& Color::operator = (dword argb)
	{
		m_Blue	= static_cast<float>(argb & 0xFF) / 255.0f;
		argb >>= 8;
		m_Green	= static_cast<float>(argb & 0xFF) / 255.0f;
		argb >>= 8;
		m_Red	= static_cast<float>(argb & 0xFF) / 255.0f;
		argb >>= 8;
		m_Alpha	= static_cast<float>(argb & 0xFF) / 255.0f;
		return *this;
	}

	bool Color::operator == (const Color& col) const
	{
		if(&col == this) return true;
		return (m_Alpha == col.m_Alpha &&
			m_Red == col.m_Red &&
			m_Green == col.m_Green &&
			m_Blue == col.m_Blue);
	}
}
