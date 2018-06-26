/*<FH>*************************************************************************
* 版权所有: Copyright (C) 2007-2020, Sinexcel Electric Co., Ltd.
* 文件名称: record_file.h
* 文件标识: 
* 内容摘要: 充电记录文件类
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

#ifndef __CHARGER_RECORD_H__
#define __CHARGER_RECORD_H__

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
        char charging_pile_number[32];                    //充电桩编号16字节
        char charge_flow_number[64];                      //充电流水号
        char card_number[32];                            //卡号 16字节
        char car_vin[18];                                //车辆VIN 17字节
        char plate_number[9];                            //车牌号 8字节
        unsigned char start_charging_soc;                 //开始充电SOC 1字节
        unsigned char end_charging_soc;                   //结束充电SOC 1字节
        unsigned short charging_energy;                  //本次累计充电能 2字节
        unsigned int start_meter_val;                     //开始电表度数4字节
        unsigned int end_meter_val;                       //结束电表度数4字节
        unsigned int charging_time;                               //充电时间长度 4字节
        unsigned char charging_full_policy;               //充电满策略 1字节
        unsigned char normal_end;                        //是否正常结束 1字节
        char start_time[32];                             //开始充电时间
        char end_time[32];                               //结束充电时间
        unsigned int  period_electric_quantity_data[48];//分时段电量
        //没有列全，需要再补充,而且变量命名没有按规范
    } ChargingRecord;
    
    
    typedef std::deque<ChargingRecord*> ChargingRecordList;
        
//    typedef std::map<int, SdataInfo> ChargeRecordStructFieldMap;

    
    class ChargerRecordFile
    {
    public:
        ChargerRecordFile(bool debug = false);
        virtual ~ChargerRecordFile();
        
        void closed();
        int open();
        
        int size();
        bool get(unsigned int index, ChargingRecord &record);
        bool add(ChargingRecord &record);
        
    private:
        ChargerRecordFile(const ChargerRecordFile& other);
        ChargerRecordFile& operator=(const ChargerRecordFile& other);
        
        int sqlite_void_callback_fun(void *not_used, int argc,char **data, char **az_col_name);
        int charger_record_num_callback(void *not_used, int argc, char **data, char **az_col_name);
        int charger_record_get_callback(void *not_used, int argc, char **data, char **az_col_name);
        
        void charger_record_filed_fill(SdataInfo *sdata_info,char *string);
            
        bool create_charger_db_table();
        int charger_record_num_limit(int num);
        
        SqliteDb *sqlite_db;
        char sql_charger_size_buffer[128];
        char sql_string[4096];
        unsigned int  charger_num;
        
        bool debug;
        bool charger_table_create_ok_flag;
        
        ChargingRecord m_charge_record;
        ChargeRecordStructFieldMap charge_record_struct_field_map;
        
        static const unsigned int max_charger_num;
    };
}

#endif  // __CHARGER_RECORD_H__

