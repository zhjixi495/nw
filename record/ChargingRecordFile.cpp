/*<FH>*************************************************************************
* 版权所有: Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
* 文件名称: ChargingRecordFile.cpp
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

#include "ChargingRecordFile.h"
#include "ConfigFile.h"

using namespace esf;

namespace aiocharger
{
    const int ChargingRecordFile::MAX_RECORD_SIZE = 2500;
    const int ChargingRecordFile::LINE_LENGTH = 1024;

    ChargingRecordFile::ChargingRecordFile(const char* name, bool debug)
    {
        memset(this->name, 0, sizeof(this->name));
        strcpy(this->name, name);
        
        this->debug = debug;
        
        memset(nextLine, 0, sizeof(nextLine));
        memset(recordSize, 0, sizeof(recordSize));
        pthread_mutex_init(&file_mutex,NULL);
    }

    ChargingRecordFile::~ChargingRecordFile()
    {
        for (ChargingRecordList::iterator i = recordList.begin(); i != recordList.end(); ++i)
        {
            delete (*i);
        }
        recordList.clear();
        
        pthread_mutex_destroy(&file_mutex);
    }

    bool ChargingRecordFile::read()
    {
        pthread_mutex_lock(&file_mutex);
        for (ChargingRecordList::iterator i = recordList.begin(); i != recordList.end(); ++i)
        {
            delete (*i);
        }
        recordList.clear();
    
        std::fstream fs(name, std::ios::in);
        if (!fs.is_open())
        {
            fs.clear();
            create();
            fs.open(name, std::ios::in);
            if (!fs.is_open())
            {
                if (debug)
                {
                    printf("ChargingRecordFile read open error\n");
                }
            
                pthread_mutex_unlock(&file_mutex);
                return false;
            }
        }
    
        fs.seekg(7, std::ios::beg);
    
        memset(nextLine, 0, sizeof(nextLine));
        memset(recordSize, 0, sizeof(recordSize));
        fs >> nextLine;
        int next = atoi(nextLine);
        fs.seekg(21, std::ios::beg);
        fs >> recordSize;
        int size = atoi(recordSize);
        
        if (debug)
        {
            printf("ChargingRecordFile open next=%d size=%d\n", next, size);
        }
    
        if(size < MAX_RECORD_SIZE)
        {
            if(!readRecord(0, size, fs))
            {
                fs.close();

                pthread_mutex_unlock(&file_mutex);
                return false;
            }
            fs.clear();
        }
        else
        {
            if(!readRecord((next - 1), size, fs))
            {
                fs.close();

                pthread_mutex_unlock(&file_mutex);
                return false;
            }
            fs.clear();
        
            if(!readRecord(0, (next - 1), fs))
            {
                fs.close();

                pthread_mutex_unlock(&file_mutex);
                return false;
            }
            fs.clear();
        }

        fs.close();

        pthread_mutex_unlock(&file_mutex);
        return true;
    }

    bool ChargingRecordFile::write()
    {
        pthread_mutex_lock(&file_mutex);
        std::fstream fs(name, std::ios::in | std::ios::out);
        if (!fs.is_open())
        {
            if (debug)
            {
                printf("ChargingRecordFile write open error\n");
            }

            pthread_mutex_unlock(&file_mutex);
            return false;
        }
        
        char line[LINE_LENGTH];
        for (ChargingRecordList::iterator i = recordList.begin(); i != recordList.end(); ++i)
        {
            ChargingRecord* record = (*i);
        
            if (record->state == 0)
            {
                continue;
            }
        
            //更新下一行位置索引号
            int next = atoi(nextLine);
            record->recordNumber = next - 1;
            sprintf(nextLine, "%05d", ((next + 1) > MAX_RECORD_SIZE) ? 1 : (next + 1));
            fs.seekp(7, std::ios::beg);
            fs << nextLine << std::endl;
        
            //更新记录个数
            int size = atoi(recordSize);
            size++;
            sprintf(recordSize, "%05d", (size >= MAX_RECORD_SIZE) ? MAX_RECORD_SIZE : size);
            fs.seekp(21, std::ios::beg);
            fs << recordSize << std::endl;
        
            /*
            记录序号/充电桩编号/卡号/车辆VIN/车牌号/
            开始充电SOC/结束充电SOC/本次累计充电量/本次累计充电能/起始度数/结束度数/充电时间长度/
            充电满策略/是否正常结束/
            开始日期时间 年/开始日期时间 月/开始日期时间 日/开始日期时间 时/开始日期时间 分/开始日期时间 秒/开始日期时间 /
            结束日期时间 年/结束日期时间 月/结束日期时间 日/结束日期时间 时/结束日期时间 分/结束日期时间 秒/结束日期时间 /
            交易日期时间 年/交易日期时间 月/交易日期时间 日/交易日期时间 时/交易日期时间 分/交易日期时间 秒/交易日期时间 /
            电池BMS编码/充电满策略参数/结束原因/上传标志/保留1/保留2/保留3/保留4/保留5/保留6/保留7/保留8
            */
            memset(line, 0, sizeof(line));
            sprintf(line,"%05d/%s/%s/%s/%s/%03d/%03d/%05d/%05d/%010d/%010d/%010d/%03d/%03d/%s/%s/%s/%s/%05d/%05d/%05d/%06d/%05d/%05d/%05d/%05d/%05d/%05d/%05d/%05d", 
                    record->recordNumber, record->chargingPileNumber, record->cardNumber, record->carVIN, record->plateNumber, 
                    record->startChargingSOC, record->endChargingSOC, record->chargingCapacity, record->chargingEnergy, record->startMeterVal,record->endMeterVal,record->chargingTime,
                    record->chargingFullPolicy, record->normalEnd,
                    record->startTime, record->endTime, record->tradeTime, record->bmsNumber,record->chargingFullPolicyParameter, record->endCause,
                    record->uploadFlag,record->amountMoney,record->recv[0],record->recv[1],record->recv[2],record->recv[3],record->recv[4],record->recv[5],record->recv[6],record->recv[7]);
                    
            fs.seekp((36 + (next) * (278)), std::ios::beg);
            fs << std::endl << line;
            fs.clear();
        
            record->state = 0;
        }
    
        fs.close();

        pthread_mutex_unlock(&file_mutex);
        return true;
    }
    
    bool ChargingRecordFile::modifyRecord()
    {
        pthread_mutex_lock(&file_mutex);
        std::fstream fs(name, std::ios::in | std::ios::out);
        if (!fs.is_open())
        {
            if (debug)
            {
                printf("ChargingRecordFile write open error\n");
            }

            pthread_mutex_unlock(&file_mutex);
            return false;
        }
        
        char line[LINE_LENGTH];
        for (ChargingRecordList::iterator i = recordList.begin(); i != recordList.end(); ++i)
        {
            ChargingRecord* record = (*i);
        
            if (record->state == 0)
            {
                continue;
            }
            
            int index = record->recordNumber+1;
            if(index > MAX_RECORD_SIZE)
            {
                index = MAX_RECORD_SIZE;
            }
            
            /*
            记录序号/充电桩编号/卡号/车辆VIN/车牌号/
            开始充电SOC/结束充电SOC/本次累计充电量/本次累计充电能/起始度数/结束度数/充电时间长度/
            充电满策略/是否正常结束/
            开始日期时间 年/开始日期时间 月/开始日期时间 日/开始日期时间 时/开始日期时间 分/开始日期时间 秒/开始日期时间 /
            结束日期时间 年/结束日期时间 月/结束日期时间 日/结束日期时间 时/结束日期时间 分/结束日期时间 秒/结束日期时间 /
            交易日期时间 年/交易日期时间 月/交易日期时间 日/交易日期时间 时/交易日期时间 分/交易日期时间 秒/交易日期时间 /
            电池BMS编码/充电满策略参数/结束原因/上传标志/保留1/保留2/保留3/保留4/保留5/保留6/保留7/保留8
            */
            memset(line, 0, sizeof(line));
            sprintf(line,"%05d/%s/%s/%s/%s/%03d/%03d/%05d/%05d/%010d/%010d/%010d/%03d/%03d/%s/%s/%s/%s/%05d/%05d/%05d/%06d/%05d/%05d/%05d/%05d/%05d/%05d/%05d/%05d", 
                    record->recordNumber, record->chargingPileNumber, record->cardNumber, record->carVIN, record->plateNumber, 
                    record->startChargingSOC, record->endChargingSOC, record->chargingCapacity, record->chargingEnergy, record->startMeterVal,record->endMeterVal,record->chargingTime,
                    record->chargingFullPolicy, record->normalEnd,
                    record->startTime, record->endTime, record->tradeTime,record->bmsNumber,record->chargingFullPolicyParameter, record->endCause,
                    record->uploadFlag,record->amountMoney,record->recv[0],record->recv[1],record->recv[2],record->recv[3],record->recv[4],record->recv[5],record->recv[6],record->recv[7]);
                    
            fs.seekp((36 + (index) * (278)), std::ios::beg);
            fs << std::endl << line;
            fs.clear();
        
            record->state = 0;
        }
    
        fs.close();

        pthread_mutex_unlock(&file_mutex);
        return true;
    }
    
    int ChargingRecordFile::setUploadFlag(int index,int flagVal)
    {
        if ((index >= (int)(recordList.size())))
        {
            return false;
        }
        
        pthread_mutex_lock(&file_mutex);
        
        printf("setUploadFlag index=%d\n",index);
        
        ChargingRecord* recordCurr = recordList[index];
        flagVal = flagVal&0x01;
        recordCurr->uploadFlag = (recordCurr->uploadFlag|flagVal);
        recordCurr->state = 1;

        pthread_mutex_unlock(&file_mutex);
        
        return modifyRecord();
    }
    
    int ChargingRecordFile::setTrade(int index,char *tradeTime)
    {
        if ((index >= (int)(recordList.size())))
        {
            return false;
        }
        
        if (strlen(tradeTime) != 19)
        {
            return false;
        }
        
        pthread_mutex_lock(&file_mutex);

        ChargingRecord* recordCurr = recordList[index];
        
        strcpy(recordCurr->tradeTime,tradeTime);
        printf("recordCurr->tradeTime=%s\n",recordCurr->tradeTime);
        recordCurr->uploadFlag = (recordCurr->uploadFlag|0x02);
        recordCurr->state = 1;

        pthread_mutex_unlock(&file_mutex);
        
        return modifyRecord();
    }
    
    int ChargingRecordFile::size()
    {
        pthread_mutex_lock(&file_mutex);
        if (recordList.empty())
        {
            pthread_mutex_unlock(&file_mutex);
            return 0;
        }
        
        int tmp = recordList.size();
    
        pthread_mutex_unlock(&file_mutex);
        
        if(MAX_RECORD_SIZE == tmp)
        {
            return MAX_RECORD_SIZE-1;
        }
        
        return tmp;
    }

    bool ChargingRecordFile::get(int index, ChargingRecord* record)
    {
        if(index < 0)
        {
            return false;
        }
        
        pthread_mutex_lock(&file_mutex);
        if ((index >= (int)(recordList.size())) || (record == 0))
        {
            pthread_mutex_unlock(&file_mutex);
            return false;
        }
        
        ChargingRecord* recordCurr = recordList[index];
        memset(record, 0, sizeof(ChargingRecord));
        *record = *recordCurr;
        record->state = 0;

        pthread_mutex_unlock(&file_mutex);
        return true;
    }

    bool ChargingRecordFile::set(ChargingRecord* record)
    {
        pthread_mutex_lock(&file_mutex);
        if ((record == 0) || (strlen(record->chargingPileNumber) != 16) || (strlen(record->cardNumber) != 16) 
            || (strlen(record->carVIN) != 17) || (strlen(record->plateNumber) != 8) 
            || (strlen(record->startTime) != 19)|| (strlen(record->endTime) != 19)|| (strlen(record->tradeTime) != 19)||(strlen(record->bmsNumber) != 16))
        {
            pthread_mutex_unlock(&file_mutex);
            
            if(strlen(record->chargingPileNumber) != 16)
            {
                printf("strlen(record->chargingPileNumber) = %d\n",strlen(record->chargingPileNumber));
                memset(record->chargingPileNumber,0,sizeof(record->chargingPileNumber));
                memset(record->chargingPileNumber,'-',16);
            }    
            
            for(int i = 0;i<(int)(sizeof(record->chargingPileNumber)-1);i++)
            {
                if ((record->chargingPileNumber[i] == 0x0a)||(record->chargingPileNumber[i] =='/'))
                {
                    record->chargingPileNumber[i] = '*';
                }
            }
            
            if(strlen(record->cardNumber) != 16)
            {
                printf("strlen(record->cardNumber) != 16 %s\n",record->cardNumber);
                memset(record->cardNumber,0,sizeof(record->cardNumber));
                memset(record->cardNumber,'-',16);
            }
            
            for(int i = 0;i<(int)(sizeof(record->cardNumber)-1);i++)
            {
                if ((record->cardNumber[i] == 0x0a)||(record->cardNumber[i] =='/'))
                {
                    record->cardNumber[i] = '*';
                }
            }
            
            if(strlen(record->carVIN) != 17)
            {
                printf("strlen(record->carVIN) != 17 %s\n",record->carVIN);
                memset(record->carVIN,0,sizeof(record->carVIN));
                memset(record->carVIN,'-',17);
            }
            
            for(int i = 0;i<(int)(sizeof(record->carVIN)-1);i++)
            {
                if ((record->carVIN[i] == 0x0a)||(record->carVIN[i] =='/'))
                {
                    record->carVIN[i] = '*';
                }
            }
            
            
            if(strlen(record->plateNumber) != 8)
            {
                printf("strlen(record->plateNumber) != 8 %s\n",record->plateNumber);
                memset(record->plateNumber,0,sizeof(record->plateNumber));
                memset(record->plateNumber,'-',8);
            }
            
            for(int i = 0;i<(int)(sizeof(record->plateNumber)-1);i++)
            {
                if ((record->plateNumber[i] == 0x0a)||(record->plateNumber[i] =='/'))
                {
                    record->plateNumber[i] = '*';
                }
            }
            
            if(strlen(record->startTime) != 19)
            {
                printf("strlen(record->startTime) != 19 %s\n",record->startTime);
                memset(record->startTime,0,sizeof(record->cardNumber));
                memset(record->startTime,'-',19);
            }
            
            if(strlen(record->endTime) != 19)
            {
                printf("strlen(record->endTime) != 19 %s \n",record->endTime);
                memset(record->endTime,0,sizeof(record->endTime));
                memset(record->endTime,'-',19);
            }
            
            if(strlen(record->tradeTime) != 19)
            {
                printf("strlen(record->tradeTime) != 19 %s \n",record->tradeTime);
                memset(record->tradeTime,0,sizeof(record->tradeTime));
                memset(record->tradeTime,'-',19);
            }
            
            if(strlen(record->bmsNumber) != 16)
            {
                printf("strlen(record->bmsNumber) != 16 %s\n",record->bmsNumber);
                memset(record->bmsNumber,0,sizeof(record->bmsNumber));
                memset(record->bmsNumber,'-',16);
            }
            //return false;
        }
    
        ChargingRecord* recordCurr;
        if ((int)(recordList.size()) >= MAX_RECORD_SIZE)
        {
            recordCurr = recordList.front();
            recordList.pop_front();
        }
        else
        {
            recordCurr = new ChargingRecord;
        }
    
        memset(recordCurr, 0, sizeof(ChargingRecord));
        *recordCurr = *record;
        recordCurr->state = 1;
        recordList.push_back(recordCurr);
        
        if (debug)
        {
            printf("ChargingRecordFile set\n");
        }
    
        pthread_mutex_unlock(&file_mutex);
        return true;
    }
    
    bool ChargingRecordFile::create()
    {   
        char line[LINE_LENGTH];
        memset(line, 0, sizeof(line));
        sprintf(line, "rm -f %s;touch %s", name, name);
        
        system(line);
        
        std::fstream fs(name, std::ios::in | std::ios::out);
        if (!fs.is_open())
        {
            if (debug)
            {
                printf("ChargingRecordFile create error\n");
            }
            
            return false;
        }       
                
        fs.seekp(0, std::ios::beg);
        fs << "[NEXT]" << std::endl;
        fs << "00001" << std::endl << std::endl;
        fs << "[SIZE]" << std::endl;
        fs << "00000" << std::endl << std::endl;
        fs << "[RECORD]" << std::endl;
        fs <<"序号 /充电机编号      /       卡号     /     车辆VIN     / 车牌号 /SOC/SOC/  AH / KWH / 起始度数 / 结束度数 /   TIME   /MOD/STA/     START TIME    /      END TIME     /    Trade TIME     /     BMS编码    /参数 /原因 /上传 / 金额 /保留 /保留 /保留 /保留 /保留 /保留 /保留 /保留 "<<std::endl;   
        fs.close();
       
        return true;
    }

    bool ChargingRecordFile::readRecord(int start, int end, std::fstream& fs)
    {
        if (debug)
        {
            printf("ChargingRecordFile readRecord start=%d end=%d\n", start, end);
        }
    
        fs.seekg((36 + 1 + (start+1) * (278)), std::ios::beg);
        
        char line[LINE_LENGTH];
        for (int i = start; i < end; i++)
        {
            memset(line, 0, sizeof(line));
            fs.getline(line, sizeof(line), '\n');
            if(strlen(line) == 0)
            {
                continue;
            }
                    
            char* buf[32];
            if(ConfigFile::split(line, (char*)("/"), buf, 32) < 17)
            {
                if (debug)
                {
                    printf("ChargingRecordFile readRecord split error i=%d\n", i);
                }
                
                continue;
                //return false;
            }
            
            
            if ((strlen(buf[1]) != 16) || (strlen(buf[2]) != 16) || (strlen(buf[3]) != 17) || (strlen(buf[4]) != 8) 
                || (strlen(buf[14]) != 19) || (strlen(buf[15]) != 19) 
                || (strlen(buf[17]) != 16))
            {
                if (debug)
                {
                    printf("ChargingRecordFile readRecord strlen error i=%d\n", i);
                    printf("%d %d %d %d %d %d %d\n",strlen(buf[1]),strlen(buf[2]),strlen(buf[3]),strlen(buf[4]),strlen(buf[14]),strlen(buf[15]),strlen(buf[17]));
                }
                
                //return false;
                continue;
            }
            
            
            ChargingRecord* record = new ChargingRecord;
            memset(record, 0, sizeof(ChargingRecord));
            record->recordNumber = atoi(buf[0]);//记录序号 4字节
            strcpy(record->chargingPileNumber, buf[1]);//充电桩编号 16字节
            strcpy(record->cardNumber, buf[2]);//卡号 16字节
            strcpy(record->carVIN, buf[3]);//车辆VIN 17字节
            strcpy(record->plateNumber, buf[4]);//车牌号 8字节
            record->startChargingSOC = (unsigned char)atoi(buf[5]);//开始充电SOC 1字节
            record->endChargingSOC = (unsigned char)atoi(buf[6]);//结束充电SOC 1字节
            record->chargingCapacity = (unsigned short)atoi(buf[7]);//本次累计充电量 2字节
            record->chargingEnergy = (unsigned short)atoi(buf[8]);//本次累计充电能 2字节
            
            record->startMeterVal = (int)atoi(buf[9]);//开始电表度数4字节
            record->endMeterVal = (int)atoi(buf[10]);//结束电表度数4字节
            
            record->chargingTime = (int)atoi(buf[11]);//充电时间长度 4字节
            record->chargingFullPolicy = (unsigned char)atoi(buf[12]);//充电满策略 1字节
            record->normalEnd = (unsigned char)atoi(buf[13]);//是否正常结束 1字节
            strcpy(record->startTime, buf[14]);//起始时间
            strcpy(record->endTime, buf[15]);//结束充电时间
            strcpy(record->endTime, buf[16]);//交易时间
            strcpy(record->bmsNumber, buf[17]);//电池BMS编码 16字节
            record->chargingFullPolicyParameter = (unsigned short)atoi(buf[18]);
            record->endCause = (unsigned short)atoi(buf[19]);
            record->uploadFlag = (unsigned short)atoi(buf[20]);
            record->amountMoney = (unsigned short)atoi(buf[21]);
            record->state = 0;
            recordList.push_back(record);
            
            if (debug)
            {
                printf("ChargingRecordFile readRecord i=%d recordNumber=%d\n", i, record->recordNumber);
            }
        }
    
        return true;
    }
}

