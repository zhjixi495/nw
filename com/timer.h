/*<FH>*************************************************************************
* ��Ȩ����: Copyright (C) 2007-2013, Sinexcel Electric Co., Ltd.
* �ļ�����: 
* �ļ���ʶ: 
* ����ժҪ: 
* ����˵��: 
* ��ǰ�汾: V1.0
* ��    ��: ZJX
* �������:  
* �޸ļ�¼1: 
*     �޸�����: 
*     �� �� ��: 
*     �� �� ��: 
*     �޸�����:
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