

#include "D:\CSGO-LIZZOWARE\Hooks.cpp"
#include <vector>
#include "D:\CSGO-LIZZOWARE\gameoffsets.h"
#include "D:\CSGO-LIZZOWARE\includes.h"

#define PI 3.14159265
//#include "D:\CSGO-LIZZOWARE\Source.cpp" 

struct vec2
{
    int x, y;
};
struct vec4
{
    int x, y, z, w;
};
class backtracking
{
public:
    cheatparts backtrack;
	DWORD pid;
	HWND hgamewindow;
	HANDLE processhandle = NULL;
	DWORD GBA = (DWORD)GetModuleHandle("client.dll");
    DWORD enginebaseaddress = (DWORD)GetModuleHandle("engine.dll");
    bool WorldToScreen(Vector3 pos, vec2& screen, float matrix[15], int windowWidth, int windowHeight)
    {
        //Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
        vec4 clipCoords;
        clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
        clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
        clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
        clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

        if (clipCoords.w < 0.1f)
            return false;

        //perspective division, dividing by clip.W = Normalized Device Coordinates
        vec3 NDC;
        NDC.x = clipCoords.x / clipCoords.w;
        NDC.y = clipCoords.y / clipCoords.w;
        NDC.z = clipCoords.z / clipCoords.w;

        screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
        screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
        return true;
    }
    float LengthSqr(vec3 angles) {
        return (angles.x * angles.x + angles.y * angles.y + angles.z * angles.z);
    }

    float Length(vec3 angles) {
        return sqrt(LengthSqr(angles));
    }
	struct Tick
	{
		Tick(int Tickcount, Vector3 Head)
		{
			tick = Tickcount;
			head = Head;
		}
		int tick = -1;
		Vector3 head;
	};
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
	void backtrackstart(CUserCmd * cmd, int tocks)
	{
		uintptr_t local = *(uintptr_t*)(GBA + dwLocalPlayer); // throwing NULL mem error, huh?
		if (!local)
			return;
		for (int i = 0; i < 64; i++)
		{
			uintptr_t entity = *(uintptr_t*)(GBA + dwEntityList + i * 0x10);
			if (entity != NULL && *(int*)(entity + m_iHealth) > 0 && *(int*)(entity + m_iTeamNum) != *(int*)(local + m_iTeamNum) && !*(bool*)(entity + m_bDormant)) // get rid of this spaghetti 
			{
				Vector3 bonepos = backtrack.Gethead(entity,8); 

				tick[i].insert(tick[i].begin(), Tick(cmd->tick_count, bonepos));

				if (tick[i].size() > tocks)
					tick[i].pop_back();
			}
		}
	}
    Vector3 getheadattick()
    {
        DWORD clientstate = *(DWORD*)(enginebaseaddress + dwClientState); // update these offsets l8r causing NULL mem error to throw
        uintptr_t local = *(uintptr_t*)(GBA + dwLocalPlayer);
        if (!local)
            return { 0,0,0 };

        vec3 viewangles;
        viewangles = backtrack.getviewangles(processhandle, enginebaseaddress);
        vec3 punchangle = *(vec3*)(local + m_aimPunchAngle) * 2;
        viewangles = (*(vec3*)(clientstate + dwClientState_ViewAngles));
        viewangles.x += punchangle.x * 2;
        viewangles.y += punchangle.y * 2;
        viewangles.z += punchangle.z * 2;
        int closestplayer = -1;
        float tempdelta = FLT_MAX;

        for (int i = 0; i < 64; i++)
        {
            uintptr_t entity = *(uintptr_t*)(GBA + dwEntityList + i * 0x10);
            if (entity != NULL && *(int*)(entity + m_iHealth) > 0 && *(int*)(entity + m_iTeamNum) != *(int*)(local + m_iTeamNum) && !*(bool*)(entity + m_bDormant))
            {
                vec3 angle = calcangles(backtrack.getplayerlocation(local, processhandle), backtrack.Gethead(entity, 8)) - viewangles;
                float dist = Length(angle);
                if (dist < tempdelta)
                {
                    closestplayer = i;
                    tempdelta = dist;
                }
            }
        }

        int index = -1;
        if (closestplayer != -1)
        {
            for (int i = 0; i < tick[closestplayer].size(); i++)
            {
                Vector3 pos = tick[closestplayer].at(i).head;

                vec3 angle = calcangles(backtrack.getplayerlocation(local, processhandle), tick[closestplayer].at(i).head) - viewangles;
                float dist = Length(angle);

                if (dist < tempdelta)
                {
                    index = i;
                    tempdelta = dist;
                }
            }
        }
        if (index != -1)
        {
            return tick[closestplayer].at(index).head;
        }
        else
        {
            return { 0,0,0 };
        }
    }
    int BacktrackingCalc(CUserCmd* cmd)
    {
        DWORD clientstate = *(DWORD*)(enginebaseaddress + dwClientState);
        uintptr_t local = *(uintptr_t*)(GBA + dwLocalPlayer);
        if (!local)
            return 69;

        vec3 viewangles;
        viewangles = backtrack.getviewangles(processhandle, enginebaseaddress);
        vec3 punchangle = *(vec3*)(local + m_aimPunchAngle) * 2;
        viewangles = (*(vec3*)(clientstate + dwClientState_ViewAngles));
        viewangles.x += punchangle.x * 2;
        viewangles.y += punchangle.y * 2;
        viewangles.z += punchangle.z * 2;
        int closestplayer = -1;
        float tempdelta = FLT_MAX;

        for (int i = 0; i < 64; i++)
        {
            uintptr_t entity = *(uintptr_t*)(GBA + dwEntityList + i * 0x10);
            if (entity != NULL && *(int*)(entity + m_iHealth) > 0 && *(int*)(entity + m_iTeamNum) != *(int*)(local + m_iTeamNum) && !*(bool*)(entity + m_bDormant))
            {
                vec3 angle = calcangles(backtrack.getplayerlocation(local, processhandle), backtrack.Gethead(entity, 8)) - viewangles;
                float dist = Length(angle);
                if (dist < tempdelta)
                {
                    closestplayer = i;
                    tempdelta = dist;
                }
            }
        }

        int index = -1;
        if (closestplayer != -1)
        {
            for (int i = 0; i < tick[closestplayer].size(); i++)
            {
                Vector3 pos = tick[closestplayer].at(i).head;

                vec3 angle = calcangles(backtrack.getplayerlocation(local, processhandle), tick[closestplayer].at(i).head) - viewangles;
                float dist = Length(angle);

                if (dist < tempdelta)
                {
                    index = i;
                    tempdelta = dist;
                }
            }
        }
        vec2 screencoords;
        float* Matrix[15];
        if ((clientstate + dwViewMatrix) != NULL) // possible issue
            memcpy(&Matrix, (PBYTE*)(clientstate + dwViewMatrix), sizeof(Matrix));
        if (index != -1 && cmd->buttons & IN_ATTACK)
        {
         
            return tick[closestplayer].at(index).tick;
        }
         //draw::drawfilledrect(tick[closestplayer].at(index).head.x, tick[closestplayer].at(index).head.y, 2 ,2, D3DCOLOR_ARGB(255, 255, 255, 255));
        else
        {
            return 69;
        }
    }
    
private:

	std::vector<Tick> tick[64];
};

