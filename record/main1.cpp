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
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <linux/unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <iostream> 
#include <functional>
#include "sqlite_db.h"

using namespace charger_monitor;
using namespace std;
using namespace std::placeholders;  

typedef struct
{
    int id;                                         //记录序号 4字节    从1开始一直到等于最大值，然后再次从1循环
    char chargingPileNumber[32];                    //充电桩编号16字节
    char cardNumber[32];                            //卡号 16字节
    char carVIN[18];                                //车辆VIN 17字节
    char plateNumber[9];                            //车牌号 8字节
    unsigned char startChargingSOC;                 //开始充电SOC 1字节
    unsigned char endChargingSOC;                   //结束充电SOC 1字节
    unsigned short chargingCapacity;                //本次累计充电量 2字节
    unsigned short chargingEnergy;                  //本次累计充电能 2字节
    unsigned int startMeterVal;                     //开始电表度数4字节
    unsigned int endMeterVal;                       //结束电表度数4字节
    int chargingTime;                               //充电时间长度 4字节
    unsigned char chargingFullPolicy;               //充电满策略 1字节
    unsigned char normalEnd;                        //是否正常结束 1字节
    char startTime[32];                             //开始充电时间
    char endTime[32];                               //结束充电时间
    unsigned int  periodElectricQuantityData[48];
} ChargingRecord;

ChargingRecord chargingRecord;

typedef struct 
{
    int argcNum;
    char argv[32][64];
    char az_col_name[32][64];
}ChargerBufferInfo;


typedef struct 
{
    char numBuffer[64];
}SqlChargerNumBuffer;

char *fillStrSpareTailSpace(char * pStr, int expectedMinLen, char fillChar)
{
    int strLen = strlen(pStr);
    int fillBytes = expectedMinLen - strLen;

    if(fillBytes > 0)
    {
        memset(pStr + strLen, fillChar, fillBytes);
    }

    pStr[expectedMinLen] = '\0';

    return pStr;
}

void msleep(int millis)
{
    if (millis <= 0)
    {
        return;
    }
    
    struct timeval tv;

    while (millis > 0)
    {    
        if (millis < 5000)
        {
            tv.tv_usec = millis % 1000 * 1000;
            tv.tv_sec  = millis / 1000;
            
            millis = 0;
        }
        else
        {
            tv.tv_usec = 0;
            tv.tv_sec  = 5000 / 1000;

            millis -= 5000;
        }
        
        select(0, NULL, NULL, NULL, &tv); 
    }
}

void getCurTime(char *curTime)
{
    struct tm when;
    time_t referenceTime;
    long referenceMillitm;
    timeval tv;
    char debugLine[128];
    
    ::time(&referenceTime);
    ::gettimeofday(&tv, NULL);
    referenceMillitm = tv.tv_usec / (long)1000;
    ::localtime_r(&referenceTime, &when);
       
    const char * const TIME_FORMAT_ = "%Y-%m-%d %H:%M:%S"; 
    ::sprintf(debugLine, ".%03ld",referenceMillitm);
        
    strftime(curTime,sizeof(chargingRecord.startTime),TIME_FORMAT_, &when);
    strcat(curTime, debugLine);
}


class ChargerRecordDb
{
public:
        ChargerRecordDb();
        virtual ~ChargerRecordDb();
        void open_record();
        int read_charger_record(int num);
        int add_charger_record();
        
        int insert_db_table(ChargingRecord *pchargingRecord);
        
        int read_all_record(void);
        int read_all_record_callback(void *not_used, int argc,char **data, char **az_col_name);
        
        int read_one_record(int index,ChargingRecord *pchargingRecord);
        int read_one_record_callback(void *not_used, int argc,char **data, char **az_col_name);
                
        int get_charger_record_num(void);
        int charger_record_num_callback(void *not_used, int argc, char **data, char **az_col_name);
        
        int sqlite_test_callback_fun(void *not_used, int argc,char **data, char **az_col_name);
private:
        ChargerRecordDb(const ChargerRecordDb& other);
        ChargerRecordDb& operator=(const ChargerRecordDb& other);
        
        char sqlstring[8*1024];
        ChargerBufferInfo charger_buffer_info;
        SqlChargerNumBuffer sql_charger_num_buffer;
        
        SqliteDb *db;
};

ChargerRecordDb::ChargerRecordDb()
{
    db = 0;
}

ChargerRecordDb::~ChargerRecordDb()
{
    if (db)
    {
        delete db;
        db = 0;
    }
}


void ChargerRecordDb::open_record()
{
    
    db = new SqliteDb((char *)"charger_record.db");
    char *sql;
    char *err_msg = 0;
    int rc;
    
    sql=(char *)"CREATE TABLE CHANGER_RECORD("\
         "ID INTEGER PRIMARY KEY   autoincrement," \
         "chargingPileNumber        VARCHAR(32)," \
         "cardNumber        VARCHAR(32)," \
         "carVIN        BLOB(18)," \
         "plateNumber        BLOB(18)," \
         "startChargingSOC    INTEGER ," \
         "endChargingSOC    INTEGER ," \
         "chargingCapacity    INTEGER ," \
         "chargingEnergy    INTEGER ," \
         "startMeterVal    INTEGER ," \
         "endMeterVal    INTEGER ," \
         "chargingTime    INTEGER ," \
         "chargingFullPolicy    VARCHAR(1)  ," \
         "normalEnd    VARCHAR(1)  ," \
         "startTime        VARCHAR(32)," \
         "endTime        VARCHAR(32),"
         "periodElectricQuantityData   VARCHAR(2000));";


    rc = db->exec_sql(this,&ChargerRecordDb::sqlite_test_callback_fun,0,sql,&err_msg);
    
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "file=%s line =%d rc=%d SQL error: %s\n", __FILE__,__LINE__,rc,err_msg);
        sqlite3_free(err_msg);
    }
    else
    {
        fprintf(stdout, "创建 charger_record.db 表成功\n");
    }

}


int ChargerRecordDb::insert_db_table(ChargingRecord *pchargingRecord)
{
    char *err_msg = 0;
    int rc;
    char *sql;
    
    if( !db )
    {
        return -1;
    }
    
    fillStrSpareTailSpace(pchargingRecord->cardNumber, sizeof(pchargingRecord->cardNumber) - 1, '-');
    fillStrSpareTailSpace(pchargingRecord->carVIN, sizeof(pchargingRecord->carVIN) - 1, '-');
    fillStrSpareTailSpace(pchargingRecord->startTime, sizeof(pchargingRecord->startTime) - 1, '-');
    fillStrSpareTailSpace(pchargingRecord->endTime, sizeof(pchargingRecord->endTime) - 1, '-');
    
    sql = (char *)"INSERT INTO CHANGER_RECORD (cardNumber,carVIN,startChargingSOC,endChargingSOC,startTime,endTime) "  \
             "VALUES (\"%s\",\"%s\",%d,%d,\"%s\",\"%s\");";
    
             
    sprintf(sqlstring,sql,pchargingRecord->cardNumber,pchargingRecord->carVIN,pchargingRecord->startChargingSOC,pchargingRecord->endChargingSOC,pchargingRecord->startTime,pchargingRecord->endTime);

    rc = db->exec_sql(this,&ChargerRecordDb::sqlite_test_callback_fun,0,sqlstring,&err_msg);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "file=%s line =%d 充电记录数据插入失败 SQL error: %s\n", __FILE__,__LINE__,err_msg);
        sqlite3_free(err_msg);
        return -2;
    }

    return 0;
}


int ChargerRecordDb::read_all_record(void)
{
    char *err_msg = 0;
    int rc;
    char *sql;
    
    if( !db )
    {
        return -1;
    }
    
    sql = (char *)"SELECT * from CHANGER_RECORD";

    rc = db->exec_sql(this, &ChargerRecordDb::read_all_record_callback, 0, sql, &err_msg);

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "file=%s line =%d 读取所有记录失败 SQL error: %s\n", __FILE__,__LINE__,err_msg);
        sqlite3_free(err_msg);
    }
    
    return 0;
}

//ok
int ChargerRecordDb::read_all_record_callback(void *not_used, int argc,char **data, char **az_col_name)
{
   char *string_data;
   int i;
   char **argv;
   argv = (char **)data;
   
   #if 1
   string_data = (char *)not_used;
   printf("file=%s line=%d fun=%s\n",__FILE__,__LINE__,__FUNCTION__);
   
   if (string_data)
   {
       printf("line=%d string_data=%s\n",__LINE__,string_data);
   }
   
   for(i=0; i<argc; i++)
   {
       printf("line=%d %s = %s\n",__LINE__, az_col_name[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n\n");
   
   printf("file=%s line=%d fun=%s\n",__FILE__,__LINE__,__FUNCTION__);
   #endif
   
   return 0;
}




int ChargerRecordDb::read_one_record(int index,ChargingRecord *pchargingRecord)
{
    char *err_msg = 0;
    int rc;
    char *sql;
    char* data = (char *)"read_one_record function called";
    
    int record_num = get_charger_record_num();
    
    if (index >= record_num)
    {
        return -1;
    }
    
    if( !db )
    {
        return -2;
    }
    
    sql = (char *)"select * from changer_record limit 1 offset %d";
    sprintf(sqlstring,sql,record_num-index-1);
    
    rc = db->exec_sql(this, &ChargerRecordDb::read_one_record_callback, data, (char *)sqlstring, &err_msg);
    
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -3;
    }

        
    printf("获取第%d条数据成功!!!:\n",index);
    for(int i=0;i<charger_buffer_info.argcNum;i++)
    {
    printf("%s = %s\n", charger_buffer_info.az_col_name[i], charger_buffer_info.argv[i][0]? charger_buffer_info.argv[i] : "NULL");
    }
    
    printf("read_one_record successfully\n");
    
    
    return 0;
}


int ChargerRecordDb::read_one_record_callback(void *not_used, int argc, char **argv, char **az_col_name)
{
   int i;
   char *string_data;
   
   string_data = (char *)not_used;
   charger_buffer_info.argcNum = argc;
   
   /*
   if (string_data)
   {
       printf("line=%d string_data=%s\n",__LINE__,string_data);
   }
   */
   
   for(i=0; i<argc; i++)
   {
      strcpy(charger_buffer_info.az_col_name[i],az_col_name[i]);
      if (argv[i])
      {
        strcpy(charger_buffer_info.argv[i],argv[i]);
      }
      else
      {
         charger_buffer_info.argv[i][0]=0;
      }
   }
   printf("\n");
   
   
   return 0;
}

int ChargerRecordDb::sqlite_test_callback_fun(void *not_used, int argc,char **data, char **az_col_name)
{
    return 0;
}

int ChargerRecordDb::get_charger_record_num(void)
{
    char *err_msg = 0;
    int rc;
    char *sql;
    int record_num = 0;
    
    if( !db )
    {
        return 0;
    }

    sql = (char *)"select count(ID) from changer_record;";

    rc = db->exec_sql(this, &ChargerRecordDb::charger_record_num_callback, 0, sql, &err_msg);

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }

    record_num = atoi(sql_charger_num_buffer.numBuffer);
    
    return record_num;
}


int ChargerRecordDb::charger_record_num_callback(void *not_used, int argc, char **data, char **az_col_name)
{
    int num = 0;
    int i;
    
    if((data[0])&&(argc>=1))
    {
       strcpy(sql_charger_num_buffer.numBuffer,data[0]);
    }
    else
    {
       strcpy(sql_charger_num_buffer.numBuffer,"0");
    }
    
    return num;
}



int main(void)
{
    ChargerRecordDb charger_record_db;
    srand((unsigned int)(time(NULL)));
    
    charger_record_db.open_record();
    
    for(int i=0;i<10;i++)
    {
        memset(&chargingRecord,0,sizeof(chargingRecord));
        
        sprintf(chargingRecord.cardNumber,"112233445566%04d",i);
        
        for(unsigned int j=0;j<sizeof(chargingRecord.carVIN)-1;j++)
        {
            chargingRecord.carVIN[j]=(unsigned char )(rand()%10+'0');
        }
        
        chargingRecord.startChargingSOC = (unsigned char )(rand()%30);
        
        chargingRecord.endChargingSOC = (unsigned char )(chargingRecord.startChargingSOC+50);
        
        getCurTime(chargingRecord.startTime);
        msleep(50);
        getCurTime(chargingRecord.endTime);
        charger_record_db.insert_db_table(&chargingRecord);
        
        msleep(300);
    }
    charger_record_db.get_charger_record_num();
    
    printf("当前数据库中的充电记录信息如下\n");
    charger_record_db.read_all_record();
    //charger_record_db.read_one_record(0,&chargingRecord);
    charger_record_db.read_one_record(5,&chargingRecord);

    return 0;
}
