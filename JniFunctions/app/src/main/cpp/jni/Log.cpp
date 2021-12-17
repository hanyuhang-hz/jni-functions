#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include "inc/Log.h"
#define LOG_BUF_SIZE 512

static bool g_isLogOpen = true;
static bool g_writeLogFile = false;

void enableLog(bool enable)
{
	g_isLogOpen = enable;
}

void dLog(const char *fmt, ...) {
	if (!g_isLogOpen) {
		return;
	}

	va_list ap;
	char buf[LOG_BUF_SIZE];

	va_start(ap, fmt);
	vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
	va_end(ap);

	__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf);
}

void iLog(const char *fmt, ...) {
	if (!g_isLogOpen) {
		return;
	}

	va_list ap;
	char buf[LOG_BUF_SIZE];

	va_start(ap, fmt);
	vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
	va_end(ap);

	__android_log_write(ANDROID_LOG_INFO, LOG_TAG, buf);
}

void wLog(const char *fmt, ...) {
	if (!g_isLogOpen) {
		return;
	}

	va_list ap;
	char buf[LOG_BUF_SIZE];

	va_start(ap, fmt);
	vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
	va_end(ap);

	__android_log_write(ANDROID_LOG_WARN, LOG_TAG, buf);
}

void eLog(const char *fmt, ...) {
	if (!g_isLogOpen) {
		return;
	}

	va_list ap;
	char buf[LOG_BUF_SIZE];

	va_start(ap, fmt);
	vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
	va_end(ap);

	__android_log_write(ANDROID_LOG_ERROR, LOG_TAG, buf);
}

void fLog(const char *fmt, ...) {
	if (!g_isLogOpen) {
		return;
	}

	va_list ap;
	char buf[LOG_BUF_SIZE];

	va_start(ap, fmt);
	vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
	va_end(ap);

	__android_log_write(ANDROID_LOG_FATAL, LOG_TAG, buf);
}

FileLog::FileLog()
{
    memset(m_szFileName,0,sizeof(m_szFileName));
}

int FileLog::SetFilePath(const char* fileName)
{
    if(NULL == fileName || strlen(fileName) < 1)
    {
        eLog("FileLog::FileLog fileName is empty or null ");
        return -1;
    }

    strcpy(m_szFileName,fileName);

    return 0;
}

void FileLog::Write(const char *fmt, ...)
{
    if(strlen(m_szFileName) < 1 || !g_writeLogFile)
    {
        return;
    }
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    FILE *fp = fopen(m_szFileName, "at+");
    if (fp == 0)
    {
        return;
    }

    fwrite(buf, strlen(buf) * sizeof(char), sizeof(char), fp);

    fwrite("\r\n", 2, 1, fp);
    fclose(fp);
}

