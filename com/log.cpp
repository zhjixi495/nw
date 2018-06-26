#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <errno.h>
#include <sys/syscall.h>
#include "mutex_and_condvar.h"
#include "log.h"


namespace charger
{
    

static char log_file_name[128] = "/tmp/test.log";
static int max_log_file_size = 2*1024*1024;
static int log_circle_write_offset = 50*1024;
    
static inline char* time_of_day(char*, int);
extern void print_time(FILE* stream)
{
    char buf[sizeof"0000-00-00 00:00:00.000"] = {0};
    time_of_day(buf, (int)sizeof(buf));
    fprintf(stream, "%s", buf);
}

static inline char *time_of_day(char* to_str, int32_t len)
{
    struct timeval systemTime;
    if (0 != gettimeofday(&systemTime, NULL))
    {
        return to_str;
    }

    strncpy(to_str, "0000-00-00 00:00:00.000", len);
    int32_t offset = strftime(to_str, len, "%F %H:%M:%S", localtime(&systemTime.tv_sec));
    snprintf(to_str + offset, len - offset, ".%04u", (unsigned)systemTime.tv_usec / 100);

    return to_str;
};

/*
static uint64_t gettid() 
{
    pthread_t tid = pthread_self();
    uint64_t thread_id = 0;
    memcpy(&thread_id, &tid, std::min(sizeof(thread_id), sizeof(tid)));
    return thread_id;
}
*/

extern void log_hex(FILE* stream, char* log_data, int32_t len)
{
    char buf[512] = {0};
    char* ptr = buf;
    if (sizeof(buf) < (unsigned)len + 1)
    {
        ptr = new char[len + 1];
    } 

    int32_t offset = 0; 
    for (int i = 0; i < len; i++)
    {
        int ret = snprintf(ptr + offset, sizeof("FF "), "%02X ", (unsigned char)log_data[i]);
        offset += ret;
    }
    fprintf(stream, "%s\n", ptr);

    if (ptr != buf)
    {
        delete[] ptr;
    }
}

class Writer
{
    public:
        Writer() {  }
        virtual ~Writer() {  }
        virtual int32_t write(const char* str, int len) = 0;
    private:
        Writer(const Writer&);
        void operator=(const Writer&);
};

class LogWriter : public Writer
{
    public:
        enum
        {
            kDEFAULT_LOG_FILE_SIZE = 5*1024*1024,
            KDEFAULT_CIRCLE_WRITE_OFFSET = 100*1024,
        };
        struct Options
        {
            Options() : file_path("/tmp/unname.log"),
            max_file_size(kDEFAULT_LOG_FILE_SIZE),
            circle_write_offset(kDEFAULT_LOG_FILE_SIZE)
            {  }
            const char* file_path;
            int32_t max_file_size;
            int32_t circle_write_offset;
        };
        explicit LogWriter(const Options& opt);
        virtual ~LogWriter();
        virtual int32_t write(const char* str, int len);

    private:
        void circle_write_stream_relocate();
        void print_circle_write_tips();
        FILE* m_stream;

        tcu::Mutex m_mu;
        Options m_opt;
};

LogWriter::LogWriter(const Options& opt) : m_opt(opt)
{
    m_stream = fopen(opt.file_path, "wb+");
    if (!m_stream)
    {
        fprintf(stderr, "log writer fopen: %s\n", strerror(errno));
    }
}

LogWriter::~LogWriter()
{
    if(!m_stream)
    {
        fclose(m_stream);
    }
}

int32_t LogWriter::write(const char* str, int len)
{
    tcu::MutexLock lock(&m_mu);
    circle_write_stream_relocate();
    return fwrite(str, 1, len, m_stream);
}

inline void LogWriter::circle_write_stream_relocate()
{
    if (ftell(m_stream) >= m_opt.max_file_size)
    {
        fseek(m_stream, m_opt.circle_write_offset, SEEK_SET);
        print_circle_write_tips();
    }
}

inline void LogWriter::print_circle_write_tips()
{
    char buf[256] = {0};

    int ret = snprintf(buf, sizeof(buf), 
                       "\n============================================================\n");
    ret += snprintf(buf + ret, sizeof(buf) - ret, 
                    "\n\t日记长度超过 %0.2f 兆，从 %0.2fKb 覆盖保存\n", 
                    static_cast<float>(m_opt.max_file_size)/1024/1024, 
                    static_cast<float>(m_opt.circle_write_offset)/1024);
    snprintf(buf + ret, sizeof(buf) - ret, 
             "\n============================================================\n");
    fprintf(m_stream, buf);
    fprintf(stdout, buf);
    fflush(m_stream);fflush(stdout);
}

class Logger 
{
    public:
        Logger() {}
        virtual ~Logger() {}
        virtual void logv(const char *format, va_list ap,int mode) = 0;
    private:
        Logger(const Logger&);
        void operator= (const Logger&);
};

class DebugLogger : public Logger
{
    public:
        static DebugLogger& get_instance(const char* save_file)
        {
            static DebugLogger* logger = new DebugLogger(save_file);
            return *logger;
        }
        virtual ~DebugLogger() {}
        virtual void logv(const char *format, va_list ap,int mode);

    private:
        explicit DebugLogger(const char* save_file);
        Writer* m_writer;
};

DebugLogger::DebugLogger(const char* save_file)
{
    LogWriter::Options opt;
    opt.file_path = save_file;
    opt.max_file_size = max_log_file_size;
    opt.circle_write_offset = log_circle_write_offset;
    m_writer =  new LogWriter(opt);
}

void DebugLogger::logv(const char *format, va_list ap,int mode) 
{
    char buffer[500] = {0};
    for (int iter = 0; iter < 2; iter++) 
    {
        char* base;
        int bufsize;

        if (iter == 0) 
        {
            bufsize = sizeof(buffer);
            base = buffer;
        }
        else 
        {
            bufsize = 30000;
            base = new char[bufsize];
        }

        char* p = base;
        char* limit = base + bufsize;
        
        if (mode&kLOG_PRINTF_TIME_MODE)
        {
            struct timeval now_tv;
            gettimeofday(&now_tv, NULL);
            const time_t seconds = now_tv.tv_sec;
            struct tm t;
            localtime_r(&seconds, &t);
            
            if((mode&kE_LOG_PRINTF_FLAG) ==0)
            {
                 p += snprintf(p, limit - p,"[I ");
            }
            else
            {
                p += snprintf(p, limit - p,ERROR_COLOR);
                p += snprintf(p, limit - p,"[E ");
            }
            
            p += snprintf(p, limit - p,
                          "%04d/%02d/%02d-%02d:%02d:%02d.%06d pid:%d ",
                          t.tm_year + 1900,
                          t.tm_mon + 1,
                          t.tm_mday,
                          t.tm_hour,
                          t.tm_min,
                          t.tm_sec,
                          static_cast<int>(now_tv.tv_usec),
                          (int)syscall(__NR_gettid));
        }
        
        if (p < limit) 
        {
            va_list backup_ap;
            va_copy(backup_ap, ap);
            p += vsnprintf(p, limit - p, format, backup_ap);
            va_end(backup_ap);
        }
        
        if (mode&kLOG_PRINTF_TIME_MODE)
        {
            p += snprintf(p, limit - p,CLOSE_ERROR_COLOR);
        }
        
        if (p >= limit) 
        {
            if (iter == 0) {
                continue;       // Try again with larger buffer
            } else {
                p = limit - 1;
            }
        }
        
        /*
        if (p == base || p[-1] != '\n') 
        {
            *p++ = '\n';
        }
        */

        assert(p <= limit);
        
        if (mode&kLOG_FILE_FLAG)
        {
            m_writer->write(base, p - base);
        }
        
        fprintf(stdout, base); fflush(stdout);
        if (base != buffer) 
        {
            delete[] base;
        }
        break;
    }
}

int log_filename_init(char *file_name,int log_file_size)
{
    int len = strlen(file_name);
    
    max_log_file_size = log_file_size;
    
    if (max_log_file_size <256*1024)
    {
        max_log_file_size = 256*1024;
        log_circle_write_offset = 10*1024;
    }
    else
    {
        log_circle_write_offset = 20*1024;
    }
    
    
    
    if (len >= (int)sizeof(log_file_name))
    {
        return -1;
    }
    
    strcpy(log_file_name,file_name);
    
    printf("log_file_name:%s max_log_file_size=%d\n",log_file_name,max_log_file_size);
    
    return 0;
}


void debug_log_mode(int mode,const char* format, ...)
{
    Logger* info_log = &DebugLogger::get_instance(log_file_name);
    va_list ap;
    
    va_start(ap, format);
    info_log->logv(format, ap,mode);
    va_end(ap);
}


} // namespace tcu
