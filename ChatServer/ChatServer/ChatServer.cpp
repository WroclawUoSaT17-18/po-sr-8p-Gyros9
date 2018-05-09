#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <cstdio>
#pragma comment(lib, "Ws2_32.lib")
#include <cstdlib>
#include <iostream>
#include <string>
#include <winsock2.h>
#include <istream>

using namespace std;

//const string IP = "192.168.2.10";
//const char *IpAddress = IP.c_str();
//const int  Port = 3333;
const int Buffor = 128;

int main()
{
	
	string IP;
	int Port;
	cout << "Podaj IP tego komputera: ";
	getline(cin, IP);
	cout << "Podaj port na jakim ma pracowac serwer: ";
	cin >> Port;

	const char *IpAddress = IP.c_str();
	

	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR)
		printf("Blad inicjalizacji.\n");

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

	if (bind(mainSocket, (SOCKADDR *)& service, sizeof(service)) == SOCKET_ERROR)
	{
		closesocket(mainSocket);
		return 1;
	}

	if (listen(mainSocket, 1) == SOCKET_ERROR)
		printf("Blad nasluchiwania.\n");


	SOCKET acceptSocket = SOCKET_ERROR;
	printf("Oczekiwanie na uzytkownika...\n");

	while (acceptSocket == SOCKET_ERROR)
	{
		acceptSocket = accept(mainSocket, NULL, NULL);
	}

	printf("Uzywkotnik podlaczony.\n");
	mainSocket = acceptSocket;
	send(mainSocket, "OK", 2, 0);

	int bytesSent;
	int bytesRecv = SOCKET_ERROR;

	string str = "";
	char sendbuf[32] = "";
	char recvbuf[32] = "";
	while (result == 0){
		memset(recvbuf, 0, strlen(recvbuf));
		recv(mainSocket, recvbuf, 32, 0);
		printf("%s\n", recvbuf);

		memset(sendbuf, 0, strlen(sendbuf));
		//getline(cin, str);
		cin>>sendbuf;
		const char *napis = str.c_str();
		send(mainSocket, sendbuf, strlen(sendbuf), 0);
	}
	system("PAUSE");
	return 0;
}