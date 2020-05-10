//server.cpp
//���Ǽ�Ⱥ������ķ�����
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

//����Ϊ�ػ�����
void initDaemon();

int main(int argc, char *argv[])
{
    /***************��ʼ����***************/
    //1.����Ϊ�ػ�����
    initDaemon();
    DealWithMsg msgManager; //������Ϣ�������
    msgManager.initLog();   //��ʼ��Log����

    //2.�������
    int flagIP = 0;   //IP����������
    int flagPort = 0; //Port����������

    for (int i = 0; i < argc - 1; i++)
    {
        if (strcmp(argv[i], "--ip") == 0)
        {
            flagIP = ++i; //�Թ���һ��
        }
        else if (strcmp(argv[i], "--port") == 0)
        {
            flagPort = ++i; //�Թ���һ��
        }
    }

    if (flagPort == 0)
    {
        printf("Server����ʧ�ܣ��������\n\n");
        printf("�÷��������� [--options]\n");
        printf("����ѡ�������\n");
        printf("    --ip x.x.x.x       ����ѡ���ʾҪ�󶨵ı���IP��ַ\n");
        printf("    --port xx          ����ѡ���ʾҪbind��TCP�˿ں�\n");
        msgManager.myLog.addLog("Server������ʧ�ܣ��������");
        return -1;
    }

    //3.����socket
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0)
    {
        printf("Server����ʧ�ܣ�����socketʧ�ܣ�\n");
        msgManager.myLog.addLog("Server������socketʧ�ܣ�");
        return -2;
    }

    //4.����Ϊ������ģʽ
    int flag = fcntl(serverSock, F_GETFL, 0);
    if (flag < 0)
    {
        printf("Server����ʧ�ܣ�����Ϊ������ģʽʱ����ȡ�ļ�״̬���ʧ�ܣ�\n");
        msgManager.myLog.addLog("Server������Ϊ������ģʽʱ����ȡ�ļ�״̬���ʧ�ܣ�");
        close(serverSock);
        return -3;
    }
    if (fcntl(serverSock, F_SETFL, flag | O_NONBLOCK) < 0)
    {
        printf("Server����ʧ�ܣ�����Ϊ������ģʽʧ�ܣ�\n");
        msgManager.myLog.addLog("Server������Ϊ������ģʽʧ�ܣ�");
        close(serverSock);
        return -4;
    }

    //5.���ö˿�����(��ʵ��Ҫ�ǲ���ʱ��)
    int opt = 1;
    if (setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        printf("Server����ʧ�ܣ����ö˿�����ʧ�ܣ�\n");
        msgManager.myLog.addLog("Server�����ö˿�����ʧ�ܣ�");
        close(serverSock);
        return -5;
    }

    //6.�󶨶˿�
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
        printf("Server����ʧ�ܣ��󶨶˿�ʧ�ܣ�\n");
        msgManager.myLog.addLog("Server����ʧ�ܣ��󶨶˿�ʧ�ܣ�");
        close(serverSock);
        return -6;
    }

    //7.��������״̬
    if (listen(serverSock, QUEUE_LEN) < 0)
    {
        printf("Server����ʧ�ܣ���������״̬ʧ�ܣ�\n");
        msgManager.myLog.addLog("Server����ʧ�ܣ���������״̬ʧ�ܣ�");
        close(serverSock);
        return -7;
    }
    /***************�����ɹ�***************/
    printf("Server�����ɹ���\n");
    printf("\tip  : %s\n", ip);
    printf("\tport: %d\n\n", port);
    msgManager.myLog.addLog("Server�������ɹ���");

    /***************�����ݿ�***************/
    msgManager.initDatabase(); //��ʼ�����ݿ����

    /***************��ʼͨ��***************/
    vector<struct clientInfo> client; //�������ӵĿͻ��˵���Ϣ

    int maxSock;    //��ǰ�����ļ�������
    fd_set readfds; //���ļ���������
    //fd_set writefds;          //д�ļ���������
    struct clientInfo clientBuffer; //�洢client�Ļ�����
    struct message messageBuffer;   //�洢�յ������ݱ�

    while (1)
    {
        //(1)��ʼ���ļ���������
        FD_ZERO(&readfds);
        //FD_ZERO(&writefds);
        FD_SET(serverSock, &readfds); //����serverSock
        maxSock = serverSock;
        for (int i = 0; i < client.size(); i++) //����clientSock
        {
            int tempSock = client[i].clientSock;
            FD_SET(tempSock, &readfds);
            //FD_SET(tempSock, &writefds);
            if (tempSock > maxSock)
            {
                maxSock = tempSock;
            }
        }

        //2.ʹ��select�����ȴ��¼�����
        int selReturn = select(maxSock + 1, &readfds, NULL, NULL, NULL);
        if (selReturn > 0)
        {
            //��������
            if (FD_ISSET(serverSock, &readfds))
            {
                //<1>�����λ
                FD_CLR(serverSock, &readfds);

                //<2>������пգ��������Ӽ��뵽client��
                if (client.size() < MAX_CLIENT) //*��ʱ�������ƣ�����ը��
                {
                    struct sockaddr_in clientAddr;
                    memset(&clientAddr, 0, sockAddrLen);
                    int newSock = accept(serverSock, (struct sockaddr *)&clientAddr, &sockAddrLen);
                    if (newSock < 0)
                    {
                        printf("����������ʧ�ܣ�\n");
                        msgManager.myLog.addLog("Server������������ʧ�ܣ�accept��������");
                    }
                    else //����
                    {
                        const char *clientIP = inet_ntoa(clientAddr.sin_addr); //��ȡTCP����client�˵�ip
                        int flag = fcntl(newSock, F_GETFL, 0);
                        if (flag < 0)
                        {
                            printf("����ʱ������Ϊ������ģʽʱ����ȡ�ļ�״̬���ʧ�ܣ�\n");
                            msgManager.myLog.addLog(clientIP, "����ʧ�ܣ����������÷�����ģʽʧ�ܣ�");
                            close(newSock);
                        }
                        else
                        {
                            if (fcntl(newSock, F_SETFL, flag | O_NONBLOCK) < 0)
                            {
                                printf("����ʱ������Ϊ������ģʽʧ��\n");
                                msgManager.myLog.addLog(clientIP, "����ʧ�ܣ����������÷�����ģʽʧ�ܣ�");
                                close(newSock);
                            }
                            else
                            {
                                memset(&clientBuffer, 0, sizeof(struct clientInfo));                               //���㻺����
                                clientBuffer.clientSock = newSock;                                                 //����socket
                                strcpy(clientBuffer.clientIP, clientIP);                                           //����ip
                                client.push_back(clientBuffer);                                                    //����client������
                                printf("���������ӳɹ��� ip: %s ��ǰ������������%d��\n", clientIP, client.size()); //*���ڵ���
                                msgManager.myLog.addLog(clientIP, "���ӳɹ���");                                   //��¼����
                            }
                        }
                    }
                }
                else //*�������Ϊ���õ�����
                {
                    printf("Server����������ʧ�ܣ��ܽ��ܵ�TCP�������Ѵ����ޣ�\n");
                    msgManager.myLog.addLog("Server������������ʧ�ܣ��ܽ��ܵ�TCP�������Ѵ����ޣ�");
                }
            }

            //���¼�����
            for (int i = 0; i < client.size(); i++)
            {
                int tempSock = client[i].clientSock;
                if (FD_ISSET(tempSock, &readfds)) //�ǲ������յ����ݱ���
                {
                    FD_CLR(tempSock, &readfds);          //�����λ
                    memset(&messageBuffer, 0, MSG_SIZE); //������Ϣ������׼���������ݱ�

                    //��ȡ����
                    int recvReturn = recv(tempSock, &messageBuffer, MSG_SIZE, 0); //�������Զ�
                    //if (recvReturn > 0)
                    if (recvReturn == MSG_SIZE)
                    {
                        int messageType = messageBuffer.type; //��ȡ���ݱ�����

                        switch (messageType) //��ͬ���ͽ��в�ͬ����
                        {
                            case IM_TYPE_REGISTER_REQ: //client��������
                            {
                                msgManager.dealWithMsgRegister(messageBuffer, client, i);
                                break;
                            }
                            case IM_TYPE_TEXTINFO_REQ: //client�����ı�
                            {
                                msgManager.dealWithMsgText(messageBuffer, client, i);
                                break;
                            }
                            case IM_TYPE_UPDATEPASSWORD_REQ: //client�����޸���������
                            {
                                msgManager.dealWithMsgUpdatePasswd(messageBuffer, client, i);
                                break;
                            }
                            case IM_TYPE_HISTORY_REQ: //client�����ؿ���¼����
                            {
                                msgManager.dealWithMsgHistory(messageBuffer, client, i);
                                break;
                            }
                            case IM_TYPE_EXIT_REQ: //client���������˳�����
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

//����Ϊ�ػ�����
void initDaemon()
{
    int pid = fork();
    if (pid < 0) //����ʧ��
    {
        printf("Server����ʧ�ܣ�����Ϊ�ػ�����ʧ�ܣ�\n");
        exit(-1);
    }
    if (pid > 0) //�������˳�
    {
        exit(0);
    }
}
