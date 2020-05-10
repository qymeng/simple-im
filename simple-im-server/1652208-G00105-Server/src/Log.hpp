//Log.hpp
//写日志类
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
    string logPath;     //日志文件路径
    FILE *fp;           //文件指针
    struct tm *curTime; //当前时间结构体

    //获取当前时间到结构体
    void getCurrentTime();

  public:
    //构造函数
    Log();
    //析构函数
    ~Log();
    //初始化函数
    void init();
    //写一条日志
    void addLog(const char *str);
    void addLog(const char *ip, const char *str);
    void addLog(const char *ip, const char *userName, const char *str);
};

//获取当前时间到tm结构体
void Log::getCurrentTime()
{
    time_t temp = time(NULL);   //获取秒数时间
    curTime = localtime(&temp); //转换到结构体中
}

//构造函数
Log::Log()
{
}

//析构函数
Log::~Log()
{
    fclose(fp);
}

//初始化函数
void Log::init()
{
    //1.建立文件夹
    mkdir(LOG_DIR, 0755);

    //2.初始化路径
    logPath = LOG_DIR;
    logPath.append("/");
    logPath.append(LOG_NAME);

    //3.建立日志文件
    fp = fopen(logPath.c_str(), "a+"); //不会构造函数处理异常
    setbuf(fp, NULL);                  //*解决缓存问题，大坑

    printf("日志路径：%s\n", logPath.c_str());
}

//记录一条log
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
    fprintf(fp, "[%d-%02d-%02d %02d:%02d:%02d] %s：%s\n",
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
    fprintf(fp, "[%d-%02d-%02d %02d:%02d:%02d] %s(%s)：%s\n",
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
