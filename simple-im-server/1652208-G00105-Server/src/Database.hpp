//���ݿ���
//�����������ݿ�
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
    //���캯��
    Database();
    //��������
    ~Database();
    //��ʼ������
    void init();
    //�ж��û������Ƿ���ȷ
    bool login(const char *name, const char *password);
    //�ж����ݿ����Ƿ�������˺�
    bool haveThisAccount(const char *name);
    //�ж�ĳ���˺��Ƿ�����
    bool isOnline(const char *name);
    //�ж�ĳ���˺��Ƿ��Ѿ��޸Ĺ�����
    bool havaUpdatedPassword(const char *name);
    //��ȡ��ǰ�����û��б�
    void getOnlineList(short &messageLen, char *content, const char *myself);
    //����ʹĳ���û�����
    bool updateOnline(const char *name);
    //����ʹĳ���û�����
    bool updateOffline(const char *name);
    //���һ�������¼
    bool insertChatHistory(const char *sender, const char *receiver, const char *content);
    //�޸�����
    bool updatePassword(const char *name, const char *password);
    //��ȡ�ؿ�����
    int getReviewAmount(const char *name);
    //��ȡ�ؿ���¼
    bool getChatHistory(const char *user1, const char *user2, int reviewIndex, struct message &msg);
};

//���캯��
Database::Database()
{
}

//��������
Database::~Database()
{
    //�ر���������
    mysql_close(mysql);
}

//��ʼ������
void Database::init()
{
    //1.��ʼ��mysql������ʧ�ܷ���NULL
    if ((mysql = mysql_init(NULL)) == NULL)
    {
        printf("���ݿ�����ʧ�ܣ���ʼ�����ݿ�ʧ�ܣ�\n");
        return;
    }

    //2.�������ݿ⣬ʧ�ܷ���NULL
    if (mysql_real_connect(mysql, host, user, passwd, db, 0, NULL, 0) == NULL)
    {
        printf("���ݿ�����ʧ�ܣ�%s\n", mysql_error(mysql));
        return;
    }

    //3.�����ַ���������������ַ����룬��ʹ/etc/my.cnf������Ҳ����
    mysql_set_character_set(mysql, "gbk");
    printf("���ݿ����ӳɹ���\n\n");
}

//�ж����ݿ����Ƿ�������˻�
bool Database::login(const char *name, const char *password)
{
    //1.����MYSQL����
    string command = "SELECT COUNT(*) FROM account WHERE name='";
    command += name;
    command += "' AND password=MD5('";
    command += password;
    command += "');";

    printf("%s\n", command.c_str());

    //2.���в�ѯ���ɹ�����0�������0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("���ݿ��ѯʧ�ܣ�%s\n", mysql_error(mysql));
        return false;
    }

    //3.����ѯ����洢����������NULL�������NULL
    if ((result = mysql_store_result(mysql)) == NULL)
    {
        printf("���ݿ��ѯ����洢ʧ�ܣ�\n");
        return false;
    }

    //4.��ȡ���������ļ�¼
    row = mysql_fetch_row(result);

    //5.�ͷŲ�ѯ���
    mysql_free_result(result);

    return (atoi(row[0]) ? true : false);
}

//�ж��Ƿ�������˻�
bool Database::haveThisAccount(const char *name)
{
    //1.����MYSQL����
    string command = "SELECT COUNT(*) FROM account WHERE name='";
    command += name;
    command += "';";

    printf("%s\n", command.c_str());

    //2.���в�ѯ���ɹ�����0�������0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("���ݿ��ѯʧ�ܣ�%s\n", mysql_error(mysql));
        return false;
    }

    //3.����ѯ����洢����������NULL�������NULL
    if ((result = mysql_store_result(mysql)) == NULL)
    {
        printf("���ݿ��ѯ����洢ʧ�ܣ�\n");
        return false;
    }

    //4.��ȡ���������ļ�¼
    row = mysql_fetch_row(result);

    //5.�ͷŲ�ѯ���
    mysql_free_result(result);

    return (atoi(row[0]) ? true : false);
}

//�ж�ĳ���˻��Ƿ�����
bool Database::isOnline(const char *name)
{
    //1.����MYSQL����
    string command = "SELECT is_online FROM account WHERE name='";
    command += name;
    command += "';";

    printf("%s\n", command.c_str());

    //2.���в�ѯ���ɹ�����0�������0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("���ݿ��ѯʧ�ܣ�%s\n", mysql_error(mysql));
        return false;
    }

    //3.����ѯ����洢����������NULL�������NULL
    if ((result = mysql_store_result(mysql)) == NULL)
    {
        printf("���ݿ��ѯ����洢ʧ�ܣ�\n");
        return false;
    }

    //4.��ȡ���������ļ�¼
    row = mysql_fetch_row(result);

    //5.�ͷŲ�ѯ���
    mysql_free_result(result);

    return (atoi(row[0]) ? true : false);
}

//�ж�ĳ���˻��Ƿ��ǵ�һ�ε�¼
bool Database::havaUpdatedPassword(const char *name)
{
    //1.����MYSQL����
    string command = "SELECT update_password FROM account WHERE name='";
    command += name;
    command += "';";

    printf("%s\n", command.c_str());

    //2.���в�ѯ���ɹ�����0�������0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("���ݿ��ѯʧ�ܣ�%s\n", mysql_error(mysql));
        return false;
    }

    //3.����ѯ����洢����������NULL�������NULL
    if ((result = mysql_store_result(mysql)) == NULL)
    {
        printf("���ݿ��ѯ����洢ʧ�ܣ�\n");
        return false;
    }

    //4.��ȡ���������ļ�¼
    row = mysql_fetch_row(result);

    //5.�ͷŲ�ѯ���
    mysql_free_result(result);

    return (atoi(row[0]) ? true : false);
}

//��ȡ��ǰ�����û��б�
void Database::getOnlineList(short &messageLen, char *content, const char *myself)
{
    //1.��ѯ��������
    //(1)��������
    string command = "SELECT COUNT(*) FROM account WHERE is_online=1 AND name<>'";
    command += myself;
    command += "';";
    printf("%s\n", command.c_str()); //****
    //(2)ִ��
    mysql_query(mysql, command.c_str());
    //(3)����
    result = mysql_store_result(mysql);
    //(4)ȡ���
    row = mysql_fetch_row(result);
    messageLen = htons((short)atoi(row[0]));
    //(5)�ͷŲ�ѯ���
    mysql_free_result(result);

    //2.��ѯ�����û�
    //(1)��������
    command = "SELECT is_online,name FROM account WHERE name<>'";
    command += myself;
    command += "';";
    printf("%s\n", command.c_str()); //****
    //(2)ִ��
    mysql_query(mysql, command.c_str());
    //(3)����
    result = mysql_store_result(mysql);
    //(4)ȡ���
    for (int i = 0; (row = mysql_fetch_row(result)) != NULL;)
    {
        int flag = atoi(row[0]);
        content[i] = flag ? '1' : '0';
        memcpy(content + i + 1, row[1], strlen(row[1]));
        i += USERNAME_LEN;
    }
    //(5)�ͷŲ�ѯ���
    mysql_free_result(result);
}

//����ʹĳ���û�����
bool Database::updateOnline(const char *name)
{
    //1.����MYSQL����
    string command = "UPDATE account SET is_online=1 WHERE name='";
    command += name;
    command += "';";
    printf("%s\n", command.c_str());

    //2.ִ������ɹ�����0�������0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("���ݿ��޸�ʧ�ܣ�%s\n", mysql_error(mysql));
        return false;
    }

    return true;
}

//����ʹĳ���û�����
bool Database::updateOffline(const char *name)
{
    //1.����MYSQL����
    string command = "UPDATE account SET is_online=0 WHERE name='";
    command += name;
    command += "';";
    printf("%s\n", command.c_str());

    //2.ִ������ɹ�����0�������0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("���ݿ��޸�ʧ�ܣ�%s\n", mysql_error(mysql));
        return false;
    }

    return true;
}

//���һ�������¼
bool Database::insertChatHistory(const char *sender, const char *receiver, const char *content)
{
    //1.����MYSQL����
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

    //2.ִ������ɹ�����0�������0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("���ݿ���������¼ʧ�ܣ�%s\n", mysql_error(mysql));
        return false;
    }

    return true;
}

//�޸�����
bool Database::updatePassword(const char *name, const char *password)
{
    //1.����MYSQL����
    string command = "UPDATE account SET password=MD5('";
    command += password;
    command += "'),update_password=1 WHERE name='";
    command += name;
    command += "';";

    printf("%s\n", command.c_str());

    //2.ִ������ɹ�����0�������0
    if (mysql_query(mysql, command.c_str()))
    {
        printf("���ݿ���������¼ʧ�ܣ�%s\n", mysql_error(mysql));
        return false;
    }

    return true;
}

//��ȡ�ؿ�����
int Database::getReviewAmount(const char *name)
{
    //1.����MYSQL����
    string command = "SELECT review_amount FROM account,settings WHERE name='";
    command += name;
    command += "' AND account.num=settings.num;";

    printf("%s\n", command.c_str());

    //2.ִ��
    mysql_query(mysql, command.c_str());
    //3.����
    result = mysql_store_result(mysql);
    //4.ȡ���
    row = mysql_fetch_row(result);
    //5.�ͷŲ�ѯ���
    mysql_free_result(result);
    //6.���ؽ��
    return atoi(row[0]);
}

//��ȡ�ؿ���¼
bool Database::getChatHistory(const char *user1, const char *user2, int reviewIndex, struct message &msg)
{
    //1.����MYSQL����
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
    //2.ִ��
    mysql_query(mysql, command.c_str());
    //3.����
    result = mysql_store_result(mysql);
    //4.ȡ���
    if ((row = mysql_fetch_row(result)) == NULL)
    {
        return false;
    }

    msg.type = 0x66;
    //msg.answer = 0x00;
    msg.messageLen = htons((short)strlen(row[2])); //ȡ���ȡ�ת���͡���������
    memcpy(msg.content, user2, USERNAME_LEN - 1);  //���ͷ�����
    sprintf(msg.content + USERNAME_LEN - 1, " %s %s:\n%s", row[1], row[0], row[2]);

    //5.�ͷŲ�ѯ���
    mysql_free_result(result);
    //6.���ز�ѯ���
    return true;
}
