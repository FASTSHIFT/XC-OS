#include "FixedMath.h"
#include "Generated/LUT.h"

uint16_t RandomOld()
{
	static uint16_t randVal = 0xABC;

	uint16_t lsb = randVal & 1;
	randVal >>= 1;
	if (lsb == 1)
		randVal ^= 0xB400u;

	return randVal - 1;
}

static uint16_t xs = 1;

uint16_t Random()
{
	xs ^= xs << 7;
	xs ^= xs >> 9;
	xs ^= xs << 8;
	return xs;
}

void SeedRandom(uint16_t seed)
{
	xs = seed | 1;
}

Matrix3x3 Matrix3x3::Identity()
{
	Matrix3x3 result;

	result.m[0][0] = 1;
	result.m[1][0] = 0;
	result.m[2][0] = 0;
	result.m[0][1] = 0;
	result.m[1][1] = 1;
	result.m[2][1] = 0;
	result.m[0][2] = 0;
	result.m[1][2] = 0;
	result.m[2][2] = 1;

	return result;
}

Matrix3x3 Matrix3x3::RotateX(angle_t angle)
{
	Matrix3x3 result;
	fixed8_t cosAngle = FixedMath::Cos(angle);
	fixed8_t sinAngle = FixedMath::Sin(angle);
	
	result.m[0][0] = 1;
	result.m[1][0] = 0;
	result.m[2][0] = 0;
	result.m[0][1] = 0;
	result.m[1][1] = cosAngle;
	result.m[2][1] = -sinAngle;
	result.m[0][2] = 0;
	result.m[1][2] = sinAngle;
	result.m[2][2] = cosAngle;
	
	return result;
}

Matrix3x3 Matrix3x3::RotateY(angle_t angle)
{
	Matrix3x3 result;
	fixed8_t cosAngle = FixedMath::Cos(angle);
	fixed8_t sinAngle = FixedMath::Sin(angle);

	result.m[0][0] = cosAngle;
	result.m[1][0] = 0;
	result.m[2][0] = sinAngle;
	result.m[0][1] = 0;
	result.m[1][1] = 1;
	result.m[2][1] = 0;
	result.m[0][2] = -sinAngle;
	result.m[1][2] = 0;
	result.m[2][2] = cosAngle;
	
	return result;
}

Matrix3x3 Matrix3x3::RotateZ(angle_t angle)
{
	Matrix3x3 result;
	fixed8_t cosAngle = FixedMath::Cos(angle);
	fixed8_t sinAngle = FixedMath::Sin(angle);

	result.m[0][0] = cosAngle;
	result.m[1][0] = -sinAngle;
	result.m[2][0] = 0;
	result.m[0][1] = sinAngle;
	result.m[1][1] = cosAngle;
	result.m[2][1] = 0;
	result.m[0][2] = 0;
	result.m[1][2] = 0;
	result.m[2][2] = 1;
	
	return result;
}

Matrix3x3 Matrix3x3::Transpose() const
{
	Matrix3x3 result;
	
	result.m[0][0] = m[0][0];
	result.m[0][1] = m[1][0];
	result.m[0][2] = m[2][0];
	result.m[1][0] = m[0][1];
	result.m[1][1] = m[1][1];
	result.m[1][2] = m[2][1];
	result.m[2][0] = m[0][2];
	result.m[2][1] = m[1][2];
	result.m[2][2] = m[2][2];
	
	return result;
}

Vector3b Matrix3x3::operator * (const Vector3b& v)
{
	return Vector3b(
		v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0],
		v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1],
		v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2]);
}

Vector3s Matrix3x3::operator * (const Vector3s& v)
{
	return Vector3s(
		v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0],
		v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1],
		v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2]);
}

Matrix3x3 Matrix3x3::operator * (const Matrix3x3& other)
{
	fixed16_t temp = 0;
	int a, b, c;
	Matrix3x3 result;

	for(a = 0; a < 3; a++)
	{
	   for(b = 0; b < 3; b++)
	   {
		   for(c = 0; c < 3; c++)
		   {
			   temp += m[b][c] * other.m[c][a];
		   }
		   result.m[b][a] = (fixed8_t) temp;
		   temp = 0;
	   }
	}
	
	return result;
}

void Matrix3x3::Normalize()
{
	Vector3s forward = Forward();
	forward.Normalize();

	Vector3s up = Up();
	up.Normalize();

	Vector3s right = up.Cross(forward);
	up = forward.Cross(right);

	m[0][0] = (fixed8_t) right.x;
	m[0][1] = (fixed8_t) right.y;
	m[0][2] = (fixed8_t) right.z;
	m[1][0] = (fixed8_t) up.x;
	m[1][1] = (fixed8_t) up.y;
	m[1][2] = (fixed8_t) up.z;
	m[2][0] = (fixed8_t) forward.x;
	m[2][1] = (fixed8_t) forward.y;
	m[2][2] = (fixed8_t) forward.z;

}

