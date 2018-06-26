/*<FH>*************************************************************************
* ��Ȩ����: Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
* �ļ�����: ChargingRecordFile.h
* �ļ���ʶ: 
* ����ժҪ: ��¼�ļ���
* ����˵��: 
* ��ǰ�汾: V1.0
* ��    ��: 
* �������: 2012-9-21 11:35:11
* �޸ļ�¼1: 
*     �޸�����: 
*     �� �� ��: 
*     �� �� ��: 
*     �޸�����:
**<FH>************************************************************************/

#ifndef __CHARGINGRECORDFILE_H__
#define __CHARGINGRECORDFILE_H__

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <deque>

namespace aiocharger
{
    typedef struct
    {
        int recordNumber;                               //��¼��� 4�ֽ�
        char chargingPileNumber[17];                    //���׮���16�ֽ�
        char cardNumber[17];                            //���� 16�ֽ�
        char carVIN[18];                                //����VIN 17�ֽ�
        char plateNumber[9];                            //���ƺ� 8�ֽ�
        unsigned char startChargingSOC;                 //��ʼ���SOC 1�ֽ�
        unsigned char endChargingSOC;                   //�������SOC 1�ֽ�
        unsigned short chargingCapacity;                //�����ۼƳ���� 2�ֽ�
        unsigned short chargingEnergy;                  //�����ۼƳ���� 2�ֽ�
        unsigned int startMeterVal;                     //��ʼ������4�ֽ�
        unsigned int endMeterVal;                       //����������4�ֽ�
        int chargingTime;                               //���ʱ�䳤�� 4�ֽ�
        unsigned char chargingFullPolicy;               //��������� 1�ֽ�
        unsigned char normalEnd;                        //�Ƿ��������� 1�ֽ�
        char startTime[32];                             //��ʼ���ʱ��
        char endTime[32];                               //�������ʱ��
        char tradeTime[32];                             //����ʱ��
        char bmsNumber[17];                             //���BMS���� 16�ֽ�
        unsigned short chargingFullPolicyParameter;     //��������Բ��� 2�ֽ�
        unsigned short endCause;                        //����ԭ�� 2�ֽ�
        int uploadFlag;                                 //�ϴ���־
        int amountMoney;                                //���ѽ��
        int recv[8];                                    //����8��
        int state;
    }ChargingRecord;
    
    typedef std::deque<ChargingRecord*> ChargingRecordList;
    
    class ChargingRecordFile
	{
	public:
	    ChargingRecordFile(const char* name, bool debug = false);
		virtual ~ChargingRecordFile();
		
		static const int MAX_RECORD_SIZE;
		static const int LINE_LENGTH;
        
        virtual bool read();
        virtual bool write();
        virtual int size();
        virtual bool get(int index, ChargingRecord* record);
        virtual bool set(ChargingRecord* record);
        int setUploadFlag(int index,int flagVal);
        int setTrade(int index,char *tradeTime);
    private:
    	ChargingRecordFile(const ChargingRecordFile& other);
        ChargingRecordFile& operator=(const ChargingRecordFile& other);
        
        bool create();
        bool readRecord(int start, int end, std::fstream& fs);
		bool modifyRecord();
		
		char name[128];
		
		bool debug;
		
		char nextLine[8];
		char recordSize[8];		
		ChargingRecord chargingRecord;
        ChargingRecordList recordList;
        pthread_mutex_t file_mutex;
	};
}

#endif  // CHARGINGRECORDFILE_H__

