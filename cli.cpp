#include<WinSock2.h>
#include<iostream>
#include<string>
using namespace std;
#include <stdlib.h>
#define BUF_SIZE 64
#pragma comment(lib, "WS2_32.lib")

int main()
{
	WSADATA wsd;
	SOCKET sHost;
	SOCKADDR_IN servAddr;
	int retVal;
	char buf[BUF_SIZE];
	if(WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("WSAStartup failed!\n");
		return 1;
	}
	//创建监听的Socket
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(INVALID_SOCKET == sHost)
	{
		printf("socket failed!\n");
		WSACleanup();
		return -1;
	}

	//设置服务器Socket地址
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = inet_addr("192.168.1.124");
	servAddr.sin_port = htons(1298);
	int sServerAddlen = sizeof(servAddr);
	retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
	if(SOCKET_ERROR == retVal)
	{
		printf("send failed!\n");
		closesocket(sHost);
		WSACleanup();
		return -1;
	}
	while(true)
	{
		printf("input a string to send:");
		string str;
		getline(cin, str);
		ZeroMemory(buf, BUF_SIZE);
		strcpy_s(buf, str.c_str());
		retVal = send(sHost, buf, strlen(buf), 0);
		if(SOCKET_ERROR == retVal)
		{
			printf("send failed!\n");
			closesocket(sHost);
			WSACleanup();
			return -1;
		}
		//retVal = recv(sHost, buf, strlen(buf), 0);
		//printf("Recv From Server:%s\n", buf);
		if(strcmp(buf, "quit") == 0)
		{
			printf("quit!\n");
			break;
		}
	}
	return 0;
}
