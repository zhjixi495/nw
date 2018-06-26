/*<FH>*************************************************************************
* 版权所有: Copyright (C) 2007-2013, Sinexcel Electric Co., Ltd.
* 文件名称: 
* 文件标识: 
* 内容摘要: 
* 其它说明: 
* 当前版本: V1.0
* 作    者: ZJX
* 完成日期:  
* 修改记录1: 
*     修改日期: 
*     版 本 号: 
*     修 改 人: 
*     修改内容:
**<FH>************************************************************************/
#include "timer.h"

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

namespace charger{
    
unsigned int Timer::timer_cnt = 0;

Timer::Timer()
{
    int stacksize = 128*1024; //thread 堆栈设置为128K，stacksize以字节为单位
    pthread_attr_t attr;    
    pthread_attr_init(&attr); //初始化线程属性
    pthread_attr_setstacksize(&attr, stacksize);
    pthread_t id;
    if (::pthread_create(&id, &attr, &timer_handle, (void*)this) == 0)
    {
        ::pthread_detach(id);
    }
}

Timer::~Timer()
{
    
}

void Timer::set_timer_count(unsigned int val)
{
    timer_cnt = val;
}

unsigned int Timer::get_timer_count()
{
    return timer_cnt;
}

unsigned int Timer::get_timer_second()
{
    unsigned int timerSecond = time(NULL);
    
    return timerSecond;
}


double Timer::get_current_time_millis()
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
    {
        tv.tv_sec = 0;
        tv.tv_usec = 0;
    }

    return tv.tv_sec*(double)1000 + tv.tv_usec/(double)1000;
}
    
void Timer::msleep(int millis)
{
    if (millis <= 0)
    {
        ::sleep(0);
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
        
        ::select(0, NULL, NULL, NULL, &tv); 
    }
}

void *Timer::timer_handle(void *param)
{
    Timer* handler = (Timer*)param;
    struct timeval tempval;
    char tname[16];
    memset(tname, 0, sizeof(tname));
    
    prctl(PR_SET_NAME, (unsigned long)"sys_timer"); //设置线程名，最大15个字节
    
    prctl(PR_GET_NAME, (unsigned long)tname);
    
    printf("%s %s - Thread tid=%d tname=%s\n",  __FILE__,__PRETTY_FUNCTION__, (int)syscall(__NR_gettid), tname);

    while(1)
    {
        tempval.tv_sec = 0;
        tempval.tv_usec = 10*1000;
        select(0,NULL,NULL,NULL,&tempval);
        handler->timer_cnt++;
    }
    
    return 0;
}

}
