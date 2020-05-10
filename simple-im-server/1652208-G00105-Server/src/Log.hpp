//Log.hpp
//д��־��
#include <string>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

#define LOG_DIR "./log"
#define LOG_NAME "log.txt"

class Log
{
  private:
    string logPath;     //��־�ļ�·��
    FILE *fp;           //�ļ�ָ��
    struct tm *curTime; //��ǰʱ��ṹ��

    //��ȡ��ǰʱ�䵽�ṹ��
    void getCurrentTime();

  public:
    //���캯��
    Log();
    //��������
    ~Log();
    //��ʼ������
    void init();
    //дһ����־
    void addLog(const char *str);
    void addLog(const char *ip, const char *str);
    void addLog(const char *ip, const char *userName, const char *str);
};

//��ȡ��ǰʱ�䵽tm�ṹ��
void Log::getCurrentTime()
{
    time_t temp = time(NULL);   //��ȡ����ʱ��
    curTime = localtime(&temp); //ת�����ṹ����
}

//���캯��
Log::Log()
{
}

//��������
Log::~Log()
{
    fclose(fp);
}

//��ʼ������
void Log::init()
{
    //1.�����ļ���
    mkdir(LOG_DIR, 0755);

    //2.��ʼ��·��
    logPath = LOG_DIR;
    logPath.append("/");
    logPath.append(LOG_NAME);

    //3.������־�ļ�
    fp = fopen(logPath.c_str(), "a+"); //���ṹ�캯�������쳣
    setbuf(fp, NULL);                  //*����������⣬���

    printf("��־·����%s\n", logPath.c_str());
}

//��¼һ��log
void Log::addLog(const char *str)
{
    getCurrentTime();
    fprintf(fp, "[%d-%02d-%02d %02d:%02d:%02d] %s\n",
            curTime->tm_year + 1900,
            curTime->tm_mon + 1,
            curTime->tm_mday,
            curTime->tm_hour,
            curTime->tm_min,
            curTime->tm_sec,
            str);
}

void Log::addLog(const char *ip, const char *str)
{
    getCurrentTime();
    fprintf(fp, "[%d-%02d-%02d %02d:%02d:%02d] %s��%s\n",
            curTime->tm_year + 1900,
            curTime->tm_mon + 1,
            curTime->tm_mday,
            curTime->tm_hour,
            curTime->tm_min,
            curTime->tm_sec,
            ip,
            str);
}

void Log::addLog(const char *ip, const char *userName, const char *str)
{
    getCurrentTime();
    fprintf(fp, "[%d-%02d-%02d %02d:%02d:%02d] %s(%s)��%s\n",
            curTime->tm_year + 1900,
            curTime->tm_mon + 1,
            curTime->tm_mday,
            curTime->tm_hour,
            curTime->tm_min,
            curTime->tm_sec,
            userName,
            ip,
            str);
}
