#pragma comment(lib,"pthreadVSE2.lib")

#include <pthread.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/pointer.h"
#include "rapidjson/allocators.h"
#include "rapidjson/rapidjson.h"
using namespace std;
#pragma comment(lib, "WS2_32.lib")
#define BUF_SIZE 2000
char buf[BUF_SIZE];
char buffer[4096] = {0};
vector <string> PhoneNum;
vector <pthread_t *> pThreads;
vector <int> iPorts;
string str;

const char *JsonCmd[6] = { "0","1","2","3","4","5" };
const char *JsonCode[5]=
{
	"0","101","102","103","104"
};

typedef struct Phone
{
	string sId;
	struct Phone* pnext;
}Phone;
class  MessgSolver
{
public:
#define SUCCESS 0
#define FAULT_WIFI_NAME_UNFOUND 101
#define FAULT_WIFI_CONNECT 102
#define FAULT_SPEEDPRO_UNFOUND 103
#define APP_CMD_CONNECT 1
#define APP_CMD_WIFI_CONNECT 2
#define APP_CMD_OPEN_SPEEDPRO 3
#define APP_CMD_PLAY 4
#define APP_CMD_PLAY_RESULT 5

#define CODE_BASE 100
    
    string SendControlMesg(int cmd)
    {
        rapidjson::Document d;
        rapidjson::Pointer("/cmd").Set(d,JsonCmd[cmd]);
        if(cmd == APP_CMD_WIFI_CONNECT)
        {
            char username[50],password[50];
            printf("please input wifi username:\n");
            cin>>username;
            printf("please input wifi password:\n");
            cin>>password;
            rapidjson::Pointer("/data/username").Set(d,username);
            rapidjson::Pointer("/data/password").Set(d,password);
        }
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);
        string ret=(buffer.GetString());
        return ret;
    }
    int ProcessMesg(char* json)
    {
        rapidjson::Document doc;
        doc.Parse<0>(json);
        if(doc.HasParseError())
        {
            printf("message format error!\n");
            return 0;
        }
        int cmd = doc["cmd"].GetInt();
        int code = doc["code"].GetInt();
        if(cmd == APP_CMD_CONNECT)
        {
            if(code == SUCCESS)
            {
                printf("connect success.\n");
            }
            else
            {
                printf("connect fail!\n");
            }
        }
        else if(cmd == APP_CMD_WIFI_CONNECT)
        {
            if(code == SUCCESS)
            {
                printf("wifi connect success.\n");
            }
            else if(code == FAULT_WIFI_CONNECT)
            {
                printf("wifi connect fail!\n");
            }
            else if(code == FAULT_WIFI_NAME_UNFOUND)
            {
                printf("wifi name unfound fail!.\n");
            }

        }
        else if(cmd == APP_CMD_OPEN_SPEEDPRO)
        {
            if(code == SUCCESS)
            {
                printf("speedpro open success.\n");
            }
            else
            {
                printf("speedpro open fail!\n");
            }
        }
        else if(cmd == APP_CMD_PLAY)
        {
            if(code == SUCCESS)
            {
                printf("video plays success!\n");
            }
            else
            {
                printf("video plays fail!\n");
            }
        }
        else if(cmd == APP_CMD_PLAY_RESULT)
        {
            if(code == SUCCESS)
            {
                printf("receive data success.\n");
            }
            else
            {
                printf("receive data fail!!\n");
            }
        }
        else
        {
            printf("command error!\n");
        }
        return cmd;
    }
};




Phone *Phead = NULL;

BOOL ExecDosCmd(const char *pcCmd)
{

	SECURITY_ATTRIBUTES sa;
	HANDLE hRead,hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead,&hWrite,&sa,0))
	{
		return FALSE;
	}
	char command[1024];
	strcpy(command, "Cmd.exe /C ");
	strcat(command, pcCmd);
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

	DWORD bytesRead;
	ZeroMemory(buffer, sizeof(buffer));
	while (true)
	{
		if (ReadFile(hRead,buffer,4095,&bytesRead,NULL) == NULL)
			break;
	}
	CloseHandle(hRead);
	return TRUE;
}

BOOL GetDevicesNum()
{
	string sCmd = "adb devices";
	if(!ExecDosCmd(sCmd.c_str()))
	{
		return FALSE;
	}
	istringstream is(buffer);
	string s1, s2;
	while(is >> s2)
	{
		if(s2 == "device")
		{
			PhoneNum.push_back(s1);
		}
		s1 = s2;
	}
	return TRUE;
}

BOOL SetPort(int iPort, string PhoneNum)
{
	string sCmd;
	string sPort;
	stringstream ss;
	ss.str("");
	ss<<iPort;
	sPort = ss.str();
	sCmd = "adb -s " + PhoneNum + " forward tcp:" +sPort + " tcp:9000";
	cout << sCmd << endl;
	ExecDosCmd(sCmd.c_str());
	return TRUE;
}

BOOL StartApp()
{
	int i = 0;
	int j = PhoneNum.size();
	string sCmd;
	for(i = 0; i < j; i++)
	{
		sCmd = "adb -s " + PhoneNum[i] + " shell am start -n com.mobile.dian.huaweiwifitest/.activity.MainActivity";
		ExecDosCmd(sCmd.c_str());
	}
	return TRUE;
}

BOOL StartSer()
{
	int i = 0;
	int j = PhoneNum.size();
	string sCmd;
	for(i = 0; i < j; i++)
	{
		sCmd = "adb -s " + PhoneNum[i] + " shell am broadcast -a NotifyServiceStart";
		ExecDosCmd(sCmd.c_str());
	}
	return TRUE;
}

BOOL StopSer()
{
	int i = 0;
	int j = PhoneNum.size();
	string sCmd;
	for(i = 0; i < j; i++)
	{
		sCmd = "adb -s " + PhoneNum[i] + " shell am broadcast -a NotifyServiceStop";
		ExecDosCmd(sCmd.c_str());
	}
	return TRUE;
}

void* Create_Socket(void *pPort)
{
	int iPort = *((int*)pPort);
	WSADATA wsd;
	SOCKET sHost;
	SOCKADDR_IN servAddr;
	int retVal;

	if(WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("WSAStartup failed!\n");
		return NULL;
	}
	//创建监听的Socket
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(INVALID_SOCKET == sHost)
	{
		printf("socket failed!\n");
		WSACleanup();
		return NULL;
	}

	//设置服务器Socket地址
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(iPort);
	int sServerAddlen = sizeof(servAddr);
	retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
	if(SOCKET_ERROR == retVal)
	{
		printf("send failed1!\n");
		closesocket(sHost);
		WSACleanup();
		return NULL;
	}
	while(true)
	{
		ZeroMemory(buf, BUF_SIZE);
		strcpy_s(buf, str.c_str());
		retVal = send(sHost, buf, strlen(buf), 0);
		if(SOCKET_ERROR == retVal)
		{
			printf("send failed2!\n");
			closesocket(sHost);
			WSACleanup();
			return NULL;
		}
	}
}

void* Input_string(void*)
{
    printf("-----------Menu--------------\n");
    printf("1.connect mobile.\n");
    printf("2.connect wifi.\n");
    printf("3.open speedpro.\n");
    printf("4.play video.\n");
    printf("5.quit.\n");
    printf("-----------Menu--------------\n");
    MessgSolver *solver=new class MessgSolver();
	while(1)
	{
		/*char buf[2000];
        printf("android seed json:\n");
        cin>>buf;
        solver->ProcessMesg(buf);*/
        printf("input a number to send:\n");
        cin>>str;
        if(str=="1")
        {
            str=solver->SendControlMesg(1);
			str+='\n';
        }
        else if(str=="2")
        {
            str=solver->SendControlMesg(2);
			str+='\n';
        }
        else if(str=="3")
        {
            str=solver->SendControlMesg(3);
			str+='\n';
        }
        else if(str=="4")
        {
            str=solver->SendControlMesg(4);
			str+='\n';
        }
        else if(str=="5")
        {
            break;
        }
        else
        {
            printf("input error.\n");
            continue;
        }
		cout<<"send to android: "<<str<<endl;
	}
	return NULL;
}

int main()
{
	int i = 0;
	int iPort = 5000;
	GetDevicesNum();
	int j = PhoneNum.size();
	cout << j;
	for(i = 0; i < j; i++)
	{
		SetPort(iPort, PhoneNum[i]);
		iPort += 1;
	}
	StartApp();
	StartSer();
	int Device_count = PhoneNum.size();
	for(i = 0; i < Device_count; i++)
	{
		iPorts.push_back(i+5000);
	}
	pthread_t pIthread;
	pthread_create(&pIthread, NULL, Input_string, NULL);
	pthread_t *p;
	for(i = 0; i < Device_count; i++)
	{
		p = (pthread_t *)malloc(sizeof(pthread_t));
		if(!p)
		{
			printf("Malloc Error");
			exit(-1);
		}
		pthread_create(p, NULL, Create_Socket, (void*)&iPorts[i]);
		pThreads.push_back(p);
	}
	for(i = 0; i < Device_count; i++)
	{
		pthread_join(*(pThreads[i]), NULL);
	}
	return 0;
}
