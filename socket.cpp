#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <winsock2.h>
#include "serversocket.h"

using namespace std;


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
        server.Send(m_Client, "1010", 4);
        server.SendFile(m_Client, "origin.txt");
        //SendFile();
        closesocket(m_Client);  
    }  
      
    server.Close();
    WSACleanup();  
    return 0;  
} 
