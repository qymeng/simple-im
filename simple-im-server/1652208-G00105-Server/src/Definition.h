//Definition.h
//��Ÿ��ֹ�������
#ifndef IM_DEFINITION
#define IM_DEFINITION

#define TRUE 1
#define FALSE 0
#define QUEUE_LEN 200
#define MAX_CLIENT 20     //���֧�ֵĿͻ������������ڲ��Եĺÿ��ܼӵ�100
#define CONTENT_SIZE 1020 //����������󳤶�
#define MSG_SIZE 1024     //���ݱ��ṹ���С
#define IP_LEN 16         //IP�ַ�������    *����������Ԥ����һ���ֽڴ�\0
#define USERNAME_LEN 16   //�û�����󳤶�
#define PASSWORD_LEN 16   //������󳤶�
#define TIME_LEN 20       //ʱ�������
#define TIME_OUT 20000    //ʱ����

//���ݱ�����
//IM:��ʱͨѶ   REQ:client=>server ACK:server=>client
//client->server
#define IM_TYPE_REGISTER_REQ 0x11       //�����ı�
#define IM_TYPE_TEXTINFO_REQ 0x12       //�ı���Ϣ
#define IM_TYPE_FILEINFO_REQ 0x13       //�ļ���Ϣ
#define IM_TYPE_UPDATEPASSWORD_REQ 0x14 //�޸�����
#define IM_TYPE_HISTORY_REQ 0x15        //����鿴��Ϣ��¼
#define IM_TYPE_EXIT_REQ 0x16           //�˳�����

//server->client
#define IM_TYPE_REGISTER_ACK 0x61       //����Ӧ��
#define IM_TYPE_TEXTINFO_ACK 0x62       //�ı���ϢӦ��
#define IM_TYPE_UPDATEPASSWORD_ACK 0x63 //�޸�����Ӧ��
#define IM_TYPE_TEXTINFO_SEND 0x64      //serverת�����ı���Ϣ
#define IM_TYPE_HISTORY_ACK 0x66        //�ظ���Ϣ��¼
#define IM_TYPE_SOMEONE_ONOFF 0x67      //����������
#define IM_TYPE_FORCE_OFFLINE 0x68      //ǿ������

//Ӧ������ack
#define NO_SENSE 0x00 //����Ҫ

#define REGISTER_NOEXIST 0x00 //�˺Ų�����
#define REGISTER_SUCCESS 0x01 //�ǵ�һ�ε�¼��
#define REGISTER_REMOTE 0x02  //��ص�¼
#define REGISTER_FIRST 0x03   //��һ�ε�¼
#define REGISTER_WRONG 0x04   //�������

#define TRANSMIT_SUCCESS 0x00 //�����ȷ���ɹ�ת��
#define TRANSMIT_ERROR 0xFF   //�﷨����
#define TRANSMIT_NOEXIST 0xFE //�û�������
#define TRANSMIT_OFFLINE 0xFD //�û�������

#define UPDATE_PASSWORD_SUCCESS 0x00 //�޸�����ɹ�
#define UPDATE_PASSWORD_FAIL 0x01    //�޸�����ʧ��

#define SOMEONE_ONLINE 0x00  //���û���¼
#define SOMEONE_OFFLINE 0x01 //���û��˳�

//����ʽ�����ݱ��ṹ
struct message
{
    char type;                  //���ݱ�����
    char answer;                //�ظ�����
    short messageLen;           //���ĳ���
    char content[CONTENT_SIZE]; //��Ϣ����
};

//�ͻ��˵���Ϣ
struct clientInfo
{
    int clientSock;              //�ͻ���socket
    int isLogined;               //ͨ����¼
    char clientIP[IP_LEN];       //�ͻ���IP
    char userName[USERNAME_LEN]; //�û���
    char password[PASSWORD_LEN]; //����
};

#endif