// HookProc.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"

#include<stdio.h>
#include<process.h>
#include"HookProc.h"

#pragma comment(lib,"user32.lib")
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )//隐藏窗口

#define WH_KEYBOARD_LL 13

HHOOK MyHook;                  //接收由SetWindowsHookEx返回的旧的钩子

//char str1[20];
CString strProc1, strProc2;
//

//主函数
int main(int argc, _TCHAR* argv[])
{
	//安装钩子
	MyHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)&KeyboardProc, GetModuleHandle(NULL), NULL);

	//printf("请输入进程名：");
	//scanf_s("%s", str1);
	strProc1 = "firefox";
	strProc2 = "studio";

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != -1)                   //消息循环
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	};

	UnhookWindowsHookEx(MyHook);
	return 0;
}



//回调函数，用于处理截获的按键消息
int CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	UINT fuModifiers = (UINT)LOWORD(lParam);  // key-modifier flags 
	UINT uVirtKey = (UINT)HIWORD(lParam);     // virtual-key code 


	if (code >= HC_ACTION && wParam == WM_KEYDOWN) //有键按下
	{
		DWORD vk_code = ((KBDLLHOOKSTRUCT2*)lParam)->vkCode;
		//DWORD vk_code = ((KBDLLHOOKSTRUCT*)lParam)->vkCode;
		if (vk_code == 0x5b && ((GetAsyncKeyState(VK_LCONTROL) & 0x8000)))   // CTRL=0X43、LWIN=0X5B、DEL=0X2E 键值
		{
			printf("成功监听到 CTRL+WIN 组合键\n");
			KillProcess(strProc1);
			KillProcess(strProc2);
		}
		//printf("lParam = %d code = %d HC_ACTION = %d WM_KEYDOWN=%d wParam = %d vk_code = %d\n", lParam, code, HC_ACTION, WM_KEYDOWN, wParam, vk_code);
		//BYTE ks[256];
		//GetKeyboardState(ks);
		//WORD w;
		//ToAscii(vk_code, 0, ks, &w, 0);
		//char ch = char(w);
		//log(ch);
		//printf("%d ",vk_code);         //输出按键信息，注意这里按下和弹起都会输出
	}
	return CallNextHookEx(MyHook, code, wParam, lParam);  //将消息还给钩子链，不要影响别人
}

/*
//记录到文件
int log(char vkcode){
	FILE *fl;
	fl = fopen("log.txt", "a+");
	if (vkcode == 13)
		fwrite("\r\n", 1, 2, fl);//注意此处 count=2  
	else
		fwrite(&vkcode, sizeof(char), 1, fl);//把按键字符 记录到文件
	//printf("write ok\n");
	fclose(fl);
	return 0;
}
*/

/*
可进行调试，查看进程的名字
在需要关闭进程的地方调用KillProcess(_T("要关闭的进程的名字"));即可。
*/

void KillProcess(CString sExeName)
{

	//CreateToolhelp32Snapshot()得到系统进程的一个快照
	HANDLE hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);


	if (hSnapShot == 0)
		return;
	PROCESSENTRY32 thePE;
	thePE.dwSize = sizeof(PROCESSENTRY32);


	//遍历正在运行的第一个系统进程
	bool Status = (BOOL)Process32First(hSnapShot, &thePE);

	bool bHaveFlag = false; //找到进程名状态
	DWORD ProcessID = 0;

	while (Status)
	{
		//遍历正在运行的下一个系统进程  
		Status = (BOOL)Process32Next(hSnapShot, &thePE);

		//找到相应的进程 **.exe
		// if(0 == wcscmp(thePE.szExeFile,L""))
		CString sFindName = thePE.szExeFile;//进程中的进程名
		CString sTemp = sExeName.Mid(0, sFindName.GetLength()+1);//要关闭的进程名


		if (sFindName.Find(sTemp) != -1)
		{
			bHaveFlag = true;
			ProcessID = thePE.th32ProcessID;

			//结束指定的进程 ProcessID
			if (!TerminateProcess(OpenProcess
				(PROCESS_TERMINATE || PROCESS_QUERY_INFORMATION, false, ProcessID), 0))

			{
				printf_s("无法终止指定的进程！");
			}
			else
			{
				printf_s("进程结束！");
				break;
			}
		}
	}
	CloseHandle(hSnapShot);
}

//原文链接https://blog.csdn.net/laiyinping/article/details/39493457
