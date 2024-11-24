#pragma once
#pragma region enginetrace_enumerations
#include <limits>
#include <iostream>
#include <cmath>
#include <windows.h>
#include <functional>
#include <cassert>

inline int UtlMemory_CalcNewAllocationCount(int nAllocationCount, int nGrowSize, int nNewSize, int nBytesItem)
{
	if (nGrowSize)
		nAllocationCount = ((1 + ((nNewSize - 1) / nGrowSize)) * nGrowSize);
	else
	{
		if (!nAllocationCount)
			nAllocationCount = (31 + nBytesItem) / nBytesItem;

		while (nAllocationCount < nNewSize)
			nAllocationCount *= 2;
	}

	return nAllocationCount;
}

template <class T, class N = int>
class CUtlMemory
{
public:
	T& operator[](N i)
	{
		return pMemory[i];
	}

	const T& operator[](N i) const
	{
		return pMemory[i];
	}

	T* Base()
	{
		return pMemory;
	}

	int NumAllocated() const
	{
		return iAllocationCount;
	}

	void Grow(int iNum = 1)
	{
		if (IsExternallyAllocated())
			return;

		int iOldAllocationCount = iAllocationCount;
		int iAllocationRequested = iAllocationCount + iNum;
		int iNewAllocationCount = UtlMemory_CalcNewAllocationCount(iAllocationCount, iGrowSize, iAllocationRequested, sizeof(T));

		if (static_cast<int>(static_cast<N>(iNewAllocationCount)) < iAllocationRequested)
		{
			if (static_cast<int>(static_cast<N>(iNewAllocationCount)) == 0 && static_cast<int>(static_cast<N>(iNewAllocationCount - 1)) >= iAllocationRequested)
				--iNewAllocationCount;
			else
			{
				if (static_cast<int>(static_cast<N>(iAllocationRequested)) != iAllocationRequested)
				{
					return;
				}

				while (static_cast<int>(static_cast<N>(iNewAllocationCount)) < iAllocationRequested)
					iNewAllocationCount = (iNewAllocationCount + iAllocationRequested) / 2;
			}
		}

		iAllocationCount = iNewAllocationCount;

		if (pMemory != nullptr)
		{
			//pMemory = reinterpret_cast<T*>(I::MemAlloc->Realloc(pMemory, iAllocationCount * sizeof(T)));

			std::byte* pData = new std::byte[iAllocationCount * sizeof(T)];
			memcpy(pData, pMemory, iOldAllocationCount * sizeof(T));
			pMemory = reinterpret_cast<T*>(pData);
		}
		else
			//pMemory = reinterpret_cast<T*>(I::MemAlloc->Alloc(iAllocationCount * sizeof(T)));
			pMemory = reinterpret_cast<T*>(new std::byte[iAllocationCount * sizeof(T)]);
	}

	bool IsExternallyAllocated() const
	{
		return iGrowSize < 0;
	}

protected:
	T* pMemory;
	int iAllocationCount;
	int iGrowSize;
};
template <class T>
void Destruct(T* pMemory)
{
	pMemory->~T();
}

template <class T>
T* Construct(T* pMemory)
{
	return new(pMemory) T;
}

template< class T >
T* Copy(T* pMemory, T const& src)
{
	return new(pMemory) T(src);
}

template <class T, class A = CUtlMemory<T>>
class CUtlVector
{
	using CAllocator = A;
public:
	auto begin() const noexcept
	{
		return pMemory.Base();
	}

	auto end() const noexcept
	{
		return pMemory.Base() + iSize;
	}

	T& operator[](int i)
	{
		return pMemory[i];
	}

	const T& operator[](int i) const
	{
		return pMemory[i];
	}

	T& Element(int i)
	{
		return pMemory[i];
	}

	const T& Element(int i) const
	{
		return pMemory[i];
	}

	T* Base()
	{
		return pMemory.Base();
	}

	int Count() const
	{
		return iSize;
	}

	int& Size()
	{
		return iSize;
	}

	void GrowVector(int nCount = 1)
	{
		if (iSize + nCount > pMemory.NumAllocated())
			pMemory.Grow(iSize + nCount - pMemory.NumAllocated());

		iSize += nCount;
	}

	void ShiftElementsRight(const int nElement, const int nShift = 1)
	{
		const int nToMove = iSize - nElement - nShift;

		if (nToMove > 0 && nShift > 0)
			memmove(&Element(nElement + nShift), &Element(nElement), nToMove * sizeof(T));
	}

	void ShiftElementsLeft(const int nElement, const int nShift = 1)
	{
		const int nToMove = iSize - nElement - nShift;

		if (nToMove > 0 && nShift > 0)
			memmove(&Element(nElement), &Element(nElement + nShift), nToMove * sizeof(T));
	}

	int InsertBefore(const int nElement)
	{
		// can insert at the end
		GrowVector();
		ShiftElementsRight(nElement);
		Construct(&Element(nElement));
		return nElement;
	}

	int InsertBefore(int nElement, const T& src)
	{
		// reallocate if can't insert something that's in the list
		// can insert at the end
		GrowVector();
		ShiftElementsRight(nElement);
		Copy(&Element(nElement), src);
		return nElement;
	}

	int AddToTail()
	{
		return InsertBefore(iSize);
	}

	int AddToTail(const T& src)
	{
		return InsertBefore(iSize, src);
	}

	int Find(const T& src) const
	{
		for (int i = 0; i < Count(); ++i)
		{
			if (Element(i) == src)
				return i;
		}

		return -1;
	}

	void Remove(const int nElement)
	{
		Destruct(&Element(nElement));
		ShiftElementsLeft(nElement);
		--iSize;
	}

	void RemoveAll()
	{
		for (int i = iSize; --i >= 0;)
			Destruct(&Element(i));

		iSize = 0;
	}
	bool FindAndRemove(const T& src)
	{
		if (const int nElement = Find(src); nElement != -1)
		{
			Remove(nElement);
			return true;
		}

		return false;
	}

protected:
	CAllocator pMemory;
	int iSize;
	T* pElements;
};
#define CONTENTS_EMPTY					0
#define CONTENTS_SOLID					0x1
#define CONTENTS_WINDOW					0x2
#define CONTENTS_AUX					0x4
#define CONTENTS_GRATE					0x8
#define CONTENTS_SLIME					0x10
#define CONTENTS_WATER					0x20
#define CONTENTS_BLOCKLOS				0x40
#define CONTENTS_OPAQUE					0x80
#define CONTENTS_TESTFOGVOLUM			0x100
#define CONTENTS_UNUSED					0x200
#define CONTENTS_BLOCKLIGHT				0x400
#define CONTENTS_TEAM1					0x800
#define CONTENTS_TEAM2					0x1000
#define CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000
#define CONTENTS_MOVEABLE				0x4000
#define CONTENTS_AREAPORTAL				0x8000
#define CONTENTS_PLAYERCLIP				0x10000
#define CONTENTS_MONSTERCLIP			0x20000
#define CONTENTS_CURRENT_0				0x40000
#define CONTENTS_CURRENT_90				0x80000
#define CONTENTS_CURRENT_180			0x100000
#define CONTENTS_CURRENT_270			0x200000
#define CONTENTS_CURRENT_UP				0x400000
#define CONTENTS_CURRENT_DOWN			0x800000
#define CONTENTS_ORIGIN					0x1000000
#define CONTENTS_MONSTER				0x2000000
#define CONTENTS_DEBRIS					0x4000000
#define CONTENTS_DETAIL					0x8000000
#define CONTENTS_TRANSLUCENT			0x10000000
#define CONTENTS_LADDER					0x20000000
#define CONTENTS_HITBOX					0x40000000
#define LAST_VISIBLE_CONTENTS			CONTENTS_OPAQUE
#define ALL_VISIBLE_CONTENTS			(LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define SURF_LIGHT						0x0001
#define SURF_SKY2D						0x0002
#define SURF_SKY						0x0004
#define SURF_WARP						0x0008
#define SURF_TRANS						0x0010
#define SURF_NOPORTAL					0x0020
#define SURF_TRIGGER					0x0040
#define SURF_NODRAW						0x0080
#define SURF_HINT						0x0100
#define SURF_SKIP						0x0200
#define SURF_NOLIGHT					0x0400
#define SURF_BUMPLIGHT					0x0800
#define SURF_NOSHADOWS					0x1000
#define SURF_NODECALS					0x2000
#define SURF_NOCHOP						0x4000
#define SURF_HITBOX						0x8000

#define MASK_ALL						(0xFFFFFFFF)
#define MASK_SOLID						(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define MASK_PLAYERSOLID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define MASK_NPCSOLID					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define MASK_NPCFLUID					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define MASK_WATER						(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
#define MASK_OPAQUE						(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
#define MASK_OPAQUE_AND_NPCS			(MASK_OPAQUE|CONTENTS_MONSTER)
#define MASK_BLOCKLOS					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
#define MASK_BLOCKLOS_AND_NPCS			(MASK_BLOCKLOS|CONTENTS_MONSTER)
#define MASK_VISIBLE					(MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define MASK_VISIBLE_AND_NPCS			(MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define MASK_SHOT						(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
#define MASK_SHOT_BRUSHONLY				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
#define MASK_SHOT_HULL					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
#define MASK_SHOT_PORTAL				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define MASK_SOLID_BRUSHONLY			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define MASK_PLAYERSOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
#define MASK_NPCSOLID_BRUSHONLY			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define MASK_NPCWORLDSTATIC				(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define MASK_NPCWORLDSTATIC_FLUID		(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
#define MASK_SPLITAREAPORTAL			(CONTENTS_WATER|CONTENTS_SLIME)
#define MASK_CURRENT					(CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)
#define MASK_DEADSOLID					(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)
class Vector2D
{
public:
	constexpr Vector2D(float x = 0.f, float y = 0.f) :
		x(x), y(y) { }

	[[nodiscard]] bool IsZero() const
	{
		return (std::fpclassify(this->x) == FP_ZERO &&
			std::fpclassify(this->y) == FP_ZERO);
	}

public:
	float x, y;
};

class Vector
{
public:
	constexpr Vector(float x = 0.f, float y = 0.f, float z = 0.f) :
		x(x), y(y), z(z) { }

	constexpr Vector(const float* arrVector) :
		x(arrVector[0]), y(arrVector[1]), z(arrVector[2]) { }

	constexpr Vector(const Vector2D& vecBase2D) :
		x(vecBase2D.x), y(vecBase2D.y), z(0.0f) { }

	[[nodiscard]] bool IsValid() const
	{
		return std::isfinite(this->x) && std::isfinite(this->y) && std::isfinite(this->z);
	}

	constexpr void Invalidate()
	{
		this->x = this->y = this->z = std::numeric_limits<float>::infinity();
	}

	[[nodiscard]] float* data()
	{
		return reinterpret_cast<float*>(this);
	}

	[[nodiscard]] const float* data() const
	{
		return reinterpret_cast<float*>(const_cast<Vector*>(this));
	}

	float& operator[](const std::size_t i)
	{
		return this->data()[i];
	}

	const float& operator[](const std::size_t i) const
	{
		return this->data()[i];
	}

	bool operator==(const Vector& vecBase) const
	{
		return this->IsEqual(vecBase);
	}

	bool operator!=(const Vector& vecBase) const
	{
		return !this->IsEqual(vecBase);
	}

	constexpr Vector& operator=(const Vector& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z;
		return *this;
	}

	constexpr Vector& operator=(const Vector2D& vecBase2D)
	{
		this->x = vecBase2D.x; this->y = vecBase2D.y; this->z = 0.0f;
		return *this;
	}

	constexpr Vector& operator+=(const Vector& vecBase)
	{
		this->x += vecBase.x; this->y += vecBase.y; this->z += vecBase.z;
		return *this;
	}

	constexpr Vector& operator-=(const Vector& vecBase)
	{
		this->x -= vecBase.x; this->y -= vecBase.y; this->z -= vecBase.z;
		return *this;
	}

	constexpr Vector& operator*=(const Vector& vecBase)
	{
		this->x *= vecBase.x; this->y *= vecBase.y; this->z *= vecBase.z;
		return *this;
	}

	constexpr Vector& operator/=(const Vector& vecBase)
	{
		this->x /= vecBase.x; this->y /= vecBase.y; this->z /= vecBase.z;
		return *this;
	}

	constexpr Vector& operator+=(const float flAdd)
	{
		this->x += flAdd; this->y += flAdd; this->z += flAdd;
		return *this;
	}

	constexpr Vector& operator-=(const float flSubtract)
	{
		this->x -= flSubtract; this->y -= flSubtract; this->z -= flSubtract;
		return *this;
	}

	constexpr Vector& operator*=(const float flMultiply)
	{
		this->x *= flMultiply; this->y *= flMultiply; this->z *= flMultiply;
		return *this;
	}

	constexpr Vector& operator/=(const float flDivide)
	{
		this->x /= flDivide; this->y /= flDivide; this->z /= flDivide;
		return *this;
	}

	Vector operator+(const Vector& vecAdd) const
	{
		return Vector(this->x + vecAdd.x, this->y + vecAdd.y, this->z + vecAdd.z);
	}

	Vector operator-(const Vector& vecSubtract) const
	{
		return Vector(this->x - vecSubtract.x, this->y - vecSubtract.y, this->z - vecSubtract.z);
	}

	Vector operator*(const Vector& vecMultiply) const
	{
		return Vector(this->x * vecMultiply.x, this->y * vecMultiply.y, this->z * vecMultiply.z);
	}

	Vector operator/(const Vector& vecDivide) const
	{
		return Vector(this->x / vecDivide.x, this->y / vecDivide.y, this->z / vecDivide.z);
	}

	Vector operator+(const float flAdd) const
	{
		return Vector(this->x + flAdd, this->y + flAdd, this->z + flAdd);
	}

	Vector operator-(const float flSubtract) const
	{
		return Vector(this->x - flSubtract, this->y - flSubtract, this->z - flSubtract);
	}

	Vector operator*(const float flMultiply) const
	{
		return Vector(this->x * flMultiply, this->y * flMultiply, this->z * flMultiply);
	}

	Vector operator/(const float flDivide) const
	{
		return Vector(this->x / flDivide, this->y / flDivide, this->z / flDivide);
	}

	[[nodiscard]] bool IsEqual(const Vector& vecEqual) const
	{
		return (std::fabsf(this->x - vecEqual.x) < std::numeric_limits<float>::epsilon() &&
			std::fabsf(this->y - vecEqual.y) < std::numeric_limits<float>::epsilon() &&
			std::fabsf(this->z - vecEqual.z) < std::numeric_limits<float>::epsilon());
	}

	[[nodiscard]] bool IsZero() const
	{
		return (std::fpclassify(this->x) == FP_ZERO &&
			std::fpclassify(this->y) == FP_ZERO &&
			std::fpclassify(this->z) == FP_ZERO);
	}

	[[nodiscard]] Vector2D ToVector2D() const
	{
		return Vector2D(this->x, this->y);
	}

	[[nodiscard]] float Length() const
	{
		return std::sqrtf(this->LengthSqr());
	}

	[[nodiscard]] constexpr float LengthSqr() const
	{
		return DotProduct(*this);
	}

	[[nodiscard]] float Length2D() const
	{
		return std::sqrtf(this->Length2DSqr());
	}

	[[nodiscard]] constexpr float Length2DSqr() const
	{
		return (this->x * this->x + this->y * this->y);
	}

	[[nodiscard]] float DistTo(const Vector& vecEnd) const
	{
		return (*this - vecEnd).Length();
	}

	[[nodiscard]] constexpr float DistToSqr(const Vector& vecEnd) const
	{
		return (*this - vecEnd).LengthSqr();
	}

	[[nodiscard]] Vector Normalized() const
	{
		Vector vecOut = *this;
		vecOut.NormalizeInPlace();
		return vecOut;
	}

	float NormalizeInPlace()
	{
		const float flLength = this->Length();
		const float flRadius = 1.0f / (flLength + std::numeric_limits<float>::epsilon());

		this->x *= flRadius;
		this->y *= flRadius;
		this->z *= flRadius;

		return flLength;
	}

	[[nodiscard]] constexpr float DotProduct(const Vector& vecDot) const
	{
		return (this->x * vecDot.x + this->y * vecDot.y + this->z * vecDot.z);
	}

	[[nodiscard]] constexpr Vector CrossProduct(const Vector& vecCross) const
	{
		return Vector(this->y * vecCross.z - this->z * vecCross.y, this->z * vecCross.x - this->x * vecCross.z, this->x * vecCross.y - this->y * vecCross.x);
	}

public:
	float x, y, z;
};

class Vector4D
{
public:
	constexpr Vector4D(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f) :
		x(x), y(y), z(z), w(w) { }

public:
	float x, y, z, w;
};

class __declspec(align(16)) VectorAligned : public Vector
{
public:
	VectorAligned() = default;

	explicit VectorAligned(const Vector& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z; this->w = 0.f;
	}

	constexpr VectorAligned& operator=(const Vector& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z; this->w = 0.f;
		return *this;
	}

public:
	float w;
};
struct ViewMatrix_t
{
	ViewMatrix_t() = default;

	float* operator[](const int nIndex)
	{
		return flData[nIndex];
	}

	const float* operator[](const int nIndex) const
	{
		return flData[nIndex];
	}

	float flData[4][4] = { };
};

using matrix3x3_t = float[3][3];
struct matrix3x4_t
{
	matrix3x4_t() = default;

	matrix3x4_t(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23)
	{
		arrData[0][0] = m00; arrData[0][1] = m01; arrData[0][2] = m02; arrData[0][3] = m03;
		arrData[1][0] = m10; arrData[1][1] = m11; arrData[1][2] = m12; arrData[1][3] = m13;
		arrData[2][0] = m20; arrData[2][1] = m21; arrData[2][2] = m22; arrData[2][3] = m23;
	}

	matrix3x4_t(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin)
	{
		Init(xAxis, yAxis, zAxis, vecOrigin);
	}

	constexpr void Init(const Vector& vecForward, const Vector& vecLeft, const Vector& vecUp, const Vector& vecOrigin)
	{
		SetForward(vecForward);
		SetLeft(vecLeft);
		SetUp(vecUp);
		SetOrigin(vecOrigin);
	}

	constexpr void SetForward(const Vector& vecForward)
	{
		this->arrData[0][0] = vecForward.x;
		this->arrData[1][0] = vecForward.y;
		this->arrData[2][0] = vecForward.z;
	}

	constexpr void SetLeft(const Vector& vecLeft)
	{
		this->arrData[0][1] = vecLeft.x;
		this->arrData[1][1] = vecLeft.y;
		this->arrData[2][1] = vecLeft.z;
	}

	constexpr void SetUp(const Vector& vecUp)
	{
		this->arrData[0][2] = vecUp.x;
		this->arrData[1][2] = vecUp.y;
		this->arrData[2][2] = vecUp.z;
	}

	constexpr void SetOrigin(const Vector& vecOrigin)
	{
		this->arrData[0][3] = vecOrigin.x;
		this->arrData[1][3] = vecOrigin.y;
		this->arrData[2][3] = vecOrigin.z;
	}

	constexpr void Invalidate()
	{
		for (auto& arrSubData : arrData)
		{
			for (auto& flData : arrSubData)
				flData = std::numeric_limits<float>::infinity();
		}
	}

	float* operator[](const int nIndex)
	{
		return arrData[nIndex];
	}

	const float* operator[](const int nIndex) const
	{
		return arrData[nIndex];
	}

	[[nodiscard]] constexpr Vector at(const int nIndex) const
	{
		return Vector(arrData[0][nIndex], arrData[1][nIndex], arrData[2][nIndex]);
	}

	float* Base()
	{
		return &arrData[0][0];
	}

	[[nodiscard]] const float* Base() const
	{
		return &arrData[0][0];
	}

	float arrData[3][4] = { };
};

__declspec(align(16)) class matrix3x4a_t : public matrix3x4_t
{
public:
	matrix3x4a_t& operator=(const matrix3x4_t& matSource)
	{
		std::copy_n(matSource.Base(), sizeof(float) * 3U * 4U, this->Base());
		return *this;
	}
};
enum EDispSurfFlags
{
	DISPSURF_FLAG_SURFACE = (1 << 0),
	DISPSURF_FLAG_WALKABLE = (1 << 1),
	DISPSURF_FLAG_BUILDABLE = (1 << 2),
	DISPSURF_FLAG_SURFPROP1 = (1 << 3),
	DISPSURF_FLAG_SURFPROP2 = (1 << 4)
};
enum ETraceType
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
	TRACE_FILTERSKY
};

enum EDebugTraceCounterBehavior
{
	TRACE_COUNTER_SET = 0,
	TRACE_COUNTER_INC,
};
#pragma endregion

struct BrushSideInfo_t
{
	VectorAligned	vecPlane;	// the plane of the brush side
	std::uint16_t	uBevel;		// bevel plane?
	std::uint16_t	uThin;		// thin?
};

struct cplane_t
{
	Vector		vecNormal;
	float		flDistance;
	std::byte	dType;
	std::byte	dSignBits;
	std::byte	pad[0x2];
};

struct csurface_t
{
	const char* szName;
	short			nSurfaceProps;
	std::uint16_t	uFlags;
};

class CBaseTrace
{
public:
	CBaseTrace() { }

	Vector			vecStart;		// start position
	Vector			vecEnd;			// final position
	cplane_t		plane;			// surface normal at impact
	float			flFraction;		// time completed, 1.0 = didn't hit anything
	int				iContents;		// contents on other side of surface hit
	std::uint16_t	fDispFlags;		// displacement flags for marking surfaces with data
	bool			bAllSolid;		// if true, plane is not valid
	bool			bStartSolid;	// if true, the initial point was in a solid area
};

class CBaseEntity;
class CGameTrace : public CBaseTrace
{
public:
	CGameTrace() : pHitEntity(nullptr) { }

	float				flFractionLeftSolid;	// time we left a solid, only valid if we started in solid
	csurface_t			surface;				// surface hit (impact surface)
	int					iHitGroup;				// 0 == generic, non-zero is specific body part
	short				sPhysicsBone;			// physics bone hit by trace in studio
	std::uint16_t		uWorldSurfaceIndex;		// index of the msurface2_t, if applicable
	CBaseEntity* pHitEntity;				// entity hit by trace
	int					iHitbox;				// box hit by trace in studio

	inline bool DidHit() const
	{
		return (flFraction < 1.0f || bAllSolid || bStartSolid);
	}

	inline bool IsVisible() const
	{
		return (flFraction > 0.97f);
	}

private:
	CGameTrace(const CGameTrace& other)
	{
		this->vecStart = other.vecStart;
		this->vecEnd = other.vecEnd;
		this->plane = other.plane;
		this->flFraction = other.flFraction;
		this->iContents = other.iContents;
		this->fDispFlags = other.fDispFlags;
		this->bAllSolid = other.bAllSolid;
		this->bStartSolid = other.bStartSolid;
		this->flFractionLeftSolid = other.flFractionLeftSolid;
		this->surface = other.surface;
		this->iHitGroup = other.iHitGroup;
		this->sPhysicsBone = other.sPhysicsBone;
		this->uWorldSurfaceIndex = other.uWorldSurfaceIndex;
		this->pHitEntity = other.pHitEntity;
		this->iHitbox = other.iHitbox;
	}
};

using Trace_t = CGameTrace;

struct Ray_t
{
	Ray_t(const Vector& vecStart, const Vector& vecEnd) :
		vecStart(vecStart), vecDelta(vecEnd - vecStart), matWorldAxisTransform(nullptr), bIsRay(true)
	{
		this->bIsSwept = (this->vecDelta.LengthSqr() != 0.f);
	}

	Ray_t(const Vector& vecStart, const Vector& vecEnd, const Vector& vecMins, const Vector& vecMaxs)
	{
		this->vecDelta = vecEnd - vecStart;

		this->matWorldAxisTransform = nullptr;
		this->bIsSwept = (this->vecDelta.LengthSqr() != 0.f);

		this->vecExtents = vecMaxs - vecMins;
		this->vecExtents *= 0.5f;
		this->bIsRay = (this->vecExtents.LengthSqr() < 1e-6);

		this->vecStartOffset = vecMins + vecMaxs;
		this->vecStartOffset *= 0.5f;
		this->vecStart = vecStart + this->vecStartOffset;
		this->vecStartOffset *= -1.0f;
	}

	VectorAligned		vecStart;
	VectorAligned		vecDelta;
	VectorAligned		vecStartOffset;
	VectorAligned		vecExtents;
	const matrix3x4_t* matWorldAxisTransform;
	bool				bIsRay;
	bool				bIsSwept;
};

class IHandleEntity;
class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntity, int fContentsMask) = 0;
	virtual ETraceType GetTraceType() const = 0;
};

class CTraceFilter : public ITraceFilter
{
	using FilterCallbackFn = std::function<bool(IHandleEntity*, int)>;

public:
	// @todo: sig ctracefiltersimple constructor and use it

	CTraceFilter(const IHandleEntity* pSkipEntity, ETraceType iTraceType = TRACE_EVERYTHING)
		: pSkip(pSkipEntity), iTraceType(iTraceType) { }

	CTraceFilter(FilterCallbackFn&& checkCallback, ETraceType iTraceType = TRACE_EVERYTHING)
		: checkCallback(std::move(checkCallback)), iTraceType(iTraceType) { }

	bool ShouldHitEntity(IHandleEntity* pHandleEntity, int fContentsMask) override
	{
		// if given user defined callback - check it
		if (checkCallback != nullptr)
			return checkCallback(pHandleEntity, fContentsMask);

		assert(pSkip);

		// else skip given entity
		return !(pHandleEntity == pSkip);
	}

	ETraceType GetTraceType() const override
	{
		return iTraceType;
	}

private:
	const IHandleEntity* pSkip = nullptr;
	FilterCallbackFn checkCallback = nullptr;
	ETraceType iTraceType = TRACE_EVERYTHING;
};

class ITraceListData
{
public:
	virtual			~ITraceListData() { }
	virtual void	Reset() = 0;
	virtual bool	IsEmpty() = 0;
	virtual bool	CanTraceRay(const Ray_t& ray) = 0;
};

class IEntityEnumerator
{
public:
	// this gets called with each handle
	virtual bool EnumEntity(IHandleEntity* pHandleEntity) = 0;
};

struct virtualmeshlist_t;
struct AABB_t;
class ICollideable;
class CPhysCollide;
class CBrushQuery;
class IEngineTrace
{
public:
	virtual int GetPointContents(const Vector& vecAbsPosition, int fContentsMask = MASK_ALL, IHandleEntity** ppEntity = nullptr) = 0;
	virtual int GetPointContents_WorldOnly(const Vector& vecAbsPosition, int fContentsMask = MASK_ALL) = 0;
	virtual int GetPointContents_Collideable(ICollideable* pCollide, const Vector& vecAbsPosition) = 0;
	virtual void ClipRayToEntity(const Ray_t& ray, unsigned int fMask, IHandleEntity* pEntity, Trace_t* pTrace) = 0;
	virtual void ClipRayToCollideable(const Ray_t& ray, unsigned int fMask, ICollideable* pCollide, Trace_t* pTrace) = 0;
	virtual void TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) = 0;
	virtual void SetupLeafAndEntityListRay(const Ray_t& ray, ITraceListData* pTraceData) = 0;
	virtual void SetupLeafAndEntityListBox(const Vector& vecBoxMin, const Vector& vecBoxMax, ITraceListData* pTraceData) = 0;
	virtual void TraceRayAgainstLeafAndEntityList(const Ray_t& ray, ITraceListData* pTraceData, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) = 0;
	virtual void SweepCollideable(ICollideable* pCollide, const Vector& vecAbsStart, const Vector& vecAbsEnd, const Vector& vecAngles, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) = 0;
	virtual void EnumerateEntities(const Ray_t& ray, bool bTriggers, IEntityEnumerator* pEnumerator) = 0;
	virtual void EnumerateEntities(const Vector& vecAbsMins, const Vector& vecAbsMaxs, IEntityEnumerator* pEnumerator) = 0;
	virtual ICollideable* GetCollideable(IHandleEntity* pEntity) = 0;
	virtual int GetStatByIndex(int nIndex, bool bClear) = 0;
	virtual void GetBrushesInAABB(const Vector& vecMins, const Vector& vecMaxs, CUtlVector<int>* pOutput, int fContentsMask = MASK_ALL) = 0;
	virtual CPhysCollide* GetCollidableFromDisplacementsInAABB(const Vector& vecMins, const Vector& vecMaxs) = 0;
	virtual int GetNumDisplacements() = 0;
	virtual void GetDisplacementMesh(int nIndex, virtualmeshlist_t* pMeshTriList) = 0;
	virtual bool GetBrushInfo(int iBrush, CUtlVector<BrushSideInfo_t>* pBrushSideInfoOut, int* pContentsOut) = 0;
	virtual bool PointOutsideWorld(const Vector& vecPoint) = 0;
	virtual int GetLeafContainingPoint(const Vector& vecPoint) = 0;
	virtual ITraceListData* AllocTraceListData() = 0;
	virtual void FreeTraceListData(ITraceListData* pListData) = 0;
	virtual int GetSetDebugTraceCounter(int iValue, EDebugTraceCounterBehavior behavior) = 0;
	virtual int GetMeshesFromDisplacementsInAABB(const Vector& vecMins, const Vector& vecMaxs, virtualmeshlist_t* pOutputMeshes, int nMaxOutputMeshes) = 0;
	virtual void GetBrushesInCollideable(ICollideable* pCollideable, CBrushQuery& BrushQuery) = 0;
	virtual bool IsFullyOccluded(int nOcclusionKey, const AABB_t& aabb1, const AABB_t& aabb2, const Vector& vecShadow) = 0;
	virtual void SuspendOcclusionTests() = 0;
	virtual void ResumeOcclusionTests() = 0;
	virtual void FlushOcclusionQueries() = 0;
};
