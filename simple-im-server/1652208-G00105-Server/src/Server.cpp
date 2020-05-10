//server.cpp
//我是简单群聊软件的服务器
#include "DealWithMsg.hpp"

#include <vector>
#include <fcntl.h>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

//运行为守护进程
void initDaemon();

int main(int argc, char *argv[])
{
    /***************开始启动***************/
    //1.运行为守护进程
    initDaemon();
    DealWithMsg msgManager; //定义消息处理对象
    msgManager.initLog();   //初始化Log对象

    //2.处理参数
    int flagIP = 0;   //IP参数的索引
    int flagPort = 0; //Port参数的索引

    for (int i = 0; i < argc - 1; i++)
    {
        if (strcmp(argv[i], "--ip") == 0)
        {
            flagIP = ++i; //略过下一个
        }
        else if (strcmp(argv[i], "--port") == 0)
        {
            flagPort = ++i; //略过下一个
        }
    }

    if (flagPort == 0)
    {
        printf("Server启动失败：输入错误！\n\n");
        printf("用法：进程名 [--options]\n");
        printf("其中选项包括：\n");
        printf("    --ip x.x.x.x       ：可选项，表示要绑定的本机IP地址\n");
        printf("    --port xx          ：必选项，表示要bind的TCP端口号\n");
        msgManager.myLog.addLog("Server：启动失败，输入错误！");
        return -1;
    }

    //3.建立socket
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0)
    {
        printf("Server启动失败：创建socket失败！\n");
        msgManager.myLog.addLog("Server：创建socket失败！");
        return -2;
    }

    //4.设置为非阻塞模式
    int flag = fcntl(serverSock, F_GETFL, 0);
    if (flag < 0)
    {
        printf("Server启动失败：设置为非阻塞模式时，获取文件状态标记失败！\n");
        msgManager.myLog.addLog("Server：设置为非阻塞模式时，获取文件状态标记失败！");
        close(serverSock);
        return -3;
    }
    if (fcntl(serverSock, F_SETFL, flag | O_NONBLOCK) < 0)
    {
        printf("Server启动失败：设置为非阻塞模式失败！\n");
        msgManager.myLog.addLog("Server：设置为非阻塞模式失败！");
        close(serverSock);
        return -4;
    }

    //5.设置端口重用(其实主要是测试时用)
    int opt = 1;
    if (setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        printf("Server启动失败：设置端口重用失败！\n");
        msgManager.myLog.addLog("Server：设置端口重用失败！");
        close(serverSock);
        return -5;
    }

    //6.绑定端口
    int port = atoi(argv[flagPort]);
    const char *ip = flagIP ? argv[flagIP] : "0.0.0.0";

    struct sockaddr_in serverAddr;
    unsigned int sockAddrLen = sizeof(struct sockaddr_in);
    memset(&serverAddr, 0, sockAddrLen);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    if (bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) < 0)
    {
        printf("Server启动失败：绑定端口失败！\n");
        msgManager.myLog.addLog("Server启动失败：绑定端口失败！");
        close(serverSock);
        return -6;
    }

    //7.进入侦听状态
    if (listen(serverSock, QUEUE_LEN) < 0)
    {
        printf("Server启动失败：进入侦听状态失败！\n");
        msgManager.myLog.addLog("Server启动失败：进入侦听状态失败！");
        close(serverSock);
        return -7;
    }
    /***************启动成功***************/
    printf("Server启动成功！\n");
    printf("\tip  : %s\n", ip);
    printf("\tport: %d\n\n", port);
    msgManager.myLog.addLog("Server：启动成功！");

    /***************连数据库***************/
    msgManager.initDatabase(); //初始化数据库对象

    /***************开始通信***************/
    vector<struct clientInfo> client; //保存连接的客户端的信息

    int maxSock;    //当前最大的文件描述符
    fd_set readfds; //读文件描述符集
    //fd_set writefds;          //写文件描述符集
    struct clientInfo clientBuffer; //存储client的缓冲区
    struct message messageBuffer;   //存储收到的数据报

    while (1)
    {
        //(1)初始化文件描述符集
        FD_ZERO(&readfds);
        //FD_ZERO(&writefds);
        FD_SET(serverSock, &readfds); //填入serverSock
        maxSock = serverSock;
        for (int i = 0; i < client.size(); i++) //填入clientSock
        {
            int tempSock = client[i].clientSock;
            FD_SET(tempSock, &readfds);
            //FD_SET(tempSock, &writefds);
            if (tempSock > maxSock)
            {
                maxSock = tempSock;
            }
        }

        //2.使用select阻塞等待事件发生
        int selReturn = select(maxSock + 1, &readfds, NULL, NULL, NULL);
        if (selReturn > 0)
        {
            //有新连接
            if (FD_ISSET(serverSock, &readfds))
            {
                //<1>清除置位
                FD_CLR(serverSock, &readfds);

                //<2>如果还有空，把新连接加入到client中
                if (client.size() < MAX_CLIENT) //*暂时设置限制，避免炸裂
                {
                    struct sockaddr_in clientAddr;
                    memset(&clientAddr, 0, sockAddrLen);
                    int newSock = accept(serverSock, (struct sockaddr *)&clientAddr, &sockAddrLen);
                    if (newSock < 0)
                    {
                        printf("接受新连接失败！\n");
                        msgManager.myLog.addLog("Server：接受新连接失败，accept函数出错！");
                    }
                    else //填入
                    {
                        const char *clientIP = inet_ntoa(clientAddr.sin_addr); //获取TCP连接client端的ip
                        int flag = fcntl(newSock, F_GETFL, 0);
                        if (flag < 0)
                        {
                            printf("连接时：设置为非阻塞模式时，获取文件状态标记失败！\n");
                            msgManager.myLog.addLog(clientIP, "连接失败，新连接设置非阻塞模式失败！");
                            close(newSock);
                        }
                        else
                        {
                            if (fcntl(newSock, F_SETFL, flag | O_NONBLOCK) < 0)
                            {
                                printf("连接时：设置为非阻塞模式失败\n");
                                msgManager.myLog.addLog(clientIP, "连接失败，新连接设置非阻塞模式失败！");
                                close(newSock);
                            }
                            else
                            {
                                memset(&clientBuffer, 0, sizeof(struct clientInfo));                               //清零缓冲区
                                clientBuffer.clientSock = newSock;                                                 //填入socket
                                strcpy(clientBuffer.clientIP, clientIP);                                           //填入ip
                                client.push_back(clientBuffer);                                                    //加入client链表中
                                printf("接受新连接成功！ ip: %s 当前保有连接数：%d。\n", clientIP, client.size()); //*用于调试
                                msgManager.myLog.addLog(clientIP, "连接成功！");                                   //记录连接
                            }
                        }
                    }
                }
                else //*这个是人为设置的上限
                {
                    printf("Server接受新连接失败，能接受的TCP连接数已达上限！\n");
                    msgManager.myLog.addLog("Server：接受新连接失败，能接受的TCP连接数已达上限！");
                }
            }

            //有事件发生
            for (int i = 0; i < client.size(); i++)
            {
                int tempSock = client[i].clientSock;
                if (FD_ISSET(tempSock, &readfds)) //是不是你收到数据报了
                {
                    FD_CLR(tempSock, &readfds);          //清除置位
                    memset(&messageBuffer, 0, MSG_SIZE); //清零消息缓冲区准备接收数据报

                    //读取数据
                    int recvReturn = recv(tempSock, &messageBuffer, MSG_SIZE, 0); //非阻塞性读
                    //if (recvReturn > 0)
                    if (recvReturn == MSG_SIZE)
                    {
                        int messageType = messageBuffer.type; //获取数据报类型

                        switch (messageType) //不同类型进行不同处理
                        {
                            case IM_TYPE_REGISTER_REQ: //client发来报到
                            {
                                msgManager.dealWithMsgRegister(messageBuffer, client, i);
                                break;
                            }
                            case IM_TYPE_TEXTINFO_REQ: //client发来文本
                            {
                                msgManager.dealWithMsgText(messageBuffer, client, i);
                                break;
                            }
                            case IM_TYPE_UPDATEPASSWORD_REQ: //client发来修改密码请求
                            {
                                msgManager.dealWithMsgUpdatePasswd(messageBuffer, client, i);
                                break;
                            }
                            case IM_TYPE_HISTORY_REQ: //client发来回看记录请求
                            {
                                msgManager.dealWithMsgHistory(messageBuffer, client, i);
                                break;
                            }
                            case IM_TYPE_EXIT_REQ: //client发来主动退出请求
                            {
                                msgManager.dealWithMsgExit(messageBuffer, client, i);
                                break;
                            }
                            default:
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

//运行为守护进程
void initDaemon()
{
    int pid = fork();
    if (pid < 0) //分类失败
    {
        printf("Server启动失败：运行为守护进程失败！\n");
        exit(-1);
    }
    if (pid > 0) //父进程退出
    {
        exit(0);
    }
}
