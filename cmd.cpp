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
#define EXECDOSCMD "adb devices" //���Ի����������

	SECURITY_ATTRIBUTES sa; 
	HANDLE hRead,hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	sa.lpSecurityDescriptor = NULL; 
	sa.bInheritHandle = TRUE; 
	if (!CreatePipe(&hRead,&hWrite,&sa,0)) 
	{ 
		return FALSE; 
	} 
	char command[1024];    //����1K�������У������˰� 
	strcpy(command,"Cmd.exe /C "); 
	strcat(command,EXECDOSCMD); 
	STARTUPINFO si; 
	PROCESS_INFORMATION pi; 
	si.cb = sizeof(STARTUPINFO); 
	GetStartupInfo(&si); 
	si.hStdError = hWrite;            //�Ѵ������̵ı�׼��������ض��򵽹ܵ����� 
	si.hStdOutput = hWrite;           //�Ѵ������̵ı�׼����ض��򵽹ܵ����� 
	si.wShowWindow = SW_HIDE; 
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 
	//�ؼ����裬CreateProcess�����������������MSDN
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

	          //��4K�Ŀռ����洢��������ݣ�ֻҪ������ʾ�ļ����ݣ�һ��������ǹ����ˡ� 
	DWORD bytesRead; 
	while (true) 
	{ 
		if (ReadFile(hRead,buffer,4095,&bytesRead,NULL) == NULL) 
			break; 
		//buffer�о���ִ�еĽ�������Ա��浽�ı���Ҳ����ֱ����� 
		printf("%s", buffer);   //�����ǵ����Ի�����ʾ 
	} 
	CloseHandle(hRead); 
	return TRUE; 
}

int main(void)
{
	ExecDosCmd();
	return 0;
}