/************************************************************************/
// ����ʱ��: 080906
// ���ȷ��: 081012

#ifndef _Color_h_
#define _Color_h_

#include "../AnyPlay.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay���ƿռ�
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// ��ɫģ��
	class Color
	{
	public:
		// ���� ����Ĭ����ɫΪ��ɫ
		Color() { SetWhiteColor(); }
		Color(const Color& col) { *this = col; }
		Color(float a, float r, float g, float b) { SetColor(a, r, g, b); }
		Color(dword argb) { SetColor(argb); }

		// ������ɫ
		void SetColor(float a, float r, float g, float b);
		void SetColor(byte a, byte r, byte g, byte b);
		void SetColor(const Color& color) { *this = color; }
		void SetColor(dword argb) { *this = argb; }

		// �����ɫ
		dword GetColor() const;
		bool GetColor(float argb[4]) const;
		void GetColor(float& a, float& r, float& g, float& b) const;

		// ������ɫ
		void SetAlpha(byte val) { m_Alpha = static_cast<float>(val) / 255.0f; }
		void SetRed(byte val) { m_Red = static_cast<float>(val) / 255.0f; }
		void SetGreen(byte val) { m_Green = static_cast<float>(val) / 255.0f; }
		void SetBlue(byte val) { m_Blue = static_cast<float>(val) / 255.0f; }

		void SetWhiteColor() { m_Alpha = m_Red = m_Green = m_Blue = 1.0f; }
		void SetBlackColor() { m_Alpha = 1.0f; m_Red = m_Green = m_Blue = 0.0f; }

		byte GetAlpha() const { return static_cast<byte>(m_Alpha * 255.0f); }
		byte GetRed() const { return static_cast<byte>(m_Red * 255.0f); }
		byte GetGreen() const { return static_cast<byte>(m_Green * 255.0f); }
		byte GetBlue() const { return static_cast<byte>(m_Blue * 255.0f); }

		// ���ɫ��
		float GetHue() const;
		// ���ɫ���Ͷ�
		float GetSaturation() const;
		// �������
		float GetLumination() const;
		// ����ɫ��,ɫ���Ͷ�,����
		void SetHSLA(float hue, float saturation, float luminance, float alpha);
		
		// ��ɫ
		void InvertColor();
		void InvertColorWithAlpha();

		Color& operator = (const Color& col);
		Color& operator = (const float argb[4]);
		Color& operator = (dword argb);

		bool operator == (const Color& col) const;
		bool operator != (const Color& col) const { return !(*this == col); }

		Color operator + (const Color& col) const { return Color(m_Alpha + col.m_Alpha, m_Red + col.m_Red, m_Green + col.m_Green, m_Blue + col.m_Blue); }
		Color operator - (const Color& col) const { return Color(m_Alpha - col.m_Alpha, m_Red - col.m_Red, m_Green - col.m_Green, m_Blue - col.m_Blue); }
		Color operator * (const Color& col) const { return Color(m_Alpha * col.m_Alpha, m_Red * col.m_Red, m_Green * col.m_Green, m_Blue * col.m_Blue); }
		Color operator / (const Color& col) const { return Color(m_Alpha / col.m_Alpha, m_Red / col.m_Red, m_Green / col.m_Green, m_Blue / col.m_Blue); }

		void operator += (const Color& col) { m_Alpha += col.m_Alpha; m_Red += col.m_Red; m_Green += col.m_Green; m_Blue += col.m_Blue; } 
		void operator -= (const Color& col) { m_Alpha -= col.m_Alpha; m_Red -= col.m_Red; m_Green -= col.m_Green; m_Blue -= col.m_Blue; } 
		void operator *= (const Color& col) { m_Alpha *= col.m_Alpha; m_Red *= col.m_Red; m_Green *= col.m_Green; m_Blue *= col.m_Blue; } 
		void operator /= (const Color& col) { m_Alpha /= col.m_Alpha; m_Red /= col.m_Red; m_Green /= col.m_Green; m_Blue /= col.m_Blue; } 

		Color operator + (float v) const { return Color(m_Alpha + v, m_Red + v, m_Green + v, m_Blue + v); }
		Color operator - (float v) const { return Color(m_Alpha - v, m_Red - v, m_Green - v, m_Blue - v); }
		Color operator * (float v) const { return Color(m_Alpha * v, m_Red * v, m_Green * v, m_Blue * v); }
		Color operator / (float v) const { return Color(m_Alpha / v, m_Red / v, m_Green / v, m_Blue / v); }

		void operator += (float v) { m_Alpha += v; m_Red += v; m_Green += v; m_Blue += v; }
		void operator -= (float v) { m_Alpha -= v; m_Red -= v; m_Green -= v; m_Blue -= v; }
		void operator *= (float v) { m_Alpha *= v; m_Red *= v; m_Green *= v; m_Blue *= v; }
		void operator /= (float v) { m_Alpha /= v; m_Red /= v; m_Green /= v; m_Blue /= v; }

	public:
		float	m_Alpha;	// ͸����ͨ��
		float	m_Red;		// ��ɫ
		float	m_Green;	// ��ɫ
		float	m_Blue;		// ��ɫ
	};
}

#endif