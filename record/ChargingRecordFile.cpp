/*<FH>*************************************************************************
* ��Ȩ����: Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
* �ļ�����: ChargingRecordFile.cpp
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
        
            //������һ��λ��������
            int next = atoi(nextLine);
            record->recordNumber = next - 1;
            sprintf(nextLine, "%05d", ((next + 1) > MAX_RECORD_SIZE) ? 1 : (next + 1));
            fs.seekp(7, std::ios::beg);
            fs << nextLine << std::endl;
        
            //���¼�¼����
            int size = atoi(recordSize);
            size++;
            sprintf(recordSize, "%05d", (size >= MAX_RECORD_SIZE) ? MAX_RECORD_SIZE : size);
            fs.seekp(21, std::ios::beg);
            fs << recordSize << std::endl;
        
            /*
            ��¼���/���׮���/����/����VIN/���ƺ�/
            ��ʼ���SOC/�������SOC/�����ۼƳ����/�����ۼƳ����/��ʼ����/��������/���ʱ�䳤��/
            ���������/�Ƿ���������/
            ��ʼ����ʱ�� ��/��ʼ����ʱ�� ��/��ʼ����ʱ�� ��/��ʼ����ʱ�� ʱ/��ʼ����ʱ�� ��/��ʼ����ʱ�� ��/��ʼ����ʱ�� /
            ��������ʱ�� ��/��������ʱ�� ��/��������ʱ�� ��/��������ʱ�� ʱ/��������ʱ�� ��/��������ʱ�� ��/��������ʱ�� /
            ��������ʱ�� ��/��������ʱ�� ��/��������ʱ�� ��/��������ʱ�� ʱ/��������ʱ�� ��/��������ʱ�� ��/��������ʱ�� /
            ���BMS����/��������Բ���/����ԭ��/�ϴ���־/����1/����2/����3/����4/����5/����6/����7/����8
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
            ��¼���/���׮���/����/����VIN/���ƺ�/
            ��ʼ���SOC/�������SOC/�����ۼƳ����/�����ۼƳ����/��ʼ����/��������/���ʱ�䳤��/
            ���������/�Ƿ���������/
            ��ʼ����ʱ�� ��/��ʼ����ʱ�� ��/��ʼ����ʱ�� ��/��ʼ����ʱ�� ʱ/��ʼ����ʱ�� ��/��ʼ����ʱ�� ��/��ʼ����ʱ�� /
            ��������ʱ�� ��/��������ʱ�� ��/��������ʱ�� ��/��������ʱ�� ʱ/��������ʱ�� ��/��������ʱ�� ��/��������ʱ�� /
            ��������ʱ�� ��/��������ʱ�� ��/��������ʱ�� ��/��������ʱ�� ʱ/��������ʱ�� ��/��������ʱ�� ��/��������ʱ�� /
            ���BMS����/��������Բ���/����ԭ��/�ϴ���־/����1/����2/����3/����4/����5/����6/����7/����8
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
        fs <<"��� /�������      /       ����     /     ����VIN     / ���ƺ� /SOC/SOC/  AH / KWH / ��ʼ���� / �������� /   TIME   /MOD/STA/     START TIME    /      END TIME     /    Trade TIME     /     BMS����    /���� /ԭ�� /�ϴ� / ��� /���� /���� /���� /���� /���� /���� /���� /���� "<<std::endl;   
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
            record->recordNumber = atoi(buf[0]);//��¼��� 4�ֽ�
            strcpy(record->chargingPileNumber, buf[1]);//���׮��� 16�ֽ�
            strcpy(record->cardNumber, buf[2]);//���� 16�ֽ�
            strcpy(record->carVIN, buf[3]);//����VIN 17�ֽ�
            strcpy(record->plateNumber, buf[4]);//���ƺ� 8�ֽ�
            record->startChargingSOC = (unsigned char)atoi(buf[5]);//��ʼ���SOC 1�ֽ�
            record->endChargingSOC = (unsigned char)atoi(buf[6]);//�������SOC 1�ֽ�
            record->chargingCapacity = (unsigned short)atoi(buf[7]);//�����ۼƳ���� 2�ֽ�
            record->chargingEnergy = (unsigned short)atoi(buf[8]);//�����ۼƳ���� 2�ֽ�
            
            record->startMeterVal = (int)atoi(buf[9]);//��ʼ������4�ֽ�
            record->endMeterVal = (int)atoi(buf[10]);//����������4�ֽ�
            
            record->chargingTime = (int)atoi(buf[11]);//���ʱ�䳤�� 4�ֽ�
            record->chargingFullPolicy = (unsigned char)atoi(buf[12]);//��������� 1�ֽ�
            record->normalEnd = (unsigned char)atoi(buf[13]);//�Ƿ��������� 1�ֽ�
            strcpy(record->startTime, buf[14]);//��ʼʱ��
            strcpy(record->endTime, buf[15]);//�������ʱ��
            strcpy(record->endTime, buf[16]);//����ʱ��
            strcpy(record->bmsNumber, buf[17]);//���BMS���� 16�ֽ�
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

