#include "USBHook.h"

HINSTANCE gInst = NULL;
WCHAR szPath[MAX_PATH] = { 0 };

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved)
{
	gInst = hModule;

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			GetModuleFileName(NULL, szPath, MAX_PATH);
			PathStripPath(szPath);
			if (CheckExe())
				break;
			srand(time(NULL));
			DetourHookOn();
			break;
		case DLL_PROCESS_DETACH:
			if (CheckExe())
				break;
			DetourHookOff();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}