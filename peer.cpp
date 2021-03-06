#include <stdio.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdio>
#include <winsock2.h>
#include <string>
#include <map>
#include <ctime>
#include <time.h>
#include <io.h>
#include "serversocket.h"
using namespace std;

#define MAX_FILE_NAME 200
#define MAX_MSG 2000
#define MAX_FILE_NUM 500
#define MAX_FILE_LENGTH 10000000
#define MAX_CHUNK 200
#define MAX_TOTAL_MESSAGE 20000

const int choke_time = 10;
const int buffer_size = 1024;
const int chunk_size = 1024 * 1024;
const string local_IP = "127.0.0.1";
const int max_run = 1;

int local_port;
int now_run = 0;
vector<pair<string, int>> chokedlist;
vector<pair<string, int>> downloadlist;

time_t last_time;

map<string, int> filemap;
int peer_file_num;
vector<string> sp;
int request_num;
string file_list[MAX_FILE_NUM];

string next_ip;

string new_file;
int new_file_length;

string temp_file_list[MAX_FILE_NUM];

string recv_file_name[MAX_FILE_NUM];
int recv_file_size[MAX_FILE_NUM];
int recv_file_num;
string recv_ips[MAX_FILE_NUM];
int recv_ports[MAX_FILE_NUM];
int recv_tmplist_size;
int recv_chunk_num[MAX_FILE_NUM];

string local_file_path = ".\\files";
string local_chunk_path = ".\\chunks";

bool marker = true;



ofstream f1("download.log");

struct register_requset  //request 1 message to server
{
	int file_num;
	//string file_name[MAX_FILE_NUM];
	string file_name;
	//int file_length[MAX_FILE_NUM];
	int file_length;
};

struct file_location_request //request 2 message to server
{
	string file_name;
};

struct chunk_register_request // request 3 message to server
{
	char file_name[MAX_FILE_NAME];
	int chunk_num;
};

struct file_chuck_request // request 4 message to server
{
	char file_name[MAX_FILE_NAME];
	int chunk_num;
};


//Message Analysis: split the string with " "
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


//Assemble chunks into a file
void file_assembler(string file_name,int file_num)
{
	fstream fain,faout;
	string temp_chunk;
	faout.open((local_file_path + "\\" + file_name).c_str(),ios::out|ios::binary);

	char num[10];
	char chunk_in[chunk_size];
	for(int i = 0; i < file_num - 1; i++)
	{
		itoa(i,num,10);
		temp_chunk = local_chunk_path + "\\" + file_name + "#" + num;
		fain.open(temp_chunk.c_str(),ios::in|ios::binary);
		fain.read(chunk_in,sizeof(chunk_in));
		faout.write(chunk_in,sizeof(chunk_in));
		fain.close();
	}
	delete [] chunk_in;

	itoa(file_num - 1,num,10);
	temp_chunk = local_chunk_path + "\\" + file_name + "#" + num;
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

struct Download
{
	string file_name;
	int chunk_num;
	int start_num = 0;
	int peer_num;
	vector<vector<pair<string,int>>> chunkpeer;
	int chunk_down[MAX_CHUNK] = {0};
	int downchunknum = 0;
};

vector<Download*> Downloads;
map<pair<string,int>, bool> Chokedpeer;

struct ChunkDownload
{
	Download* dl;
	string file_name;
	int chunk_name;
	vector<pair<string,int>> peerlist;
};

DWORD WINAPI ThreadDownloadChunk(LPVOID pParam)
{
	ChunkDownload *cdown = (ChunkDownload *)pParam;
	char num_buffer[buffer_size];
	char buffer[buffer_size];
	bool success = 0;

	f1 << "File chunk " << cdown->chunk_name <<" requested!" << endl;


	while(1)
	{

		for(int i = 0; i < cdown->peerlist.size(); i++)
		{
			if(Chokedpeer.find(cdown->peerlist[i]) != Chokedpeer.end() && Chokedpeer[cdown->peerlist[i]] == true)
			{
				Sleep(1000 * choke_time);
				Chokedpeer[cdown->peerlist[i]] = false;
			}

			int total_message = 0;
			total_message = sprintf(num_buffer, "1 %s %d %s %d", local_IP.c_str(),local_port,cdown->file_name.c_str(),cdown->chunk_name);
				
			SOCKET target = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    		sockaddr_in serAddr;
    		serAddr.sin_family = AF_INET;

		    serAddr.sin_port = htons(cdown->peerlist[i].second);
		    serAddr.sin_addr.S_un.S_addr = inet_addr(cdown->peerlist[i].first.c_str());

	        if(connect(target, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	        {
				f1<< cdown->chunk_name<<" "<< cdown->peerlist[i].first << " "<< cdown->peerlist[i].second<< " Target error !\n";
	            closesocket(target);
	            continue;
	        }

			send(target, num_buffer, total_message, 0);
			int len = recv(target, buffer, buffer_size, 0);

			if(buffer[0] == '0')//choked
			{
				f1<< cdown->chunk_name<<" "<< cdown->peerlist[i].first << " "<< cdown->peerlist[i].second<< " Target choked !\n";
	            closesocket(target);
	            Chokedpeer[cdown->peerlist[i]] = true;
	            continue;
			}


			if(buffer[0] == '2')//choked
			{
				//f1<< cdown->chunk_name<<" "<< cdown->peerlist[i].first << " "<< cdown->peerlist[i].second<< " Target used !\n";
	            closesocket(target);
	            continue;
			}
			// closesocket(target);
			// target = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			// connect(target, (sockaddr *)&serAddr, sizeof(serAddr));

			// total_message = sprintf(num_buffer, "2 %s %d",cdown->file_name.c_str(),cdown->chunk_name);
			// send(target, num_buffer, total_message, 0);


			//receive file

			f1<< cdown->chunk_name<<" "<< cdown->peerlist[i].first << " "<< cdown->peerlist[i].second<< " Start receive chunk !\n";
			char file_buffer[buffer_size] = {0};
			int readLen = 0;
	    	string desFileName = local_chunk_path + "\\" + cdown->file_name + "#" + to_string(cdown->chunk_name);
	    	ofstream desFile;
	    	desFile.open(desFileName.c_str(), ios::binary);

	    	while(1)
	    	{
	        	readLen = recv(target,file_buffer,buffer_size, 0);
	        	if (readLen == 0)
	            	break;
	        	else
	            	desFile.write(file_buffer, readLen);
	    	}
	    	success = 1;
			cdown->dl->downchunknum++;
	    	desFile.close();
			break;
		}

		if(success == 1)
			break;
		//Sleep(5000);
	}


	f1 << "Chunk register requested!" << endl;
	char request_buffer[buffer_size];

	int total = sprintf(request_buffer,"4 ");
	total += sprintf(request_buffer + total, "%s %d %s %d",local_IP.c_str(),local_port,cdown->file_name.c_str(),cdown->chunk_name);

	SOCKET servertarget;
	servertarget = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    char recv_buffer[1024];

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7777);
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	connect(servertarget, (sockaddr *)&serverAddr, sizeof(serverAddr));


	send(servertarget, request_buffer,total,0);
	int len0 = recv(servertarget,recv_buffer,buffer_size,0);
	// if(recv_buffer == "1")
	// 	cout << "Chunk register success!" << endl;

	return 1;
}

DWORD WINAPI ThreadDownloadFile(LPVOID pParam)
{
	Download *dl = (Download *)pParam;
	for(int i = 0; i < dl->chunk_num; i++)
	{
		while(dl->start_num - dl->downchunknum >= dl->peer_num)
			Sleep(100);

		int minnum = -1;
		for(int j = 0; j < dl->chunk_num; j++)
			if(dl->chunk_down[j] == 0)
			{
				minnum = j;
				break;
			}

		if(minnum == -1)
			break;

		for(int j = 1; j < dl->chunk_num; j++)
			if(dl->chunk_down[j] == 0 && dl->chunkpeer[j].size() < dl->chunkpeer[minnum].size())
				minnum = j;
		
		ChunkDownload* cdown = new ChunkDownload;
		cdown->peerlist = dl->chunkpeer[minnum];
		cdown->file_name = dl->file_name;
		cdown->chunk_name = minnum;
		cdown->dl = dl;
		dl->chunk_down[minnum] = 1;

		dl->start_num ++;

		HANDLE CThread;
		DWORD  threadId;
		CThread = CreateThread(NULL, 0, ThreadDownloadChunk, cdown, 0, &threadId);
	}

	while(1)
	{
		if(dl->chunk_num == dl->downchunknum)
		{
			cout<<dl->file_name<<" download Completed!\n";
			file_assembler(dl->file_name, dl->chunk_num);
			break;
		}
	}

	return 1;
}

//init request to server(user input 1,2)
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
		total_message = sprintf(num_buffer,"%d ",request_num);
		total_message += sprintf(num_buffer + total_message, "%d%s",rr.file_num," ");
		//for(int i = 0; i < peer_file_num; i++)
		//{
		//	rr.file_name[i] = new_file_list[i];
		//	total_message += sprintf(num_buffer + total_message,"%s%s%d%s",rr.file_name[i]," ",rr.file_length[i]," ");
		//}
		rr.file_name = new_file;
		rr.file_length = new_file_length;
		total_message += sprintf(num_buffer + total_message,"%s%s%d%s",rr.file_name.c_str()," ",rr.file_length," ");
		total_message += sprintf(num_buffer + total_message,"%s%s%d%s",local_IP.c_str()," ",local_port," ");
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
		total_message = sprintf(num_buffer,"%d ",request_num);
		send(target,num_buffer,2,0);

		//receieve message
		len = recv(target,recv_buffer,buffer_size,0);

		SplitString(recv_buffer,sp," ");
		recv_file_num = atoi(sp[0].c_str());

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
		string put_in;
		bool correctness = false;
		while(correctness == false)
		{
			cout << "Choose a file to download:" << endl;
			cin >> put_in;
			for(int i  = 0; i< recv_file_num; i++)
			{
				if(put_in == recv_file_name[i])
				{
					correctness = true;
					break;
				}
				//correctness = false;
			}
			if(correctness != false)
				break;
			else
				cout << "Invalid file name! Please try again!" << endl;

		}

		//SOCKET target;
		flr.file_name = put_in;
		cout << "File location requested!" << endl;
		total_message = sprintf(num_buffer,"%d ",request_num);
		total_message += sprintf(num_buffer + total_message, "%s ",flr.file_name.c_str());
		send(target,num_buffer,total_message,0);
		cout<<"!!!!"<<endl;

		//receive file
		Download* newdown = new Download;
		newdown->file_name = flr.file_name;

		len = recv(target,recv_buffer,buffer_size,0);
		SplitString(recv_buffer,sp," ");
		recv_tmplist_size = atoi(sp[0].c_str());
		newdown->chunk_num = atoi(sp[1].c_str());

		for(int i = 0; i < newdown->chunk_num; i++)
		{
			vector<pair<string,int>> v;
			newdown->chunkpeer.push_back(v);
		}

		int k = 2;
		for(int i = 1; i <= recv_tmplist_size; i++)
		{
			cout << "endpoint " << i-1 << endl;
			recv_ips[i-1] = sp[k];
			recv_ports[i-1] = atoi(sp[k+1].c_str());
			recv_chunk_num[i-1] = atoi(sp[k+2].c_str());
			cout << recv_ips[i-1] << " " << recv_ports[i-1] <<" " << recv_chunk_num[i-1]<<endl;
			for(int j = 0; j < recv_chunk_num[i-1]; j++)
			{
				int nowchunk = atoi(sp[k+3+j].c_str());
				newdown->chunkpeer[nowchunk].push_back(make_pair(recv_ips[i-1],recv_ports[i-1]));
				cout << nowchunk << " ";
			}
			k = k + 3 + recv_chunk_num[i-1];
			cout<<endl;
		}

		newdown->peer_num = recv_tmplist_size;

		Downloads.push_back(newdown);
		HANDLE DThread;
		DWORD  threadId;
		DThread = CreateThread(NULL, 0, ThreadDownloadFile, newdown, 0, &threadId);
	}
}

//Find all files under path file
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
		//cout << "Files under path:" << endl;
		//cout << fileInfo.name << endl;
		i++;
	}while(_findnext(hFile,&fileInfo) == 0);
	peer_file_num = i+1;
	_findclose(hFile);
	//return;

}

//Split file into chunks
void file_splitter(string file_name)
{
	fstream fsin,fsout;
	string temp_chunk;
	fsin.open((local_file_path + '\\' + file_name).c_str(),ios::in|ios::binary);
	cout << (local_file_path + '\\' + file_name).c_str() << endl;
	if(!fsin)
	{
		cout << "Invalid file name" << endl;
		marker = false;
		return;
	}
	fsin.seekg(0,ios::end);
	int file_size = fsin.tellg();
	new_file_length = file_size;
	fsin.seekg(0,ios::beg);
	int chunks = file_size / chunk_size + 1;
	char num[10];
	char chunk_in[chunk_size];
	for(int i = 0; i < chunks - 1; i++)
	{
		itoa(i,num,10);
		temp_chunk = local_chunk_path + "/" + file_name + "#" + num;
		fsout.open(temp_chunk.c_str(),ios::out|ios::binary);
		fsin.read(chunk_in,sizeof(chunk_in));
		fsout.write(chunk_in,sizeof(chunk_in));
		fsout.close();
	}
	//cout << "n-1!" << endl;
	delete [] chunk_in;

	int endchunk = file_size - chunk_size*(chunks-1);
	itoa(chunks - 1,num,10);
	temp_chunk = local_chunk_path + "/" + file_name + "#" + num;
	fsout.open(temp_chunk.c_str(),ios::out|ios::binary);
	char chunk_last[endchunk];
	fsin.read(chunk_last,sizeof(chunk_last));
	fsout.write(chunk_last,sizeof(chunk_last));
	fsout.close();
	delete [] chunk_last;
	cout << "Chunk Splitted!" << endl;
}


struct upload
{
	Server_socket* server;
	int request_peer;
	string request_file_name;
	int request_chunk;
	pair<string, int> host;
};

ofstream f("listen.log");
HANDLE hMutex = NULL;

DWORD WINAPI Threadhandle(LPVOID pParam) //UPloadFile
{	
	upload *ul = (upload *)pParam;
	char recv_msg[buffer_size];
	int len;
	sockaddr_in remoteAddr;  
	int nAddrlen = sizeof(remoteAddr);  
	char revData[255];   

	f<<ul->request_file_name<<" chunk:"<<ul->request_chunk<<endl;

	//send chunk
	char buffer[buffer_size];

	int havesend = 0;
	int readlen = 0;

	string actual_file_name;

	actual_file_name = local_chunk_path +  "\\" + ul->request_file_name + "#" + to_string(ul->request_chunk);
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


		ul->server->Send(ul->request_peer, buffer, readlen);


		havesend += readlen;	
	}
	
	f<< "File Chunk send!" << endl;
	closesocket(ul->request_peer);
	srcFile.close();
	now_run --;
	for(int i = 0; i < downloadlist.size(); i++)
		if(downloadlist[i] == ul->host)
		{
			downloadlist.erase(downloadlist.begin() + i);
			break;
		}
}

DWORD WINAPI ThreadListen(LPVOID pParam) //Upload Listen
{
    Server_socket server(local_port);
    server.Start_listen();
    vector<string> fstring;

	sockaddr_in remoteAddr;  
	int nAddrlen = sizeof(remoteAddr);  
	char revData[255];   
	SOCKET Client; 
	hMutex = CreateMutex(NULL, FALSE, "server receive");
	
	DWORD  threadId = 0;
	f<<"Backend Listening"<<endl;  

	while(1)  
	{  
		Client = accept(server.server, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if(Client == INVALID_SOCKET)  
		{  
			f<<"Socket Error!"<<endl;  
			continue;  
		}  

		f<< "File Chunk Request received!" << endl;
		
		upload* ul = new upload;


		char recv_msg[1024];


		int len = server.Recv(Client, recv_msg);
	    string tmp = recv_msg;
	    tmp = tmp.substr(0,len);
		SplitString(tmp, fstring, " ");

		pair<string, int> host = make_pair(fstring[1], atoi(fstring[2].c_str()));
		f<<"Connection received: "<<fstring[1]<<":"<<fstring[2]<<endl;

		int con = 1;
		for(int i = 0; i < downloadlist.size(); i++)
			if(host == downloadlist[i])
			{
				f<<"Connection used: "<<fstring[1]<<":"<<fstring[2]<<endl;

				send(Client, "2", 1,0);
				con = 0;
				break;
			}

		for(int i = 0; i < chokedlist.size(); i++)
			if(host == chokedlist[i])
			{
				f<<"Connection choked: "<<fstring[1]<<":"<<fstring[2]<<endl;
				send(Client, "0", 1,0);
				con = 0;
				break;
			}

		if(con == 0)
			continue;

		if(now_run >= max_run)
		{
			send(Client, "0", 1,0);
			chokedlist.push_back(host);
			printf("%s:%d is choked\n", host.first.c_str(), host.second);
			continue;
		}

		downloadlist.push_back(host);
		send(Client, "1", 1,0);
		now_run++;
	
		
		// closesocket(Client);

		// Client = accept(ul->server->server, (SOCKADDR *)&remoteAddr, &nAddrlen);
		// len = ul->server->Recv(Client, recv_msg);

		
		ul->server = &server;
		ul->request_peer = Client;
		ul->request_file_name = fstring[3];
		ul->request_chunk = atoi(fstring[4].c_str());
		ul->host = host;

		HANDLE hThread;
		hThread = CreateThread(NULL, 0, Threadhandle, ul, 0, &threadId);
		threadId++;
	}

    return 0;
}


DWORD WINAPI ThreadUnchoke(LPVOID pParam)
{
	time_t now;
	while(1)
	{
		now = time(NULL);
		if(now - last_time == choke_time)
		{
			last_time = now;

			if(max_run == now_run)
				continue;

			for(int i = 0; i < max_run - now_run; i++)
			{
				if(chokedlist.size() == 0)
					break;

				int k = rand() % chokedlist.size();
				printf("%s:%d is unchoked\n", chokedlist[k].first.c_str(), chokedlist[k].second);
				chokedlist.erase(chokedlist.begin() + k);
			}
		}
	}
	

    return 0;
}

//Request 1's action
void regist_prepare()
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
		cout << "Print the file name you want to register, if you do not want registeration, print NO:" << endl;
		cin >> put_in;
		new_file = put_in;
		if(put_in == "NO")
		{
			break;
		}

		file_splitter(put_in);

		if(marker == false)
		{
			marker = true;
			continue;
		}
	
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

        init_request(1,target);


		
	}while(put_in != "NO");
	//init_request(1,target);
	 
}

//Request 2's action
void ask_request()
{
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
	init_request(2,target);
	closesocket(target);


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
    last_time = time(NULL);  
    srand(time(NULL));

    local_port = rand();

	WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if(WSAStartup(sockVersion, &data)!=0)
    {
        return 0;
    }
		
	HANDLE LThread, CThread;
	DWORD  threadIdL, threadchoke;
	LThread = CreateThread(NULL, 0, ThreadListen, NULL, 0,&threadIdL);
	CThread = CreateThread(NULL, 0, ThreadUnchoke, NULL, 0,&threadchoke);

	marker = true;

	//request
	while(1)
	{
	    cout << "Print request number, if you want to quit, print 0:" << endl;
	    cin >> request_num;
	    if(request_num == 0)
	    {
	    	break;
	    }
	    if(request_num > 2 && request_num < 1)
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

	    	if(request_num == 1)
	    	{
	    		regist_prepare();

	    	}

	    	if(request_num == 2)
	    	{
	    		ask_request();
	    	}

	    	if(request_num == 3)
	    	{
	    		for(int i = 0; i < Downloads.size(); i++)
	    			printf("%d. %s %d/%d\n",i + 1, Downloads[i]->file_name.c_str(), Downloads[i]->downchunknum, Downloads[i]->chunk_num);
	    	}
	    }
	}


}