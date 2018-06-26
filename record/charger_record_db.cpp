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
#include "charger_record_db.h"
#include <string>
#include <stdlib.h>
#include "config_file.h"
#include "mixed_c.h"



namespace charger
{

//const unsigned int ChargerRecordFile::max_charger_num = 50000;
const unsigned int ChargerRecordFile::max_charger_num = 10;

/*
SdataInfo s_data_infos[] = 
{
    {&m_charge_record.id,"id",DATA_TYPE_INT,0},
    {&m_charge_record.charging_pile_number,"charging_pile_number",DATA_TYPE_STRING,0},
};
*/

ChargerRecordFile::ChargerRecordFile(bool _debug)
{
    this->debug = _debug;
    sqlite_db = new SqliteDb((char *)"charger_record.db");
    charger_table_create_ok_flag = false;
    charger_num = 0;
    
    int index = 0;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.id;
    charge_record_struct_field_map[index].filed_name = "id";
    charge_record_struct_field_map[index].type = DATA_TYPE_INT;
    charge_record_struct_field_map[index].max_len = 4;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.charging_pile_number;
    charge_record_struct_field_map[index].filed_name = "charging_pile_number";
    charge_record_struct_field_map[index].type = DATA_TYPE_STRING;
    charge_record_struct_field_map[index].max_len = sizeof(m_charge_record.charging_pile_number)-1;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.charge_flow_number;
    charge_record_struct_field_map[index].filed_name = "charge_flow_number";
    charge_record_struct_field_map[index].type = DATA_TYPE_STRING;
    charge_record_struct_field_map[index].max_len = sizeof(m_charge_record.charge_flow_number)-1;
    index++;

    charge_record_struct_field_map[index].pdata = &m_charge_record.card_number;
    charge_record_struct_field_map[index].filed_name = "card_number";
    charge_record_struct_field_map[index].type = DATA_TYPE_STRING;
    charge_record_struct_field_map[index].max_len = sizeof(m_charge_record.card_number)-1;
    index++;
    

    charge_record_struct_field_map[index].pdata = &m_charge_record.car_vin;
    charge_record_struct_field_map[index].filed_name = "car_vin";
    charge_record_struct_field_map[index].type = DATA_TYPE_STRING;
    charge_record_struct_field_map[index].max_len = sizeof(m_charge_record.car_vin)-1;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.plate_number;
    charge_record_struct_field_map[index].filed_name = "plate_number";
    charge_record_struct_field_map[index].type = DATA_TYPE_STRING;
    charge_record_struct_field_map[index].max_len = sizeof(m_charge_record.plate_number)-1;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.start_charging_soc;
    charge_record_struct_field_map[index].filed_name = "start_charging_soc";
    charge_record_struct_field_map[index].type = DATA_TYPE_CHAR;
    charge_record_struct_field_map[index].max_len = 1;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.end_charging_soc;
    charge_record_struct_field_map[index].filed_name = "end_charging_soc";
    charge_record_struct_field_map[index].type = DATA_TYPE_CHAR;
    charge_record_struct_field_map[index].max_len = 1;
    index++;
    
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.charging_energy;
    charge_record_struct_field_map[index].filed_name = "charging_energy";
    charge_record_struct_field_map[index].type = DATA_TYPE_INT16U;
    charge_record_struct_field_map[index].max_len = 2;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.start_meter_val;
    charge_record_struct_field_map[index].filed_name = "start_meter_val";
    charge_record_struct_field_map[index].type = DATA_TYPE_INT;
    charge_record_struct_field_map[index].max_len = 4;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.end_meter_val;
    charge_record_struct_field_map[index].filed_name = "end_meter_val";
    charge_record_struct_field_map[index].type = DATA_TYPE_INT;
    charge_record_struct_field_map[index].max_len = 4;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.end_meter_val;
    charge_record_struct_field_map[index].filed_name = "charging_time";
    charge_record_struct_field_map[index].type = DATA_TYPE_INT;
    charge_record_struct_field_map[index].max_len = 4;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.charging_full_policy;
    charge_record_struct_field_map[index].filed_name = "charging_full_policy";
    charge_record_struct_field_map[index].type = DATA_TYPE_CHAR;
    charge_record_struct_field_map[index].max_len = 1;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.normal_end;
    charge_record_struct_field_map[index].filed_name = "normal_end";
    charge_record_struct_field_map[index].type = DATA_TYPE_CHAR;
    charge_record_struct_field_map[index].max_len = 1;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.start_time;
    charge_record_struct_field_map[index].filed_name = "start_time";
    charge_record_struct_field_map[index].type = DATA_TYPE_STRING;
    charge_record_struct_field_map[index].max_len = sizeof(m_charge_record.start_time)-1;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.end_time;
    charge_record_struct_field_map[index].filed_name = "end_time";
    charge_record_struct_field_map[index].type = DATA_TYPE_STRING;
    charge_record_struct_field_map[index].max_len = sizeof(m_charge_record.end_time)-1;
    index++;
    
    charge_record_struct_field_map[index].pdata = &m_charge_record.period_electric_quantity_data;
    charge_record_struct_field_map[index].filed_name = "period_electric_quantity_data";
    charge_record_struct_field_map[index].type = DATA_TYPE_INT_STRING;
    charge_record_struct_field_map[index].max_len = sizeof(m_charge_record.end_time)-1;
    index++;
}

ChargerRecordFile::~ChargerRecordFile()
{
    if (sqlite_db)
    {
        sqlite_db->closed();
        delete sqlite_db;
        sqlite_db = 0;
    }
}



int ChargerRecordFile::open()
{
    if (!sqlite_db)
    {
        printf("sqlite_db = NULL \n");
        return -1;
    }
    
    int ret;
    
    ret = sqlite_db->open();
    if(ret)
    {
        return ret;
    }
    
    if (charger_table_create_ok_flag == false)
    {
        if (true == create_charger_db_table())
        {
            charger_table_create_ok_flag = true;
        }
        else
        {
            return -2;
        }
        
        if (charger_table_create_ok_flag)
        {
            charger_num = size();
            
            printf("打开充电成功，当前共有%d条充电记录\n",charger_num);
        }
    }
    
    return 0;
}

void ChargerRecordFile::closed()
{
    if (sqlite_db)
    {
        sqlite_db->closed();
    }
}




bool ChargerRecordFile::get(unsigned int index, ChargingRecord &record)
{
    if(open() != 0)
    {
        printf("充电记录数据库打开失败!\n");
        return -1;
    }
    
    if (index >= charger_num)
    {
        printf("获取的充电记录不存在\n");
        return -2;
    }
    
    
    char *err_msg = 0;
    int rc;
    char *sql = (char *)"select * from changer_record limit 1 offset %d";
    
    sprintf(sql_string,sql,charger_num-index-1);
    

    rc = sqlite_db->exec_sql(this, &ChargerRecordFile::charger_record_get_callback, &record, sql_string, &err_msg);
    
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "file=%s line =%d SQL rc=%d error: %s\n", __FILE__,__LINE__,rc,err_msg);
        sqlite3_free(err_msg);
    }
    
    closed();

    return 0;
}

bool ChargerRecordFile::add(ChargingRecord &record)
{
    if(open() != 0)
    {
        printf("充电记录数据库打开失败!\n");
        return -1;
    }
    
    char *err_msg = 0;
    int rc;
    char *sql;

    sql = (char *)"INSERT INTO CHANGER_RECORD (card_number,car_vin,start_charging_soc,end_charging_soc,start_time,end_time) "  \
             "VALUES (\"%s\",\"%s\",%d,%d,\"%s\",\"%s\");";
    

    sprintf(sql_string,sql,record.card_number,record.car_vin,record.start_charging_soc,record.end_charging_soc,record.start_time,record.end_time);

    char* data = (char*)"insert_db_callback  function called";

    rc = sqlite_db->exec_sql(this, &ChargerRecordFile::sqlite_void_callback_fun, data, sql_string, &err_msg);

    
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "file=%s line =%d SQL error: %s\n", __FILE__,__LINE__,err_msg);
        sqlite3_free(err_msg);
    }
    else 
    {
        if (debug)
        {
            fprintf(stdout, "数据插入成功\n");
        }
    }
    
    charger_record_num_limit(max_charger_num);
    
    closed();
    
    charger_num++;
    if (charger_num > max_charger_num)
    {
        charger_num = max_charger_num;
    }
    
    return 0;
}

int ChargerRecordFile::size()
{
    if(open() != 0)
    {
        printf("充电记录数据库打开失败!\n");
        return -1;
    }
    
    int rc;
    char *sql;
    char *err_msg = 0;
    int record_num = 0;

    sql = (char *)"select count(ID) from changer_record;";

    rc = sqlite_db->exec_sql(this, &ChargerRecordFile::charger_record_num_callback, 0, sql, &err_msg);

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    
    closed();
    
    record_num = atoi(sql_charger_size_buffer);
    
    charger_num = record_num;
    
    return record_num;

}

void ChargerRecordFile::charger_record_filed_fill(SdataInfo *sdata_info,char *string)
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

int ChargerRecordFile::charger_record_get_callback(void *not_used, int argc, char **data, char **az_col_name)
{
    ChargingRecord *charging_record = (ChargingRecord *)not_used;
    int index = 0;
    
    memset(&m_charge_record,0,sizeof(m_charge_record));
    while (index < argc)
    {
        for (ChargeRecordStructFieldMap::iterator i = charge_record_struct_field_map.begin(); i != charge_record_struct_field_map.end(); ++i)
        {
            SdataInfo mdata_info = (*i).second;
            
            const char *string = mdata_info.filed_name;

            if (0 ==strcmp(string,az_col_name[index]))
            {
                charger_record_filed_fill(&mdata_info,data[index]);
                
                break;
            }
        }
        
        index++;
    }
    
    memcpy(charging_record,&m_charge_record,sizeof(m_charge_record));

    return 0;
}

int ChargerRecordFile::charger_record_num_callback(void *not_used, int argc, char **data, char **az_col_name)
{
    not_used = not_used;
    az_col_name = az_col_name;
    
    if((data[0]) && (argc>=1) && (strlen(data[0])<sizeof(sql_charger_size_buffer)))
    {
        strcpy(sql_charger_size_buffer,data[0]);
    }
    else
    {
        strcpy(sql_charger_size_buffer,"0");
    }
    
    return 0;
}

int ChargerRecordFile::sqlite_void_callback_fun(void *not_used, int argc,char **data, char **az_col_name)
{
    not_used = not_used;
    argc = argc;
    data = data;
    az_col_name = az_col_name;
    
    char *string_data;
    string_data = (char *)not_used;
    
    if (debug)
    {
        if (string_data)
        {
           printf("line=%d string_data=%s argc=%d\n",__LINE__,string_data,argc);
        }
        
        for(int i=0; i<argc; i++)
        {
            printf("argv[%d]: %s = %s\n",i, az_col_name[i], data[i] ? data[i] : "NULL");
        }
    }
    
    return 0;
}

int ChargerRecordFile::charger_record_num_limit(int max_num)
{
    sprintf(sql_string,"DELETE FROM changer_record where ID<=(SELECT ID from changer_record ORDER BY ID DESC LIMIT 1 offset %d);",max_num);
    
    char *err_msg = 0;
    int rc;
    
    rc = sqlite_db->exec_sql(this, &ChargerRecordFile::sqlite_void_callback_fun, 0, sql_string, &err_msg);

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "记录限制失败  SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -2;
    }
    
    return 0;
}

bool ChargerRecordFile::create_charger_db_table()
{
    char *sql;
    char *err_msg = 0;
    int rc;

    //TODO 充电记录数据表没有完善，以下只是初步创建的表格，实际表格要重新根据需求梳理下，如增加分时段电量等等
    sql=(char *)"CREATE TABLE  IF NOT EXISTS CHANGER_RECORD("\
         "id INTEGER PRIMARY KEY   autoincrement," \
         "charging_pile_number        VARCHAR(32)," \
         "card_number        VARCHAR(32)," \
         "car_vin        BLOB(18)," \
         "plate_number        BLOB(18)," \
         "start_charging_soc    INTEGER ," \
         "end_charging_soc    INTEGER ," \
         "charging_capacity    INTEGER ," \
         "charging_energy    INTEGER ," \
         "start_meter_val    INTEGER ," \
         "end_meter_val    INTEGER ," \
         "charging_time    INTEGER ," \
         "charging_full_policy    VARCHAR(1)  ," \
         "normal_end    VARCHAR(1)  ," \
         "start_time        VARCHAR(32)," \
         "end_time        VARCHAR(32),"\
         "period_electric_quantity_data   VARCHAR(1000));";


    rc = sqlite_db->exec_sql(this,&ChargerRecordFile::sqlite_void_callback_fun,0,sql,&err_msg);
        
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
            fprintf(stdout, "创建 charger_record.sqlite_db 表成功\n");
        }
    }
    
    return true;
}

}

