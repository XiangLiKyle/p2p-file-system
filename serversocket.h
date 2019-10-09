#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <winsock2.h>

using namespace std;

class Server_socket
{

	const int chunk_size = 1<<20;
	const int buffer_size = 1<<10;

 
public:	
	SOCKET server;
	sockaddr_in sin;  

	Server_socket(int host);
	void Start_listen();
	void Close();
	void Send(int client, char* msg, int len);
	int Recv(int client, char* buffer);
	void SendFile(int client, string filename);
	void RecvFile(int client, string filename);

};