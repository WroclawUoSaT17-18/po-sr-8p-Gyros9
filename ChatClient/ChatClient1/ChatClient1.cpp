#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <winsock2.h>
#include <thread>
#include <stdio.h>
#include <string>

using namespace std;

#include <cstdio>
#include <cstdlib>
#include <winsock2.h>

#define BUFLEN 512

struct client_type
{
	SOCKET socket;
	int id;
	char received_message[BUFLEN];
};

int process_client(client_type &new_client)
{
	while (1)
	{
		memset(new_client.received_message, 0, BUFLEN);

		if (new_client.socket != 0)
		{
			int iResult = recv(new_client.socket, new_client.received_message, BUFLEN, 0);

			if (iResult != SOCKET_ERROR)
				cout << new_client.received_message << endl;
			else
			{
				cout << "Blad recv(): " << WSAGetLastError() << endl;
				break;
			}
		}
	}

	if (WSAGetLastError() == WSAECONNRESET)
		cout << "Serwer padl" << endl;

	return 0;
}


int main()
{
	struct addrinfo *resultt = NULL, *ptr = NULL;
	string message, nick, IP;
	int iResult = 0;
	string sent_message = "";
	client_type client = { INVALID_SOCKET, -1, ""};

	cout << "Podaj adres IP serwer: ";
	getline(cin, IP);

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
	service.sin_port = htons(27015);

	if (connect(mainSocket, (SOCKADDR *)& service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("Blad polaczenia.\n");
		WSACleanup();
		system("PAUSE");
		return 1;
	}

	client.socket = mainSocket;
	client.id = atoi(client.received_message);
	cout << "Podaj nick: ";
	thread my_thread(process_client, client);

	while (true)
	{
		getline(cin, sent_message);
		iResult = send(client.socket, sent_message.c_str(), strlen(sent_message.c_str()), 0);

		if (iResult <= 0)
		{
			cout << "Blad wysylania: " << WSAGetLastError() << endl;
			break;
		}
	}

	my_thread.detach();

	iResult = shutdown(client.socket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		closesocket(client.socket);
		WSACleanup();
		system("PAUSE");
		return 1;
	}

	closesocket(client.socket);
	WSACleanup();

	system("PAUSE");
	return 0;
}