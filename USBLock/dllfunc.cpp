#include <Windows.h>
#include "dllfunc.h"

HMODULE hDll;
typedef VOID(*SETHOOKON)();
typedef VOID(*SETHOOKOFF)();
typedef VOID(*SETMASK)(DWORD* mask);
typedef DWORD(*GETMASK)();

VOID hookon()
{
	SETHOOKON SetHookOn = (SETHOOKON)GetProcAddress(hDll, "SetHookOn");
	SetHookOn();
}

VOID hookoff()
{
	SETHOOKOFF SetHookOff = (SETHOOKOFF)GetProcAddress(hDll, "SetHookOff");
	SetHookOff();
}

VOID setmask(DWORD mask)
{
	SETMASK SetMask = (SETMASK)GetProcAddress(hDll, "SetMask");
	SetMask(&mask);
}

DWORD getmask()
{
	GETMASK GetMask = (GETMASK)GetProcAddress(hDll, "GetMask");
	return GetMask();
}
