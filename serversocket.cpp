#include "serversocket.h"

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
	listen(server, 100);
	return;
}

void Server_socket::Send(int client, char* msg, int len)
{
	send(client, msg, len, 0);
}

int Server_socket::Recv(int client, char* buffer)
{
	int readlen = recv(client, buffer, buffer_size, 0);
	return readlen;
}

void Server_socket::SendFile(int client, string filename)
{
	char buffer[buffer_size];

	int havesend = 0;
	int readlen = 0;

	ifstream srcFile;
	srcFile.open(filename.c_str(), ios::binary);
	if(!srcFile)
		return;
	
	while(!srcFile.eof())
	{
		srcFile.read(buffer, buffer_size);
		readlen = srcFile.gcount();
		send(client, buffer, readlen, 0);
		havesend += readlen;	
	}

	srcFile.close();
}

void Server_socket::RecvFile(int client, string filename)
{
	char buffer[buffer_size];

	int havesend = 0;
	int readlen = 0;

	ofstream desFile;
	desFile.open(filename.c_str(), ios::binary);
	if (!desFile)
		return ;

	while(true)
	{
		readlen = recv(client, buffer, buffer_size, 0);
		if (readlen == 0)
			break;
		else
			desFile.write(buffer, readlen);
	}
	desFile.close();
}

void Server_socket::Close()
{
	closesocket(server);  
}