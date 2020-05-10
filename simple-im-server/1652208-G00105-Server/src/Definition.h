//Definition.h
//存放各种公共定义
#ifndef IM_DEFINITION
#define IM_DEFINITION

#define TRUE 1
#define FALSE 0
#define QUEUE_LEN 200
#define MAX_CLIENT 20     //最多支持的客户端数量，后期测试的好可能加到100
#define CONTENT_SIZE 1020 //报文内容最大长度
#define MSG_SIZE 1024     //数据报结构体大小
#define IP_LEN 16         //IP字符串长度    *下面三个均预留了一个字节存\0
#define USERNAME_LEN 16   //用户名最大长度
#define PASSWORD_LEN 16   //密码最大长度
#define TIME_LEN 20       //时间戳长度
#define TIME_OUT 20000    //时间间隔

//数据报类型
//IM:即时通讯   REQ:client=>server ACK:server=>client
//client->server
#define IM_TYPE_REGISTER_REQ 0x11       //报到文本
#define IM_TYPE_TEXTINFO_REQ 0x12       //文本消息
#define IM_TYPE_FILEINFO_REQ 0x13       //文件消息
#define IM_TYPE_UPDATEPASSWORD_REQ 0x14 //修改密码
#define IM_TYPE_HISTORY_REQ 0x15        //请求查看消息记录
#define IM_TYPE_EXIT_REQ 0x16           //退出请求

//server->client
#define IM_TYPE_REGISTER_ACK 0x61       //报道应答
#define IM_TYPE_TEXTINFO_ACK 0x62       //文本消息应答
#define IM_TYPE_UPDATEPASSWORD_ACK 0x63 //修改密码应答
#define IM_TYPE_TEXTINFO_SEND 0x64      //server转发的文本消息
#define IM_TYPE_HISTORY_ACK 0x66        //回复消息记录
#define IM_TYPE_SOMEONE_ONOFF 0x67      //有人上下线
#define IM_TYPE_FORCE_OFFLINE 0x68      //强制下线

//应答类型ack
#define NO_SENSE 0x00 //不重要

#define REGISTER_NOEXIST 0x00 //账号不存在
#define REGISTER_SUCCESS 0x01 //非第一次登录，
#define REGISTER_REMOTE 0x02  //异地登录
#define REGISTER_FIRST 0x03   //第一次登录
#define REGISTER_WRONG 0x04   //密码错误

#define TRANSMIT_SUCCESS 0x00 //检查正确，成功转发
#define TRANSMIT_ERROR 0xFF   //语法有误
#define TRANSMIT_NOEXIST 0xFE //用户不存在
#define TRANSMIT_OFFLINE 0xFD //用户不在线

#define UPDATE_PASSWORD_SUCCESS 0x00 //修改密码成功
#define UPDATE_PASSWORD_FAIL 0x01    //修改密码失败

#define SOMEONE_ONLINE 0x00  //有用户登录
#define SOMEONE_OFFLINE 0x01 //有用户退出

//带格式的数据报结构
struct message
{
    char type;                  //数据报类型
    char answer;                //回复类型
    short messageLen;           //报文长度
    char content[CONTENT_SIZE]; //消息内容
};

//客户端的信息
struct clientInfo
{
    int clientSock;              //客户端socket
    int isLogined;               //通过登录
    char clientIP[IP_LEN];       //客户端IP
    char userName[USERNAME_LEN]; //用户名
    char password[PASSWORD_LEN]; //密码
};

#endif