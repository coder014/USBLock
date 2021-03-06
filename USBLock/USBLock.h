#include "Windows.h"
#include <stdio.h>
#include <set>
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
using namespace std;

typedef set<DWORD> SET;
extern HWND hWnd;
extern FILE* fp;
extern SET S;

ATOM MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID HandleDevMsg(WPARAM wParam, LPARAM lParam);
BOOL CheckMutex();
VOID SelfStart();
VOID ChangeWorkingDir();
VOID Start32Helper();
