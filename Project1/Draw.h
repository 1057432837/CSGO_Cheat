#pragma once
#include <Windows.h>

struct screenCoor {
	float x, y;
};
struct worldCoor {
	float x, y, z;
};
struct clipCoor {
	float x, y, z, w;
};
struct ndcCoor {
	float x, y, z;
};

class Draw
{
public:
	Draw(HWND hwnd, HANDLE hprocess, HANDLE hmoduleClient);
public:
	HWND hWnd;
	HANDLE hProcess;
	HANDLE hModuleClient;
	RECT rect;
	int hight;
	int width;
	RECT rectEx;
	int hightEx;
	int widthEx;
private:
	void GetWindowsInfo();
public:
	void DrawLine();
	void DrawRect(HDC hdc, HBRUSH hbrush, int x, int y, int w, int a);
	void DrawBlock(HDC hdc, HBRUSH hbrush, int x, int y, int w, int h, int a);
	void DrawWord(HDC hdc, int x, int y, COLORREF color, const char *text);
	bool WorldToScreen(worldCoor &worldcoor, screenCoor &screenCoor);
	void DrawSlot(HDC hdc, HBRUSH hbrush, int x, int y, int w, int h, int hp);
	void DrawBoneLine(HDC hdc, int x1, int y1, int x2, int y2);
};

