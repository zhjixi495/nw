/*<FH>*************************************************************************
* 版权所有: Copyright (C) 2007-2020, Sinexcel Electric Co., Ltd.
* 文件名称: record_file.h
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
**<FH>************************************************************************/

#ifndef __RECORD_FILE_H__
#define __RECORD_FILE_H__

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <deque>

namespace charger
{    
    //充电记录：充入电量、结束SOC、结束原因、开始时间、结束时间
    //充电机故障：模块编号、故障编号、开始时间、结束时间
    
    //索引号/项目1/项目2/项目3/开始时间/结束时间/当前标志
    typedef struct
    {
        int index;
        int item1;
        int item2;
        int item3;
        char startTime[32];
        char endTime[32];
        bool active;
        int state;
    } Record;
    
    typedef std::deque<Record*> RecordList;
        
    class RecordFile
    {
    public:
        RecordFile(const char* name, bool debug = false);
        virtual ~RecordFile();
        
        static const int MAX_RECORD_SIZE;
        static const int LINE_LENGTH;
        static const char* NULL_STRING;
        
        static const char* BAR_STRING;
        static const char* TIME_FORMAT;
        
        virtual bool read();
        virtual bool write();
        virtual int size();
        virtual bool get(int index, Record* record);
        virtual bool set(const Record* record);
        
    private:
        RecordFile(const RecordFile& other);
        RecordFile& operator=(const RecordFile& other);
        
        bool readRecord(int start, int end, std::fstream& fs);
        
        char name[128];
        
        bool debug;
        
        char nextLine[8];
        char recordSize[8];        
        RecordList recordList;
        pthread_mutex_t file_mutex;
    };
}

#endif  // RECORDFILE_H__

