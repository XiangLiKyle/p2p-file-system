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
#define MAX_FILE_NUM 500
#define MAX_FILE_LENGTH 10000000
#define MAX_CHUNK 100
#define MAX_TOTAL_MESSAGE 20000

const int local_port = 10020;
const int buffer_size = 1024;
const string local_IP = "127.0.0.1";

map<string, int> filemap;
int peer_file_num;
int peer_chunk_num;
vector<string> sp;
int request_num;
string file_list[MAX_FILE_NUM];
string chunk_list[MAX_FILE_NUM];

string next_ip;

string new_file;
int new_file_length;
string new_chunk_list[MAX_FILE_NUM];

string temp_file_list[MAX_FILE_NUM];
string temp_chunk_list[MAX_FILE_NUM];
int temp_file_length[MAX_FILE_NUM];
string temp_message[MAX_FILE_NUM];

int temp_file_num;
string local_file_path = "D:\\work\\psu\\distributed system\\project1\\backup\\files";
string local_chunk_path = "D:\\work\\psu\\distributed system\\project1\\backup\\chunks";


struct register_requset
{
	int file_num;
	//string file_name[MAX_FILE_NUM];
	string file_name;
	//int file_length[MAX_FILE_NUM];
	int file_length;
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


void init_request(int request_num, SOCKET target)
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
		rr.file_num = 1;
		cout<<"Register requseted"<<endl;
		total_message = sprintf(num_buffer,"%d %s",request_num," ");
		total_message += sprintf(num_buffer + total_message, "%d%s",rr.file_num," ");
		//for(int i = 0; i < peer_file_num; i++)
		//{
		//	rr.file_name[i] = new_file_list[i];
		//	total_message += sprintf(num_buffer + total_message,"%s%s%d%s",rr.file_name[i]," ",rr.file_length[i]," ");
		//}
		rr.file_name = new_file;
		rr.file_length = new_file_length;
		total_message += sprintf(num_buffer + total_message,"%s%s%d%s",rr.file_name," ",rr.file_length," ");
		total_message += sprintf(num_buffer + total_message,"%s%s%d%s",local_IP," ",local_port," ");
		send(target,num_buffer,total_message,0);


		//receive message
		len = recv(target,recv_buffer,buffer_size,0);
		if(recv_buffer == "1")
		{
			cout << "Register request success!" << endl;
		}
		//next step
	}


	if(request_num == 2)
	{
		//SOCKET target;
		cout << "File list requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num," ");
		send(target,num_buffer,2,0);

		//receieve message
		len = recv(target,recv_buffer,buffer_size,0);

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

		printf("File List\n"); //display file list
		for(int i = 1; i <= recv_file_num; i++)
			printf("%d. File name: %s File size:%d\n",i,recv_file_name[i-1].c_str(),recv_file_size[i-1]);
	}

	if(request_num == 3)
	{
		//SOCKET target;
		cout << "File location requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num," ");
		total_message += sprintf(num_buffer + total_message, "%d%s",flr.file_name," ");
		send(target,num_buffer,total_message,0);

		//receive file
		len = recv(target,recv_buffer,buffer_size,0);
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

	if(request_num == 4)
	{
		//SOCKET target;
		cout << "Chunk register requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num," ");
		total_message += sprintf(num_buffer + total_message, "%s%s%d%s",crr.file_name," ",crr.chunk_num," ");
		send(target,num_buffer,total_message,0);

		len = recv(target,recv_buffer,buffer_size,0);
		if(recv_buffer == "1")
		{
			cout << "Chunk register success!" << endl;
		}
	}


	if(request_num == 5)
	{
		//SOCKET target;
		cout << "File chunk requested!" << endl;
		total_message = sprintf(num_buffer,"%d%s",request_num," ");
		total_message += sprintf(num_buffer + total_message, "%s%s%d%s",fcr.file_name," ",fcr.chunk_num," ");
		send(target,num_buffer,total_message,0);

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
	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1)
        return;
	int i = 0;
	do
	{
		temp_file_list[i] = fileInfo.name;
		//temp_file_length[i] = fileInfo.size;
		//cout << "Files under path:" << endl;
		//cout << fileInfo.name << endl;
		i++;
	}while(_findnext(hFile,&fileInfo) == 0);
	peer_file_num = i+1;
	_findclose(hFile);
	//return;

}

void file_splitter(string file_name)
{
	fstream fsin,fsout;
	string temp_chunk;
	fsin.open((local_file_path + '\\' + file_name).c_str(),ios::in|ios::binary);
	cout << (local_file_path + '\\' + file_name).c_str() << endl;
	if(!fsin)
	{
		cout << "Invalid file name" << endl;
		return;
	}
	fsin.seekg(0,ios::end);
	int file_size = fsin.tellg();
	new_file_length = file_size;
	fsin.seekg(0,ios::beg);
	int chunks = file_size / buffer_size + 1;
	char num[10];
	char chunk_in[buffer_size];
	for(int i = 0; i < chunks - 1; i++)
	{
		itoa(i,num,10);
		temp_chunk = local_chunk_path + "/" + file_name + "#" + num;
		//new_chunk_list[i] = file_name + "#" + num;
		fsout.open(temp_chunk.c_str(),ios::out|ios::binary);
		fsin.read(chunk_in,sizeof(chunk_in));
		fsout.write(chunk_in,sizeof(chunk_in));
		fsout.close();
	}
	cout << "n-1!" << endl;
	delete [] chunk_in;

	int endchunk = file_size - buffer_size*(chunks-1);
	itoa(chunks - 1,num,10);
	temp_chunk = local_chunk_path + "/" + file_name + "#" + num;
	//new_chunk_list[num] = file_name + "#" + num;
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

struct upload
{
	Server_socket* server;
	int request_peer;
};

ofstream f("listen.log");

DWORD WINAPI Threadhandle(LPVOID pParam)
{	
	upload *ul = (upload *)pParam;

	char recv_msg[buffer_size];
	int len;

	len = ul->server->Recv(ul->request_peer, recv_msg);
	SplitString(recv_msg,sp," ");
	string request_file_name = sp[1];
	int request_chunk = atoi(sp[2].c_str());
	
	f<<request_file_name<<" chunk:"<<request_chunk<<endl;

	//send chunk
	char buffer[buffer_size];

	int havesend = 0;
	int readlen = 0;

	string actual_file_name;

	actual_file_name = request_file_name + "#" + sp[2];
	ifstream srcFile;
	srcFile.open(actual_file_name, ios::binary);
	if(!srcFile)
	{
		f<< "No File Chunk!" << endl;
		return 1;
	}
	
	while(!srcFile.eof())
	{
		srcFile.read(buffer, buffer_size);
		readlen = srcFile.gcount();
		send(ul->request_peer, buffer, readlen, 0);
		havesend += readlen;	
	}
	
	f<< "File Chunk send!" << endl;

	srcFile.close();
}

DWORD WINAPI ThreadListen(LPVOID pParam)
{
    Server_socket server(local_port);
    server.Start_listen();

	sockaddr_in remoteAddr;  
	int nAddrlen = sizeof(remoteAddr);  
	char revData[255];   
	SOCKET Client; 

	while(1)  
	{  
		f<<"Backend Listening"<<endl;  
		Client = accept(server.server, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if(Client == INVALID_SOCKET)  
		{  
			f<<"Socket Error!"<<endl;  
			continue;  
		}  

		f<<"Connection received: "<<inet_ntoa(remoteAddr.sin_addr)<<endl;
		f<< "File Chunk Request received!" << endl;
		
		upload* ul = new upload;
		
		ul->server = &server;
		ul->request_peer = Client;

		HANDLE hThread;
		DWORD  threadId;
		hThread = CreateThread(NULL, 0, Threadhandle, ul, 0, &threadId);
		closesocket(Client);
	}

    return 0;
}

void regist_prepare(SOCKET target)
{
	string put_in;
	dir(local_file_path);
	//peer_file_num = temp_file_num;
	//int k = 0;
	for(int i = 0; i < peer_file_num; i++)
	{
		file_list[i] = temp_file_list[i];
		cout << file_list[i] << endl;
	}
	do
	{
		
		//k++;
		cout << "Print the file name you want to regist, if you do not want to regist, print NO:" << endl;
		cin >> put_in;
		new_file = put_in;

		file_splitter(put_in);

		init_request(1,target);
		//closesocket(target);
		SOCKET target;
    	target = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(target == INVALID_SOCKET)
        {
            printf("invalid socket!");
            return;
        }
        
        char buffer [1024];
        //buffer = "3 file1 321 file2 3331"
        sockaddr_in serAddr;
        serAddr.sin_family = AF_INET;
        serAddr.sin_port = htons(7777);
        serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
        if(connect(target, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
        {  //连接失败 
            printf("connect error !");
            closesocket(target);
            return;
        }

		
	}while(put_in != "NO");
	//init_request(1,target);
	 
}

void ask_request(SOCKET target)
{
	//SOCKET target;
    target = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(target == INVALID_SOCKET)
    {
        printf("invalid socket!");
        return;
    }
        
    char buffer [1024];
        //buffer = "3 file1 321 file2 3331"
    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(7777);
    serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    if(connect(target, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {  //连接失败 
        printf("connect error !");
        closesocket(target);
        return;
    }
	init_request(2,target);
	closesocket(target);


	string put_in;
	cout << "Choose a file to download:" << endl;
	cin >> put_in;


	//SOCKET target;
    target = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(connect(target, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {  //连接失败 
        printf("connect error !");
        closesocket(target);
        return;
    }
	init_request(3,target);
	closesocket(target);
	//connected and downloading
}

int main()
{
	//dir(local_file_path);
	//for(int i = 0; i < peer_file_num; i++)
	//{
	//	file_splitter(file_list[i]);
	//}
	//cout << "file splitted!" << endl;
	//listen
	//dir(local_chunk_path);
	//for(int i = 0; i < peer_file_num; i++)
	//{
	//	chunk_list[i] = file_list[i];
	//	cout << chunk_list[i] << endl;
	//}
	WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if(WSAStartup(sockVersion, &data)!=0)
    {
        return 0;
    }
		
	HANDLE LThread;
	DWORD  threadIdL;
	LThread = CreateThread(NULL, 0, ThreadListen, NULL, 0,&threadIdL);


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
    	target = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    	if(target == INVALID_SOCKET)
    	{
        	printf("invalid socket!");
        	return 0;
    	}
        
    	char buffer [1024];
        //buffer = "3 file1 321 file2 3331"
    	sockaddr_in serAddr;
    	serAddr.sin_family = AF_INET;
    	serAddr.sin_port = htons(7777);
    	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    	if(connect(target, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    	{  //连接失败 
       		printf("connect error !");
        	closesocket(target);
        	return 0;
    	}
    	if(request_num == 1)
    	{
    		regist_prepare(target);

    	}

    	if(request_num == 2)
    	{
    		ask_request(target);
    	}
    }


}