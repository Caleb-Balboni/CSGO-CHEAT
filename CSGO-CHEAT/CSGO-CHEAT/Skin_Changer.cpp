#pragma once
#include "D:\CSGO-LIZZOWARE\head.h"
#include "D:\CSGO-LIZZOWARE\gameoffsets.h"
class skinchanger
{
public:
	void Skinchanger(DWORD gamebaseaddress)
	{
     
       
		int weaponid = 7; //change later to be a value passed in within args (for now ak as testing)
		int tpaintkit = 639; //id for bloodsport change this to be passed in within args
		float wear = 0.00001f;
		if ((DWORD*)(gamebaseaddress + dwLocalPlayer) != NULL)
		{
			DWORD* localplayer = (DWORD*)(gamebaseaddress + dwLocalPlayer);
            for (int i = 0; i < 8; i++) //iterate over the weapons in the players inventory
            {
                int cWep = *(int*)(localplayer + m_hMyWeapons + i * 0x4) & 0xfff; //get the current weapon
                cWep = *(int*)(gamebaseaddress + dwEntityList + (cWep - 1) * 0x10); //find the weapon in the entity list
                if (cWep != 0) { //if the weapon is valid
                    short cWepID = *(short*)(cWep + m_iItemDefinitionIndex); //get the weapon ID (AK-47 is ID 7)
                    if (cWepID == 7) { //if the weapon is an AK-47
                        *(int*)(cWep + m_iItemIDHigh) = -1; //force the game to use the fallback values
                        *(int*)(cWep + m_nFallbackPaintKit) = tpaintkit; //set the paintkit
                        *(float*)(cWep + m_flFallbackWear) = 0.00001f; //set the wear
                    }
                }
            }
        }
	}
		
	


};