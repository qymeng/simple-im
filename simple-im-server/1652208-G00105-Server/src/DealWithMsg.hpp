//DealWithMsg.hpp
//����Server�������ݱ�
#include "Definition.h"

#include "Log.hpp"
#include "Database.hpp"

#include <string>
#include <vector>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;

class DealWithMsg
{
private:
  Database myDatabase;

public:
  Log myLog;

  //���캯��
  DealWithMsg();
  //��������
  ~DealWithMsg();
  //��ʼ������
  void initLog();
  void initDatabase();
  //server�������ݱ�
  bool sendMessage(struct clientInfo &dstClient, int type,
                   int answer, int messageLen, const char *content);
  bool sendMessage(struct clientInfo &dstClient, const struct message &msg);
  //����������͵����ݺ���
  /*
    ������
    (1)�յ�����Ϣmsg��Ϊ��ȡ���û���������
    (2)������еĿͻ���client����
    (3)��������client������index
  */
  //1.����client->server�ı�������
  void dealWithMsgRegister(struct message &msg, vector<struct clientInfo> &client, int index);
  //2.����client->server���ı���Ϣ��
  void dealWithMsgText(struct message &msg, vector<struct clientInfo> &client, int index);
  //3.����client->server���ļ���Ϣ��
  //��
  //4.����client->server���޸���������
  void dealWithMsgUpdatePasswd(struct message &msg, vector<struct clientInfo> &client, int index);
  //5.����client->server������ؿ�Ҫ��
  void dealWithMsgHistory(struct message &msg, vector<struct clientInfo> &client, int index);
  //6.����client->server���˳�����
  void dealWithMsgExit(struct message &msg, vector<struct clientInfo> &client, int index);
};

//���캯��
DealWithMsg::DealWithMsg()
{
}

//��������
DealWithMsg::~DealWithMsg()
{
}

//��ʼ��Log����
void DealWithMsg::initLog()
{
  myLog.init();
}

//��ʼ�����ݿ����
void DealWithMsg::initDatabase()
{
  myDatabase.init();
  myLog.addLog("Server���������ݿ�ɹ���");
}

//server�������ݱ�
//������
//Ŀ�Ŀͻ��ˣ��������ͣ�Ӧ�����ͣ����ĳ���
bool DealWithMsg::sendMessage(struct clientInfo &dstClient, int type, int answer, int messageLen, const char *content)
{
  //1.������ݱ�������
  struct message tempMsg;
  memset(&tempMsg, 0, MSG_SIZE);

  //2.�������ݱ�������д���ݱ�
  tempMsg.type = (char)type;
  tempMsg.answer = (char)answer;
  tempMsg.messageLen = htons((short)messageLen);
  if (content) //û�����ݵ�ֱ��null
  {
    memcpy(tempMsg.content, content, messageLen);
  }

  //3.����
  int sendReturn = send(dstClient.clientSock, &tempMsg, MSG_SIZE, 0);
  if (sendReturn == MSG_SIZE)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool DealWithMsg::sendMessage(struct clientInfo &dstClient, const struct message &msg)
{
  int sendReturn = send(dstClient.clientSock, &msg, MSG_SIZE, 0);
  if (sendReturn == MSG_SIZE)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*
  1.����client->server�ı�������
  (1)���ȼ���û��Լ������Ƿ���ȷ������ȷֱ�ӷ��ͱ��Ľ���������ȷ������һ��
  (2)����˺��Ƿ��Ѿ���¼�����Ѿ���¼���߳��ȵ�¼�û������¼�û�����
  (3)����˺��Ƿ��Ѿ��޸Ĺ����룬��û�У���client�޸�����
  (4)���ĵ�¼
*/
void DealWithMsg::dealWithMsgRegister(struct message &msg, vector<struct clientInfo> &client, int index)
{
  //1.��ȡҪ�����û���������
  memcpy(client[index].userName, msg.content, USERNAME_LEN - 1);                                       //ȡ���ݱ��е��û������ṹ��
  memcpy(client[index].password, msg.content + USERNAME_LEN - 1, PASSWORD_LEN - 1);                    //ȡ���ݱ��е����뵽�ṹ��
  printf("userName = %s\tpassword = %s ������֤��\n", client[index].userName, client[index].password); //*�����ڲ���

  //2.�жϵ�ǰ�û��Ƿ����
  bool flag = myDatabase.haveThisAccount(client[index].userName);
  if (flag == false) //���˺Ų�����
  {
    sendMessage(client[index], IM_TYPE_REGISTER_ACK, REGISTER_NOEXIST, 0, NULL);
    printf("���û������ڣ�\n");
    myLog.addLog(client[index].clientIP, client[index].userName, "��֤ʧ�ܣ����û������ڣ�");
    return;
  }

  //3.�ж��û������Ƿ���ȷ
  flag = myDatabase.login(client[index].userName, client[index].password);
  if (flag == false) //�������
  {
    sendMessage(client[index], IM_TYPE_REGISTER_ACK, REGISTER_WRONG, 0, NULL);
    printf("�����������\n");
    myLog.addLog(client[index].clientIP, client[index].userName, "��֤ʧ�ܣ������������");
    return;
  }

  //4.�ж��û��Ƿ����ߣ�����true/false
  flag = myDatabase.isOnline(client[index].userName);
  if (flag == true) //�����ǰ���ߣ���ôһ�����ǵ�һ�ε�¼,
  {                 //�ߵ��ȵ�¼�û�
    for (int i = 0; i < client.size(); i++)
    {
      if (i != index && strcmp(client[i].userName, client[index].userName) == 0 && client[i].isLogined == TRUE) //*****
      {
        //<1>�ߵ��ȵ�¼���û�
        sendMessage(client[i], IM_TYPE_FORCE_OFFLINE, NO_SENSE, 0, NULL);
        close(client[i].clientSock); //�Ͽ��ȵ�¼�û�
        myLog.addLog(client[i].clientIP, client[i].userName, "��ص�¼��ǿ�����ߣ�");
        client.erase(client.begin() + i); //�Ƴ����client

        //<2>�õ�ǰ�û���̨
        struct message msgBuffer; //������ݱ�
        memset(&msgBuffer, 0, MSG_SIZE);
        msgBuffer.type = IM_TYPE_REGISTER_ACK;
        msgBuffer.answer = REGISTER_REMOTE;
        myDatabase.getOnlineList(msgBuffer.messageLen, msgBuffer.content, client[index].userName); //�����ݱ�

        client[index].isLogined = TRUE; //�޸�����״̬

        sendMessage(client[index], msgBuffer); //��ͻ��˷���Ӧ��
        printf("��ص�¼�ɹ���\n");
        myLog.addLog(client[index].clientIP, client[index].userName, "��֤�ɹ�����ص�¼�ɹ���");

        //<3>֪ͨ���������û������б�
        for (int j = 0; j < client.size(); j++)
        {
          if (client[j].isLogined == TRUE && j != index)
          {
            sendMessage(client[j], IM_TYPE_SOMEONE_ONOFF, SOMEONE_ONLINE, USERNAME_LEN - 1, client[index].userName);
            printf("Server��(%s)����(%s)����֪ͨ��\n", client[i].userName, client[index].userName);
          }
        }

        return;
      }
    }
  }

  //5.�ж��û��Ƿ����״ε�¼
  flag = myDatabase.havaUpdatedPassword(client[index].userName);
  if (flag == false)
  {
    sendMessage(client[index], IM_TYPE_REGISTER_ACK, REGISTER_FIRST, 0, NULL);
    printf("�״ε�¼����ȥ�����룡\n");
    myLog.addLog(client[index].clientIP, client[index].userName, "�״ε�¼�������޸����룡");
    return;
  }

  //6.������¼
  {
    struct message msgBuffer; //������ݱ�
    memset(&msgBuffer, 0, MSG_SIZE);
    msgBuffer.type = IM_TYPE_REGISTER_ACK;
    msgBuffer.answer = REGISTER_SUCCESS;
    myDatabase.getOnlineList(msgBuffer.messageLen, msgBuffer.content, client[index].userName);

    myDatabase.updateOnline(client[index].userName); //�޸����ݿ�
    client[index].isLogined = TRUE;                  //�޸�����״̬
    sendMessage(client[index], msgBuffer);           //��ͻ��˷���Ӧ��
    printf("��¼�ɹ���\n");
    myLog.addLog(client[index].clientIP, client[index].userName, "��֤�ɹ�����¼�ɹ���");

    for (int i = 0; i < client.size(); i++)
    {
      if (client[i].isLogined == TRUE && i != index)
      {
        sendMessage(client[i], IM_TYPE_SOMEONE_ONOFF, SOMEONE_ONLINE, USERNAME_LEN - 1, client[index].userName);
        printf("Server��(%s)����(%s)����֪ͨ��\n", client[i].userName, client[index].userName);
      }
    }
  }
}

/*
  2.����client->server���ı���Ϣ��
  (1)ȡ����Ϣ���ݵ�string���͵�content
  (2)��鷢�͵���Ϣ�Ƿ����﷨����(�Ƿ���@���Ƿ���:��receiver��name�Ƿ񳬳�)
  (3)���receiver�ǵ����û�����all
  (4)���ǵ����û�������Ƿ���ڣ��Ƿ�����
*/
void DealWithMsg::dealWithMsgText(struct message &msg, vector<struct clientInfo> &client, int index)
{
  //1.ȡ����Ϣ���ݵ�string���͵�content
  string content = msg.content;
  printf("�� %s �յ���\n", client[index].userName); //****
  printf("\t%s\n", content.c_str());                //****
  myLog.addLog(client[index].clientIP, client[index].userName, msg.content);

  //2.��鷢����Ϣ�Ƿ����﷨����
  struct clientInfo receiver;                      //��Ž��շ���Ϣ
  memset(&receiver, 0, sizeof(struct clientInfo)); //������շ���Ϣ������

  bool flagError = false; //��¼�Ƿ����
  int nameEnd;            //��¼receiver���ֵĽ�β�±꣬��ȡ������
  int chatBegin;          //��¼content�Ŀ�ʼ�±꣬��ȡ���桤��������

  if (content[0] != '@') //�����һ���ַ�����'@'
  {
    flagError = true;
  }

  if (!flagError && content.find(':') > 0) //����ҵõ�':'
  {
    nameEnd = content.find(':') - 1;
    chatBegin = nameEnd + 2;
  }
  else if (!flagError && content.find("��") > 0) //����ҵõ�"��"
  {
    nameEnd = content.find("��") - 1;
    chatBegin = nameEnd + 3;
  }
  else //����
  {
    flagError = true;
  }

  if (!flagError) //�����û�г���
  {
    if (content.substr(1, nameEnd).length() > USERNAME_LEN - 1)
    {
      flagError = true;
    }
    else
    {
      printf("%s\n", content.substr(1, nameEnd).c_str());
      strcpy(receiver.userName, content.substr(1, nameEnd).c_str());
    }
  }

  if (flagError) //������
  {
    sendMessage(client[index], IM_TYPE_TEXTINFO_ACK, TRANSMIT_ERROR, 0, NULL); //���ͷ���
    printf("ת��ʧ�ܣ���ʽ����ȷ��\n");
    myLog.addLog("Serverת��ʧ�ܣ���ʽ����ȷ��\n");
    return;
  }

  //3.����û�״̬
  if (strcmp(receiver.userName, "all") == 0) //���͸�ȫԱ
  {
    struct message msgBuffer; //������ݱ�
    memset(&msgBuffer, 0, MSG_SIZE);
    msgBuffer.type = IM_TYPE_TEXTINFO_SEND;
    //msgBuffer.answer = 0x00;
    msgBuffer.messageLen = htons((short)content.substr(chatBegin).length());
    memcpy(msgBuffer.content, client[index].userName, USERNAME_LEN - 1);
    sprintf(msgBuffer.content + USERNAME_LEN - 1, ":%s", content.substr(chatBegin).c_str());

    for (int i = 0; i < client.size(); i++)
    {
      if (i != index && client[i].isLogined == TRUE) //���ҽ��շ��û�
      {
        sendMessage(client[i], msgBuffer);                                                                           //��ͻ��˷���Ӧ��
        myDatabase.insertChatHistory(client[index].userName, client[i].userName, content.substr(chatBegin).c_str()); //�������ݿ�
        printf("ת����%s�ɹ���\n", client[i].userName);
        myLog.addLog(client[i].clientIP, client[i].userName, "ת�������û��ɹ���");
      }
    }
  }
  else //���͸�ָ���û�
  {
    bool exist = myDatabase.haveThisAccount(receiver.userName); //�ж�����˺��Ƿ����
    if (!exist)                                                 //����˺Ų�����
    {
      sendMessage(client[index], IM_TYPE_TEXTINFO_ACK, TRANSMIT_NOEXIST, 0, NULL);
      printf("ת��ʧ�ܣ����շ������ڣ�\n");
      myLog.addLog("Serverת��ʧ�ܣ����շ������ڣ�\n");
      return;
    }

    bool isOnline = myDatabase.isOnline(receiver.userName); //�жϽ��շ��Ƿ�����
    if (isOnline)                                           //�����û�����
    {
      struct message msgBuffer; //������ݱ�
      memset(&msgBuffer, 0, MSG_SIZE);
      msgBuffer.type = IM_TYPE_TEXTINFO_SEND;
      //msgBuffer.answer = 0x00;
      msgBuffer.messageLen = htons((short)content.substr(chatBegin).length());
      memcpy(msgBuffer.content, client[index].userName, USERNAME_LEN - 1);
      sprintf(msgBuffer.content + USERNAME_LEN - 1, ":%s", content.substr(chatBegin).c_str());

      for (int i = 0; i < client.size(); i++)
      {
        if (strcmp(client[i].userName, receiver.userName) == 0 && client[i].isLogined == TRUE) //���ҽ��շ��û�
        {
          sendMessage(client[i], msgBuffer);                                                                          //��ͻ��˷���Ӧ��
          myDatabase.insertChatHistory(client[index].userName, receiver.userName, content.substr(chatBegin).c_str()); //�������ݿ�
          printf("ת�����ĳɹ���\n");
          myLog.addLog(client[i].clientIP, client[i].userName, "ת�������û��ɹ���");
          return;
        }
      }
    }
    else //���û�������
    {
      sendMessage(client[index], IM_TYPE_TEXTINFO_ACK, TRANSMIT_OFFLINE, 0, NULL);
      printf("ת��ʧ�ܣ����շ������ߣ�\n");
      myLog.addLog("Serverת��ʧ�ܣ����ܷ������ߣ�\n");
    }
  }
}

/*
  3.����client->server���ļ���Ϣ��

*/

/*
  4.����client->server���޸���������
  (1)�����û������û�
  (2)�޸�����
  (3)��client����Ӧ��
*/
void DealWithMsg::dealWithMsgUpdatePasswd(struct message &msg, vector<struct clientInfo> &client, int index)
{
  //1.�����û������û�
  bool flag = myDatabase.haveThisAccount(client[index].userName);
  if (flag == false)
  {
    printf("�޸�����ʧ�ܣ�û�� %s ����û���\n", client[index].userName);
    myLog.addLog(client[index].clientIP, client[index].userName, "�޸�����ʧ�ܣ�û�и��û�");
    return;
  }

  //2.��ȡ������
  char newPassword[PASSWORD_LEN] = "";
  memcpy(newPassword, msg.content, PASSWORD_LEN - 1);

  //3.�޸�����
  flag = myDatabase.updatePassword(client[index].userName, newPassword);
  flag &= myDatabase.login(client[index].userName, newPassword);
  if (flag) //�޸ĳɹ�
  {
    sendMessage(client[index], IM_TYPE_UPDATEPASSWORD_ACK, UPDATE_PASSWORD_SUCCESS, 0, NULL);
    close(client[index].clientSock);
    printf("%s �޸�����ɹ������ӹرգ�\n", client[index].userName);
    myLog.addLog(client[index].clientIP, client[index].userName, "�޸�����ɹ������ӹرգ�");
    client.erase(client.begin() + index);
  }
  else //�޸�ʧ��
  {
    sendMessage(client[index], IM_TYPE_UPDATEPASSWORD_ACK, UPDATE_PASSWORD_FAIL, 0, NULL);
    printf("%s �޸�����ʧ�ܣ�������ݿ�ը�ˣ�\n", client[index].userName);
    myLog.addLog(client[index].clientIP, client[index].userName, "�޸�����ʧ�ܣ�������ݿ�ը�ˣ�");
  }
}

/*
  5.����client->server�Ļؿ������¼����
  (1)�����ñ��л�ȡҪ�ؿ��������¼����
  (2)��ȡ�����������
  (3)���������¼
*/
void DealWithMsg::dealWithMsgHistory(struct message &msg, vector<struct clientInfo> &client, int index)
{
  //1.��ȡҪ�ؿ��������¼����
  //*û���õ���Ϣ
  int reviewAmount = myDatabase.getReviewAmount(client[index].userName);

  //2.��ȡ�����������
  char theOther[USERNAME_LEN] = "";
  memcpy(theOther, msg.content, USERNAME_LEN - 1);
  printf("%s ����鿴 %d ���� %s �������¼��\n", client[index].userName, reviewAmount, theOther);
  myLog.addLog(client[index].userName, "����ؿ������¼��");

  //3.���������¼
  int i;
  for (i = reviewAmount - 1; i >= 0; i--)
  {
    struct message tempMsg;
    memset(&tempMsg, 0, MSG_SIZE);
    if (myDatabase.getChatHistory(client[index].userName, theOther, i, tempMsg) == false)
    {
      break;
    }
    usleep(TIME_OUT);
    sendMessage(client[index], tempMsg);
  }

  printf("Serverʵ�ʷ��� %d �������¼��\n", reviewAmount - 1 - i);
  myLog.addLog(client[index].userName, "����û����������¼�ɹ���");
}

/*
  6.����client->server���˳�����
  (1)�����������û����͡���Ҫ�˳��ˡ��ź�
  (2)�����ݿ��������
  (3)��client�������Ƴ�
*/
void DealWithMsg::dealWithMsgExit(struct message &msg, vector<struct clientInfo> &client, int index)
{
  //1.֪ͨ���������û����������û���
  for (int i = 0; i < client.size(); i++)
  {
    if (client[i].isLogined == TRUE && i != index)
    {
      sendMessage(client[i], IM_TYPE_SOMEONE_ONOFF, SOMEONE_OFFLINE, USERNAME_LEN - 1, client[index].userName);
      printf("Server��(%s)����(%s)����֪ͨ��\n", client[i].userName, client[index].userName);
    }
  }

  //2.�����ݿ�������
  myDatabase.updateOffline(client[index].userName);

  //3.��client�������Ƴ�
  close(client[index].clientSock);
  printf("�û� %s �����˳���\n", client[index].userName);
  myLog.addLog(client[index].clientIP, client[index].userName, "�û������˳���");
  client.erase(client.begin() + index);
}
