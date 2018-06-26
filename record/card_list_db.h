/*<FH>*************************************************************************
* ��Ȩ����: Copyright (C) 2007-2020, Sinexcel Electric Co., Ltd.
* �ļ�����: 
* �ļ���ʶ: 
* ����ժҪ: 
* ����˵��: 
* ��ǰ�汾: V1.0
* ��    ��: 
* �������: 
* �޸ļ�¼1: 
*     �޸�����: 
*     �� �� ��: 
*     �� �� ��: 
*     �޸�����:
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
    //����¼���������������SOC������ԭ�򡢿�ʼʱ�䡢����ʱ��...
    typedef struct
    {
        int id;                                         //��¼��� 4�ֽ�    ��1��ʼһֱ���������ֵ��Ȼ���ٴδ�1ѭ��
        char card_number[32];                            //���� 16�ֽ�
        char modify_time[32];                             //��ʼ���ʱ��
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

