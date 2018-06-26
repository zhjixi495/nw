/*<FH>*************************************************************************
* 版权所有: Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
* 文件名称: ChargingRecordFile.h
* 文件标识: 
* 内容摘要: 记录文件类
* 其它说明: 
* 当前版本: V1.0
* 作    者: 
* 完成日期: 2012-9-21 11:35:11
* 修改记录1: 
*     修改日期: 
*     版 本 号: 
*     修 改 人: 
*     修改内容:
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
        int recordNumber;                               //记录序号 4字节
        char chargingPileNumber[17];                    //充电桩编号16字节
        char cardNumber[17];                            //卡号 16字节
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
        char tradeTime[32];                             //交易时间
        char bmsNumber[17];                             //电池BMS编码 16字节
        unsigned short chargingFullPolicyParameter;     //充电满策略参数 2字节
        unsigned short endCause;                        //结束原因 2字节
        int uploadFlag;                                 //上传标志
        int amountMoney;                                //消费金额
        int recv[8];                                    //保留8个
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

