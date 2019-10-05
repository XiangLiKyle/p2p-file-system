#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <winsock2.h>
#include "serversocket.h"

using namespace std;




// void SendFile()
// {
//     int haveSend = 0;
//     const int bufferSize = 1024;
//     char buffer[bufferSize] = {0};
//     int readLen = 0;
//     string srcFileName = "in";
//     ifstream srcFile;
//     srcFile.open(srcFileName.c_str(),ios::binary);
//     if(!srcFile){
//         return;
//     }
//     while(!srcFile.eof()){
//         srcFile.read(buffer,bufferSize);
//         readLen = srcFile.gcount();
//         send(m_Client,buffer,readLen,0);
//         haveSend += readLen;    
//     }
//     srcFile.close();
//     cout<<"send: "<<haveSend<<"B"<<endl;
// }

int main(int argc, char* argv[])  
{  
    WORD sockVersion = MAKEWORD(2,2);  
    WSADATA wsaData;  
    if(WSAStartup(sockVersion, &wsaData)!=0)  
    {  
        return 0;  
    }  
  
    Server_socket server(8888);
    server.Start_listen();

  
    //循环接收数据  
    sockaddr_in remoteAddr;  
    int nAddrlen = sizeof(remoteAddr);  
    char revData[255];   
    SOCKET m_Client; 

    while(1)  
    {  
        printf("Listening\n");  
        m_Client = accept(server.server, (SOCKADDR *)&remoteAddr, &nAddrlen);  
        if(m_Client == INVALID_SOCKET)  
        {  
            printf("accept error !");  
            continue;  
        }  
        printf("received: %s \r\n", inet_ntoa(remoteAddr.sin_addr));  
        //SendFile();
        closesocket(m_Client);  
    }  
      
    server.Close();
    WSACleanup();  
    return 0;  
} 
