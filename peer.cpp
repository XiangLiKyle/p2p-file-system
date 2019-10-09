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
	int chunk_num;
};

struct file_chuck_request
{
	char file_name[MAX_FILE_NAME];
	int chunk_indicator;
};



void init_request(int request_num)
{
	long long bytes_sent, bytes_rcvd;
	char buf[max_buf_size];
	char file_number;
	register_requset rr;
	file_location_request flr;
	chunk_register_request crr;
	file_chuck_request fcr;

	if(request_num == 1)
	{
		char rr_num[MAX_FILE_NUM];
		SOCKET m_server;
		char rr_message[peer_file_num][MAX_MSG];
		sprintf(rr_num,"%d",rr.file_num);
		char rr_file_name[MAX_FILE_NAME];
		char rr_file_length[MAX_FILE_LENGTH];
		for(int i = 0; i < peer_file_num; i++)
		{
			for(int j = 0; j < sizeof(rr.file_name[i]); i++)
			{
				rr_file_name[j] = rr.file_name[i][j];
			}
			//rr_file_name[j+1] = "/0";
			sprintf(rr_file_length,"%d",rr.file_length[i]);
			int k = 0;
			for(int j = 0; j < sizeof(rr_file_name); j++)
			{
				rr_message[i][k] = rr_file_name[j];
				k++;
			}
			rr_message[i][k] = "/0";
			k++;
			for(int j = 0; j < sizeof(rr_file_length);j++)
			{
				rr_message[i][k] = rr_file_length[j];
				k++;
			}
		}

		cout<<"Register Requset"<<endl;
		server.send(m_server, "1" ,1);
		server.send(m_server,rr_num,sizeof(rr_num));
		for(int i = 0; i < sizeof(rr_message); i++)
		{
			server.send(m_server,rr_message[i],sizeof(rr_message[i]));
		}
	}

	if(request_num = 2)
	{
		SOCKET m_server;
		server.send(m_server,"2",1);
	}

	if(request_num = 3)
	{
		SOCKET m_server;
		server.send(m_server,"3",1);
		server.send(m_server,flr.file_name,sizeof(flr.file_name));
	}

	if(request_num = 4)
	{
		SOCKET m_server;
		char crr_chunk_num[MAX_CHUNK]
		server.send(m_server,"4",1);
		sprintf(crr_chunk_num,"%d",crr.chunk_num);
		server.send(m_server,crr_chunk_num,sizeof(crr_chunk_num));
	}

	if(request_num = 5)
	{
		SOCKET target_peer;
		char fcr_chunk_indicator[MAX_CHUNK];
		char fcr_message[MAX_MSG];
		server.send(target_peer,"5",1);
		sprintf(fcr_chunk_indicator,"%d",fcr.chunk_indicator);
		int k = 0;
		for(int i = 0; i < sizeof(fcr.file_name); i++)
		{
			fcr_message[k] = fcr.file_name[i];
			k++;
		}
		fcr_message[k] = "/0";
		k++;
		for(int i = 0; i < sizeof(fcr_chunk_indicator); i++)
		{
			fcr_message[k] = fcr_chunk_indicator[i];
			k++;	
		}
		server.send(target_peer,fcr_message,sizeof(fcr_message));

	}
}

int main()
{
	Server_socket server(7777);
	server.Start_listen();

}