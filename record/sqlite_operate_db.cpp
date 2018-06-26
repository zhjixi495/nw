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

**************************************************************************/
#include "sqlite_operate_db.h"
#include <string>
#include <stdlib.h>
#include "config_file.h"
#include "mixed_c.h"



namespace charger
{


SqliteOperateDb::SqliteOperateDb()
{
    
}

SqliteOperateDb::~SqliteOperateDb()
{
   
}




void SqliteOperateDb::charger_record_filed_fill(SdataInfo *sdata_info,char *string)
{
    if (!string)
    {
        return ;
    }
    
    switch(sdata_info->type)
    {
        case DATA_TYPE_CHAR:
        {
            *(char *)sdata_info->pdata = (char)atoi(string);
        }
        break;
        
        case DATA_TYPE_INT16U:
        {
            *(unsigned short *)sdata_info->pdata = (unsigned short)(atoi(string));
        }
        break;
        
        case DATA_TYPE_INT16S:
        {
            *(short *)sdata_info->pdata = (short)(atoi(string));
        }
        break;
        
        case DATA_TYPE_INT:
        {
            *(int *)sdata_info->pdata = atoi(string);
        }
        break;
        
        case DATA_TYPE_STRING:
        {
            memcpy(sdata_info->pdata,string,sdata_info->max_len);
        }
        break;
        
        case DATA_TYPE_INT_STRING:
        {
            //memcpy(sdata_info->pdata,string,sdata_info->max_len);
        }
        break;
        
        
        default:
        break;
    }
    
    
}

int SqliteOperateDb::get_record_size(SqliteDb *db,char *sql)
{
    if ((!db) ||(!sql))
    {
        return 0;
    }
    
    int rc;

    record_size_num = 0;
    
    rc = exec_sql(db,sql,1);
    
    if( rc != SQLITE_OK )
    {
        return 0;
    }
    
    record_size_num = atoi(sql_record_size_buffer);
    
    return record_size_num;
}

int SqliteOperateDb::exec_sql(SqliteDb *db,char *sql,int operate_type)
{
    if ((!db) ||(!sql))
    {
        return -1;
    }
    
    char *err_msg = 0;
    int rc;
    
    rc = db->exec_sql(this,&SqliteOperateDb::sqlite_void_callback_fun,0,sql,&err_msg);
        
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "file=%s line =%d rc=%d SQL error: %s\n", __FILE__,__LINE__,rc,err_msg);
        sqlite3_free(err_msg);
    }
    
    return rc;
}

int SqliteOperateDb::create_db_table(SqliteDb *db,char *sql)
{
    if ((!db) ||(!sql))
    {
        return -1;
    }
    
    int rc;
    
    rc = exec_sql(db,sql,2);
    
    if( rc != SQLITE_OK )
    {
        return -2;
    }
    
    return 0;
}

int SqliteOperateDb::charger_record_num_callback(void *not_used, int argc, char **data, char **az_col_name)
{
    not_used = not_used;
    az_col_name = az_col_name;
    
    if((data[0]) && (argc>=1) && (strlen(data[0])<sizeof(sql_record_size_buffer)))
    {
        strcpy(sql_record_size_buffer,data[0]);
    }
    else
    {
        strcpy(sql_record_size_buffer,"0");
    }
    
    return 0;
}

int SqliteOperateDb::sqlite_void_callback_fun(void *not_used, int argc,char **data, char **az_col_name)
{
    not_used = not_used;
    argc = argc;
    data = data;
    az_col_name = az_col_name;
    
    /*
    char *string_data;
    string_data = (char *)not_used;

    if (string_data)
    {
       printf("line=%d string_data=%s argc=%d\n",__LINE__,string_data,argc);
    }
    
    for(int i=0; i<argc; i++)
    {
        printf("argv[%d]: %s = %s\n",i, az_col_name[i], data[i] ? data[i] : "NULL");
    }
    */
    
    return 0;
}

void SqliteOperateDb::set_db_handle(SqliteDb *_db)
{
    this->db = _db;
}

}

