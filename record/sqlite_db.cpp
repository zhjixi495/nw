/*<FH>*************************************************************************
* ��Ȩ����: Copyright (C) 2007-2018, Sinexcel Electric Co., Ltd.
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
#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>  
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "sqlite_db.h"

namespace charger
{

SqliteDb::SqliteDb(char *_filename)
{
    db = NULL;
    
    memset(&filename,0,sizeof(filename));
    strncpy(filename,_filename,sizeof(filename)-1);
}

SqliteDb::~SqliteDb()
{
    closed();
}

int SqliteDb::closed()
{
    if (db)
    {
        sqlite3_close(db); //�ر����ݿ�
        db = NULL;
    }
    
    return 0;
}

int SqliteDb::open()
{
    if (db)
    {
        return 0;
    }
    
    int ret = sqlite3_open(filename, &db); 
    if ( ret != SQLITE_OK )  
    {
        printf("�����ݿ�ʧ�ܣ�%s\n",sqlite3_errmsg(db));
        db = NULL;
        return -1;
    }

    return 0;
}

int SqliteDb::sqlite_void_callback_fun(void *not_used, int argc,char **data, char **az_col_name)
{
    not_used = not_used;

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
    
    return 0;
}


int SqliteDb::exec_sql(char *sql_cmd,char **errmsg,bool debug)
{
    if (db == NULL)
    {
        return -1;
    }
    
    char *err_msg = 0;
    
    int rc;
    
    if (!debug)
    {
        rc = sqlite3_exec(db,sql_cmd,0,0,errmsg);
    }
    else
    {
        rc = exec_sql(this, &SqliteDb::sqlite_void_callback_fun, 0, sql_cmd, errmsg);
    }
    
    if (rc != SQLITE_OK)
    {
        printf("file=%s line =%d rc=%d SQL error: %s\n", __FILE__,__LINE__,rc,err_msg);
        sqlite3_free(errmsg);
    }
    
    
    return  rc;
}

    
}