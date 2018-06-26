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
#include <functional>
#include "charger_record_db.h"
#include "card_list_db.h"
#include "base_type.h"
#include "mixed_c.h"

using namespace charger;
using namespace std;
using namespace std::placeholders;  



int main(void)
{
    ChargingRecord charger_record_data;
    ChargerRecordFile charger_record;
    CardListDb card_list_db;
    
    cur_version_info((char *)"1.0");
    
    charger_record.open();
    //printf("file=%s line=%d fun=%s\n",__FILE__,__LINE__,__FUNCTION__);
    
    for(unsigned int i=0;i<3;i++)
    {
        memset(&charger_record_data,0,sizeof(charger_record_data));
        
        sprintf(charger_record_data.card_number,"112233445566%04d",i);
        
        
        
        for(unsigned int j=0;j<sizeof(charger_record_data.car_vin)-1;j++)
        {
            charger_record_data.car_vin[j]= (char)(rand()%10+'0');
        }
        
        charger_record_data.start_charging_soc = (char)(rand()%30);
        
        charger_record_data.end_charging_soc = (char)(charger_record_data.start_charging_soc+50);
        
        get_cur_time(charger_record_data.start_time);
        msleep(50);
        get_cur_time(charger_record_data.end_time);
        
        charger_record.add(charger_record_data);
        
        msleep(300);
    }
    
    int charger_record_num = charger_record.size();
    
    printf("当前充电记录条数为:%d\n\n",charger_record_num);
    
    
    for(unsigned int i=0;i<5;i++)
    {
        memset(&charger_record_data,0,sizeof(charger_record_data));
        charger_record.get(i,charger_record_data);
        
        printf("charger_record_data.id=%d  card_number=%s \n",charger_record_data.id,charger_record_data.card_number);
        printf("charger_record_data.start_time=%s charger_record_data.end_time=%s\n\n",charger_record_data.start_time,charger_record_data.end_time);
    }

   
    card_list_db.open();
    card_list_db.closed(); 
    return 0;
}
