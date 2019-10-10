#include <stdio.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdio>
#include <winsock2.h>
#include <map>
#include <io.h>
#include "serversocket.h"
using namespace std;

#define MAX_FILE_NAME 200
#define MAX_MSG 2000
#define MAX_FILE_NUM 50
#define MAX_FILE_LENGTH 100
#define MAX_CHUNK 100
#define MAX_TOTAL_MESSAGE 20000

const int local_port = 7777;
const int buffer_size = 1024;

map<string, int> filemap;
int peer_file_num;
//int m_server;
//int target_peer;
vector<string> sp;
int request_num;
string file_list[MAX_FILE_NUM];
string local_file_path;
string local_chunk_path;


struct register_requset
{
	int file_num;
	string file_name[MAX_FILE_NUM];
	int file_length[MAX_FILE_NUM];
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


void init_request(Server_socket server, int request_num, SOCKET target)
{
	//Server_socket server(local_port);
	char num_buffer[1024];
	int total_message;
	register_requset rr;
	file_location_request flr;
	chunk_register_request crr;
	file_chuck_request fcr;
	char recv_buffer[1024];
	int len;

	//SOCKET target;

	if(request_num == 1)
	{
		//SOCKET target;
		cout<<"Register requseted"<<endl;
		total_message = sprintf(num_buffer,"%d %s",request_num," ");
		total_message += sprintf(num_buffer + total_message, "%d%s",rr.file_num,"/0");
		for(int i = 0; i < peer_file_num; i++)
		{
			total_message += sprintf(num_buffer + total_message,"%s%s%d%s",rr.file_name[i],"/0",rr.file_length[i],"/0");
		}
		server.Send(target,num_buffer,total_message);


		//receive message
		len = server.Recv(target,recv_buffer);
		if(recv_buffer == "1")
		{
			cout << "Register request success!" << endl;
		}
		//next step
	}

	if(request_num = 2)
	{
		//SOCKET target;
		cout << "File list requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		server.Send(target,num_buffer,2);

		//receieve message
		len = server.Recv(target,recv_buffer);
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
		//SOCKET target;
		cout << "File location requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		total_message += sprintf(num_buffer + total_message, "%d%s",flr.file_name,"/0");
		server.Send(target,num_buffer,total_message);

		//receive file
		len = server.Recv(target,recv_buffer);
		SplitString(recv_buffer,sp," ");
		int recv_tmplist_size = atoi(sp[0].c_str());
		string recv_ips[MAX_FILE_NUM];
		int recv_ports[MAX_FILE_NUM];
		for(int i = 1; i <= recv_tmplist_size; i++)
		{
			recv_ips[i-1] = sp[i*2-1];
			recv_ports[i-1] = atoi(sp[i*2].c_str());
		}


	}

	if(request_num = 4)
	{
		//SOCKET target;
		cout << "Chunk register requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		total_message += sprintf(num_buffer + total_message, "%s%s%d%s",crr.file_name,"/0",crr.chunk_num,"/0");
		server.Send(target,num_buffer,total_message);

		len = server.Recv(target,recv_buffer);
		if(recv_buffer == "1")
		{
			cout << "Chunk register success!" << endl;
		}
	}

	if(request_num = 5)
	{
		//SOCKET target;
		cout << "File chunk requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num,"/0");
		total_message += sprintf(num_buffer + total_message, "%s%s%d%s",fcr.file_name,"/0",fcr.chunk_num,"/0");
		server.Send(target,num_buffer,total_message);

		//receive file
		cout << "Start receive files!" << endl;
		char file_buffer[buffer_size] = {0};
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
        	readLen = recv(target,file_buffer,buffer_size, 0);
        	if (readLen == 0)
        	{
            	break;
        	}
        	else
        	{
            	desFile.write(file_buffer, readLen);
        	}
    	} while(true);
    	desFile.close();
	}

}

void dir(string path)
{
	struct _finddata_t fileInfo;
	string pathName;
	long hFile = 0;
	hFile = _findfirst(path.c_str(),&fileInfo);
	if(hFile == -1)
		return;
	int i = 0;
	do
	{
		file_list[i] = fileInfo.name;
		i++;
	}while(_findnext(hFile,&fileInfo));
	peer_file_num = i+1;
	_findclose(hFile);
	return;

}

void file_splitter(string file_name)
{
	fstream fsin,fsout;
	string temp_chunk;
	fsin.open((local_file_path + file_name).c_str(),ios::in|ios::binary);
	if(!fsin)
	{
		cout << "Invalid file name" << endl;
		return;
	}
	fsin.seekg(0,ios::end);
	int file_size = fsin.tellg();
	fsin.seekg(0,ios::beg);
	int chunks = file_size / buffer_size + 1;
	char num[10];
	char chunk_in[buffer_size];
	for(int i = 0; i < chunks - 1; i++)
	{
		itoa(i,num,10);
		temp_chunk = local_chunk_path + file_name + "#" + num;
		fsout.open(temp_chunk.c_str(),ios::out|ios::binary);

		fsin.read(chunk_in,sizeof(chunk_in));
		fsout.write(chunk_in,sizeof(chunk_in));
		fsout.close();
	}
	delete [] chunk_in;

	int endchunk = file_size - buffer_size*(chunks-1);
	itoa(chunks - 1,num,10);
	temp_chunk = local_chunk_path + file_name + "#" + num;
	fsout.open(temp_chunk.c_str(),ios::out|ios::binary);
	char chunk_last[endchunk];
	fsin.read(chunk_last,sizeof(chunk_last));
	fsout.write(chunk_last,sizeof(chunk_last));
	fsout.close();
	delete [] chunk_last;
}

void file_assembler(string file_name,int file_num)
{
	fstream fain,faout;
	string temp_chunk;
	faout.open((local_file_path + file_name).c_str(),ios::out|ios::binary);

	char num[10];
	char chunk_in[buffer_size];
	for(int i = 0; i < file_num - 1; i++)
	{
		itoa(i,num,10);
		temp_chunk = local_chunk_path + file_name + "#" + num;
		fain.open(temp_chunk.c_str(),ios::in|ios::binary);
		fain.read(chunk_in,sizeof(chunk_in));
		faout.write(chunk_in,sizeof(chunk_in));
		fain.close();
	}
	delete [] chunk_in;

	itoa(file_num - 1,num,10);
	temp_chunk = local_chunk_path + file_name + "#" + num;
	fain.open(temp_chunk.c_str(),ios::in|ios::binary);
	fain.seekg(0,ios::end);
	int endchunk = fain.tellg();
	fain.seekg(0,ios::beg);

	char chunk_last[endchunk];
	fain.read(chunk_last,sizeof(chunk_last));
	faout.write(chunk_last,sizeof(chunk_last));
	fain.close();
	faout.close();
	delete [] chunk_last;
}

void handle(Server_socket server, int request_peer)
{	
	char recv_msg[buffer_size];
	cout << "File Chunk Request received!" << endl;
	int len;
	len = server.Recv(request_peer,recv_msg);
	SplitString(recv_msg,sp," ");
	string request_file_name = sp[1];
	int request_chunk = atoi(sp[2].c_str());
	
	//send chunk
	char buffer[buffer_size];

	int havesend = 0;
	int readlen = 0;

	string actual_file_name;

	actual_file_name = request_file_name + "#" + sp[2];
	ifstream srcFile;
	srcFile.open(actual_file_name, ios::binary);
	if(!srcFile)
		return;
	
	while(!srcFile.eof())
	{
		srcFile.read(buffer, buffer_size);
		readlen = srcFile.gcount();
		send(request_peer, buffer, readlen, 0);
		havesend += readlen;	
	}

	srcFile.close();

}


int main()
{
	dir(local_file_path);
	for(int i = 0; i < peer_file_num; i++)
	{
		file_splitter(file_list[i]);
	}
	//listen
	WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if(WSAStartup(sockVersion, &data)!=0)
    {
        return 0;
    }
    Server_socket server(local_port);
    server.Start_listen();
    sockaddr_in remoteAddr;  
	int nAddrlen = sizeof(remoteAddr);  
	char revData[255];   
	SOCKET Client; 
	int len;
	string type;
	while(1)  
	{  
		printf("Start Listening\n");  
		Client = accept(server.server, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if(Client == INVALID_SOCKET)  
		{  
			printf("Socket Error!");  
			continue;  
		}  

		printf("Connection received: %s \n", inet_ntoa(remoteAddr.sin_addr));
		handle(server,Client);
		closesocket(Client);
	}
    

	//request
    cout << "Request number:" << endl;
    cin >> request_num;
    if(request_num > 5 && request_num < 1)
    {
    	cout << "invalid request" << endl;
    }
    else
    {
    	SOCKET target;
    	init_request(server,request_num,target);
    }


}