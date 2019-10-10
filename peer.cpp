#include<iostream>
#include<unistd.h> //for close
#include<sys/types.h>
#include<sys/socket.h>
#include<memory.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/ip.h>
#include<netinet/tcp.h> //for IPROTO_TCP
#include"serversocket.h"
#include<cstdlib>
#include<cstring>
using namespace std;

#define MAX_FILE_NAME = 200;
#define MAX_MSG = 200;
#define MAX_FILE_NUM = 50;
#define MAX_FILE_LENGTH = 100;
#define MAX_CHUNK = 100;
#define MAX_TOTAL_MESSAGE = 20000;

long long max_buf_size;
int peer_file_num;
int m_server;
int target_peer;

struct register_requset
{
	int file_num;
	char file_name[peer_file_num][MAX_FILE_NAME];
	int file_length[peer_file_num];
};

struct file_location_request
{
	char file_name[MAX_FILE_NAME];
};

struct chunk_register_request
{
	char file_name[MAX_FILE_NAME];
	int chunk_num;
};

struct file_chuck_request
{
	char file_name[MAX_FILE_NAME];
	int chunk_num;
};



void init_request(int request_num)
{
	//long long bytes_sent, bytes_rcvd;
	char num_buffer[1024];
	char total_message[MAX_TOTAL_MESSAGE];
	register_requset rr;
	file_location_request flr;
	chunk_register_request crr;
	file_chuck_request fcr;

	if(request_num == 1)
	{
		SOCKET m_server;
		cout<<"Register requseted"<<endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		total_message+ = sprintf(num_buffer + total_message, "%d%s",rr.file_num,"/0");
		for(int i = 0; i < peer_file_num; i++)
		{
			total_message+ = sprintf(num_buffer + total_message,,"%s%s%d%s",rr.file_name[i],"/0",rr.file_length[i],"/0");
		}
		server.send(m_server,total_message,strlen(total_message));

		
	}

	if(request_num = 2)
	{
		SOCKET m_server;
		cout << "File list requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		server.send(m_server,total_message,2);
	}

	if(request_num = 3)
	{
		SOCKET m_server;
		cout << "File location requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		total_message+ = sprintf(num_buffer + total_message, "%d%s",flr.file_name,"/0");
		server.send(m_server,total_message,strlen(total_message));
	}

	if(request_num = 4)
	{
		SOCKET m_server;
		cout << "Chunk register requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		total_message+ = sprintf(num_buffer + total_message, "%s%s%d%s",crr.file_name,"/0",crr.chunk_num,"/0");
		server.send(m_server,total_message,strlen(total_message));
	}

	if(request_num = 5)
	{
		SOCKET target_peer;
		cout << "File chunk requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		total_message+ = sprintf(num_buffer + total_message, "%s%s%d%s",fcr.file_name,"/0",fcr.chunk_num,"/0");
		server.send(target_peer,total_message,strlen(total_message));

	}
}

int main()
{
	Server_socket server(7777);
	server.Start_listen();

}