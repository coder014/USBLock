#include "USBLock.h"
#include "dllfunc.h"

SET S;
FILE* fp;
HANDLE hMutex;
HWND hWnd;
const WCHAR* szTitle = L"USBLOCK";
#ifndef _WIN64
	const WCHAR *szWindowClass = L"USBLOCK32";
	#define MYMutexName L"usblock_x86_aa0a4cf58d"
	#define MYDllName L"USBHook32.dll"
#else
	const WCHAR *szWindowClass = L"USBLOCK64";
	#define MYMutexName L"usblock_x64_aa0a4cf58d"
	#define MYDllName L"USBHook.dll"
#endif

BOOL ReadConfig()
{
	FILE* config;
	if ((config = fopen("vol.inf", "r")) == NULL)
		return FALSE;
	DWORD d;
	while (fscanf(config, "%lX", &d) != EOF)
	{
		S.insert(d);
	}
	fclose(config);
	return TRUE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	SelfStart();
	ChangeWorkingDir();
	if (!CheckMutex())
		return FALSE;
	if (!ReadConfig())
		return FALSE;
	if (!MyRegisterClass(hInstance))
		return FALSE;
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
		return FALSE;
	hDll = LoadLibrary(MYDllName);
	if (!hDll)
		return FALSE;
	setmask(0UL);
	hookon();
#ifdef _WIN64
	Start32Helper();
#endif
	
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc    = WndProc;
	wcex.hInstance      = hInstance;
	wcex.lpszClassName  = szWindowClass;

	return RegisterClassEx(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DEVICECHANGE:
			HandleDevMsg(wParam, lParam);
			break;
		case WM_CLOSE:
			hookoff();
			CloseHandle(hMutex);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL CheckMutex()
{
	hMutex = CreateMutex(NULL, FALSE, MYMutexName);
	if (hMutex)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(hMutex);
			return FALSE;
		}
	}
	else
		return FALSE;
	return TRUE;
}