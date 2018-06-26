/*<FH>*************************************************************************
* 版权所有: Copyright (C) 2007-2020, Sinexcel Electric Co., Ltd.
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

#ifndef __CARD_LIST_DB_H__
#define __CARD_LIST_DB_H__

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <deque>
#include <map>
#include "sqlite_db.h"
#include "sqlite_operate_db.h"
#include "base_type.h"


namespace charger
{    
    //充电记录：充入电量、结束SOC、结束原因、开始时间、结束时间...
    typedef struct
    {
        int id;                                         //记录序号 4字节    从1开始一直到等于最大值，然后再次从1循环
        char card_number[32];                            //卡号 16字节
        char modify_time[32];                             //开始充电时间
    } CardInfo;
    

    class CardListDb:public SqliteOperateDb
    {
    public:
        CardListDb(bool _debug = false);
        virtual ~CardListDb();
        
        void closed();
        int open();
        
        int white_list_size();
        int black_list_size();
        
        bool get_white_list(unsigned int index, CardInfo &card_info);
        bool get_black_list(unsigned int index, CardInfo &card_info);
        
        bool add_white_list(char *card_number);
        bool add_black_list(char *card_number);
        
        bool find_white_card(char *card_number);
        bool find_black_card(char *card_number);
        
    private:
        CardListDb(const CardListDb& other);
        CardListDb& operator=(const CardListDb& other);
        
        int white_card_list_cnt;
        int black_card_list_cnt;
        
        bool create_white_list_db_table();
        bool create_black_list_db_table();
        int sqlite_callback_fun(void *not_used, int argc,char **data, char **az_col_name);
        
        int charger_record_num_limit(int num);
        
        SqliteDb *card_sqlite_db;
        int table_init_ok_flag;
        
        char sql_string[256];
        bool debug;
    };
}

#endif  // __CARD_LIST_DB_H__

