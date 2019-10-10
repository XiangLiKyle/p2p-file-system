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
const int local_port = 7777;
const string local_IP = "127.0.0.1"; //local address
const int chunk_size = 1<<20; //size of chunk


map<string, int> filemap; // file2num
int filesum = 0; // total number of file
string file2name[10001]; //file name
int file2len[10001]; //file length
int file2chunk[10001]; // chunk number of file
int chunkstart[10001]; // location chunk loc
vector<vector<pair<string, int>>> chunk_loc; //chunk location
vector<string> sp;

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


void Deal(Server_socket server, int Client, sockaddr_in Addr)
{
    vector<string> v; // message 

	if(sp[0] == "1") //Register Request
	{
		int num = atoi(sp[1].c_str());

		string filename;
		int filesize, filenum, chunk_num;

		for(int i = 1; i <= num; i++)
		{
			filename = sp[i * 2];
			filesize = atoi(sp[i * 2 + 1].c_str());

            if(filemap.find(filename) == filemap.end())
            {
                filemap[filename] = filesum;
                filenum = filesum;
                file2name[filenum] = filename;
                file2len[filenum] = filesize;
                filesum++;  
            }
            else
                continue; //file exist

            chunk_num = filesize / chunk_size;
            if(filesize % chunk_size != 0)
                chunk_num ++;

            file2chunk[filenum] = chunk_num;
            chunkstart[filenum] = chunk_loc.size();

            for(int j = 0; j < chunk_num; j++)
            {
                vector<pair<string, int>> loc;
                loc.push_back(make_pair(inet_ntoa(Addr.sin_addr), Addr.sin_port));
                chunk_loc.push_back(loc);
            }
	    }

        printf("Succeessful Registration \n");
        server.Send(Client, "1", 1);
    }

    if(sp[0] == "2") //File List Request
    {
        char numbuffer[1024];
        int len;

        len = sprintf(numbuffer,"%d ",filesum);

        for(int i = 0; i < filesum; i++)
            len += sprintf(numbuffer + len,"%s %d ", file2name[i].c_str(), file2len[i]);
        
        server.Send(Client, numbuffer, len);
    }

    if(sp[0] == "3") //file location
    {
        string name = sp[1];
        vector<pair<string, int>> tmplist;
        int num = filemap[name];
        int st = chunkstart[num];
        int chunk_num = file2chunk[num];

        for(int i = 0; i < chunk_num; i++)
            for(int j = 0; j < chunk_loc[i + st].size(); j++)
                tmplist.push_back(chunk_loc[i + st][j]);
        
        sort(tmplist.begin(), tmplist.end());
        tmplist.erase(unique(tmplist.begin(), tmplist.end()), tmplist.end());

        char buffer[1024];
        int len;

        len = sprintf(buffer,"%d ",tmplist.size());
        for(int i = 0; i < tmplist.size(); i++)
            len += sprintf(buffer + len,"%s %d ", tmplist[i].first.c_str(), tmplist[i].second);
        
        server.Send(Client, buffer, len);
    }

    if(sp[0] == "4") //Chunk Register Request
    {
        string filename = sp[1];
        int chunk_id = atoi(sp[2].c_str());
        int filenum;

        if(filemap.find(filename) == filemap.end()) //file not exist
        {
            printf("Fail Chunk Registration \n");
            server.Send(Client, "0", 1);
        }

        filenum = filemap[filename];
        int id = chunkstart[filenum] + chunk_id;
        chunk_loc[id].push_back(make_pair(inet_ntoa(Addr.sin_addr), Addr.sin_port));
        printf("Succeessful Chunk Registration \n");
        server.Send(Client, "1", 1);
    }
}

int main()  
{  
	WORD sockVersion = MAKEWORD(2,2);  
	WSADATA wsaData;  
	if(WSAStartup(sockVersion, &wsaData)!=0)  
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


        char buffer[1024];
		len = server.Recv(Client, buffer);
        type = buffer;
        type = type.substr(0, len);
        cout<<type<<endl;
        SplitString(type, sp, " ");
		Deal(server, Client, remoteAddr);
            
		closesocket(Client);  
	}  
	  
	server.Close();
	WSACleanup();  
	return 0;  
} 
