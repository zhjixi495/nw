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

#ifndef __TIMER_H__
#define __TIMER_H__

namespace charger
{
    
class Timer
{
public:
    Timer();
    ~Timer();
    static void set_timer_count(unsigned int val);
    static unsigned int get_timer_count();
    static unsigned int get_timer_second();
    static void msleep(int millis);
    static double get_current_time_millis();
        
private:
	Timer(const Timer& other);
    Timer& operator=(const Timer& other);
    
    static unsigned int timer_cnt;
    static void *timer_handle(void *param);
};

}
#endif//__TIMER_H__