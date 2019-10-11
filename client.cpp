#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;
 
SOCKET m_Client; 

void RecvFile();

int main()
{
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if(WSAStartup(sockVersion, &data)!=0)
    {
        return 0;
    }
    //while(true){
        m_Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(m_Client == INVALID_SOCKET)
        {
            printf("invalid socket!");
            return 0;
        }
        
        char buffer [1024];
        //buffer = "3 file1 321 file2 3331"
        sockaddr_in serAddr, my_addr;
        serAddr.sin_family = AF_INET;
        serAddr.sin_port = htons(7777);
        serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
        

        if(connect(m_Client, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
        {  //连接失败 
            printf("connect error !");
            closesocket(m_Client);
            return 0;
        }
        //RecvFile();
        int buffer_size = 1024;
        send(m_Client, "1 ", 2, 0);
        send(m_Client, "2 ", 2, 0);
        send(m_Client, "file1 321 file2 20248881 127.0.0.1 11000", 40, 0);

        int len = recv(m_Client, buffer, buffer_size, 0);
        cout<<buffer<<endl;
                
        m_Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if(connect(m_Client, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
        {  //连接失败 
            printf("connect error !");
            closesocket(m_Client);
        }

        send(m_Client, "2", 1, 0);
        
        len = recv(m_Client, buffer, buffer_size, 0);
        cout<<buffer<<endl;
        cout<<len<<endl;

        closesocket(m_Client);
    //}
        m_Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if(connect(m_Client, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
        {  //连接失败 
            printf("connect error !");
            closesocket(m_Client);
        }

        send(m_Client, "3 file2", 7, 0);
        
        len = recv(m_Client, buffer, buffer_size, 0);
        cout<<buffer<<endl;
        cout<<len<<endl;

        closesocket(m_Client);    

        m_Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if(connect(m_Client, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
        {  //连接失败 
            printf("connect error !");
            closesocket(m_Client);
        }

        send(m_Client, "4 file2 1", 9, 0);
        
        len = recv(m_Client, buffer, buffer_size, 0);
        cout<<buffer<<endl;
        cout<<len<<endl;

        closesocket(m_Client);     
    WSACleanup();
    return 0;
    
}

void RecvFile(){
    cout<<"start recv!"<<endl;
    const int bufferSize = 1024;
    char buffer[bufferSize] = {0};
    int readLen = 0;
    string desFileName = "new_in";
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
