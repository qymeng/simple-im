//DealWithMsg.hpp
//用于Server处理数据报
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

  //构造函数
  DealWithMsg();
  //析构函数
  ~DealWithMsg();
  //初始化函数
  void initLog();
  void initDatabase();
  //server发出数据报
  bool sendMessage(struct clientInfo &dstClient, int type,
                   int answer, int messageLen, const char *content);
  bool sendMessage(struct clientInfo &dstClient, const struct message &msg);
  //处理各种类型的数据函数
  /*
    参数表：
    (1)收到的消息msg，为了取出用户名和密码
    (2)存放所有的客户端client引用
    (3)发送来的client的索引index
  */
  //1.处理client->server的报道请求：
  void dealWithMsgRegister(struct message &msg, vector<struct clientInfo> &client, int index);
  //2.处理client->server的文本消息：
  void dealWithMsgText(struct message &msg, vector<struct clientInfo> &client, int index);
  //3.处理client->server的文件消息：
  //略
  //4.处理client->server的修改密码请求：
  void dealWithMsgUpdatePasswd(struct message &msg, vector<struct clientInfo> &client, int index);
  //5.处理client->server的请求回看要求：
  void dealWithMsgHistory(struct message &msg, vector<struct clientInfo> &client, int index);
  //6.处理client->server的退出请求：
  void dealWithMsgExit(struct message &msg, vector<struct clientInfo> &client, int index);
};

//构造函数
DealWithMsg::DealWithMsg()
{
}

//析构函数
DealWithMsg::~DealWithMsg()
{
}

//初始化Log对象
void DealWithMsg::initLog()
{
  myLog.init();
}

//初始化数据库对象
void DealWithMsg::initDatabase()
{
  myDatabase.init();
  myLog.addLog("Server：连接数据库成功！");
}

//server发出数据报
//参数表：
//目的客户端，报文类型，应答类型，报文长度
bool DealWithMsg::sendMessage(struct clientInfo &dstClient, int type, int answer, int messageLen, const char *content)
{
  //1.清空数据报缓冲区
  struct message tempMsg;
  memset(&tempMsg, 0, MSG_SIZE);

  //2.根据数据报类型填写数据报
  tempMsg.type = (char)type;
  tempMsg.answer = (char)answer;
  tempMsg.messageLen = htons((short)messageLen);
  if (content) //没有内容的直接null
  {
    memcpy(tempMsg.content, content, messageLen);
  }

  //3.发送
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
  1.处理client->server的报道请求：
  (1)首先检查用户以及密码是否正确，不正确直接发送报文结束，若正确进行下一步
  (2)检查账号是否已经登录，若已经登录，踢出先登录用户，后登录用户上线
  (3)检查账号是否已经修改过密码，若没有，让client修改密码
  (4)最常规的登录
*/
void DealWithMsg::dealWithMsgRegister(struct message &msg, vector<struct clientInfo> &client, int index)
{
  //1.获取要检查的用户名和密码
  memcpy(client[index].userName, msg.content, USERNAME_LEN - 1);                                       //取数据报中的用户名到结构体
  memcpy(client[index].password, msg.content + USERNAME_LEN - 1, PASSWORD_LEN - 1);                    //取数据报中的密码到结构体
  printf("userName = %s\tpassword = %s 请求认证。\n", client[index].userName, client[index].password); //*仅用于测试

  //2.判断当前用户是否存在
  bool flag = myDatabase.haveThisAccount(client[index].userName);
  if (flag == false) //该账号不存在
  {
    sendMessage(client[index], IM_TYPE_REGISTER_ACK, REGISTER_NOEXIST, 0, NULL);
    printf("该用户不存在！\n");
    myLog.addLog(client[index].clientIP, client[index].userName, "认证失败，该用户不存在！");
    return;
  }

  //3.判断用户密码是否正确
  flag = myDatabase.login(client[index].userName, client[index].password);
  if (flag == false) //密码错误
  {
    sendMessage(client[index], IM_TYPE_REGISTER_ACK, REGISTER_WRONG, 0, NULL);
    printf("密码输入错误！\n");
    myLog.addLog(client[index].clientIP, client[index].userName, "认证失败，密码输入错误！");
    return;
  }

  //4.判断用户是否在线，返回true/false
  flag = myDatabase.isOnline(client[index].userName);
  if (flag == true) //如果当前在线，那么一定不是第一次登录,
  {                 //踢掉先登录用户
    for (int i = 0; i < client.size(); i++)
    {
      if (i != index && strcmp(client[i].userName, client[index].userName) == 0 && client[i].isLogined == TRUE) //*****
      {
        //<1>踢掉先登录的用户
        sendMessage(client[i], IM_TYPE_FORCE_OFFLINE, NO_SENSE, 0, NULL);
        close(client[i].clientSock); //断开先登录用户
        myLog.addLog(client[i].clientIP, client[i].userName, "异地登录，强制下线！");
        client.erase(client.begin() + i); //移除这个client

        //<2>让当前用户上台
        struct message msgBuffer; //填充数据报
        memset(&msgBuffer, 0, MSG_SIZE);
        msgBuffer.type = IM_TYPE_REGISTER_ACK;
        msgBuffer.answer = REGISTER_REMOTE;
        myDatabase.getOnlineList(msgBuffer.messageLen, msgBuffer.content, client[index].userName); //发数据报

        client[index].isLogined = TRUE; //修改连接状态

        sendMessage(client[index], msgBuffer); //向客户端发送应答
        printf("异地登录成功！\n");
        myLog.addLog(client[index].clientIP, client[index].userName, "认证成功，异地登录成功！");

        //<3>通知其他在线用户更新列表
        for (int j = 0; j < client.size(); j++)
        {
          if (client[j].isLogined == TRUE && j != index)
          {
            sendMessage(client[j], IM_TYPE_SOMEONE_ONOFF, SOMEONE_ONLINE, USERNAME_LEN - 1, client[index].userName);
            printf("Server向(%s)发送(%s)上线通知！\n", client[i].userName, client[index].userName);
          }
        }

        return;
      }
    }
  }

  //5.判断用户是否是首次登录
  flag = myDatabase.havaUpdatedPassword(client[index].userName);
  if (flag == false)
  {
    sendMessage(client[index], IM_TYPE_REGISTER_ACK, REGISTER_FIRST, 0, NULL);
    printf("首次登录，快去改密码！\n");
    myLog.addLog(client[index].clientIP, client[index].userName, "首次登录，立即修改密码！");
    return;
  }

  //6.正常登录
  {
    struct message msgBuffer; //填充数据报
    memset(&msgBuffer, 0, MSG_SIZE);
    msgBuffer.type = IM_TYPE_REGISTER_ACK;
    msgBuffer.answer = REGISTER_SUCCESS;
    myDatabase.getOnlineList(msgBuffer.messageLen, msgBuffer.content, client[index].userName);

    myDatabase.updateOnline(client[index].userName); //修改数据库
    client[index].isLogined = TRUE;                  //修改连接状态
    sendMessage(client[index], msgBuffer);           //向客户端发送应答
    printf("登录成功！\n");
    myLog.addLog(client[index].clientIP, client[index].userName, "认证成功，登录成功！");

    for (int i = 0; i < client.size(); i++)
    {
      if (client[i].isLogined == TRUE && i != index)
      {
        sendMessage(client[i], IM_TYPE_SOMEONE_ONOFF, SOMEONE_ONLINE, USERNAME_LEN - 1, client[index].userName);
        printf("Server向(%s)发送(%s)上线通知！\n", client[i].userName, client[index].userName);
      }
    }
  }
}

/*
  2.处理client->server的文本消息：
  (1)取出消息内容到string类型的content
  (2)检查发送的消息是否有语法错误(是否有@，是否有:，receiver的name是否超长)
  (3)检查receiver是单个用户还是all
  (4)若是单个用户，检查是否存在，是否在线
*/
void DealWithMsg::dealWithMsgText(struct message &msg, vector<struct clientInfo> &client, int index)
{
  //1.取出消息内容到string类型的content
  string content = msg.content;
  printf("从 %s 收到：\n", client[index].userName); //****
  printf("\t%s\n", content.c_str());                //****
  myLog.addLog(client[index].clientIP, client[index].userName, msg.content);

  //2.检查发送消息是否有语法错误
  struct clientInfo receiver;                      //存放接收方信息
  memset(&receiver, 0, sizeof(struct clientInfo)); //清零接收方信息缓冲区

  bool flagError = false; //记录是否出错
  int nameEnd;            //记录receiver名字的结尾下标，以取出名字
  int chatBegin;          //记录content的开始下标，以取出真·聊天内容

  if (content[0] != '@') //如果第一个字符不是'@'
  {
    flagError = true;
  }

  if (!flagError && content.find(':') > 0) //如果找得到':'
  {
    nameEnd = content.find(':') - 1;
    chatBegin = nameEnd + 2;
  }
  else if (!flagError && content.find("：") > 0) //如果找得到"："
  {
    nameEnd = content.find("：") - 1;
    chatBegin = nameEnd + 3;
  }
  else //出错
  {
    flagError = true;
  }

  if (!flagError) //如果还没有出错
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

  if (flagError) //出错了
  {
    sendMessage(client[index], IM_TYPE_TEXTINFO_ACK, TRANSMIT_ERROR, 0, NULL); //发送反馈
    printf("转发失败，格式不正确！\n");
    myLog.addLog("Server转发失败：格式不正确！\n");
    return;
  }

  //3.检查用户状态
  if (strcmp(receiver.userName, "all") == 0) //发送给全员
  {
    struct message msgBuffer; //填充数据报
    memset(&msgBuffer, 0, MSG_SIZE);
    msgBuffer.type = IM_TYPE_TEXTINFO_SEND;
    //msgBuffer.answer = 0x00;
    msgBuffer.messageLen = htons((short)content.substr(chatBegin).length());
    memcpy(msgBuffer.content, client[index].userName, USERNAME_LEN - 1);
    sprintf(msgBuffer.content + USERNAME_LEN - 1, ":%s", content.substr(chatBegin).c_str());

    for (int i = 0; i < client.size(); i++)
    {
      if (i != index && client[i].isLogined == TRUE) //查找接收方用户
      {
        sendMessage(client[i], msgBuffer);                                                                           //向客户端发送应答
        myDatabase.insertChatHistory(client[index].userName, client[i].userName, content.substr(chatBegin).c_str()); //插入数据库
        printf("转发到%s成功！\n", client[i].userName);
        myLog.addLog(client[i].clientIP, client[i].userName, "转发到该用户成功！");
      }
    }
  }
  else //发送给指定用户
  {
    bool exist = myDatabase.haveThisAccount(receiver.userName); //判断这个账号是否存在
    if (!exist)                                                 //如果账号不存在
    {
      sendMessage(client[index], IM_TYPE_TEXTINFO_ACK, TRANSMIT_NOEXIST, 0, NULL);
      printf("转发失败，接收方不存在！\n");
      myLog.addLog("Server转发失败：接收方不存在！\n");
      return;
    }

    bool isOnline = myDatabase.isOnline(receiver.userName); //判断接收方是否在线
    if (isOnline)                                           //单个用户在线
    {
      struct message msgBuffer; //填充数据报
      memset(&msgBuffer, 0, MSG_SIZE);
      msgBuffer.type = IM_TYPE_TEXTINFO_SEND;
      //msgBuffer.answer = 0x00;
      msgBuffer.messageLen = htons((short)content.substr(chatBegin).length());
      memcpy(msgBuffer.content, client[index].userName, USERNAME_LEN - 1);
      sprintf(msgBuffer.content + USERNAME_LEN - 1, ":%s", content.substr(chatBegin).c_str());

      for (int i = 0; i < client.size(); i++)
      {
        if (strcmp(client[i].userName, receiver.userName) == 0 && client[i].isLogined == TRUE) //查找接收方用户
        {
          sendMessage(client[i], msgBuffer);                                                                          //向客户端发送应答
          myDatabase.insertChatHistory(client[index].userName, receiver.userName, content.substr(chatBegin).c_str()); //插入数据库
          printf("转发报文成功！\n");
          myLog.addLog(client[i].clientIP, client[i].userName, "转发到该用户成功！");
          return;
        }
      }
    }
    else //该用户不在线
    {
      sendMessage(client[index], IM_TYPE_TEXTINFO_ACK, TRANSMIT_OFFLINE, 0, NULL);
      printf("转发失败，接收方不在线！\n");
      myLog.addLog("Server转发失败：接受方不在线！\n");
    }
  }
}

/*
  3.处理client->server的文件消息：

*/

/*
  4.处理client->server的修改密码请求：
  (1)检查有没有这个用户
  (2)修改密码
  (3)向client发送应答
*/
void DealWithMsg::dealWithMsgUpdatePasswd(struct message &msg, vector<struct clientInfo> &client, int index)
{
  //1.检查有没有这个用户
  bool flag = myDatabase.haveThisAccount(client[index].userName);
  if (flag == false)
  {
    printf("修改密码失败，没有 %s 这个用户！\n", client[index].userName);
    myLog.addLog(client[index].clientIP, client[index].userName, "修改密码失败，没有该用户");
    return;
  }

  //2.获取新密码
  char newPassword[PASSWORD_LEN] = "";
  memcpy(newPassword, msg.content, PASSWORD_LEN - 1);

  //3.修改密码
  flag = myDatabase.updatePassword(client[index].userName, newPassword);
  flag &= myDatabase.login(client[index].userName, newPassword);
  if (flag) //修改成功
  {
    sendMessage(client[index], IM_TYPE_UPDATEPASSWORD_ACK, UPDATE_PASSWORD_SUCCESS, 0, NULL);
    close(client[index].clientSock);
    printf("%s 修改密码成功，连接关闭！\n", client[index].userName);
    myLog.addLog(client[index].clientIP, client[index].userName, "修改密码成功，连接关闭！");
    client.erase(client.begin() + index);
  }
  else //修改失败
  {
    sendMessage(client[index], IM_TYPE_UPDATEPASSWORD_ACK, UPDATE_PASSWORD_FAIL, 0, NULL);
    printf("%s 修改密码失败，大概数据库炸了！\n", client[index].userName);
    myLog.addLog(client[index].clientIP, client[index].userName, "修改密码失败，大概数据库炸了！");
  }
}

/*
  5.处理client->server的回看聊天记录请求：
  (1)从设置表中获取要回看的聊天记录条数
  (2)获取聊天对象名字
  (3)发送聊天记录
*/
void DealWithMsg::dealWithMsgHistory(struct message &msg, vector<struct clientInfo> &client, int index)
{
  //1.获取要回看的聊天记录条数
  //*没有用到消息
  int reviewAmount = myDatabase.getReviewAmount(client[index].userName);

  //2.获取聊天对象名字
  char theOther[USERNAME_LEN] = "";
  memcpy(theOther, msg.content, USERNAME_LEN - 1);
  printf("%s 请求查看 %d 条与 %s 的聊天记录！\n", client[index].userName, reviewAmount, theOther);
  myLog.addLog(client[index].userName, "请求回看聊天记录！");

  //3.发送聊天记录
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

  printf("Server实际发送 %d 条聊天记录！\n", reviewAmount - 1 - i);
  myLog.addLog(client[index].userName, "向该用户发送聊天记录成功！");
}

/*
  6.处理client->server的退出请求：
  (1)向其他在线用户发送“我要退出了”信号
  (2)从数据库表中下线
  (3)从client容器中移除
*/
void DealWithMsg::dealWithMsgExit(struct message &msg, vector<struct clientInfo> &client, int index)
{
  //1.通知其他在线用户更新在线用户表
  for (int i = 0; i < client.size(); i++)
  {
    if (client[i].isLogined == TRUE && i != index)
    {
      sendMessage(client[i], IM_TYPE_SOMEONE_ONOFF, SOMEONE_OFFLINE, USERNAME_LEN - 1, client[index].userName);
      printf("Server向(%s)发送(%s)下线通知！\n", client[i].userName, client[index].userName);
    }
  }

  //2.从数据库里下线
  myDatabase.updateOffline(client[index].userName);

  //3.从client容器里移除
  close(client[index].clientSock);
  printf("用户 %s 主动退出！\n", client[index].userName);
  myLog.addLog(client[index].clientIP, client[index].userName, "用户主动退出！");
  client.erase(client.begin() + index);
}
