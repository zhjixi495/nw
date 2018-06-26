/*<FH>*************************************************************************
* 版权所有: Copyright (C) 2007-2018, Sinexcel Electric Co., Ltd.
* 文件名称: 
* 文件标识: 
* 内容摘要: 
* 其它说明: 
* 当前版本: V1.0
* 作    者: 
* 完成日期: 
* 修改记录1: 
*     修改日期: 
*     版 本 号: 
*     修 改 人: 
*     修改内容:
**<FH>************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <linux/unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <iostream> 

#include "mixed_c.h"

char *fill_str_spare_tail_space(char * pstr, int expected_min_len, char fill_char)
{
    int strLen = strlen(pstr);
    int fillBytes = expected_min_len - strLen;

    if(fillBytes > 0)
    {
        memset(pstr + strLen, fill_char, fillBytes);
    }

    pstr[expected_min_len] = '\0';

    return pstr;
}

void msleep(int millis)
{
    if (millis <= 0)
    {
        return;
    }
    
    struct timeval tv;

    while (millis > 0)
    {    
        if (millis < 5000)
        {
            tv.tv_usec = millis % 1000 * 1000;
            tv.tv_sec  = millis / 1000;
            
            millis = 0;
        }
        else
        {
            tv.tv_usec = 0;
            tv.tv_sec  = 5000 / 1000;

            millis -= 5000;
        }
        
        select(0, NULL, NULL, NULL, &tv); 
    }
}

void get_cur_time(char *cur_time)
{
    struct tm when;
    time_t reference_time;
    long reference_millitm;
    timeval tv;
    char time_string[128];
    
    ::time(&reference_time);
    ::gettimeofday(&tv, NULL);
    reference_millitm = tv.tv_usec / (long)1000;
    ::localtime_r(&reference_time, &when);
       
    const char * const TIME_FORMAT_ = "%Y-%m-%d %H:%M:%S"; 
    ::sprintf(time_string, ".%03ld",reference_millitm);
        
    strftime(cur_time,32,TIME_FORMAT_, &when);
    strcat(cur_time, time_string);
}