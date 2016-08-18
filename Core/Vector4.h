/************************************************************************/
// 创建时间: 080906
// 最后确定: 081011

#ifndef _Vector4_h_
#define _Vector4_h_

#include "../AnyPlay.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay名称空间
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// 4D向量
	class Vector4
	{
	public:
		Vector4() { x = y = z = 0.0f; w = 1.0f; }
		Vector4(float X, float Y) { x = X; y = Y; z = 0.0f; w = 1.0f; }
		Vector4(float X, float Y, float Z) { x = X; y = Y; z = Z; w = 1.0f; }
		Vector4(float X, float Y, float Z, float W) { x = X; y = Y; z = Z; w = W; }
		Vector4(const Vector4& v) { *this = v; }
		
		Vector4&	operator = (const Vector4& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
		Vector4		operator + (const Vector4& v) const { return Vector4(x + v.x, y + v.y, z + v.z); }
		Vector4		operator - (const Vector4& v) const { return Vector4(x - v.x, y - v.y, z - v.z); }
		Vector4		operator * (const Vector4& v) const { return Vector4(x * v.x, y * v.y, z * v.z); }
		Vector4		operator / (const Vector4& v) const { return Vector4(x / v.x, y / v.y, z / v.z); }
		Vector4		operator + (float f) const { return Vector4(x + f, y + f, z + f); }
		Vector4		operator - (float f) const { return Vector4(x - f, y - f, z - f); }
		Vector4		operator * (float f) const { return Vector4(x * f, y * f, z * f); }
		Vector4		operator / (float f) const { f = 1.0f / f; return Vector4(x * f, y * f, z * f); }
		
		void		operator += (const Vector4& v) { x += v.x; y += v.y; z += v.z; }
		void		operator -= (const Vector4& v) { x -= v.x; y -= v.y; z -= v.z; }
		void		operator *= (const Vector4& v) { x *= v.x; y *= v.y; z *= v.z; }
		void		operator /= (const Vector4& v) { x /= v.x; y /= v.y; z /= v.z; }
		void		operator += (float f) { x += f; y += f; z += f; }
		void		operator -= (float f) { x -= f; y -= f; z -= f; }
		void		operator *= (float f) { x *= f; y *= f; z *= f; }
		void		operator /= (float f) { f = 1.0f / f; x *= f; y *= f; z *= f; }
		
		bool		operator == (const Vector4& v) const { return ((x == v.x) && (y == v.y) && (z == v.z) && (w == v.w)); }
		bool		operator != (const Vector4& v) const { return !(*this == v); }
		
		void	CrossProduct(Vector4 v1, Vector4 v2);
		void	CrossProduct3(Vector4 v1, Vector4 v2, Vector4 v3);
		float	DotProduct3(Vector4 v1);
		float	DotProduct4(Vector4 v1);
		float	GetLength();
		void	Normal();
		void	Normalize(Vector4 Triangle[]);
		void	ExtendVertexPos(Vector4 lightPos, float Extend);
		void	ExtendVertexPos(Vector4 currentVertex, Vector4 lightPos, float Extend);
		Vector4 GetRotatedX(double angle);
		Vector4 GetRotatedY(double angle);
		Vector4 GetRotatedZ(double angle);
		Vector4 GetRotatedAxis(double angle, Vector4 axis);
		void	CalculateBinormalVector(Vector4 tangent, Vector4 normal);
		void	ClampTo01();

	public:
		float x, y, z, w;
	};

	typedef Vector4 Point;
}

#endif
