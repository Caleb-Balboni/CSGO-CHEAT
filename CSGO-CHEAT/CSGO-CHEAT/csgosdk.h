#pragma once
#include <windows.h>
#include "D:\CSGO-LIZZOWARE\gameoffsets.h"
#include "D:\CSGO-LIZZOWARE\CSGO-LIZZOWARE\CSGO-LIZZOWARE\csgoVector.cpp"
#include <iostream>
#include <vector>
#include <cstdint>
#define WIN32_LEAN_AND_MEAN  
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}
//https://guidedhacking.com/threads/csgo-how-to-find-call-traceray-tutorial.14696/
typedef void* (__cdecl* tCreateInterface)(const char* name, int* pReturnCode);
class INTERFACE_CREATE
{
public:

	void* GetInterfaces(tCreateInterface fn, const char* name)
	{
		return (fn)(name, 0);
	}
};


class EntListObj
{
public:
    class Ent* ent; //0x0000
    char pad_0004[12]; //0x0004
}; //Size: 0x0010

class EntList
{
public:
    EntListObj entListObjs[32]; //0x0000
}; //Size: 0x0200

class Ent
{
public:
	union
	{
		DEFINE_MEMBER_N(Vec3, origin, m_vecOrigin);
		DEFINE_MEMBER_N(int, clientId, 0x64);
		DEFINE_MEMBER_N(int, mhealth, m_iHealth);
		DEFINE_MEMBER_N(Vec3, vecViewOffset, m_vecViewOffset);
	};
}; //Size: 0x0284

class IClientEntityList
{
public:
	// Get IClientNetworkable interface for specified entity
	virtual void* GetClientNetworkable(int entnum) = 0;
	virtual void* GetClientNetworkableFromHandle(int hEnt) = 0;
	virtual void* GetClientUnknownFromHandle(int hEnt) = 0;

	// NOTE: This function is only a convenience wrapper.
	// It returns GetClientNetworkable( entnum )->GetIClientEntity().
	virtual void* GetClientEntity(int entnum) = 0;
	virtual void* GetClientEntityFromHandle(int hEnt) = 0;

	// Returns number of entities currently in use
	virtual int					NumberOfEntities(bool bIncludeNonNetworkable) = 0;

	// Returns highest index actually used
	virtual int					GetHighestEntityIndex(void) = 0;

	// Sizes entity list to specified size
	virtual void				SetMaxEntities(int maxents) = 0;
	virtual int					GetMaxEntities() = 0;
};

typedef struct PlayerInfo_s
{
	int64_t __pad0;
	union {
		int64_t xuid;
		struct {
			int xuidlow;
			int xuidhigh;
		};
	};
	char name[128];
	int userid;
	char guid[33];
	unsigned int friendsid;
	char friendsname[128];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[4];
	unsigned char filesdownloaded;
}player_info_t;