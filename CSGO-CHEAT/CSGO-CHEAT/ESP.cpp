#pragma once

#include <iostream>
#include <windows.h>
#include "D:\CSGO-LIZZOWARE\head.h"
#include <cassert>
#include "D:\CSGO-LIZZOWARE\includes.h"
#include <stdint.h>
#include <cstdint>
#include "Qangle.cpp"
#include <algorithm>
#include "D:\CSGO-LIZZOWARE\CSGO-LIZZOWARE\CSGO-LIZZOWARE\csgosdk.h"

#define QAngle Vector
struct model_t;

	typedef void* (__cdecl* pCreateInterface)(const char* name, int* returnCode);


template< class HandleType >
class CBaseIntHandle
{
public:

	inline bool			operator==(const CBaseIntHandle& other) { return m_Handle == other.m_Handle; }
	inline bool			operator!=(const CBaseIntHandle& other) { return m_Handle != other.m_Handle; }

	// Only the code that doles out these handles should use these functions.
	// Everyone else should treat them as a transparent type.
	inline HandleType	GetHandleValue() { return m_Handle; }
	inline void			SetHandleValue(HandleType val) { m_Handle = val; }

	typedef HandleType	HANDLE_TYPE;

protected:

	HandleType	m_Handle;
};
#pragma once
#ifndef BASETYPES_H
#define BASETYPES_H

#ifdef COMPILER_MSVC
#pragma once
#endif

// This is a trick to get the DLL extension off the -D option on the command line.
#define DLLExtTokenPaste(x) #x
#define DLLExtTokenPaste2(x) DLLExtTokenPaste(x)
#define DLL_EXT_STRING DLLExtTokenPaste2( _DLL_EXT )

//////////////////////////////////////////////////////////////////////////

#ifndef schema
#define schema namespace ValveSchemaMarker {}
#endif

#ifdef COMPILING_SCHEMA
#define UNSCHEMATIZED_METHOD( x )
#else
#define UNSCHEMATIZED_METHOD( x ) x
#endif

//////////////////////////////////////////////////////////////////////////


// There's a different version of this file in the xbox codeline
// so the PC version built in the xbox branch includes things like 
// tickrate changes.


// stdio.h
#ifndef NULL
#define NULL 0
#endif


#ifdef POSIX
template<class T>
T abs(const T& a)
{
	if (a < 0)
		return -a;
	else
		return a;
}
#endif


#define ExecuteNTimes( nTimes, x )	\
	{								\
	static int __executeCount=0;\
	if ( __executeCount < nTimes )\
		{							\
			++__executeCount;		\
			x;						\
		}							\
	}


#define ExecuteOnce( x )			ExecuteNTimes( 1, x )



// Pad a number so it lies on an N byte boundary.
// So PAD_NUMBER(0,4) is 0 and PAD_NUMBER(1,4) is 4
#define PAD_NUMBER(number, boundary) \
	( ((number) + ((boundary)-1)) / (boundary) ) * (boundary)

// In case this ever changes
#ifndef M_PI
#define M_PI			3.14159265358979323846
#endif


// #define COMPILETIME_MAX and COMPILETIME_MIN for max/min in constant expressions
#define COMPILETIME_MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#define COMPILETIME_MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#ifndef MIN
#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif

#ifndef MAX
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif



#if !defined(_X360)
FORCEINLINE float fpmin(float a, float b)
{
	return (a < b) ? a : b;
}

FORCEINLINE float fpmax(float a, float b)
{
	return (a > b) ? a : b;
}
#endif

#ifdef __cplusplus

template< class T, class Y >
inline T clamp(T const& val, Y const& minVal, Y const& maxVal)
{
	if (val < minVal)
		return minVal;
	else if (val > maxVal)
		return maxVal;
	else
		return val;
}

#else

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

#endif

#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif

//-----------------------------------------------------------------------------
// fsel
//-----------------------------------------------------------------------------
#ifndef _X360

#define fsel(c,x,y) ( (c) >= 0 ? (x) : (y) )

// integer conditional move
// if a >= 0, return x, else y
#define isel(a,x,y) ( ((a) >= 0) ? (x) : (y) )

// if x = y, return a, else b
#define ieqsel(x,y,a,b) (( (x) == (y) ) ? (a) : (b))

// if the nth bit of a is set (counting with 0 = LSB),
// return x, else y
// this is fast if nbit is a compile-time immediate 
#define ibitsel(a, nbit, x, y) ( ( ((a) & (1 << (nbit))) != 0 ) ? (x) : (y) )

#else

// __fsel(double fComparand, double fValGE, double fLT) == fComparand >= 0 ? fValGE : fLT
// this is much faster than if ( aFloat > 0 ) { x = .. }
// the XDK defines two intrinsics, one for floats and one for doubles -- it's the same
// opcode, but the __fself version tells the compiler not to do a wasteful unnecessary
// rounding op after each sel.
// #define fsel __fsel
FORCEINLINE double fsel(double fComparand, double fValGE, double fLT) { return __fsel(fComparand, fValGE, fLT); }
FORCEINLINE float fsel(float fComparand, float fValGE, float fLT) { return __fself(fComparand, fValGE, fLT); }

// if a >= 0, return x, else y
FORCEINLINE int isel(int a, int x, int y)
{
	int mask = a >> 31; // arithmetic shift right, splat out the sign bit
	return x + ((y - x) & mask);
};

// if a >= 0, return x, else y
FORCEINLINE unsigned isel(int a, unsigned x, unsigned y)
{
	int mask = a >> 31; // arithmetic shift right, splat out the sign bit
	return x + ((y - x) & mask);
};

// ( x == y ) ? a : b
FORCEINLINE unsigned ieqsel(unsigned x, unsigned y, unsigned a, unsigned b)
{
	unsigned mask = (x == y) ? 0 : -1;
	return a + ((b - a) & mask);
};

// ( x == y ) ? a : b
FORCEINLINE int ieqsel(int x, int y, int a, int b)
{
	int mask = (x == y) ? 0 : -1;
	return a + ((b - a) & mask);
};

// if the nth bit of a is set (counting with 0 = LSB),
// return x, else y
// this is fast if nbit is a compile-time immediate 
#define ibitsel(a, nbit, x, y) ( (x) + (((y) - (x)) & (((a) & (1 << (nbit))) ? 0 : -1)) )

#endif

#if CROSS_PLATFORM_VERSION < 1

#ifndef DONT_DEFINE_BOOL // Needed for Cocoa stuff to compile.
typedef int BOOL;
#endif

typedef int qboolean;
//typedef uint32 ULONG;
typedef uint8_t BYTE;
typedef uint8_t byte;
typedef uint16_t word;
#endif

#ifdef _WIN32
typedef wchar_t ucs2; // under windows wchar_t is ucs2
struct Vector { float x, y, z; };
#else
typedef unsigned short ucs2;
#endif
enum ThreeState_t
{
	TRS_FALSE,
	TRS_TRUE,
	TRS_NONE,
};

typedef float vec_t;
#ifdef _WIN32
typedef __int32 vec_t_as_gpr; // a general purpose register type equal in size to a vec_t (in case we have to avoid the fpu for some reason)
#endif


template <typename T>
inline T AlignValue(T val, int alignment)
{
	return (T)(((int)val + alignment - 1) & ~(alignment - 1));
}


// FIXME: this should move 
#ifndef __cplusplus
#define true TRUE
#define false FALSE
#endif

//-----------------------------------------------------------------------------
// look for NANs, infinities, and underflows. 
// This assumes the ANSI/IEEE 754-1985 standard
//-----------------------------------------------------------------------------

#ifdef __cplusplus

inline uint32_t& FloatBits(vec_t& f)
{
	return *reinterpret_cast<uint32_t*>((char*)(&f));
}

inline uint32_t const FloatBits(const vec_t& f)
{
	union Convertor_t
	{
		vec_t f;
		uint32_t ul;
	}tmp;
	tmp.f = f;
	return tmp.ul;
}

inline vec_t BitsToFloat(uint32_t i)
{
	union Convertor_t
	{
		vec_t f;
		unsigned long ul;
	}tmp;
	tmp.ul = i;
	return tmp.f;
}

inline bool IsFinite(const vec_t& f)
{
#if _X360
	return f == f && fabs(f) <= FLT_MAX;
#else
	return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
#endif
}

inline uint32_t FloatAbsBits(vec_t f)
{
	return FloatBits(f) & 0x7FFFFFFF;
}

inline float FloatMakeNegative(vec_t f)
{
	return BitsToFloat(FloatBits(f) | 0x80000000);
}

#if defined( WIN32 )

//#include <math.h>
// Just use prototype from math.h
#ifdef __cplusplus
extern "C"
{
#endif
	double __cdecl fabs(double);
#ifdef __cplusplus
}
#endif

// In win32 try to use the intrinsic fabs so the optimizer can do it's thing inline in the code
#pragma intrinsic( fabs )
// Also, alias float make positive to use fabs, too
// NOTE:  Is there a perf issue with double<->float conversion?
inline float FloatMakePositive(vec_t f)
{
	return (float)fabs(f);
}
#else
inline float FloatMakePositive(vec_t f)
{
	return BitsToFloat(FloatBits(f) & 0x7FFFFFFF);
}
#endif

inline float FloatNegate(vec_t f)
{
	return BitsToFloat(FloatBits(f) ^ 0x80000000);
}


#define FLOAT32_NAN_BITS     (uint32)0x7FC00000	// not a number!
#define FLOAT32_NAN          BitsToFloat( FLOAT32_NAN_BITS )

#define VEC_T_NAN FLOAT32_NAN

#endif

// FIXME: why are these here?  Hardly anyone actually needs them.
struct color24
{
	BYTE r, g, b;
};

typedef struct color32_s
{
	bool operator!=(const struct color32_s& other) const;
	BYTE r, g, b, a;

	// assign and copy by using the whole register rather
	// than byte-by-byte copy. (No, the compiler is not
	// smart enough to do this for you. /FAcs if you 
	// don't believe me.)
	inline unsigned* asInt(void) { return reinterpret_cast<unsigned*>(this); }
	inline const unsigned* asInt(void) const { return reinterpret_cast<const unsigned*>(this); }
	// This thing is in a union elsewhere, and union members can't have assignment
	// operators, so you have to explicitly assign using this, or be slow. SUCK.
	inline void Copy(const color32_s& rhs)
	{
		*asInt() = *rhs.asInt();
	}

} color32;

inline bool color32::operator!=(const color32& other) const
{
	return r != other.r || g != other.g || b != other.b || a != other.a;
}

struct colorVec
{
	unsigned r, g, b, a;
};


#ifndef NOTE_UNUSED
#define NOTE_UNUSED(x)	(x = x)	// for pesky compiler / lint warnings
#endif
#ifdef __cplusplus

struct vrect_t
{
	int				x, y, width, height;
	vrect_t* pnext;
};

#endif


//-----------------------------------------------------------------------------
// MaterialRect_t struct - used for DrawDebugText
//-----------------------------------------------------------------------------
struct Rect_t
{
	int x, y;
	int width, height;
};

struct Rect3D_t
{
	int x, y, z;
	int width, height, depth;

	FORCEINLINE Rect3D_t(int nX, int nY, int nZ, int nWidth, int nHeight, int nDepth)
	{
		x = nX;
		y = nY;
		z = nZ;
		width = nWidth;
		height = nHeight;
		depth = nDepth;
	}

	FORCEINLINE Rect3D_t(void)
	{
	}

};



//-----------------------------------------------------------------------------
// Interval, used by soundemittersystem + the game
//-----------------------------------------------------------------------------
struct interval_t
{
	float start;
	float range;
};


//-----------------------------------------------------------------------------
// Declares a type-safe handle type; you can't assign one handle to the next
//-----------------------------------------------------------------------------

// 32-bit pointer handles.

// Typesafe 8-bit and 16-bit handles.
template< class DummyType >
class CIntHandle16 : public CBaseIntHandle< unsigned short >
{
public:
	inline			CIntHandle16() {}

	static inline	CIntHandle16<DummyType> MakeHandle(HANDLE_TYPE val)
	{
		return CIntHandle16<DummyType>(val);
	}

protected:
	inline			CIntHandle16(HANDLE_TYPE val)
	{
		m_Handle = val;
	}
};

// NOTE: This macro is the same as windows uses; so don't change the guts of it
#define DECLARE_HANDLE_16BIT(name)	typedef CIntHandle16< struct name##__handle * > name;
#define DECLARE_HANDLE_32BIT(name)	typedef CIntHandle32< struct name##__handle * > name;

#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#define FORWARD_DECLARE_HANDLE(name) typedef struct name##__ *name

#define DECLARE_DERIVED_POINTER_HANDLE( _name, _basehandle ) struct _name##__ : public _basehandle##__ {}; typedef struct _name##__ *_name
#define DECLARE_ALIASED_POINTER_HANDLE( _name, _alias ) typedef struct _alias##__ *name

// @TODO: Find a better home for this
#if !defined(_STATIC_LINKED) && !defined(PUBLISH_DLL_SUBSYSTEM)
// for platforms built with dynamic linking, the dll interface does not need spoofing
#define PUBLISH_DLL_SUBSYSTEM()
#endif

#define UID_PREFIX generated_id_
#define UID_CAT1(a,c) a ## c
#define UID_CAT2(a,c) UID_CAT1(a,c)
#define EXPAND_CONCAT(a,c) UID_CAT1(a,c)
#ifdef _MSC_VER
#define UNIQUE_ID UID_CAT2(UID_PREFIX,__COUNTER__)
#else
#define UNIQUE_ID UID_CAT2(UID_PREFIX,__LINE__)
#endif

#define _MKSTRING(arg) #arg
#define MKSTRING(arg) _MKSTRING(arg)


// this allows enumerations to be used as flags, and still remain type-safe!
#define DEFINE_ENUM_BITWISE_OPERATORS( Type ) \
	inline Type  operator|  ( Type  a, Type b ) { return Type( int( a ) | int( b ) ); } \
	inline Type  operator&  ( Type  a, Type b ) { return Type( int( a ) & int( b ) ); } \
	inline Type  operator^  ( Type  a, Type b ) { return Type( int( a ) ^ int( b ) ); } \
	inline Type  operator<< ( Type  a, int  b ) { return Type( int( a ) << b ); } \
	inline Type  operator>> ( Type  a, int  b ) { return Type( int( a ) >> b ); } \
	inline Type &operator|= ( Type &a, Type b ) { return a = a |  b; } \
	inline Type &operator&= ( Type &a, Type b ) { return a = a &  b; } \
	inline Type &operator^= ( Type &a, Type b ) { return a = a ^  b; } \
	inline Type &operator<<=( Type &a, int  b ) { return a = a << b; } \
	inline Type &operator>>=( Type &a, int  b ) { return a = a >> b; } \
	inline Type  operator~( Type a ) { return Type( ~int( a ) ); }

// defines increment/decrement operators for enums for easy iteration
#define DEFINE_ENUM_INCREMENT_OPERATORS( Type ) \
	inline Type &operator++( Type &a      ) { return a = Type( int( a ) + 1 ); } \
	inline Type &operator--( Type &a      ) { return a = Type( int( a ) - 1 ); } \
	inline Type  operator++( Type &a, int ) { Type t = a; ++a; return t; } \
	inline Type  operator--( Type &a, int ) { Type t = a; --a; return t; }

#define MAX_SPLITSCREEN_CLIENT_BITS 2
// this should == MAX_JOYSTICKS in InputEnums.h
#define MAX_SPLITSCREEN_CLIENTS	( 1 << MAX_SPLITSCREEN_CLIENT_BITS ) // 4



#endif // BASETYPES_H
class Quaternion;
struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct mstudiobodyparts_t;
struct mstudiotexture_t;
template <typename Fn> __forceinline Fn GetVirtualFunction(void* pClassBase, int nFunctionIndex)
{
	return (Fn)((PDWORD) * (PDWORD*)pClassBase)[nFunctionIndex];
}
class IMaterialVar;
typedef uint64_t VertexFormat_t;
#ifndef COLOR_H
#define COLOR_H

#ifdef _WIN32
#pragma once
#endif


//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
class Color
{
public:
	// constructors
	Color()
	{
		*((int*)this) = 0;
	}
	Color(int _r, int _g, int _b)
	{
		SetColor(_r, _g, _b, 0);
	}
	Color(int _r, int _g, int _b, int _a)
	{
		SetColor(_r, _g, _b, _a);
	}

	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	void SetColor(int _r, int _g, int _b, int _a = 0)
	{
		_color[0] = (unsigned char)_r;
		_color[1] = (unsigned char)_g;
		_color[2] = (unsigned char)_b;
		_color[3] = (unsigned char)_a;
	}

	void GetColor(int& _r, int& _g, int& _b, int& _a) const
	{
		_r = _color[0];
		_g = _color[1];
		_b = _color[2];
		_a = _color[3];
	}

	void SetRawColor(int color32)
	{
		*((int*)this) = color32;
	}

	int GetRawColor() const
	{
		return *((int*)this);
	}

	inline int r() const { return _color[0]; }
	inline int g() const { return _color[1]; }
	inline int b() const { return _color[2]; }
	inline int a() const { return _color[3]; }

	unsigned char& operator[](int index)
	{
		return _color[index];
	}

	const unsigned char& operator[](int index) const
	{
		return _color[index];
	}

	bool operator == (const Color& rhs) const
	{
		return (*((int*)this) == *((int*)&rhs));
	}

	bool operator != (const Color& rhs) const
	{
		return !(operator==(rhs));
	}

	Color& operator=(const Color& rhs)
	{
		SetRawColor(rhs.GetRawColor());
		return *this;
	}

	Color& operator=(const color32& rhs)
	{
		_color[0] = rhs.r;
		_color[1] = rhs.g;
		_color[2] = rhs.b;
		_color[3] = rhs.a;
		return *this;
	}

	color32 ToColor32() const
	{
		color32 newColor;
		newColor.r = _color[0];
		newColor.g = _color[1];
		newColor.b = _color[2];
		newColor.a = _color[3];
		return newColor;
	}

private:
	unsigned char _color[4];
};


#endif // COLOR_H
enum MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),

	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	//	MATERIAL_VAR_UNUSED					  = (1 << 9),
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
	//	MATERIAL_VAR_UNUSED					  = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
	MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
	MATERIAL_VAR_VERTEXFOG = (1 << 31),

	// NOTE: Only add flags here that either should be read from
	// .vmts or can be set directly from client code. Other, internal
	// flags should to into the flag enum in IMaterialInternal.h
};

enum PreviewImageRetVal_t
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};

enum ImageFormat {
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,            // Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,    // NOTE: D3D9 does not have this format
	IMAGE_FORMAT_RGBA32323232F,
	IMAGE_FORMAT_RG1616F,
	IMAGE_FORMAT_RG3232F,
	IMAGE_FORMAT_RGBX8888,

	IMAGE_FORMAT_NULL,            // Dummy format which takes no video memory

	// Compressed normal map formats
	IMAGE_FORMAT_ATI2N,            // One-surface ATI2N / DXN format
	IMAGE_FORMAT_ATI1N,            // Two-surface ATI1N format

	IMAGE_FORMAT_RGBA1010102,    // 10 bit-per component render targets
	IMAGE_FORMAT_BGRA1010102,
	IMAGE_FORMAT_R16F,            // 16 bit FP format

	// Depth-stencil texture formats
	IMAGE_FORMAT_D16,
	IMAGE_FORMAT_D15S1,
	IMAGE_FORMAT_D32,
	IMAGE_FORMAT_D24S8,
	IMAGE_FORMAT_LINEAR_D24S8,
	IMAGE_FORMAT_D24X8,
	IMAGE_FORMAT_D24X4S4,
	IMAGE_FORMAT_D24FS8,
	IMAGE_FORMAT_D16_SHADOW,    // Specific formats for shadow mapping
	IMAGE_FORMAT_D24X8_SHADOW,    // Specific formats for shadow mapping

	// supporting these specific formats as non-tiled for procedural cpu access (360-specific)
	IMAGE_FORMAT_LINEAR_BGRX8888,
	IMAGE_FORMAT_LINEAR_RGBA8888,
	IMAGE_FORMAT_LINEAR_ABGR8888,
	IMAGE_FORMAT_LINEAR_ARGB8888,
	IMAGE_FORMAT_LINEAR_BGRA8888,
	IMAGE_FORMAT_LINEAR_RGB888,
	IMAGE_FORMAT_LINEAR_BGR888,
	IMAGE_FORMAT_LINEAR_BGRX5551,
	IMAGE_FORMAT_LINEAR_I8,
	IMAGE_FORMAT_LINEAR_RGBA16161616,

	IMAGE_FORMAT_LE_BGRX8888,
	IMAGE_FORMAT_LE_BGRA8888,

	NUM_IMAGE_FORMATS
};

enum MaterialPropertyTypes_t
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0,					// bool
	MATERIAL_PROPERTY_OPACITY,								// int (enum MaterialPropertyOpacityTypes_t)
	MATERIAL_PROPERTY_REFLECTIVITY,							// vec3_t
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS				// bool
};
struct matrix3x4_t
{
	matrix3x4_t() = default;
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_matrix[0][0] = m00;
		m_matrix[0][1] = m01;
		m_matrix[0][2] = m02;
		m_matrix[0][3] = m03;
		m_matrix[1][0] = m10;
		m_matrix[1][1] = m11;
		m_matrix[1][2] = m12;
		m_matrix[1][3] = m13;
		m_matrix[2][0] = m20;
		m_matrix[2][1] = m21;
		m_matrix[2][2] = m22;
		m_matrix[2][3] = m23;
	}
	matrix3x4_t(const vec3& x_axis, const vec3& y_axis, const vec3& z_axis, const vec3& vec_origin)
	{
		init(x_axis, y_axis, z_axis, vec_origin);
	}

	void init(const vec3& x_axis, const vec3& y_axis, const vec3& z_axis, const vec3& vec_origin)
	{
		m_matrix[0][0] = x_axis.x;
		m_matrix[0][1] = y_axis.x;
		m_matrix[0][2] = z_axis.x;
		m_matrix[0][3] = vec_origin.x;
		m_matrix[1][0] = x_axis.y;
		m_matrix[1][1] = y_axis.y;
		m_matrix[1][2] = z_axis.y;
		m_matrix[1][3] = vec_origin.y;
		m_matrix[2][0] = x_axis.z;
		m_matrix[2][1] = y_axis.z;
		m_matrix[2][2] = z_axis.z;
		m_matrix[2][3] = vec_origin.z;
	}

	float* operator[](int i)
	{
		return m_matrix[i];
	}

	const float* operator[](int i) const
	{
		return m_matrix[i];
	}

	float m_matrix[3][4];
};
class RadianEuler
{
public:
	inline RadianEuler(void) { }

	inline RadianEuler(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	inline RadianEuler(Quaternion const& q);    // evil auto type promotion!!!
	inline RadianEuler(QAngle const& angles);    // evil auto type promotion!!!

	// Initialization
	inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
	{
		x = ix;
		y = iy;
		z = iz;
	}

	//	conversion to qangle
	QAngle ToQAngle(void) const;

	bool IsValid() const;

	void Invalidate();

	inline float* Base()
	{
		return &x;
	}

	inline const float* Base() const
	{
		return &x;
	}

	// array access...
	float operator[](int i) const;

	float& operator[](int i);

	float x, y, z;
};

class Quaternion				// same data-layout as engine's vec4_t,
{                                //		which is a float[4]
public:
	inline Quaternion(void) { }

	inline Quaternion(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw) { }

	inline Quaternion(RadianEuler const& angle);    // evil auto type promotion!!!

	inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f)
	{
		x = ix;
		y = iy;
		z = iz;
		w = iw;
	}

	bool IsValid() const;

	void Invalidate();

	bool operator==(const Quaternion& src) const;

	bool operator!=(const Quaternion& src) const;

	float* Base()
	{
		return (float*)this;
	}

	const float* Base() const
	{
		return (float*)this;
	}

	// array access...
	float operator[](int i) const;

	float& operator[](int i);

	float x, y, z, w;
};

struct mstudiobone_t {
	int sznameindex;

	inline char* const pszName(void) const
	{
		return ((char*)this) + sznameindex;
	}

	int parent;        // parent bone
	int bonecontroller[6];    // bone controller index, -1 == none

	// default values
	Vector pos;
	Quaternion quat;
	RadianEuler rot;
	// compression scale
	Vector posscale;
	Vector rotscale;

	matrix3x4_t poseToBone;
	Quaternion qAlignment;
	int flags;
	int proctype;
	int procindex;        // procedural rule
	mutable int physicsbone;    // index into physically simulated bone

	inline void* pProcedure() const
	{
		if (procindex == 0)
			return NULL;
		else
			return (void*)(((unsigned char*)this) + procindex);
	};

	int surfacepropidx;    // index into string tablefor property name

	inline char* const pszSurfaceProp(void) const
	{
		return ((char*)this) + surfacepropidx;
	}

	inline int GetSurfaceProp(void) const
	{
		return surfacepropLookup;
	}

	int contents;        // See BSPFlags.h for the contents flags
	int surfacepropLookup;    // this index must be cached by the loader, not saved in the file
	int unused[7];        // remove as appropriate
};

struct mstudiobbox_t {
	int bone;
	int group;
	Vector bbmin;
	Vector bbmax;
	int hitboxnameindex;
	int pad[3];
	float radius;
	int pad2[4];

	char* pszHitboxName()
	{
		if (hitboxnameindex == 0)
			return NULL;

		return ((char*)this) + hitboxnameindex;
	}
};

struct mstudiohitboxset_t {
	int sznameindex;

	inline char* const pszName() const
	{
		return ((char*)this) + sznameindex;
	}

	int numhitboxes;
	int hitboxindex;

	inline mstudiobbox_t* pHitbox(int i) const
	{
		return (mstudiobbox_t*)(((unsigned char*)this) + hitboxindex) + i;
	};
};

struct studiohdr_t {
	int id;
	int version;
	int checksum;        // this has to be the same in the phy and vtx files to load!
	char name[64];
	int length;

	Vector eyeposition;    // ideal eye position
	Vector illumposition;    // illumination center
	Vector hull_min;        // ideal movement hull size
	Vector hull_max;
	Vector view_bbmin;        // clipping bounding box
	Vector view_bbmax;

	int flags;
	int numbones;            // bones
	int boneindex;
	inline mstudiobone_t* pBone(int i) const
	{
		assert(i >= 0 && i < numbones);
		return (mstudiobone_t*)(((unsigned char*)this) + boneindex) + i;
	};

	int RemapSeqBone(int iSequence, int iLocalBone) const;    // maps local sequence bone to global bone
	int RemapAnimBone(int iAnim, int iLocalBone) const;        // maps local animations bone to global bone
	int numbonecontrollers;        // bone controllers
	int bonecontrollerindex;
	int numhitboxsets;
	int hitboxsetindex;

	// Look up hitbox set by index
	mstudiohitboxset_t* pHitboxSet(int i) const
	{
		(i >= 0 && i < numhitboxsets);
		return (mstudiohitboxset_t*)(((unsigned char*)this) + hitboxsetindex) + i;
	};

	// Calls through to hitbox to determine size of specified set
	inline mstudiobbox_t* pHitbox(int i, int set) const
	{
		mstudiohitboxset_t const* s = pHitboxSet(set);
		if (!s)
			return NULL;

		return s->pHitbox(i);
	};

	// Calls through to set to get hitbox count for set
	inline int iHitboxCount(int set) const
	{
		mstudiohitboxset_t const* s = pHitboxSet(set);
		if (!s)
			return 0;

		return s->numhitboxes;
	};

	// file local animations? and sequences
	//private:
	int numlocalanim;            // animations/poses
	int localanimindex;        // animation descriptions
	int numlocalseq;                // sequences
	int localseqindex;

	//public:
	bool SequencesAvailable() const;

	int GetNumSeq() const;

	int iRelativeAnim(int baseseq, int relanim) const;    // maps seq local anim reference to global anim index
	int iRelativeSeq(int baseseq, int relseq) const;        // maps seq local seq reference to global seq index

	//private:
	mutable int activitylistversion;    // initialization flag - have the sequences been indexed?
	mutable int eventsindexed;

	//public:
	int GetSequenceActivity(int iSequence);

	void SetSequenceActivity(int iSequence, int iActivity);

	int GetActivityListVersion();

	void SetActivityListVersion(int version) const;

	int GetEventListVersion();

	void SetEventListVersion(int version);

	// raw textures
	int numtextures;
	int textureindex;

	// raw textures search paths
	int numcdtextures;
	int cdtextureindex;

	inline char* pCdtexture(int i) const
	{
		return (((char*)this) + *((int*)(((unsigned char*)this) + cdtextureindex) + i));
	};

	// replaceable textures tables
	int numskinref;
	int numskinfamilies;
	int skinindex;

	inline short* pSkinref(int i) const
	{
		return (short*)(((unsigned char*)this) + skinindex) + i;
	};
	int numbodyparts;
	int bodypartindex;

	// queryable attachable points
	//private:
	int numlocalattachments;
	int localattachmentindex;

	//public:
	int GetNumAttachments() const;

	int GetAttachmentBone(int i);

	// used on my tools in hlmv, not persistant
	void SetAttachmentBone(int iAttachment, int iBone);

	// animation node to animation node transition graph
	//private:
	int numlocalnodes;
	int localnodeindex;
	int localnodenameindex;

	inline char* pszLocalNodeName(int iNode) const
	{
		(iNode >= 0 && iNode < numlocalnodes);
		return (((char*)this) + *((int*)(((unsigned char*)this) + localnodenameindex) + iNode));
	}

	inline unsigned char* pLocalTransition(int i) const
	{
		(i >= 0 && i < (numlocalnodes* numlocalnodes));
		return (unsigned char*)(((unsigned char*)this) + localnodeindex) + i;
	};

	//public:
	int EntryNode(int iSequence);

	int ExitNode(int iSequence);

	char* pszNodeName(int iNode);

	int GetTransition(int iFrom, int iTo) const;

	int numflexdesc;
	int flexdescindex;
	int numflexcontrollers;
	int flexcontrollerindex;
	int numflexrules;
	int flexruleindex;
	int numikchains;
	int ikchainindex;
	int nummouths;
	int mouthindex;

	//private:
	int numlocalposeparameters;
	int localposeparamindex;

	//public:
	int GetNumPoseParameters() const;

	int GetSharedPoseParameter(int iSequence, int iLocalPose) const;

	int surfacepropindex;

	inline char* const pszSurfaceProp() const
	{
		return ((char*)this) + surfacepropindex;
	}

	// Key values
	int keyvalueindex;
	int keyvaluesize;

	inline const char* KeyValueText() const
	{
		return keyvaluesize != 0 ? ((char*)this) + keyvalueindex : NULL;
	}

	int numlocalikautoplaylocks;
	int localikautoplaylockindex;

	int GetNumIKAutoplayLocks() const;

	int CountAutoplaySequences() const;

	int CopyAutoplaySequences(unsigned short* pOut, int outCount) const;

	int GetAutoplayList(unsigned short** pOut) const;

	// The collision model mass that jay wanted
	float mass;
	int contents;

	// external animations, models, etc.
	int numincludemodels;
	int includemodelindex;

	// implementation specific call to get a named model
	const studiohdr_t* FindModel(void** cache, char const* modelname) const;

	// implementation specific back pointer to virtual data
	mutable void* virtualModel;
	//virtualmodel_t		GetVirtualModel() const;

	// for demand loaded animation blocks
	int szanimblocknameindex;

	inline char* const pszAnimBlockName() const
	{
		return ((char*)this) + szanimblocknameindex;
	}

	int numanimblocks;
	int animblockindex;
	mutable void* animblockModel;

	unsigned char* GetAnimBlock(int i) const;

	int bonetablebynameindex;

	inline const unsigned char* GetBoneTableSortedByName() const
	{
		return (unsigned char*)this + bonetablebynameindex;
	}

	// used by tools only that don't cache, but persist mdl's peer data
	// engine uses virtualModel to back link to cache pointers
	void* pVertexBase;
	void* pIndexBase;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting
	// on static props
	unsigned char constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	unsigned char rootLOD;

	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	unsigned char numAllowedRootLODs;
	unsigned char unused[1];
	int unused4; // zero out if version < 47
	int numflexcontrollerui;
	int flexcontrolleruiindex;
	int unused3[2];

	// FIXME: Remove when we up the model version. Move all fields of studiohdr2_t into studiohdr_t.
	int studiohdr2index;

	// NOTE: No room to add stuff? Up the .mdl file format version
	// [and move all fields in studiohdr2_t into studiohdr_t and kill studiohdr2_t],
	// or add your stuff to studiohdr2_t. See NumSrcBoneTransforms/SrcBoneTransform for the pattern to use.
	int unused2[1];

	studiohdr_t() { }
private:
	// No copy constructors allowed
	studiohdr_t(const studiohdr_t& vOther);

	friend struct virtualmodel_t;
};
class IMaterial
{
public:
	// Get the name of the material.  This is a full path to
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	virtual const char* GetName() const = 0;
	virtual const char* GetTextureGroupName() const = 0;

	// Get the preferred size/bitDepth of a preview image of a material.
	// This is the sort of image that you would use for a thumbnail view
	// of a material, or in WorldCraft until it uses materials to render.
	// separate this for the tools maybe
	int placeholder(int* width, int* height, ImageFormat* imageFormat, bool* isTranslucent);

	// Get a preview image at the specified width/height and bitDepth.
	// Will do resampling if necessary.(not yet!!! :) )
	// Will do color format conversion. (works now.)

	int getpreviewimageplaceholder(unsigned char* data, int width, int height, ImageFormat imageFormat);
	//
	virtual int get_mapping_width() = 0;
	virtual int get_mapping_height() = 0;

	virtual int get_num_animation_frames() = 0;

	// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
	virtual bool in_material_page(void) = 0;
	virtual void get_material_offset(float* pOffset) = 0;
	virtual void get_material_scale(float* pScale) = 0;
	virtual IMaterial* get_material_page(void) = 0;

	// find a vmt variable.
	// This is how game code affects how a material is rendered.
	// The game code must know about the params that are used by
	// the shader for the material that it is trying to affect.
	virtual IMaterialVar* find_var(const char* varName, bool* found, bool complain = true) = 0;

	// The user never allocates or deallocates materials.  Reference counting is
	// used instead.  Garbage collection is done upon a call to
	// i_material_system::UncacheUnusedMaterials.
	virtual void increment_reference_count(void) = 0;
	virtual void decrement_reference_count(void) = 0;

	inline void add_ref()
	{
		increment_reference_count();
	}

	inline void release()
	{
		decrement_reference_count();
	}

	// Each material is assigned a number that groups it with like materials
	// for sorting in the application.
	virtual int get_enumeration_id(void) const = 0;

	virtual void get_low_res_color_sample(float s, float t, float* color) const = 0;

	// This computes the state snapshots for this material
	virtual void recompute_state_snapshots() = 0;

	// Are we translucent?
	virtual bool is_translucent() = 0;

	// Are we alphatested?
	virtual bool is_alpha_tested() = 0;

	// Are we vertex lit?
	virtual bool is_vertex_lit() = 0;

	// Gets the vertex format
	virtual void get_vertex_format() const = 0;

	// returns true if this material uses a material proxy
	virtual bool has_proxy(void) const = 0;

	virtual bool uses_env_cubemap(void) = 0;

	virtual bool needs_tangent_space(void) = 0;

	virtual bool needs_power_of_two_frame_buffer_texture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool needs_full_frame_buffer_texture(bool bCheckSpecificToThisFrame = true) = 0;

	// returns true if the shader doesn't do skinning itself and requires
	// the data that is sent to it to be preskinned.
	virtual bool needs_software_skinning(void) = 0;

	// Apply constant color or alpha modulation
	virtual void alpha_modulate(float alpha) = 0;
	virtual void color_modulate(float r, float g, float b) = 0;

	// Material Var flags...
	virtual void set_matrial_var_flag(MaterialVarFlags_t flag, bool on) = 0; // Not used, index outdated, see below
	virtual bool GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;

	// Gets material reflectivity
	virtual void get_reflectivity(vec3& reflect) = 0;

	// Gets material property flags
	virtual bool get_property_flag(int MaterialPropertyTypes_t) = 0;

	// Is the material visible from both sides?
	virtual bool is_two_sided() = 0;

	// Sets the shader associated with the material
	virtual void set_shader(const char* pShaderName) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int get_num_passes(void) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int get_texture_memory_bytes(void) = 0;

	// Meant to be used with materials created using CreateMaterial
	// It updates the materials to reflect the current values stored in the material vars
	virtual void refresh() = 0;

	// GR - returns true is material uses lightmap alpha for blending
	virtual bool needs_lightmap_blend_alpha(void) = 0;

	// returns true if the shader doesn't do lighting itself and requires
	// the data that is sent to it to be prelighted
	virtual bool needs_software_lighting(void) = 0;

	// Gets at the shader parameters
	virtual int shader_param_count() const = 0;
	virtual IMaterialVar** get_shader_params(void) = 0;

	// Returns true if this is the error material you get back from i_material_system::FindMaterial if
	// the material can't be found.
	virtual bool IsErrorMaterial() const = 0;

	virtual void Unused() = 0;

	// Gets the current alpha modulation
	virtual float GetAlphaModulation() = 0;
	virtual void GetColorModulation(float* r, float* g, float* b) = 0;

	// Is this translucent given a particular alpha modulation?
	virtual bool is_translucent_under_modulation(float fAlphaModulation = 1.0f) const = 0;

	// fast find that stores the index of the found var in the string table in local cache
	virtual IMaterialVar* find_var_fast(char const* pVarName, unsigned int* pToken) = 0;

	// Sets new VMT shader parameters for the material
	virtual void set_shader_and_params(void* pKeyValues) = 0;
	virtual const char* get_shader_name() const = 0;

	virtual void delete_if_unreferenced() = 0;

	virtual bool is_sprite_card() = 0;

	virtual void call_bind_proxy(void* proxyData) = 0;

	virtual void refresh_preserving_material_vars() = 0;

	virtual bool was_reloaded_from_whitelist() = 0;

	virtual bool set_temp_excluded(bool bSet, int nExcludedDimensionLimit) = 0;

	virtual int get_reference_count() const = 0;

	void AlphaModulate(float alpha)
	{
		return GetVirtualFunction< void(__thiscall*)(decltype(this), float) >(this, 27)(this, alpha);
	}

	void SetMaterialVarFlag(MaterialVarFlags_t flag, bool on)
	{
		return GetVirtualFunction< void(__thiscall*)(decltype(this), MaterialVarFlags_t, bool) >(this, 29)(this, flag, on);
	}

	void ColorModulate(const float color[3])
	{
		return GetVirtualFunction< void(__thiscall*)(decltype(this), float, float, float)>(this, 28)(this, color[0], color[1], color[2]);
	}
};
class IVModelInfo
{
public:
	model_t* GetModel(int index)
	{
		typedef model_t* (*oGetModel)(void*, int);
		return GetVirtualFunction<oGetModel>(this, 2)(this, index);
	}

	int GetModelIndex(const char* Filename)
	{
		typedef int (*oGetModelIndex)(void*, const char*);
		return GetVirtualFunction<oGetModelIndex>(this, 3)(this, Filename);
	}

	const char* GetModelName(const model_t* model)
	{
		typedef const char* (*oGetModelName)(void*, const model_t*);
		return GetVirtualFunction<oGetModelName>(this, 4)(this, model);
	}

	void GetModelMaterials(const model_t* model, int count, IMaterial** ppMaterial)
	{
		typedef studiohdr_t* (*oGetModelMaterials)(void*, const model_t*, int, IMaterial**);
		GetVirtualFunction<oGetModelMaterials>(this, 18)(this, model, count, ppMaterial);
	}

	studiohdr_t* GetStudioModel(const model_t* model)
	{
		typedef studiohdr_t* (*oGetStudioModel)(void*, const model_t*);
		return GetVirtualFunction<oGetStudioModel>(this, 31)(this, model);
	}
};
class IClientRenderable;
struct ModelRenderInfo_t
{
    Vector origin;
    QAngle angles;
    char   pad[0x4];
    void* pRenderable;
    const model_t* pModel;
    const void* pModelToWorld;
    const void* pLightingOffset;
    const void* pLightingOrigin;
    int flags;
    int entity_index;
    int skin;
    int body;
    int hitboxset;
    uintptr_t instance;
    ModelRenderInfo_t()
    {
        pModelToWorld = NULL;
        pLightingOffset = NULL;
        pLightingOrigin = NULL;
    }
};

struct DrawModelState_t
{
    void* m_pStudioHdr;
    void* m_pStudioHWData;
    void* m_pRenderable;
    const void* m_pModelToWorld;
    uintptr_t        m_decals;
    int                        m_drawFlags;
    int                        m_lod;
};

class IVModelRender
{
public:
    virtual int DrawModel(int flags, void* pRenderable, uintptr_t instance, int entity_index,
        const model_t* model, vec3 const& origin, vec3 const& angles, int skin, int body,
        int hitboxset, const void* modelToWorld = NULL,
        const void* pLightingOffset = NULL) = 0;

    virtual void ForcedMaterialOverride(IMaterial* newMaterial, int nOverrideType = 0, int nOverrides = 0) = 0;
    virtual bool IsForcedMaterialOverride(void) = 0;
    virtual void SetViewTarget(const int* pStudioHdr, int nBodyIndex, const vec3& target) = 0;
    virtual uintptr_t CreateInstance(void* pRenderable, void* pCache = NULL) = 0;
    virtual void DestroyInstance(uintptr_t handle) = 0;
};

class IVModelInfoClient
{
public:
    inline void* GetModel(int Index)
    {
        return GetVirtualFunction<void* (__thiscall*)(void*, int)>(this, 1)(this, Index);
    }

    inline int GetModelIndex(const char* Filename)
    {
        return GetVirtualFunction<int(__thiscall*)(void*, const char*)>(this, 2)(this, Filename);
    }

    inline const char* GetModelName(const void* Model)
    {
        return GetVirtualFunction<const char* (__thiscall*)(void*, const void*)>(this, 3)(this, Model);
    }
    inline studiohdr_t* GetStudioModel(const model_t* model)
    {
        return GetVirtualFunction<studiohdr_t* (__thiscall*)(decltype(this), const model_t*) >(this, 32)(this, model);
    }
    void GetModelMaterials(const model_t* model, int count, IMaterial** ppMaterial)
    {
        GetVirtualFunction<void(__thiscall*)(decltype(this), const model_t*, int, IMaterial**) >(this, 18)(this, model, count, ppMaterial);
    }
};

class IMaterialVar
{
private:
public:
    void SetFloatValue(float value)
    {
        GetVirtualFunction< void(__thiscall*)(decltype(this), float) >(this, 4)(this, value);
    }

    void SetVectorValue(float r, float g, float b)
    {
        GetVirtualFunction< void(__thiscall*)(decltype(this), float, float, float) >(this, 11)(this, r, g, b);
    }

    void SetStringValue(char const* value)
    {
        GetVirtualFunction< void(__thiscall*)(decltype(this), char const*) >(this, 6)(this, value);
    }
};

class IMaterialSystem
{
public:
    const char* GetName()
    {
        return GetVirtualFunction< const char* (__thiscall*)(decltype(this)) >(this, 0)(this);
    }

    IMaterial* CreateMaterial(const char* material_name, void* kv)
    {
        return GetVirtualFunction< IMaterial* (__thiscall*)(decltype(this), const char*, void*) >(this, 83)(
            this, material_name, kv);
    }

    IMaterial* FindMaterial(const char* material_name, const char* texture_group_name = nullptr, bool complain = true, const char* complain_prefix = nullptr)
    {
        return GetVirtualFunction< IMaterial* (__thiscall*)(decltype(this), const char*, const char*, bool, const char*) >(this, 84)(this, material_name, texture_group_name, complain, complain_prefix);
    }

    int FirstMaterial()
    {
        return GetVirtualFunction< int(__thiscall*)(decltype(this)) >(this, 86)(this);
    }

    int NextMaterial(int handle)
    {
        return GetVirtualFunction< int(__thiscall*)(decltype(this), int) >(this, 87)(this, handle);
    }

    int InvalidMaterial()
    {
        return GetVirtualFunction< int(__thiscall*)(decltype(this)) >(this, 88)(this);
    }

    IMaterial* GetMaterial(short handle)
    {
        return GetVirtualFunction< IMaterial* (__thiscall*)(decltype(this), short) >(this, 89)(this, handle);
    }

    void* FindTexture(char const* pTextureName, const char* pTextureGroupName, bool complain = true)
    {
        return GetVirtualFunction< void* (__thiscall*)(decltype(this), char const*, const char*, bool)
        >(this, 91)(this, pTextureName, pTextureGroupName, complain);
    }

    void* GetRenderContext()
    {
        return GetVirtualFunction< void* (__thiscall*)(decltype(this)) >(this, 115)(this);
    }
};
