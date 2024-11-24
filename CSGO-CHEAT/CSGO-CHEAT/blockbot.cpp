#include "D:\CSGO-LIZZOWARE\Hooks.cpp"
//#include "D:\CSGO-LIZZOWARE\Source.cpp" 
#include "D:\CSGO-LIZZOWARE\gameoffsets.h"
class blackbot
{
public:
    cheatparts Cheatparts;
#define PI 3.14159265
    DWORD pid;
    HWND hgamewindow;
    HANDLE processhandle = NULL;
    DWORD GBA = (DWORD)GetModuleHandle("client.dll");
    DWORD enginebaseaddress = (DWORD)GetModuleHandle("engine.dll");
    vec3 calcangles(Vector3 localhead, Vector3 enmangles)
    {
        uintptr_t localplayer = *(uintptr_t*)(GBA + dwLocalPlayer);
        Vector3* vecoffset;
        vecoffset = (Vector3*)(localplayer + m_vecViewOffset);
        Vector3 mypos = (localhead + *vecoffset);
        float height;
        Vector3 deltavec;
        deltavec = { enmangles.x - mypos.x, enmangles.y - mypos.y, enmangles.z - mypos.z };
        float deltaveclength = sqrt(deltavec.x * deltavec.x + deltavec.y * deltavec.y + deltavec.z * deltavec.z);
        float pitch = -asin(deltavec.z / deltaveclength) * (180 / PI);
        float yaw = atan2(deltavec.y, deltavec.x) * (180 / PI);
        vec3 angleszz;
        angleszz.x = pitch;
        angleszz.y = yaw;
        angleszz.z = 0;
        return angleszz;
    }
	int doblockbot(CUserCmd* cmd)
	{
        float Finish = 10000000;
        int ClosestEntity = 1;
        Vector3 Calc = { 0, 0, 0 };
        float Closest = FLT_MAX;
        DWORD localTeam = Cheatparts.getteam(Cheatparts.getlocalplayer(GBA, NULL), NULL);
        int Dormant;
        float finaldistance = 100000000000;
        int Xdistance = 10000000;
        int Ydifference = 1000;
        DWORD localplayer;
        vec3 angles;
        for (int i = 1; i < 64; i++) { //Loops through all the entitys in the index 1-64.
            localplayer = Cheatparts.getlocalplayer(GBA, NULL);
            int Entity = Cheatparts.getplayer(GBA, i, NULL); if (Entity == NULL) continue;
            int EnmHealth = Cheatparts.getplayerhealth(Entity, NULL); if (EnmHealth < 1 || EnmHealth > 100) continue;
            Dormant = Cheatparts.playerdormantcheck(Entity, NULL); if (Dormant) continue;
            bool liveornot = Cheatparts.getlifestate(Entity, NULL); if (liveornot) continue;
            DWORD* clientstate;
            clientstate = (DWORD*)(enginebaseaddress + dwClientState);
            int* localz;
            Vector3 location = Cheatparts.getplayerlocation(Entity, NULL);
            Vector3 mypos = Cheatparts.getplayerlocation(localplayer, NULL);
            Vector3 delta = { location.x - mypos.x,location.y - mypos.y,location.z - mypos.z };
            float distance = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
            Vector3 enmangles = Cheatparts.Gethead(Entity, 9);
            if (Entity == localplayer) continue;
            Vector3 origin = Cheatparts.getplayerlocation(localplayer, NULL);
            Vector3 vecoffset;
            Vector3 deltavec;
            if ((vec3*)(*clientstate + dwClientState_ViewAngles) == NULL) continue;
            angles = *(vec3*)(*clientstate + dwClientState_ViewAngles);
            if (localplayer == NULL) continue;
            if ((Vector3*)(localplayer + m_vecViewOffset) == NULL)  continue;
            vecoffset = *(Vector3*)(localplayer + m_vecViewOffset);
            Vector3 mypoz = (origin + vecoffset);
            deltavec = { enmangles.x - mypoz.x, enmangles.y - mypoz.y, enmangles.z - mypoz.z };
            float deltaveclength = sqrt(deltavec.x * deltavec.x + deltavec.y * deltavec.y + deltavec.z * deltavec.z); // + deltavec.z * deltavec.z
            float pitch = -asin(deltavec.z / deltaveclength) * (180 / PI);
            float yaw = atan2(deltavec.y, deltavec.x) * (180 / PI);
            if (angles.y > yaw)
            {
                Ydifference = angles.y - yaw;
            }
            if (angles.y < yaw)
            {
                Ydifference = yaw - angles.y;
            }

            if (angles.y == yaw)
            {
                Ydifference = angles.y - yaw;
            }
            if (Ydifference < finaldistance)
            {
                        finaldistance = Ydifference;
                        Closest = Ydifference;
                        ClosestEntity = i;
                        Finish = 1;
            }

        }
        if (Finish == 1)
        {
            vec3 angle = calcangles(Cheatparts.getplayerlocation(localplayer, processhandle), Cheatparts.Gethead(Cheatparts.getplayer(GBA, ClosestEntity, NULL), 8));
            if (angle.y > angles.y) {
                return 2; //right
            }
            else if (angle.y < angles.y) {
                return 1; //left
            }

        }
        return 0; //nothing
	}
};