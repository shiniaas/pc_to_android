#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

char buffer[4096] = {0};

BOOL ExecDosCmd() 
{    
#define EXECDOSCMD "adb devices" //可以换成你的命令

	SECURITY_ATTRIBUTES sa; 
	HANDLE hRead,hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	sa.lpSecurityDescriptor = NULL; 
	sa.bInheritHandle = TRUE; 
	if (!CreatePipe(&hRead,&hWrite,&sa,0)) 
	{ 
		return FALSE; 
	} 
	char command[1024];    //长达1K的命令行，够用了吧 
	strcpy(command,"Cmd.exe /C "); 
	strcat(command,EXECDOSCMD); 
	STARTUPINFO si; 
	PROCESS_INFORMATION pi; 
	si.cb = sizeof(STARTUPINFO); 
	GetStartupInfo(&si); 
	si.hStdError = hWrite;            //把创建进程的标准错误输出重定向到管道输入 
	si.hStdOutput = hWrite;           //把创建进程的标准输出重定向到管道输入 
	si.wShowWindow = SW_HIDE; 
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 
	//关键步骤，CreateProcess函数参数意义请查阅MSDN
	wchar_t wtext[1024];
	mbstowcs(wtext, command, strlen(command)+1);//Plus null
	LPWSTR ptr = wtext;
	if (!CreateProcess(NULL,ptr,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi)) 
	{ 
		CloseHandle(hWrite); 
		CloseHandle(hRead); 
		return FALSE; 
	} 
	CloseHandle(hWrite);

	          //用4K的空间来存储输出的内容，只要不是显示文件内容，一般情况下是够用了。 
	DWORD bytesRead; 
	while (true) 
	{ 
		if (ReadFile(hRead,buffer,4095,&bytesRead,NULL) == NULL) 
			break; 
		//buffer中就是执行的结果，可以保存到文本，也可以直接输出 
		printf("%s", buffer);   //这里是弹出对话框显示 
	} 
	CloseHandle(hRead); 
	return TRUE; 
}

int main(void)
{
	ExecDosCmd();
	return 0;
}