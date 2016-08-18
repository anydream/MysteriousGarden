/************************************************************************/
// 创建时间: 080906
// 最后确定: 081011

#include "Vector4.h"

#define M_PI 3.141592654f

namespace AnyPlay
{
	// 叉乘
	void Vector4::CrossProduct(Vector4 v1, Vector4 v2)
	{
		x = ((v1.y * v2.z) - (v1.z * v2.y));
		y = ((v1.z * v2.x) - (v1.x * v2.z));
		z = ((v1.x * v2.y) - (v1.y * v2.x));
	}
	
	void Vector4::CrossProduct3(Vector4 v1, Vector4 v2, Vector4 v3)
	{
		x = v1.y * v2.z * v3.w +
			v1.z * v2.w * v3.y +
			v1.w * v2.y * v3.z - 
			v1.y * v2.w * v3.z -
			v1.z * v2.y * v3.w -
			v1.w * v2.z * v3.y;
		
		y = v1.x * v2.w * v3.z +
			v1.z * v2.x * v3.w +
			v1.w * v2.z * v3.x -
			v1.x * v2.z * v3.w -
			v1.z * v2.w * v3.x -
			v1.w * v2.x * v3.z;
		
		z = v1.x * v2.y * v3.w +
			v1.y * v2.w * v3.x +
			v1.w * v2.x * v3.y -
			v1.x * v2.w * v3.y -
			v1.y * v2.x * v3.w -
			v1.w * v2.y * v3.x;
		
		w = v1.x * v2.z * v3.y +
			v1.y * v2.x * v3.z +
			v1.z * v2.y * v3.x -
			v1.x * v2.y * v3.z -
			v1.y * v2.z * v3.x -
			v1.z * v2.x * v3.y;
	}
	
	// 点乘
	float Vector4::DotProduct3(Vector4 v1)
	{
		return x * v1.x + y * v1.y + z * v1.z;
	}
	
	float Vector4::DotProduct4(Vector4 v1)
	{
		return x * v1.x + y * v1.y + z * v1.z + w * v1.w;
	}
	
	// 取模
	float Vector4::GetLength()
	{
		return (float)sqrt((x * x + y * y + z * z));
	}
	
	// 单位向量
	void Vector4::Normal()
	{
		float lenght = GetLength();
		
		if(lenght == 0.0f)
			lenght = 1.0f;
		
		x = x / lenght;
		y = y / lenght;
		z = z / lenght;
		w = w / lenght;
	}
	
	void Vector4::Normalize(Vector4 Triangle[])
	{
		Vector4 v1, v2;
		
		v1.x = Triangle[0].x - Triangle[1].x;
		v1.y = Triangle[0].y - Triangle[1].y;
		v1.z = Triangle[0].z - Triangle[1].z;
		v1.w = Triangle[0].w - Triangle[1].w;
		
		v2.x = Triangle[1].x - Triangle[2].x;
		v2.y = Triangle[1].y - Triangle[2].y;
		v2.z = Triangle[1].z - Triangle[2].z;
		v2.w = Triangle[1].w - Triangle[2].w;
		
		CrossProduct(v1, v2);
		Normal();
	}
	
	void Vector4::ExtendVertexPos(Vector4 currentVertex, Vector4 lightPos, float Extend)
	{
		Vector4 lightDir;
		Vector4 newPos;
		
		lightDir = currentVertex - lightPos;
		
		x = newPos.x;
		y = newPos.y;
		z = newPos.z;
		w = newPos.w;
	}
	
	void Vector4::ExtendVertexPos(Vector4 lightPos, float Extend)
	{
		Vector4 lightDir;
		Vector4 newPos;
		
		lightDir = Vector4(x, y, z, w) - lightPos;

		newPos = lightPos + lightDir * Extend;
		
		x = newPos.x;
		y = newPos.y;
		z = newPos.z;
		w = newPos.w;
	}
	
	Vector4 Vector4::GetRotatedX(double angle)
	{
		float sinAngle = (float)sin(M_PI * angle / 180.0f);
		float cosAngle = (float)cos(M_PI * angle / 180.0f);
		
		return Vector4(x, y * cosAngle - z * sinAngle, y * sinAngle + z * cosAngle, w);
	}
	
	Vector4 Vector4::GetRotatedY(double angle)
	{
		float sinAngle = (float)sin(M_PI * angle / 180);
		float cosAngle = (float)cos(M_PI * angle / 180);
		
		return Vector4(x * cosAngle + z * sinAngle, y, -x * sinAngle + z * cosAngle, w);
	}
	
	Vector4 Vector4::GetRotatedZ(double angle)
	{
		float sinAngle = (float)sin(M_PI * angle / 180.0f);
		float cosAngle = (float)cos(M_PI * angle / 180.0f);
		
		return Vector4(x * cosAngle - y * sinAngle, x * sinAngle + y * cosAngle, z, w);
	}
	
	Vector4 Vector4::GetRotatedAxis(double angle, Vector4 axis)
	{
		if(angle == 0.0f) return(*this);
		
		axis.Normal();
		
		Vector4 RotationRow1, RotationRow2, RotationRow3;
		
		double newAngle = M_PI * angle / 180;
		float sinAngle = (float)sin(newAngle);
		float cosAngle = (float)cos(newAngle);
		float oneSubCos = 1.0f - cosAngle;
		
		RotationRow1.x = (axis.x) * (axis.x) + cosAngle * (1 - (axis.x) * (axis.x));
		RotationRow1.y = (axis.x) * (axis.y) * (oneSubCos) - sinAngle * axis.z;
		RotationRow1.z = (axis.x) * (axis.z) * (oneSubCos) + sinAngle * axis.y;
		
		RotationRow2.x = (axis.x) * (axis.y) * (oneSubCos) + sinAngle * axis.z;
		RotationRow2.y = (axis.y) * (axis.y) + cosAngle * (1 - (axis.y) * (axis.y));
		RotationRow2.z = (axis.y) * (axis.z) * (oneSubCos) - sinAngle * axis.x;
		
		RotationRow3.x = (axis.x) * (axis.z) * (oneSubCos) - sinAngle * axis.y;
		RotationRow3.y = (axis.y) * (axis.z) * (oneSubCos) + sinAngle * axis.x;
		RotationRow3.z = (axis.z) * (axis.z) + cosAngle * (1 - (axis.z) * (axis.z));
		
		return Vector4(this->DotProduct3(RotationRow1),
			this->DotProduct3(RotationRow2),
			this->DotProduct3(RotationRow3));
	}
	
	void Vector4::CalculateBinormalVector(Vector4 tangent, Vector4 normal)
	{
		CrossProduct(tangent, normal);
	}
	
	void Vector4::ClampTo01()
	{
		Vector4 temp(x, y, z, w);
		temp.Normal();
		temp = temp * 0.5f + Vector4(0.5f, 0.5f, 0.5f);
		
		x = temp.x;
		y = temp.y;
		z = temp.z;
	}
}