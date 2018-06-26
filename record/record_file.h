/*<FH>*************************************************************************
* ��Ȩ����: Copyright (C) 2007-2020, Sinexcel Electric Co., Ltd.
* �ļ�����: record_file.h
* �ļ���ʶ: 
* ����ժҪ: ��¼�ļ���
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

#ifndef __RECORD_FILE_H__
#define __RECORD_FILE_H__

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <deque>

namespace charger
{    
    //����¼���������������SOC������ԭ�򡢿�ʼʱ�䡢����ʱ��
    //�������ϣ�ģ���š����ϱ�š���ʼʱ�䡢����ʱ��
    
    //������/��Ŀ1/��Ŀ2/��Ŀ3/��ʼʱ��/����ʱ��/��ǰ��־
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

