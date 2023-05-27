#include "AutoAim.h"

AutoAim::AutoAim(HANDLE hmoduleEngine, HANDLE hprocess)
{
	hModuleEngine = hmoduleEngine;
	hProcess = hprocess;
}

void AutoAim::ToWard(worldCoor FOV, worldCoor headcoor, TowardStruct& wardangle, TowardStruct& angledifference)
{
	FLOAT horizonward;
	FLOAT verticalward;
	int tempAddr;
	ReadProcessMemory(hProcess, (LPCVOID)((int)hModuleEngine + 0x59F19C), &tempAddr, 4, NULL);
	ReadProcessMemory(hProcess, (LPCVOID)(tempAddr + 0x4d90), &verticalward, 4, NULL);
	ReadProcessMemory(hProcess, (LPCVOID)(tempAddr + 0x4d94), &horizonward, 4, NULL);

	if (headcoor.x >= FOV.x && headcoor.y >= FOV.y) 
	{
		wardangle.horizonward = (FLOAT)((double)atan2(headcoor.y - FOV.y, headcoor.x - FOV.x) * 180 / 3.1415926);
	}//1
	if (headcoor.x <= FOV.x && headcoor.y >= FOV.y)
	{
		wardangle.horizonward = 180 - (FLOAT)((double)atan2(headcoor.y - FOV.y,  FOV.x - headcoor.x) * 180 / 3.1415926);
	}//2
	if (headcoor.x <= FOV.x && headcoor.y <= FOV.y)
	{
		wardangle.horizonward = -180 + (FLOAT)((double)atan2(FOV.y - headcoor.y, FOV.x - headcoor.x) * 180 / 3.1415926);
	}//3
	if (headcoor.x >= FOV.x && headcoor.y <= FOV.y)
	{
		wardangle.horizonward = -(FLOAT)((double)atan2(FOV.y - headcoor.y, headcoor.x- FOV.x ) * 180 / 3.1415926);
	}//4

	FLOAT distance = sqrt((headcoor.x - FOV.x) * (headcoor.x - FOV.x) + (headcoor.y - FOV.y) * (headcoor.y - FOV.y));
	if (headcoor.z >= FOV.z)
	{
		wardangle.verticalward = -(FLOAT)((double)atan2(headcoor.z - FOV.z, distance) * 180 / 3.1415926);
	}
	if (headcoor.z <= FOV.z)
	{
		wardangle.verticalward = (FLOAT)((double)atan2(FOV.z - headcoor.z, distance) * 180 / 3.1415926);
	}

	angledifference.horizonward = horizonward - wardangle.horizonward;
	if (angledifference.horizonward <= -180)
	{
		angledifference.horizonward += 360;
	}
	if (angledifference.horizonward >= 180)
	{
		wardangle.horizonward -= 360;
	}

	angledifference.verticalward = wardangle.verticalward - verticalward;
}
