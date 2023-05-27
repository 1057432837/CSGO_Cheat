#include <cstdio>
#include <Windows.h>
#include <Tlhelp32.h>
#include <tchar.h>
#include "Draw.h"
#include "AutoAim.h"
#include <Math.h>
#include <string>
using namespace std;

HWND hWnd = FindWindow(NULL, L"Counter-Strike: Global Offensive - Direct3D 9");
HWND hOverlayWnd;
DWORD processID;
HANDLE hProcess;
HANDLE hModuleClient;
HANDLE hModuleEngine;
HDC hdc;
HBRUSH hbrush;
HPEN hPen;

RECT winRect;

worldCoor myCoorW;
worldCoor personCoorW;
screenCoor personCoorS;

worldCoor FOV;
TowardStruct wardangle;
TowardStruct angledifference;

worldCoor headCoorW;
screenCoor headCoorS;
worldCoor neckCoorW;
screenCoor neckCoorS;

worldCoor leftShoulderCoorW;
screenCoor leftShoulderCoorS;
worldCoor rightShoulderCoorW;
screenCoor rightShoulderCoorS;
worldCoor leftHandCoorW;
screenCoor leftHandCoorS;
worldCoor rightHandCoorW;
screenCoor rightHandCoorS;
worldCoor crotchCoorW;
screenCoor crotchCoorS;


DWORD GetProcessID(const wchar_t* processName) {
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcess == INVALID_HANDLE_VALUE) {
		return FALSE;
	};
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	BOOL bRet = Process32First(hProcess, &pe);
	while (bRet)
	{
		if (!_tcscmp(pe.szExeFile, processName)) {
			return pe.th32ProcessID;
		}
		bRet = Process32Next(hProcess, &pe);
	}
	return FALSE;
}

HANDLE GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName) {
	MODULEENTRY32 moduleEntry;
	HANDLE handle = NULL;
	handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (!handle)
	{
		CloseHandle(handle);
		return NULL;
	}
	ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(handle, &moduleEntry))
	{
		CloseHandle(handle);
		return NULL;
	}
	do
	{
		if (_tcscmp(moduleEntry.szModule, moduleName) == 0) {
			return moduleEntry.hModule;
		}
	} while (Module32Next(handle, &moduleEntry));
	CloseHandle(handle);
	return 0;
}


void InitData() {
	processID = GetProcessID(L"csgo.exe");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	hModuleClient = GetProcessModuleHandle(processID, L"client.dll");
	hModuleEngine = GetProcessModuleHandle(processID, L"engine.dll");
}



void UploadData() {
	HWND hForegroundWnd = GetForegroundWindow();
	RECT overlayRect = { 0, 0, winRect.right - winRect.left, winRect.bottom - winRect.top };
	if (hForegroundWnd == hOverlayWnd)
	{
		return;
	}

	hdc = GetDC(hOverlayWnd);
	hbrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	//SelectObject(hdc, hbrush);
	FillRect(hdc, &overlayRect, hbrush);
	DeleteObject(hbrush);
	//ReleaseDC(hOverlayWnd, hdc);
	
	MoveWindow(hOverlayWnd, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top, TRUE);



	int personListAddr = (int)(hModuleClient)+0x4DFEF0C;
	int personStructAddr;
	int myTeam;
	int enemyTeam;
	int hp;
	int flag = 0;
	int darmant = 0;
	int angletemp = 70;
	int anglefinal = 70;

	worldCoor aimobjectworldcoor;
	TowardStruct aimwardangle;

	Draw draw(hOverlayWnd, hProcess, hModuleClient);
	AutoAim autoaim(hModuleEngine, hProcess);

	while (true)
	{
		ReadProcessMemory(hProcess, (LPCVOID)personListAddr, &personStructAddr, 4, NULL);
		
		if (flag == 0)
		{
			ReadProcessMemory(hProcess, (LPCVOID)(personStructAddr + 0xa0), &myCoorW, 12, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(personStructAddr + 0xf4), &myTeam, 4, NULL);
			
			int tmpAddr;
			ReadProcessMemory(hProcess, (LPCVOID)(personStructAddr + 0x26a8), &tmpAddr, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x18c), &FOV.x, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x19c), &FOV.y, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x1ac), &FOV.z, 4, NULL);

			flag += 1;
			personListAddr += 0x10;
			continue;
		}
		flag += 1;
		if (personStructAddr == 0x00)
		{

			break;
		}
		else
		{
			ReadProcessMemory(hProcess, (LPCVOID)(personStructAddr + 0xf4), &enemyTeam, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(personStructAddr + 0x100), &hp, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(personStructAddr + 0xED), &darmant, 1, NULL);

			if (hp <= 0 || enemyTeam == myTeam || darmant == 1)
			{
				personListAddr += 0x10;
				continue;
			}

			ReadProcessMemory(hProcess, (LPCVOID)(personStructAddr + 0xa0), &personCoorW, 12, NULL);
			int tmpAddr;
			ReadProcessMemory(hProcess, (LPCVOID)(personStructAddr + 0x26a8), &tmpAddr, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x18c), &headCoorW.x, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x19c), &headCoorW.y, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x1ac), &headCoorW.z, 4, NULL);


			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0xc), &neckCoorW.x, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x1c), &neckCoorW.y, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x2c), &neckCoorW.z, 4, NULL);

			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x7bc), &leftShoulderCoorW.x, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x7cc), &leftShoulderCoorW.y, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x7dc), &leftShoulderCoorW.z, 4, NULL);

			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x87c), &leftHandCoorW.x, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x88c), &leftHandCoorW.y, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x89c), &leftHandCoorW.z, 4, NULL);

			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x21c), &rightShoulderCoorW.x, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x22c), &rightShoulderCoorW.y, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x23c), &rightShoulderCoorW.z, 4, NULL);

			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x2dc), &rightHandCoorW.x, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x2ec), &rightHandCoorW.y, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x2fc), &rightHandCoorW.z, 4, NULL);

			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0xc), &crotchCoorW.x, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x1c), &crotchCoorW.y, 4, NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(tmpAddr + 0x2c), &crotchCoorW.z, 4, NULL);



			if (draw.WorldToScreen(personCoorW, personCoorS))
			{
				if (draw.WorldToScreen(headCoorW, headCoorS))
				{
					hbrush = CreateSolidBrush(RGB(255, 255, 255));
					float height = personCoorS.y - headCoorS.y;
					float width = height / 2;

					hPen = CreatePen(PS_SOLID, 0.2, RGB(128, 0, 0));
					SelectObject(hdc, hPen);

					draw.DrawBlock(hdc, hbrush, (int)(headCoorS.x - width / 2), (int)(headCoorS.y - 5), (int)width, (int)height + 5, 1);
					draw.DrawSlot(hdc, hbrush, (int)(headCoorS.x - width / 2) - 5, (int)(headCoorS.y - 5), 2, (int)height + 5, hp);
					


					draw.WorldToScreen(neckCoorW, neckCoorS);
					draw.WorldToScreen(leftShoulderCoorW, leftShoulderCoorS);
					draw.WorldToScreen(leftHandCoorW, leftHandCoorS);
					draw.WorldToScreen(rightShoulderCoorW, rightShoulderCoorS);
					draw.WorldToScreen(rightHandCoorW, rightHandCoorS);
					draw.WorldToScreen(crotchCoorW, crotchCoorS);


					draw.DrawBoneLine(hdc, headCoorS.x, headCoorS.y, neckCoorS.x, neckCoorS.y);
					draw.DrawBoneLine(hdc, neckCoorS.x, neckCoorS.y, leftShoulderCoorS.x, leftShoulderCoorS.y);
					draw.DrawBoneLine(hdc, leftShoulderCoorS.x, leftShoulderCoorS.y, leftHandCoorS.x, leftHandCoorS.y);
					draw.DrawBoneLine(hdc, neckCoorS.x, neckCoorS.y, rightShoulderCoorS.x, rightShoulderCoorS.y);
					draw.DrawBoneLine(hdc, rightShoulderCoorS.x, rightShoulderCoorS.y, rightHandCoorS.x, rightHandCoorS.y);
					draw.DrawBoneLine(hdc, neckCoorS.x, neckCoorS.y, crotchCoorS.x, crotchCoorS.y);


					float distance = sqrt((myCoorW.x - personCoorW.x) * (myCoorW.x - personCoorW.x) + (myCoorW.y - personCoorW.y) * (myCoorW.y - personCoorW.y) + (myCoorW.z - personCoorW.z) * (myCoorW.z - personCoorW.z));
					char hpChar[255];
					_itoa_s(distance / 100, hpChar, 10);
					string hpStr = hpChar;
					draw.DrawWord(hdc, (int)personCoorS.x, (int)personCoorS.y, RGB(255, 255, 255), hpStr.c_str());
					
					autoaim.ToWard(FOV, headCoorW, wardangle, angledifference);
					if (fabs(angledifference.horizonward) + fabs(angledifference.verticalward) < 70)
					{
						angletemp = fabs(angledifference.horizonward) + fabs(angledifference.verticalward);
						if (anglefinal > angletemp)
						{
							anglefinal = angletemp;
							aimobjectworldcoor = personCoorW;
							aimwardangle = wardangle;
						}
					}
					
					DeleteObject(hPen);
					DeleteObject(hbrush);
				}
			}
			personListAddr += 0x10;
		}

		

	}
	if (anglefinal < 70)
	{
		int tempAddr;
		ReadProcessMemory(hProcess, (LPCVOID)((int)hModuleEngine + 0x59F19C), &tempAddr, 4, NULL);
		WriteProcessMemory(hProcess, (LPVOID)(tempAddr + 0x4d90), &aimwardangle.verticalward, 4, NULL);
		WriteProcessMemory(hProcess, (LPVOID)(tempAddr + 0x4d94), &aimwardangle.horizonward, 4, NULL);
	}
	Sleep(30);
	DeleteObject(hdc);
}

LRESULT OverWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_PAINT:
		UploadData();
		return 0;
	case WM_CREATE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void CreateOverlayWindow() {

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	GetWindowRect(hWnd, &winRect);
	winRect.top += 25;

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)OverWndMsgProc;

	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	wc.lpszClassName = L"OverlayWindow";

	RegisterClassEx(&wc);

	hOverlayWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, L"over", WS_POPUP, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top, NULL, NULL, wc.hInstance, NULL);
	if (hOverlayWnd == NULL) {
		return;
	}

	SetLayeredWindowAttributes(hOverlayWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	ShowWindow(hOverlayWnd, SW_SHOW);
	UpdateWindow(hOverlayWnd);

	while (msg.message != WM_QUIT) {
		
		SetWindowPos(hOverlayWnd, HWND_TOPMOST, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top, SWP_SHOWWINDOW);
		GetWindowRect(hWnd, &winRect);
		if (PeekMessage(&msg, hOverlayWnd, 0, 0, PM_NOREMOVE)) {
			if (GetMessage(&msg, hOverlayWnd, 0, 0)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				return;
			}

		}
		else
		{
			return;
		}
//		Sleep(30);
	}

	ReleaseDC(hOverlayWnd, hdc);
}

void main() {
	InitData();
	//SetTimer(FindWindow(NULL, L"over"), 1, 40, UploadData);
	CreateOverlayWindow();
}