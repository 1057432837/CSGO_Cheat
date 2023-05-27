#pragma once
#include "Draw.h"
#include <math.h>

struct TowardStruct {
	float horizonward;
	float verticalward;
};

class AutoAim {
public:
	AutoAim(HANDLE hmoduleEngine, HANDLE hprocess);
	HANDLE hModuleEngine;
	HANDLE hProcess;

	void ToWard(worldCoor FOV, worldCoor headcoor, TowardStruct& wardangle, TowardStruct& angledifference);
};