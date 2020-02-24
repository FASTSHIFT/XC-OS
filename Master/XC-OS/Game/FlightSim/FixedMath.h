#pragma once

#include <stdint.h>
#if _WIN32
#include <math.h>
#endif
#include "Defines.h"

#define FIXED_SHIFT 6
#define FIXED_ONE (1 << FIXED_SHIFT)
#define INT_TO_FIXED(x) ((x) * FIXED_ONE)
#define FIXED_TO_INT(x) ((x) >> 8)
#define FLOAT_TO_FIXED(x) ((int16_t)((x) * FIXED_ONE))

#define ABS(x) (((x) < 0) ? -(x) : (x))

#define FIXED_ANGLE_MAX 256
#define FIXED_ANGLE_WRAP(x) ((x) & 255)
#define FIXED_ANGLE_45 (FIXED_ANGLE_90 / 2)
#define FIXED_ANGLE_90 (FIXED_ANGLE_MAX / 4)
#define FIXED_ANGLE_180 (FIXED_ANGLE_90 * 2)
#define FIXED_ANGLE_270 (FIXED_ANGLE_90 * 3)
#define FIXED_ANGLE_TO_RADIANS(x) ((x) * (2.0f * 3.141592654f / FIXED_ANGLE_MAX))

constexpr int FixedFractionalBits = 6;

#if 1
struct fixed8_t
{
	int8_t rawValue;

	struct Raw
	{
		constexpr Raw(int8_t inValue) : rawValue(inValue) {}
		int8_t rawValue;
	};

	fixed8_t() {}
	constexpr fixed8_t(int inValue) : rawValue((int8_t)(inValue * (1 << FixedFractionalBits))) {}
	constexpr fixed8_t(float inValue) : rawValue(int8_t(inValue * (1 << FixedFractionalBits))) {}
	constexpr fixed8_t(Raw inValue) : rawValue(inValue.rawValue) {}

	static constexpr fixed8_t FromRaw(int8_t inRawValue)
	{
		return fixed8_t(Raw(inRawValue));
	}

	fixed8_t operator -() const
	{
		return FromRaw(-rawValue);
	}
};

constexpr fixed8_t operator+ (const fixed8_t& lhs, const fixed8_t& rhs)
{
	return fixed8_t::FromRaw(lhs.rawValue + rhs.rawValue);
}
constexpr fixed8_t operator- (const fixed8_t& lhs, const fixed8_t& rhs)
{
	return fixed8_t::FromRaw(lhs.rawValue - rhs.rawValue);
}
constexpr fixed8_t operator* (const fixed8_t& lhs, const fixed8_t& rhs)
{
	return fixed8_t::FromRaw(int8_t((int16_t(lhs.rawValue) * int16_t(rhs.rawValue)) >> FixedFractionalBits));
}
constexpr fixed8_t operator/ (const fixed8_t& lhs, const fixed8_t& rhs)
{
	return fixed8_t::FromRaw((int16_t(lhs.rawValue) << FixedFractionalBits) / rhs.rawValue);
}
constexpr bool operator < (const fixed8_t& lhs, const fixed8_t& rhs)
{
	return lhs.rawValue < rhs.rawValue;
}
constexpr bool operator > (const fixed8_t& lhs, const fixed8_t& rhs)
{
	return lhs.rawValue > rhs.rawValue;
}
constexpr bool operator <= (const fixed8_t& lhs, const fixed8_t& rhs)
{
	return lhs.rawValue <= rhs.rawValue;
}
constexpr bool operator >= (const fixed8_t& lhs, const fixed8_t& rhs)
{
	return lhs.rawValue >= rhs.rawValue;
}
constexpr bool operator == (const fixed8_t& lhs, const fixed8_t& rhs)
{
	return lhs.rawValue == rhs.rawValue;
}
constexpr bool operator != (const fixed8_t& lhs, const fixed8_t& rhs)
{
	return lhs.rawValue != rhs.rawValue;
}


struct fixed16_t
{
	int16_t rawValue;

	struct Raw
	{
		constexpr Raw(int16_t inValue) : rawValue(inValue) {}
		int16_t rawValue;
	};

	fixed16_t() {}
	constexpr fixed16_t(int inValue) : rawValue((int16_t)(inValue * (1 << FixedFractionalBits))) {}
	constexpr fixed16_t(const fixed8_t& inValue) : rawValue(inValue.rawValue) {}
	constexpr fixed16_t(float inValue) : rawValue(int16_t(inValue * (1 << FixedFractionalBits))) {}
	constexpr fixed16_t(Raw inValue) : rawValue(inValue.rawValue) {}

	constexpr static fixed16_t FromRaw(int16_t inRawValue)
	{
		return fixed16_t(Raw(inRawValue));
	}

	fixed16_t operator -() const
	{
		return FromRaw(-rawValue);
	}
	fixed16_t operator +=(const fixed16_t& other)
	{
		rawValue += other.rawValue;
		return *this;
	}

	explicit operator fixed8_t() const
	{
		return fixed8_t::FromRaw((int8_t)rawValue);
	}

	explicit operator int() const
	{
		return rawValue >> FixedFractionalBits;
	}
};

constexpr fixed16_t operator +(const fixed16_t& lhs, const fixed16_t& rhs) 
{
	return fixed16_t::FromRaw(lhs.rawValue + rhs.rawValue);
}
constexpr fixed16_t operator -(const fixed16_t& lhs, const fixed16_t& rhs) 
{
	return fixed16_t::FromRaw(lhs.rawValue - rhs.rawValue);
}
constexpr fixed16_t operator *(const fixed16_t& lhs, const fixed16_t& rhs) 
{
	return fixed16_t::FromRaw(int16_t((int32_t(lhs.rawValue) * (int32_t)rhs.rawValue) >> FixedFractionalBits));
}
constexpr fixed16_t operator *(const int lhs, const fixed16_t& rhs)
{
	return fixed16_t::FromRaw(lhs * rhs.rawValue);
}
constexpr fixed16_t operator *(const fixed16_t& lhs, const int rhs)
{
	return fixed16_t::FromRaw(lhs.rawValue * rhs);
}
constexpr fixed16_t operator /(const fixed16_t& lhs, const fixed16_t& rhs)
{
	return fixed16_t::FromRaw((int32_t(lhs.rawValue) << FixedFractionalBits) / rhs.rawValue);
}
constexpr fixed16_t operator /(const fixed16_t& lhs, const int rhs)
{
	return fixed16_t::FromRaw(lhs.rawValue / rhs);
}
constexpr bool operator < (const fixed16_t& lhs, const fixed16_t& rhs)
{
	return lhs.rawValue < rhs.rawValue;
}
constexpr bool operator > (const fixed16_t& lhs, const fixed16_t& rhs)
{
	return lhs.rawValue > rhs.rawValue;
}
constexpr bool operator <= (const fixed16_t& lhs, const fixed16_t& rhs)
{
	return lhs.rawValue <= rhs.rawValue;
}
constexpr bool operator >= (const fixed16_t& lhs, const fixed16_t& rhs)
{
	return lhs.rawValue >= rhs.rawValue;
}
constexpr bool operator == (const fixed16_t& lhs, const fixed16_t& rhs)
{
	return lhs.rawValue == rhs.rawValue;
}
constexpr bool operator != (const fixed16_t& lhs, const fixed16_t& rhs)
{
	return lhs.rawValue != rhs.rawValue;
}
#else

template<typename InternalType>
struct FixedBase
{
	InternalType rawValue;

	struct Raw
	{
		constexpr Raw(InternalType inValue) : rawValue(inValue) {}
		InternalType rawValue;
	};

	FixedBase() {}
	constexpr FixedBase(int inValue) : rawValue((InternalType)(inValue * (1 << FixedFractionalBits))) {}
	constexpr FixedBase(const FixedBase& inValue) : rawValue(inValue.rawValue) {}
	constexpr FixedBase(float inValue) : rawValue(InternalType(inValue * (1 << FixedFractionalBits))) {}
	constexpr FixedBase(Raw inValue) : rawValue(inValue.rawValue) {}

	constexpr static FixedBase FromRaw(InternalType inRawValue)
	{
		return FixedBase(Raw(inRawValue));
	}

	FixedBase operator -() const
	{
		return FromRaw(-rawValue);
	}
	FixedBase operator +=(const FixedBase& other)
	{
		rawValue += other.rawValue;
		return *this;
	}

	bool operator < (const FixedBase& rhs) const
	{
		return rawValue < rhs.rawValue;
	}
	bool operator > (const FixedBase& rhs) const
	{
		return rawValue > rhs.rawValue;
	}
	bool operator <= (const FixedBase& rhs) const
	{
		return rawValue <= rhs.rawValue;
	}
	bool operator >= (const FixedBase& rhs) const
	{
		return rawValue >= rhs.rawValue;
	}

	explicit operator int() const
	{
		return rawValue >> FixedFractionalBits;
	}
};

struct fixed8_t : public FixedBase<int8_t>
{
	fixed8_t() : FixedBase() {}
	constexpr fixed8_t (int inValue) : FixedBase(inValue) {}
	constexpr fixed8_t (const FixedBase<int8_t>& inValue) : FixedBase(inValue) {}
	constexpr fixed8_t (float inValue) : FixedBase(inValue) {}
};

struct fixed16_t : public FixedBase<int16_t>
{
	fixed16_t() : FixedBase() {}
	constexpr fixed16_t(int inValue) : FixedBase(inValue) {}
	constexpr fixed16_t(const FixedBase<int16_t>& inValue) : FixedBase(inValue) {}
	constexpr fixed16_t(float inValue) : FixedBase(inValue) {}
	constexpr fixed16_t(const FixedBase<int8_t>& inValue) : FixedBase(Raw(inValue.rawValue)) {}

	explicit operator fixed8_t() const
	{
		return fixed8_t::FromRaw((int8_t)rawValue);
	}
};

template<typename InternalType>
constexpr FixedBase<InternalType> operator +(const FixedBase<InternalType>& lhs, const FixedBase<InternalType>& rhs)
{
	return FixedBase<InternalType>::FromRaw(lhs.rawValue + rhs.rawValue);
}
template<typename InternalType>
constexpr FixedBase<InternalType> operator -(const FixedBase<InternalType>& lhs, const FixedBase<InternalType>& rhs)
{
	return FixedBase<InternalType>::FromRaw(lhs.rawValue - rhs.rawValue);
}
template<typename InternalType>
constexpr FixedBase<InternalType> operator *(const FixedBase<InternalType>& lhs, const FixedBase<InternalType>& rhs)
{
	return FixedBase<InternalType>::FromRaw(int16_t((int32_t(lhs.rawValue) * (int32_t)rhs.rawValue) >> FixedFractionalBits));
}
template<typename InternalType>
constexpr FixedBase<InternalType> operator /(const FixedBase<InternalType>& lhs, const FixedBase<InternalType>& rhs)
{
	return FixedBase<InternalType>::FromRaw((int32_t(lhs.rawValue) << FixedFractionalBits) / rhs.rawValue);
}

#endif

extern const int8_t sinTable[FIXED_ANGLE_MAX] PROGMEM;
extern const int16_t recipTable[256] PROGMEM;

typedef uint8_t angle_t;

struct FixedMath
{
	static fixed8_t Sin(angle_t angle)
	{
		return fixed8_t::FromRaw(pgm_read_byte(&sinTable[angle]));
	}

	static fixed8_t Cos(angle_t angle)
	{
		return fixed8_t::FromRaw(pgm_read_byte(&sinTable[FIXED_ANGLE_WRAP(FIXED_ANGLE_90 - angle)]));
	}

	static inline fixed16_t QuickDivide(fixed16_t x, fixed8_t denom)
	{
		//return x / denom;
		return fixed16_t::FromRaw((int16_t)(( (int32_t)(x.rawValue) * pgm_read_word(&recipTable[(uint8_t)denom.rawValue]) ) >> 4));
	}
};


uint16_t Random();
void SeedRandom(uint16_t seed);

template <typename T>
struct Vector3Base
{
	Vector3Base() {}

	Vector3Base(const T& inX, const T& inY, const T& inZ)
	{
		x = inX;
		y = inY;
		z = inZ;
	}

	Vector3Base& operator += (const Vector3Base& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vector3Base operator + (const Vector3Base& rhs) const
	{
		return Vector3Base(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vector3Base operator - (const Vector3Base& rhs) const
	{
		return Vector3Base(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vector3Base operator * (T scaler) const
	{
		return Vector3Base(x * scaler, y * scaler, z * scaler);
	}

	Vector3Base Cross(const Vector3Base& rhs) const
	{
		Vector3Base result;
		result.x = y * rhs.z - rhs.y * z;
		result.y = z * rhs.x - rhs.z * x;
		result.z = x * rhs.y - rhs.x * y;
		return result;
	}

	void Normalize()
	{
		fixed16_t x16 = x;
		fixed16_t y16 = y;
		fixed16_t z16 = z;
		fixed16_t magnitude = x16 * x16 + y16 * y16 + z16 * z16;

		// Approximate inverse sqrt
		fixed16_t invSqrt = (3 - magnitude) / 2;
		x = T(x16 * invSqrt);
		y = T(y16 * invSqrt);
		z = T(z16 * invSqrt);
	}


	T x, y, z;
};

struct Vector3b : public Vector3Base<fixed8_t>
{
	Vector3b() {}
	Vector3b(const Vector3Base<fixed8_t>& inVector) : Vector3Base(inVector.x, inVector.y, inVector.z) {}
	Vector3b(fixed8_t inX, fixed8_t inY, fixed8_t inZ) : Vector3Base(inX, inY, inZ) 
	{
	}
};

struct Vector3s : public Vector3Base<fixed16_t>
{
	Vector3s() {}
	Vector3s(fixed16_t inX, fixed16_t inY, fixed16_t inZ) : Vector3Base(inX, inY, inZ) 
	{
	}
	Vector3s(const Vector3Base<fixed16_t>& inVector) : Vector3s(inVector.x, inVector.y, inVector.z) {}
	Vector3s(const Vector3b& inVector) : Vector3s(inVector.x, inVector.y, inVector.z) {}
};


struct Matrix3x3
{
	fixed8_t m[3][3];
		
	Matrix3x3()
	{
	}
	
	Vector3b Right() const
	{
		return Vector3b(m[0][0], m[0][1], m[0][2]);
	}
	Vector3b Up() const
	{
		return Vector3b(m[1][0], m[1][1], m[1][2]);
	}
	Vector3b Forward() const
	{
		return Vector3b(m[2][0], m[2][1], m[2][2]);
	}

	void Normalize();
	
	static Matrix3x3 Identity();
	static Matrix3x3 RotateX(angle_t angle);
	static Matrix3x3 RotateY(angle_t angle);
	static Matrix3x3 RotateZ(angle_t angle);
	
	Matrix3x3 Transpose() const;
	
	Vector3b operator * (const Vector3b& v);
	Vector3s operator * (const Vector3s& v);
	Matrix3x3 operator * (const Matrix3x3& other);
};

