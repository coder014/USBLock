#include "USBLock.h"
#include "dllfunc.h"
#include <Dbt.h>
#include <time.h>

VOID SelfStart()
{
#ifdef _WIN64
	BOOL isWOW64;
	REGSAM rs = KEY_ALL_ACCESS;
	HKEY hKey;
	WCHAR wszPath[MAX_PATH];
	GetModuleFileName(NULL, wszPath, MAX_PATH);
	PathQuoteSpaces(wszPath);
	IsWow64Process(GetCurrentProcess(), &isWOW64);
	if (isWOW64) {
		rs |= KEY_WOW64_64KEY;
	}
	if (RegCreateKeyEx(
		HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		0, NULL, 0, rs, NULL, &hKey, NULL) != ERROR_SUCCESS)
		return;
	if (RegQueryValueEx(hKey, L"USBPro",
		0, NULL, NULL, NULL) == ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return;
	}
	RegSetValueEx(hKey, L"USBPro",
		0, REG_SZ,
		(BYTE*)wszPath, (wcslen(wszPath) + 1) * sizeof(WCHAR));
	RegCloseKey(hKey);
#endif
}

VOID ChangeWorkingDir()
{
	WCHAR wszDir[MAX_PATH];
	GetModuleFileName(NULL, wszDir, MAX_PATH);
	PathRemoveFileSpec(wszDir);
	SetCurrentDirectory(wszDir);
}

VOID DoCheck(const int vol)
{
	WCHAR wszVol[16];
	DWORD SN;
	DWORD mask = getmask();
	wsprintf(wszVol, L"%c:\\\\", (L'A' + vol));
	GetVolumeInformation(wszVol, NULL, 0, &SN, NULL, NULL, NULL, 0);
	if (S.find(SN) != S.end())
		setmask(mask | (1UL << vol));
	else if (mask & (1UL << vol))
		setmask(mask & (~(1UL << vol)));
}

VOID HandleDevMsg(WPARAM wParam, LPARAM lParam)
{
	if ((wParam != DBT_DEVICEARRIVAL) && (wParam != DBT_DEVICEREMOVECOMPLETE))
		return;

	DEV_BROADCAST_HDR *pdev = (DEV_BROADCAST_HDR *)lParam;
	DEV_BROADCAST_VOLUME *pdevmore = NULL;
	if (pdev->dbch_devicetype == DBT_DEVTYP_VOLUME)
		pdevmore = (DEV_BROADCAST_VOLUME *)lParam;

	if (pdevmore)
	{
		int vol = 0;
		DWORD mask = pdevmore->dbcv_unitmask;
		while (mask)
		{
			if (mask & 1)
			{
				if (wParam == DBT_DEVICEARRIVAL)
					DoCheck(vol);
				else
				{
					setmask(getmask() & (~(1UL << vol)));
				}
			}
			++vol;
			mask >>= 1;
		}
	}
}

VOID Start32Helper()
{
	WinExec("UHelper32.exe", SW_HIDE);
}