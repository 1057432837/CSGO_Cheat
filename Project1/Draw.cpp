#include "Draw.h"

Draw::Draw(HWND hwnd, HANDLE hprocess, HANDLE hmoduleClient)
{
	hWnd = hwnd;
	hProcess = hprocess;
	hModuleClient = hmoduleClient;
}

void Draw::GetWindowsInfo()
{
	GetClientRect(hWnd, &rect);
	width = rect.right - rect.left;
	hight = rect.bottom - rect.top;

	GetWindowRect(hWnd, &rectEx);
	widthEx = rectEx.right - rectEx.left;
	hightEx = rectEx.bottom - rectEx.top;
}

void Draw::DrawRect(HDC hdc, HBRUSH hbrush, int x, int y, int w, int a)
{
	RECT rect = {x, y, x + w, y + a};
	FillRect(hdc, &rect, hbrush);
}

void Draw::DrawBlock(HDC hdc, HBRUSH hbrush, int x, int y, int w, int h, int a)
{
	DrawRect(hdc, hbrush, x, y, w, a);
	DrawRect(hdc, hbrush, x, y + a, a, h - a);
	DrawRect(hdc, hbrush, x + w - a, y + a, a, h - a);
	DrawRect(hdc, hbrush, x + a, y + h - a, w - a * 2, a);
}

HFONT family;
void Draw::DrawWord(HDC hdc, int x, int y, COLORREF color, const char* text)
{
	SetTextAlign(hdc, TA_CENTER | TA_NOUPDATECP);
	SetBkColor(hdc, RGB(0, 0, 0));
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, color);
	SelectObject(hdc, family);
	TextOutA(hdc, x, y, text, strlen(text));
}

bool Draw::WorldToScreen(worldCoor& worldcoor, screenCoor& screencoor)
{
	GetWindowsInfo();
	float matrix[4][4];
	//0x4DF0D24 ��������ֳɵľ���ƫ����   
	// ��β�����Ǯ �������������� �õ� 4DFFEF4 ?????
	//�����ݶ������� ƾʲô���Գɹ����� ���ҵĻ������Է������Ǹ�����Ҳ��֪����
	//������û���� ������ݶ�����һ�¾����� ������Ӫ��־λ֮��� ���������ҵĻ����ڵ����ڶ��ڽ��� ������github�����
	//��Ŀ����
	ReadProcessMemory(hProcess, (LPCVOID)((int)hModuleClient + 0x4DFEECC), &matrix, 64, NULL);

	clipCoor clipcoor;
	clipcoor.x = matrix[0][0] * worldcoor.x + matrix[0][1] * worldcoor.y + matrix[0][2] * worldcoor.z + matrix[0][3];
	clipcoor.y = matrix[1][0] * worldcoor.x + matrix[1][1] * worldcoor.y + matrix[1][2] * worldcoor.z + matrix[1][3];
	clipcoor.z = matrix[2][0] * worldcoor.x + matrix[2][1] * worldcoor.y + matrix[2][2] * worldcoor.z + matrix[2][3];
	clipcoor.w = matrix[3][0] * worldcoor.x + matrix[3][1] * worldcoor.y + matrix[3][2] * worldcoor.z + matrix[3][3];
	
	if (clipcoor.w < 0.01) {
		return false;
	}

	ndcCoor ndccoor;
	ndccoor.x = clipcoor.x / clipcoor.w;
	ndccoor.y = clipcoor.y / clipcoor.w;
	ndccoor.z = clipcoor.z / clipcoor.w;


	
	screencoor.x = (width / 2) + (width / 2) * ndccoor.x;
	screencoor.y = (hight / 2) - (hight / 2) * ndccoor.y;


	return true;
}

void Draw::DrawSlot(HDC hdc, HBRUSH hbrush, int x, int y, int w, int h, int hp)
{
	//RECT rect = { x, y,  x + w, y + h };
	//FillRect(hdc, &rect, hbrush);


	HBRUSH hbrush2;
	if (hp > 50) {
		hbrush2 = CreateSolidBrush(RGB(53, 169, 36));
	}
	else if(hp > 20 && hp <= 50){
		hbrush2 = CreateSolidBrush(RGB(255, 165, 0));
	} else
	{
		hbrush2 = CreateSolidBrush(RGB(128, 0, 0));
	}
	RECT rectBlood = { x, h * (100 - hp) / 100 + y, x + w, y + h};
	FillRect(hdc, &rectBlood, hbrush2);
	DeleteObject(hbrush2);
}

void Draw::DrawBoneLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	GetWindowsInfo();
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}
