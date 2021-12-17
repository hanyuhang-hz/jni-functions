#ifndef _______LOG__H____
#define _______LOG__H____
#define LOG_TAG "HYH_JNI"
#include <stdio.h>
#include <android/log.h>


void enableLog(bool enable);
void dLog(const char *fmt, ...);
void iLog(const char *fmt, ...);
void wLog(const char *fmt, ...);
void eLog(const char *fmt, ...);
void fLog(const char *fmt, ...);


class FileLog
{
public:
    FileLog();
    int SetFilePath(const char* fileName);

    void Write(const char *fmt, ...);

private:
    char m_szFileName[256];
};

#define JNI_LOG 1
#ifdef JNI_LOG

#define LOGD(fmt,...)							   dLog(fmt,##__VA_ARGS__)
#define LOGI(fmt,...)                              iLog(fmt,##__VA_ARGS__)
#define LOGW(fmt,...)                              wLog(fmt,##__VA_ARGS__)
#define LOGE(fmt,...)                              eLog(fmt,##__VA_ARGS__)
#define LOGF(fmt,...)                              fLog(fmt,##__VA_ARGS__)


#define FUNCTION_ENTER	LOGI("%s -------->in", __FUNCTION__)
#define FUNCTION_QUIT	LOGI("%s -------->out", __FUNCTION__)
#define LOGD_HYH     LOGD("(file)%s (function)%s (line)%d (thread)%lu",__FILE__, __FUNCTION__,__LINE__,pthread_self());

#else // JNI_LOG
#define LOGD
#define LOGI
#define LOGW
#define LOGE
#define LOGF

#define FUNCTION_ENTER
#define FUNCTION_QUIT
#endif// JNI_LOG
#define JNI_TIME		//if JNI_TIME be defined, function cost time will be print.

#ifdef JNI_TIME
#define LOGTIME LOGD
#define BEGIN_TIME {\
		MDWord start = MGetCurTimeStamp();
#define END_TIME(FUN) \
		LOGTIME("%s:%s cost time %d ms", __FILE__, FUN, MGetCurTimeStamp()-start);}
#else
#define BEGIN_TIME
#define END_TIME(FUN)
#endif	//end of JNI_TIME


#endif

