#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <cstdio>
#pragma comment(lib, "Ws2_32.lib")
#include <cstdlib>
#include <string>
#include <iostream>
#include <istream>
#include <winsock2.h>
#include <stdlib.h>

using namespace std;

const string IP = "192.168.2.10";
const int  Port = 3333;

int main()
{
	
	string IP, Login = "";
	int Port;
	cout << "Podaj IP serwera: ";
	getline(cin, IP);
	cout << "Podaj port na jakim pracuje serwer: ";
	cin >> Port;


	const char *IpAddress = IP.c_str();

	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR)
		printf("Blat inicjalizacji.\n");

	SOCKET mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mainSocket == INVALID_SOCKET)
	{
		printf("Blad tworzenia socketu: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	sockaddr_in service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(IpAddress);
	service.sin_port = htons(Port);

	if (connect(mainSocket, (SOCKADDR *)& service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("Blad polaczenia.\n");
		WSACleanup();
		return 1;
	}

	string str;
	int bytesSent;
	int bytesRecv = SOCKET_ERROR;
	char sendbuf[32] = "";
	char recvbuf[32] = "";

	while (result == 0){

		memset(sendbuf, 0, strlen(sendbuf));
		getline(cin,str);
		const char *napis = str.c_str();
		send(mainSocket, napis, strlen(napis), 0);

		bytesRecv = recv(mainSocket, recvbuf, 32, 0);

		printf("%s\n", recvbuf);
		
		memset(recvbuf, 0, strlen(recvbuf));
	}
	system("PAUSE");
	return 0;
}