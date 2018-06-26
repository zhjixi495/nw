/**************************************************************************
* 版权所有: Copyright (C) 2007-2020, Sinexcel Electric Co., Ltd.
* 文件名称: 
* 文件标识:
* 内容摘要: 记录文件类
* 其它说明:
* 当前版本: V1.0
* 作    者:
* 完成日期: 
* 修改记录1:
*     修改日期:
*     版 本 号:
*     修 改 人:
*     修改内容:

列出该数据库中的所有表
.table

//查看changer_record表下面的所有数据
select * from changer_record;

//查看changer_record表下面的充电记录条数
//select count(1) from changer_record;

//在changer_record表中增加一列charge_flow_number,属性为varchar(64)
alter table changer_record add charge_flow_number varchar(64);

update changer_record set charge_flow_number="123abcdef110" where start_charging_soc<=30;

//查看changer_record表下面的最后一条充电记录数据
select * from changer_record limit 1 offset 0;

//查看表结构
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
            printf("打开充电成功，当前共有%d %d条充电记录\n",white_card_list_cnt,black_card_list_cnt);
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
        printf("充电记录数据库打开失败!\n");
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
        printf("充电记录数据库打开失败!\n");
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

    //TODO 充电记录数据表没有完善，以下只是初步创建的表格，实际表格要重新根据需求梳理下，如增加分时段电量等等
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
            fprintf(stdout, "创建 create_white_list_db_table 表成功\n");
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
    
    //TODO 充电记录数据表没有完善，以下只是初步创建的表格，实际表格要重新根据需求梳理下，如增加分时段电量等等
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
            fprintf(stdout, "创建 create_black_list_db_table 表成功\n");
        }
    }
    
    return true;
}

}

