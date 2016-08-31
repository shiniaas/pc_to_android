#include <iostream>
#include <WinSock2.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#define BUF_SIZE 64
#pragma comment(lib, "WS2_32.lib")

int main(void)
{	
	char cmd[256] = "adb devices";
	system(cmd);

	WSADATA wsd;
	SOCKET sServer;
	SOCKET sClient;
	int retVal;
	char buf[BUF_SIZE];

	//��ʼ��Socket����
	if(WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		printf("WSAStartup failed!\n");
		return 1;
	}

	//����������Socket
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(INVALID_SOCKET == sServer)
	{
		printf("socket failed!\n");
		WSACleanup();
		return -1;
	}

	//���÷�����Socket��ַ
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET;
	addrServ.sin_port=htons(1298);
	addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//��Sockets Server
	retVal = bind(sServer, (const struct sockaddr*)&addrServ, sizeof(SOCKADDR_IN));
	if(SOCKET_ERROR == retVal)
	{
		printf("bind failed!\n");
		closesocket(sServer);
		WSACleanup();
		return -1;
	}

	//��Sockets Server�Ͻ��м���
	retVal = listen(sServer, 1);
	if(SOCKET_ERROR == retVal)
	{
		printf("listen failed!\n");
		closesocket(sServer);
		WSACleanup();
		return -1;
	}

	//�������Կͻ��˵�����
	printf("TCPServer start...\n");
	sockaddr_in addrClient;
	int addrClientlen = sizeof(addrClient);
	sClient = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
	if(INVALID_SOCKET == sClient)
	{
		printf("accept failed!\n");
		closesocket(sServer);
		WSACleanup();
		return -1;
	}

	//ѭ�����տͻ��˵����ݣ�ֱ���ͻ��˷���quit������˳�
	while(true)
	{
		ZeroMemory(buf, BUF_SIZE);
		retVal = recv(sClient, buf, BUFSIZ, 0);
		if(SOCKET_ERROR == retVal)
		{
			printf("recv failed!\n");
			closesocket(sServer);
			closesocket(sClient);
			WSACleanup();
			return -1;
		}

		//��ȡ��ǰϵͳʱ��
		SYSTEMTIME st;
		GetLocalTime(&st);
		char sDateTime[30];
		sprintf_s(sDateTime, "%4d-%2d-%2d-%2d:%2d:%2d", st.wYear, st.wMonth, st.wDay,st.wHour, st.wMinute, st.wSecond);
		
		//��ӡ�������Ϣ
		printf("%s, Recv From Client [%s:%d]:%s\n", sDateTime, inet_ntoa(addrClient.sin_addr), addrClient.sin_port, buf);

		//����ͻ��˷���quit���������˳�
		if(strcmp(buf, "quit") == 0)
		{
			retVal = send(sClient, "quit", strlen("quit"), 0);
			break;
		}
		else
		{
			char msg[BUF_SIZE];
			sprintf_s(msg, "Message received - %s\n", buf);
			//retVal = send(sClient, msg, strlen(msg), 0);
			if(SOCKET_ERROR == retVal)
			{
				printf("send failed!\n");
				closesocket(sServer);
				closesocket(sClient);
				WSACleanup();
				return -1;
			}
			//�ͷ�Socket
			/*closesocket(sServer);
			closesocket(sClient);
			WSACleanup();
			system("pause");
			return 0;*/
		}
	}
	closesocket(sServer);
	closesocket(sClient);
	WSACleanup();
	system("pause");
	return 0;
}