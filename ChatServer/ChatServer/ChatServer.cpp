#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <winsock2.h>
#include <thread>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <winsock2.h>
#define BUFLEN 512

struct client_type
{
	int id;
	SOCKET socket;
	std::string nick;
};

const char OPTION_VALUE = 1;
const int MAX_CLIENTS = 5;

int process_client(client_type &new_client, std::vector<client_type> &client_array, std::thread &thread)
{
	std::string msg = "";
	char tempmsg[BUFLEN] = "";

	while (1)
	{
		memset(tempmsg, 0, BUFLEN);

		if (new_client.socket != 0)
		{
			int iResult = recv(new_client.socket, tempmsg, BUFLEN, 0);
			
			if (iResult != SOCKET_ERROR)
			{
				if (strcmp("", tempmsg))
					msg = new_client.nick + ": " + tempmsg;

				std::cout << msg.c_str() << std::endl;

				for (int i = 0; i < MAX_CLIENTS; i++)
				{
					if (client_array[i].socket != INVALID_SOCKET)
						if (new_client.id != i)
							iResult = send(client_array[i].socket, msg.c_str(), strlen(msg.c_str()), 0);
				}
			}
			else
			{
				msg = "Uzytkownik " + new_client.nick + " rozlaczyl sie :(";

				std::cout << msg << std::endl;

				closesocket(new_client.socket);
				closesocket(client_array[new_client.id].socket);
				client_array[new_client.id].socket = INVALID_SOCKET;

				for (int i = 0; i < MAX_CLIENTS; i++)
				{
					if (client_array[i].socket != INVALID_SOCKET)
						iResult = send(client_array[i].socket, msg.c_str(), strlen(msg.c_str()), 0);
				}

				break;
			}
		}
	}

	thread.detach();

	return 0;
}

int main()
{
	std::string msg = "", IP = "";
	std::vector<client_type> client(MAX_CLIENTS);
	int num_clients = 0;
	int temp_id = -1;
	std::thread my_thread[MAX_CLIENTS];

	std::cout << "Podaj adres IP tego komputera: ";
	getline(std::cin, IP);

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
	service.sin_addr.s_addr = inet_addr("192.168.2.10");
	service.sin_port = htons(27015);

	if (bind(mainSocket, (SOCKADDR *)& service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("Blad bind().\n");
		closesocket(mainSocket);
		return 1;
	}

	if (listen(mainSocket, 1) == SOCKET_ERROR)
		printf("Blad nasluchiwania socketu.\n");

	SOCKET acceptSocket = SOCKET_ERROR;
	printf("Czekam na uzytkownikow...\n");

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		client[i] = { -1, INVALID_SOCKET };
	}

	while (1)
	{

		SOCKET incoming = INVALID_SOCKET;
		incoming = accept(mainSocket, NULL, NULL);

		if (incoming == INVALID_SOCKET) continue;

		num_clients = -1;

		temp_id = -1;
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			if (client[i].socket == INVALID_SOCKET && temp_id == -1)
			{
				client[i].socket = incoming;
				client[i].id = i;
				temp_id = i;
			}

			if (client[i].socket != INVALID_SOCKET)
				num_clients++;

		}

		if (temp_id != -1)
		{
			char nick[10];
			memset(nick, 0, 10);
			recv(client[temp_id].socket, nick, 10, 0);
			client[temp_id].nick = nick;
			std::cout << client[temp_id].nick << " dolaczyl." << std::endl;


			my_thread[temp_id] = std::thread(process_client, std::ref(client[temp_id]), std::ref(client), std::ref(my_thread[temp_id]));
		}

		else
		{
			std::string str = "Serwer pelny";
			int length = str.length();
			send(incoming, str.c_str(), length, 0);

		}
	} 


	closesocket(mainSocket);

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		my_thread[i].detach();
		closesocket(client[i].socket);
	}

	WSACleanup();

	system("PAUSE");
	return 0;
}