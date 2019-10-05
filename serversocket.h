#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <winsock2.h>

using namespace std;

class Server_socket
{

public:	
	SOCKET server;
	sockaddr_in sin;  

	Server_socket(int host);
	void Start_listen();
	void Close();
};

Server_socket::Server_socket(int host)
{
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	sin.sin_family = AF_INET;  
	sin.sin_port = htons(host);  
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
}

void Server_socket::Start_listen()
{
	bind(server, (LPSOCKADDR)&sin, sizeof(sin));
	listen(server, 20);
	return;
}

void Server_socket::Close()
{
	closesocket(server);  
}