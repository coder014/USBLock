#include <Windows.h>
#include <Shlwapi.h>
#include <stdlib.h>
#include <time.h>
#pragma comment(lib,"shlwapi.lib")

extern HINSTANCE gInst;
extern WCHAR szPath[MAX_PATH];
extern "C" __declspec(dllexport) VOID SetHookOn();
extern "C" __declspec(dllexport) VOID SetHookOff();
extern "C" __declspec(dllexport) VOID SetMask(DWORD *mask);
extern "C" __declspec(dllexport) DWORD GetMask();

LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam);
VOID DetourHookOn();
VOID DetourHookOff();
BOOL CheckExe();

HANDLE WINAPI DetourCreateFileW(
	LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile);
HANDLE WINAPI DetourCreateFileA(
	LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile);
static HANDLE(WINAPI * OriginCreateFileW)(
	LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile) = CreateFileW;
static HANDLE(WINAPI * OriginCreateFileA)(
	LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile) = CreateFileA;