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

#ifndef __SQLITE_DB_H__
#define __SQLITE_DB_H__

#include <stdio.h>
#include <sqlite3.h>
#include <iostream> 
#include <functional>
using namespace std;
using namespace std::placeholders;  
    
namespace charger
{

typedef int (*sqlite_call_back_fun)(void *not_used, int argc, char **argv, char **az_col_name);

typedef std::function<int(void *, int , char **, char **)> SQliteCallBackFun;  

template <typename T>
struct Entry 
{
    void *pdata;
    T* owner;
    int (T::*callback)(void* param, int val, char** arry1, char** arry2);
};

template <typename T>
int callback_proxy(void* param, int val, char** arry1, char** arry2)
{
    Entry<T> *call = (Entry<T> *)param;
    return ((call->owner)->*(call->callback))(call->pdata, val, arry1, arry2);
}

    
class SqliteDb
{
    public:
        SqliteDb(char *_filename);
        virtual ~SqliteDb();
        
        int open();
        int closed();
        int exec_sql(char *sql,int operate_type);
        
        template <typename T>
        int exec_sql(T* own, int(T::*callback)(void* param, int val, char** arry1, char** arry2), void* data, char *sql_cmd,char **errmsg)
        {
            Entry<T> call;
            
            if (db == NULL)
            {
                return -1;
            }
            
            call.owner = own;
            call.callback = callback;
            call.pdata = data;
            
            int rc = sqlite3_exec(db,sql_cmd,&callback_proxy<T>,(void*)&call, errmsg);
            
            return  rc;
        }
        
        int exec_sql(char *sql_cmd,char **errmsg,bool debug = 0);

        
    private:
        SqliteDb(const SqliteDb& other);
        SqliteDb& operator=(const SqliteDb& other);
        
        int sqlite_void_callback_fun(void *not_used, int argc,char **data, char **az_col_name);
        
        char filename[128];
        
        sqlite3 *db;
};
    
}

#endif /*__SQLITE_DB_H__*/