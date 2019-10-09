#include <stdio.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <winsock2.h>
#include <map>
#include "serversocket.h"

using namespace std;
const int local_port = 7777;
const string local_IP = "127.0.0.1";
const int chunk_size = 1<<20;

vector<string> v;
map<string, int> filemap;
int filesum = 0;
int file2chunk[10001];
vector<vector<pair<string, int>>> chunk_loc;

void SplitString(const string& s, vector<std::string>& v, const string& c)
{
  string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;

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
	if(v[0] == "1") //Register Request
	{
		int num = atoi(v[1].c_str());
		string filename;
		int filesize, filenum, chunk_num;

		for(int i = 1; i <= num; i++)
		{
			filename = v[i * 2];
			filesize = atoi(v[i * 2 + 1].c_str());

            if(filemap.find(filename) == filemap.end())
            {
                filemap[filename] = filesum;
                filenum = filesum;
                filesum++;  
            }
            else
                continue; //file exist

            chunk_num = filesize / chunk_size;
            if(filesize % chunk_size != 0)
                chunk_num ++;

            file2chunk[filenum] = chunk_num;
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
	string tmp;

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
		tmp = buffer;
		cout<<tmp<<endl;

		v.clear();
		SplitString(tmp, v , " ");
		Deal(server, Client, remoteAddr);

            
		closesocket(Client);  
	}  
	  
	server.Close();
	WSACleanup();  
	return 0;  
} 
