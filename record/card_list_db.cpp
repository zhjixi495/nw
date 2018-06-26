/**************************************************************************
* ��Ȩ����: Copyright (C) 2007-2020, Sinexcel Electric Co., Ltd.
* �ļ�����: 
* �ļ���ʶ:
* ����ժҪ: ��¼�ļ���
* ����˵��:
* ��ǰ�汾: V1.0
* ��    ��:
* �������: 
* �޸ļ�¼1:
*     �޸�����:
*     �� �� ��:
*     �� �� ��:
*     �޸�����:

�г������ݿ��е����б�
.table

//�鿴changer_record���������������
select * from changer_record;

//�鿴changer_record������ĳ���¼����
//select count(1) from changer_record;

//��changer_record��������һ��charge_flow_number,����Ϊvarchar(64)
alter table changer_record add charge_flow_number varchar(64);

update changer_record set charge_flow_number="123abcdef110" where start_charging_soc<=30;

//�鿴changer_record����������һ������¼����
select * from changer_record limit 1 offset 0;

//�鿴��ṹ
select * from sqlite_master WHERE type = "table";

**************************************************************************/
#include "card_list_db.h"
#include <string>
#include <stdlib.h>
#include "config_file.h"
#include "mixed_c.h"



namespace charger
{

CardListDb::CardListDb(bool _debug)
{
    this->debug = _debug;
    card_sqlite_db = new SqliteDb((char *)"card_list.db");
    table_init_ok_flag = false;
    
    white_card_list_cnt = 0;
    black_card_list_cnt = 0;
}

CardListDb::~CardListDb()
{
    if (card_sqlite_db)
    {
        card_sqlite_db->closed();
        delete card_sqlite_db;
        card_sqlite_db = 0;
    }
}



int CardListDb::open()
{
    if (!card_sqlite_db)
    {
        printf("card_sqlite_db = NULL \n");
        return -1;
    }
    
    int ret;
    
    ret = card_sqlite_db->open();
    if(ret)
    {
        return ret;
    }
    
    if (table_init_ok_flag == false)
    {
        
        if (true == create_white_list_db_table())
        {
            if (true == create_black_list_db_table())
            {
                table_init_ok_flag = true;
            }
        }
        else
        {
            return -2;
        }
        
        if (table_init_ok_flag)
        {
            white_card_list_cnt = white_list_size();
            black_card_list_cnt = black_list_size();
            printf("�򿪳��ɹ�����ǰ����%d %d������¼\n",white_card_list_cnt,black_card_list_cnt);
        }
    }
    
    return 0;
}

void CardListDb::closed()
{
    if (card_sqlite_db)
    {
        card_sqlite_db->closed();
    }
}

int CardListDb::white_list_size()
{
    if(open() != 0)
    {
        printf("����¼���ݿ��ʧ��!\n");
        return 0;
    }
    
    char *sql = (char *)"select count(ID) from WHITE_CARD_LIST;";
    int list_cnt = get_record_size(card_sqlite_db,sql);
    
    closed();
    
    white_card_list_cnt = list_cnt;
    
    return white_card_list_cnt;
}

int CardListDb::black_list_size()
{
    if(open() != 0)
    {
        printf("����¼���ݿ��ʧ��!\n");
        return 0;
    }
    
    char *sql = (char *)"select count(ID) from BLACK_CARD_LIST;";
    int list_cnt = get_record_size(card_sqlite_db,sql);
    
    closed();
    
    black_card_list_cnt = list_cnt;
    
    return black_card_list_cnt;
}

int CardListDb::sqlite_callback_fun(void *not_used, int argc,char **data, char **az_col_name)
{
    int idata = *(int *)not_used;
    
    printf("line=%d idata=%d\n",__LINE__,idata);
    
    return sqlite_void_callback_fun(not_used,argc,data,az_col_name);
}

bool CardListDb::create_white_list_db_table()
{
    char *sql;
    char *err_msg = 0;
    int rc;

    //TODO ����¼���ݱ�û�����ƣ�����ֻ�ǳ��������ı��ʵ�ʱ��Ҫ���¸������������£������ӷ�ʱ�ε����ȵ�
    sql=(char *)"CREATE TABLE  IF NOT EXISTS WHITE_CARD_LIST("\
         "id INTEGER PRIMARY KEY   autoincrement," \
         "white_card_number        VARCHAR(32));";

    rc = card_sqlite_db->exec_sql(this,&CardListDb::sqlite_callback_fun,0,sql,&err_msg);
        
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "file=%s line =%d rc=%d SQL error: %s\n", __FILE__,__LINE__,rc,err_msg);
        sqlite3_free(err_msg);
        return false;
    }
    else
    {
        if (debug)
        {
            fprintf(stdout, "���� create_white_list_db_table ��ɹ�\n");
        }
    }
    
    return true;
    /*
            char *sql=(char *)"CREATE TABLE  IF NOT EXISTS TEST_LIST("\
         "id INTEGER PRIMARY KEY   autoincrement," \
         "white_card_number        VARCHAR(32));";
         
         create_db_table(card_sqlite_db,sql);
         */
}

bool CardListDb::create_black_list_db_table()
{
    char *sql;
    char *err_msg = 0;
    int rc;
    
    //TODO ����¼���ݱ�û�����ƣ�����ֻ�ǳ��������ı��ʵ�ʱ��Ҫ���¸������������£������ӷ�ʱ�ε����ȵ�
    sql=(char *)"CREATE TABLE  IF NOT EXISTS BLACK_CARD_LIST("\
         "id INTEGER PRIMARY KEY   autoincrement," \
         "white_card_number        VARCHAR(32));";

    rc = card_sqlite_db->exec_sql(this,&CardListDb::sqlite_callback_fun,0,sql,&err_msg);
        
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "file=%s line =%d rc=%d SQL error: %s\n", __FILE__,__LINE__,rc,err_msg);
        sqlite3_free(err_msg);
        return false;
    }
    else
    {
        if (debug)
        {
            fprintf(stdout, "���� create_black_list_db_table ��ɹ�\n");
        }
    }
    
    return true;
}

}

