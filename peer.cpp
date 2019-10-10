#include <stdio.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdio>
#include <winsock2.h>
#include <map>
#include "serversocket.h"
using namespace std;

#define MAX_FILE_NAME = 200;
#define MAX_MSG = 200;
#define MAX_FILE_NUM = 50;
#define MAX_FILE_LENGTH = 100;
#define MAX_CHUNK = 100;
#define MAX_TOTAL_MESSAGE = 20000;

map<string, int> filemap;
int peer_file_num;
int m_server;
int target_peer;
vector<string> sp;


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

void SplitString(const string& s, vector<std::string>& v, const string& c)
{
  string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  v.clear();

  while(std::string::npos != pos2)
  {
	v.push_back(s.substr(pos1, pos2-pos1));
 
	pos1 = pos2 + c.size();
	pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
	v.push_back(s.substr(pos1));
}


void init_request(int request_num)
{
	//long long bytes_sent, bytes_rcvd;
	char num_buffer[1024];
	char total_message[MAX_TOTAL_MESSAGE];
	register_requset rr;
	file_location_request flr;
	chunk_register_request crr;
	file_chuck_request fcr;
	char recv_buffer[1024];
	int len;

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


		//receive message
		len = server.Recv(m_server,recv_buffer);
		if(recv_buffer == "1")
		{
			cout << "Register request success!" << endl;
		}
		//next step
	}

	if(request_num = 2)
	{
		SOCKET m_server;
		cout << "File list requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		server.send(m_server,total_message,2);

		//receieve message
		len = server.Recv(m_server,recv_buffer);
		SplitString(recv_buffer,sp," ");
		int recv_file_num = atoi(sp[0].c_str());
		string recv_file_name[recv_file_num];
		int recv_file_size[recv_file_num];
		for(int i = 1; i <= recv_file_num; i++)
		{
			recv_file_name[i-1] = sp[i*2-1];
			recv_file_size[i-1] = atoi(sp[i*2].c_str());
		}
		//next step

	}

	if(request_num = 3)
	{
		SOCKET m_server;
		cout << "File location requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		total_message+ = sprintf(num_buffer + total_message, "%d%s",flr.file_name,"/0");
		server.send(m_server,total_message,strlen(total_message));

		//receive file
		len = server.Recv(m_server,recv_buffer);
		SplitString(recv_buffer,sp," ");
		int recv_tmplist_size = atoi(sp[0].c_str);
		string recv_ips;
		int recv_ports;
		for(int i = 1; i <= recv_tmplist_size; i++)
		{
			recv_ips[i-1] = sp[i*2-1];;
			recv_ports[i-1] = atoi(sp[i*2].c_str());
		}


	}

	if(request_num = 4)
	{
		SOCKET m_server;
		cout << "Chunk register requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		total_message+ = sprintf(num_buffer + total_message, "%s%s%d%s",crr.file_name,"/0",crr.chunk_num,"/0");
		server.send(m_server,total_message,strlen(total_message));

		len = server.Recv(m_server,recv_buffer);
		if(recv_buffer == "1")
		{
			cout << "Chunk register success!" << endl;
		}
	}

	if(request_num = 5)
	{
		SOCKET target_peer;
		cout << "File chunk requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		total_message+ = sprintf(num_buffer + total_message, "%s%s%d%s",fcr.file_name,"/0",fcr.chunk_num,"/0");
		server.send(target_peer,total_message,strlen(total_message));

		//receive file
		cout << "Start receive files!" << endl;
		char file_buffer[1024] = {0};
		int readLen = 0;
		int readLen = 0;
    	string desFileName = "peer_new_in";
    	ofstream desFile;
    	desFile.open(desFileName.c_str(), ios::binary);
    	if (!desFile)
    	{
        	return ;
    	}
    	do 
    	{
        	readLen = recv(m_Client,buffer,bufferSize, 0);
        	if (readLen == 0)
        	{
            	break;
        	}
        	else
        	{
            	desFile.write(buffer, readLen);
        	}
    	} while(true);
    	desFile.close();
	}

}

int main()
{
	Server_socket server(7777);
	server.Start_listen();

}