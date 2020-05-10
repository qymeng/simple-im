//数据库类
//用于连接数据库
#include "Definition.h"

#include <string>
#include <vector>
#include <sstream>

#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
using namespace std;

const char *host = "localhost";
const char *user = "u1652208";
const char *passwd = "u1652208";
const char *db = "db1652208";

class Database
{
  private:
    MYSQL *mysql;
    MYSQL_RES *result;
    MYSQL_ROW row;

  public:
    //构造函数
    Database();
    //析构函数
    ~Database();
    //初始化函数
    void init();
    //判断用户密码是否正确
    bool login(const char *name, const char *password);
    //判断数据库中是否有这个账号
    bool haveThisAccount(const char *name);
    //判断某个账号是否在线
    bool isOnline(const char *name);
    //判断某个账号是否已经修改过密码
    bool havaUpdatedPassword(const char *name);
    //获取当前在线用户列表
    void getOnlineList(short &messageLen, char *content, const char *myself);
    //更新使某个用户上线
    bool updateOnline(const char *name);
    //更新使某个用户下线
    bool updateOffline(const char *name);
    //添加一条聊天记录
    bool insertChatHistory(const char *sender, const char *receiver, const char *content);
    //修改密码
    bool updatePassword(const char *name, const char *password);
    //获取回看条数
    int getReviewAmount(const char *name);
    //获取回看记录
    bool getChatHistory(const char *user1, const char *user2, int reviewIndex, struct message &msg);
};

//构造函数
Database::Database()
{
}

//析构函数
Database::~Database()
{
    //关闭整个连接
    mysql_close(mysql);
}

//初始化函数
void Database::init()
{
    //1.初始化mysql变量，失败返回NULL
    if ((mysql = mysql_init(NULL)) == NULL)
    {
        printf("数据库连接失败：初始化数据库失败！\n");
        return;
    }

    //2.连接数据库，失败返回NULL
    if (mysql_real_connect(mysql, host, user, passwd, db, 0, NULL, 0) == NULL)
    {
        printf("数据库连接失败：%s\n", mysql_error(mysql));
        return;
    }

    //3.设置字符集，否则读出的字符乱码，即使/etc/my.cnf中设置也不行
    mysql_set_character_set(mysql, "gbk");
    printf("数据库连接成功！\n\n");
}

//判断数据库中是否有这个账户
bool Database::login(const char *name, const char *password)
{
    //1.构造MYSQL命令
    string command = "SELECT COUNT(*) FROM account WHERE name='";
    command += name;
    command += "' AND password=MD5('";
    command += password;
    command += "');";

    printf("%s\n", command.c_str());

    //2.进行查询，成功返回0，否则非0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("数据库查询失败：%s\n", mysql_error(mysql));
        return false;
    }

    //3.将查询结果存储起来，错误NULL，否则非NULL
    if ((result = mysql_store_result(mysql)) == NULL)
    {
        printf("数据库查询结果存储失败！\n");
        return false;
    }

    //4.读取满足条件的记录
    row = mysql_fetch_row(result);

    //5.释放查询结果
    mysql_free_result(result);

    return (atoi(row[0]) ? true : false);
}

//判断是否有这个账户
bool Database::haveThisAccount(const char *name)
{
    //1.构造MYSQL命令
    string command = "SELECT COUNT(*) FROM account WHERE name='";
    command += name;
    command += "';";

    printf("%s\n", command.c_str());

    //2.进行查询，成功返回0，否则非0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("数据库查询失败：%s\n", mysql_error(mysql));
        return false;
    }

    //3.将查询结果存储起来，错误NULL，否则非NULL
    if ((result = mysql_store_result(mysql)) == NULL)
    {
        printf("数据库查询结果存储失败！\n");
        return false;
    }

    //4.读取满足条件的记录
    row = mysql_fetch_row(result);

    //5.释放查询结果
    mysql_free_result(result);

    return (atoi(row[0]) ? true : false);
}

//判断某个账户是否在线
bool Database::isOnline(const char *name)
{
    //1.构造MYSQL命令
    string command = "SELECT is_online FROM account WHERE name='";
    command += name;
    command += "';";

    printf("%s\n", command.c_str());

    //2.进行查询，成功返回0，否则非0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("数据库查询失败：%s\n", mysql_error(mysql));
        return false;
    }

    //3.将查询结果存储起来，错误NULL，否则非NULL
    if ((result = mysql_store_result(mysql)) == NULL)
    {
        printf("数据库查询结果存储失败！\n");
        return false;
    }

    //4.读取满足条件的记录
    row = mysql_fetch_row(result);

    //5.释放查询结果
    mysql_free_result(result);

    return (atoi(row[0]) ? true : false);
}

//判断某个账户是否是第一次登录
bool Database::havaUpdatedPassword(const char *name)
{
    //1.构造MYSQL命令
    string command = "SELECT update_password FROM account WHERE name='";
    command += name;
    command += "';";

    printf("%s\n", command.c_str());

    //2.进行查询，成功返回0，否则非0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("数据库查询失败：%s\n", mysql_error(mysql));
        return false;
    }

    //3.将查询结果存储起来，错误NULL，否则非NULL
    if ((result = mysql_store_result(mysql)) == NULL)
    {
        printf("数据库查询结果存储失败！\n");
        return false;
    }

    //4.读取满足条件的记录
    row = mysql_fetch_row(result);

    //5.释放查询结果
    mysql_free_result(result);

    return (atoi(row[0]) ? true : false);
}

//获取当前在线用户列表
void Database::getOnlineList(short &messageLen, char *content, const char *myself)
{
    //1.查询在线数量
    //(1)构造命令
    string command = "SELECT COUNT(*) FROM account WHERE is_online=1 AND name<>'";
    command += myself;
    command += "';";
    printf("%s\n", command.c_str()); //****
    //(2)执行
    mysql_query(mysql, command.c_str());
    //(3)存结果
    result = mysql_store_result(mysql);
    //(4)取结果
    row = mysql_fetch_row(result);
    messageLen = htons((short)atoi(row[0]));
    //(5)释放查询结果
    mysql_free_result(result);

    //2.查询在线用户
    //(1)构造命令
    command = "SELECT is_online,name FROM account WHERE name<>'";
    command += myself;
    command += "';";
    printf("%s\n", command.c_str()); //****
    //(2)执行
    mysql_query(mysql, command.c_str());
    //(3)存结果
    result = mysql_store_result(mysql);
    //(4)取结果
    for (int i = 0; (row = mysql_fetch_row(result)) != NULL;)
    {
        int flag = atoi(row[0]);
        content[i] = flag ? '1' : '0';
        memcpy(content + i + 1, row[1], strlen(row[1]));
        i += USERNAME_LEN;
    }
    //(5)释放查询结果
    mysql_free_result(result);
}

//更新使某个用户上线
bool Database::updateOnline(const char *name)
{
    //1.构造MYSQL命令
    string command = "UPDATE account SET is_online=1 WHERE name='";
    command += name;
    command += "';";
    printf("%s\n", command.c_str());

    //2.执行命令，成功返回0，否则非0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("数据库修改失败：%s\n", mysql_error(mysql));
        return false;
    }

    return true;
}

//更新使某个用户下线
bool Database::updateOffline(const char *name)
{
    //1.构造MYSQL命令
    string command = "UPDATE account SET is_online=0 WHERE name='";
    command += name;
    command += "';";
    printf("%s\n", command.c_str());

    //2.执行命令，成功返回0，否则非0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("数据库修改失败：%s\n", mysql_error(mysql));
        return false;
    }

    return true;
}

//添加一条聊天记录
bool Database::insertChatHistory(const char *sender, const char *receiver, const char *content)
{
    //1.构造MYSQL命令
    string command = "INSERT INTO chat_history VALUES((SELECT NOW()),";
    command += " (SELECT num FROM account WHERE name='";
    command += sender;
    command += "'),";
    command += " (SELECT num FROM account WHERE name='";
    command += receiver;
    command += "'), '";
    command += content;
    command += "');";

    printf("%s\n", command.c_str());

    //2.执行命令，成功返回0，否则非0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("数据库插入聊天记录失败：%s\n", mysql_error(mysql));
        return false;
    }

    return true;
}

//修改密码
bool Database::updatePassword(const char *name, const char *password)
{
    //1.构造MYSQL命令
    string command = "UPDATE account SET password=MD5('";
    command += password;
    command += "'),update_password=1 WHERE name='";
    command += name;
    command += "';";

    printf("%s\n", command.c_str());

    //2.执行命令，成功返回0，否则非0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("数据库插入聊天记录失败：%s\n", mysql_error(mysql));
        return false;
    }

    return true;
}

//获取回看条数
int Database::getReviewAmount(const char *name)
{
    //1.构造MYSQL命令
    string command = "SELECT review_amount FROM account,settings WHERE name='";
    command += name;
    command += "' AND account.num=settings.num;";

    printf("%s\n", command.c_str());

    //2.执行
    mysql_query(mysql, command.c_str());
    //3.存结果
    result = mysql_store_result(mysql);
    //4.取结果
    row = mysql_fetch_row(result);
    //5.释放查询结果
    mysql_free_result(result);
    //6.返回结果
    return atoi(row[0]);
}

//获取回看记录
bool Database::getChatHistory(const char *user1, const char *user2, int reviewIndex, struct message &msg)
{
    //1.构造MYSQL命令
    ostringstream stream;
    stream << reviewIndex;

    string command = "SELECT send_time, a1.name, content FROM account a1, account a2, chat_history WHERE (a1.name='";
    command += user1;
    command += "' AND a2.name='";
    command += user2;
    command += "' AND a1.num=chat_history.sender AND a2.num=chat_history.receiver) OR (a1.name='";
    command += user2;
    command += "' AND a2.name='";
    command += user1;
    command += "' AND a1.num=chat_history.sender AND a2.num=chat_history.receiver) ";
    command += "ORDER BY send_time DESC LIMIT ";
    command += stream.str();
    command += ",1;";

    printf("%s\n", command.c_str());
    //2.执行
    mysql_query(mysql, command.c_str());
    //3.存结果
    result = mysql_store_result(mysql);
    //4.取结果
    if ((row = mysql_fetch_row(result)) == NULL)
    {
        return false;
    }

    msg.type = 0x66;
    //msg.answer = 0x00;
    msg.messageLen = htons((short)strlen(row[2])); //取长度、转类型、变网络序
    memcpy(msg.content, user2, USERNAME_LEN - 1);  //发送方名字
    sprintf(msg.content + USERNAME_LEN - 1, " %s %s:\n%s", row[1], row[0], row[2]);

    //5.释放查询结果
    mysql_free_result(result);
    //6.返回查询结果
    return true;
}
