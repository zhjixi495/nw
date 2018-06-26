/*<FH>*************************************************************************
* ��Ȩ����: Copyright (C) 2007-2020, Sinexcel Electric Co., Ltd.
* �ļ�����: record_file.h
* �ļ���ʶ: 
* ����ժҪ: ����¼�ļ���
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
    //����¼���������������SOC������ԭ�򡢿�ʼʱ�䡢����ʱ��...
    typedef struct
    {
        int id;                                         //��¼��� 4�ֽ�    ��1��ʼһֱ���������ֵ��Ȼ���ٴδ�1ѭ��
        char charging_pile_number[32];                    //���׮���16�ֽ�
        char charge_flow_number[64];                      //�����ˮ��
        char card_number[32];                            //���� 16�ֽ�
        char car_vin[18];                                //����VIN 17�ֽ�
        char plate_number[9];                            //���ƺ� 8�ֽ�
        unsigned char start_charging_soc;                 //��ʼ���SOC 1�ֽ�
        unsigned char end_charging_soc;                   //�������SOC 1�ֽ�
        unsigned short charging_energy;                  //�����ۼƳ���� 2�ֽ�
        unsigned int start_meter_val;                     //��ʼ������4�ֽ�
        unsigned int end_meter_val;                       //����������4�ֽ�
        unsigned int charging_time;                               //���ʱ�䳤�� 4�ֽ�
        unsigned char charging_full_policy;               //��������� 1�ֽ�
        unsigned char normal_end;                        //�Ƿ��������� 1�ֽ�
        char start_time[32];                             //��ʼ���ʱ��
        char end_time[32];                               //�������ʱ��
        unsigned int  period_electric_quantity_data[48];//��ʱ�ε���
        //û����ȫ����Ҫ�ٲ���,���ұ�������û�а��淶
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

