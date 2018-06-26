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

#ifndef __SQLITE_OPERATE_DB_H__
#define __SQLITE_OPERATE_DB_H__

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <deque>
#include <map>
#include "base_type.h"
#include "sqlite_db.h"

namespace charger
{
    typedef struct
    {
        void *pdata;
        const char *filed_name;
        int type;
        int max_len;
    }SdataInfo;
    typedef std::map<int, SdataInfo> ChargeRecordStructFieldMap;
    
    enum SQL_OPERATE_TYPE
    {
        E_EMPTY_CALL_BACK_OPERATE = 0,
        E_GET_RECORD_SIZE_OPERATE,
    };
    
    class SqliteOperateDb
    {
    public:
        SqliteOperateDb();
        virtual ~SqliteOperateDb();


        SqliteOperateDb(const SqliteOperateDb& other);
        SqliteOperateDb& operator=(const SqliteOperateDb& other);
        
        int get_record_size(SqliteDb *db,char *sql);
        int create_db_table(SqliteDb *db,char *sql);
        int charger_record_num_callback(void *not_used, int argc, char **data, char **az_col_name);
        int sqlite_void_callback_fun(void *not_used, int argc,char **data, char **az_col_name);
        void charger_record_filed_fill(SdataInfo *sdata_info,char *string);
        int exec_sql(SqliteDb *db,char *sql,int operate_type);
        
        void set_db_handle(SqliteDb *_db);
     private:
        char sql_record_size_buffer[128];
        unsigned int  record_size_num;
        
        SqliteDb *db;
    };
}

#endif  // __SQLITE_OPERATE_DB_H__

