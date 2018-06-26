/*<FH>*************************************************************************
* 版权所有: Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
* 文件名称: RecordFile.cpp
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

#include "RecordFile.h"
#include "ConfigFile.h"



namespace aiocharger
{ 
const int RecordFile::MAX_RECORD_SIZE = 2000;
const int RecordFile::LINE_LENGTH = 256;
const char* RecordFile::NULL_STRING = "                   ";
const char* RecordFile::BAR_STRING = "-------------------";
const char* RecordFile::TIME_FORMAT = "%Y-%m-%d %H:%M:%S";

RecordFile::RecordFile(const char* name, bool debug)
{
    memset(this->name, 0, sizeof(this->name));
    strcpy(this->name, name);
    
    this->debug = debug;
    
    memset(nextLine, 0, sizeof(nextLine));
    memset(recordSize, 0, sizeof(recordSize));
    pthread_mutex_init(&file_mutex,NULL);
}

RecordFile::~RecordFile()
{
    for (RecordList::iterator i = recordList.begin(); i != recordList.end(); ++i)
  {
    delete (*i);
  }
  recordList.clear();
  
  pthread_mutex_destroy(&file_mutex);
}

bool RecordFile::read()
{
    pthread_mutex_lock(&file_mutex);
    for (RecordList::iterator i = recordList.begin(); i != recordList.end(); ++i)
  {
    delete (*i);
  }
  recordList.clear();

  std::fstream fs(name, std::ios::in);
  if (!fs.is_open())
  {
      if (debug)
      {
          printf("RecordFile read open error\n");
      }
  
            pthread_mutex_unlock(&file_mutex);
          return false;
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
        printf("RecordFile open next=%d size=%d\n", next, size);
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
    if(!readRecord(next, size, fs))
    {
        fs.close();

            pthread_mutex_unlock(&file_mutex);
	    return false;
    }
    fs.clear();

    if(!readRecord(0, next, fs))
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

bool RecordFile::write()
{
    pthread_mutex_lock(&file_mutex);
    std::fstream fs(name, std::ios::in | std::ios::out);
    if (!fs.is_open())
    {
        if (debug)
      {
          printf("RecordFile write open error\n");
      }

        pthread_mutex_unlock(&file_mutex);
        return false;
    }
    
    char line[LINE_LENGTH];
    for (RecordList::iterator i = recordList.begin(); i != recordList.end(); ++i)
  {
    Record* record = (*i);

    if (record->state == 0)
    {
	    continue;
    }

    if (record->active)	//当前
    {
	    //更新下一行位置索引号
	    int next = atoi(nextLine);
	    record->index = next - 1;
	    sprintf(nextLine, "%04d", ((next + 1) > MAX_RECORD_SIZE) ? 1 : (next + 1));
	    fs.seekp(7, std::ios::beg);
	    fs << nextLine << std::endl;
	
	    //更新记录个数
	    int size = atoi(recordSize);
	    size++;
	    sprintf(recordSize, "%04d", (size >= MAX_RECORD_SIZE) ? MAX_RECORD_SIZE : size);
	    fs.seekp(21, std::ios::beg);
	    fs << recordSize << std::endl;
	
	    //索引号/项目1/项目2/项目3/开始时间/结束时间/当前标志
	    memset(line, 0, sizeof(line));
	    sprintf(line, "%04d/%010d/%010d/%010d/%s/%s/%01d", record->index, record->item1, record->item2, record->item3, record->startTime, record->endTime, record->active);
	    fs.seekp((36 + (next - 1) * 80), std::ios::beg);
	    fs << std::endl << line;
	    fs.clear();
    }
    else //历史
    {
        //索引号/项目1/项目2/项目3/开始时间/结束时间/当前标志
	    memset(line, 0, sizeof(line));
	    sprintf(line, "%04d/%010d/%010d/%010d/%s/%s/%01d", record->index, record->item1, record->item2, record->item3, record->startTime, record->endTime, record->active);
	    fs.seekp((36 + record->index * 80), std::ios::beg);
	    fs << std::endl << line;
	    fs.clear();
    }

    record->state = 0;		
  }

  fs.close();
    pthread_mutex_unlock(&file_mutex);
    return true;
}

int RecordFile::size()
{
    pthread_mutex_lock(&file_mutex);
    if (recordList.empty())
  {
        pthread_mutex_unlock(&file_mutex);
    return 0;
  }
  pthread_mutex_unlock(&file_mutex);
  
  int sizeLen = recordList.size();
  if(sizeLen == MAX_RECORD_SIZE)
  {
      return MAX_RECORD_SIZE-1;
  }
  
  return sizeLen;
}

bool RecordFile::get(int index, Record* record)
{
    pthread_mutex_lock(&file_mutex);
    
    if ((index >= (int)(recordList.size())) || (record == 0))
    {
        pthread_mutex_unlock(&file_mutex);
        return false;
    }
    
    Record* recordCurr = recordList[index];
    memset(record, 0, sizeof(Record));
    record->index = recordCurr->index;
    record->item1 = recordCurr->item1;
    record->item2 = recordCurr->item2;
    record->item3 = recordCurr->item3;
    strcpy(record->startTime, recordCurr->startTime);
    strcpy(record->endTime, recordCurr->endTime);
    record->active = recordCurr->active;
    record->state = 0;

    pthread_mutex_unlock(&file_mutex);
    return true;
}

bool RecordFile::set(const Record* record)
{
    
    if ((record == 0) || (strlen(record->startTime) != 19) || (strlen(record->endTime) != 19))
  {
    return false;
  }

  Record* recordCurr;
  if (record->active)	//当前
  {
    if ((int)(recordList.size()) >= MAX_RECORD_SIZE)
    {
	    recordCurr = recordList.front();
	    recordList.pop_front();
    }
    else
    {
	    recordCurr = new Record;
    }

    memset(recordCurr, 0, sizeof(Record));
    recordCurr->index = record->index;
    	recordCurr->item1 = record->item1;
    	recordCurr->item2 = record->item2;
    	recordCurr->item3 = record->item3;
    	strcpy(recordCurr->startTime, record->startTime);
    	strcpy(recordCurr->endTime, record->endTime);
    	recordCurr->active = record->active;
    	recordCurr->state = 1;
    recordList.push_back(recordCurr);
    
    if (debug)
        {
            printf("RecordFile set item1=%d item2=%d item3=%d startTime=%s endTime=%s active=%d\n", recordCurr->item1, recordCurr->item2, recordCurr->item3, recordCurr->startTime, recordCurr->endTime, recordCurr->active);
        }

    return true;
  }
  else		//历史
  {
    for (RecordList::reverse_iterator i = recordList.rbegin(); i != recordList.rend(); ++i)
    {
	    recordCurr = (*i);
	    if ((recordCurr->item1 == record->item1) && (recordCurr->item2 == record->item2) && (recordCurr->item3 == record->item3) && (strcmp(recordCurr->endTime, NULL_STRING) == 0))
	    {
		    strcpy(recordCurr->endTime, record->endTime);
		    recordCurr->active = record->active;
		    recordCurr->state = 1;
		    
		    if (debug)
                {
                    printf("RecordFile set item1=%d item2=%d item3=%d startTime=%s endTime=%s active=%d\n", recordCurr->item1, recordCurr->item2, recordCurr->item3, recordCurr->startTime, recordCurr->endTime, recordCurr->active);
                }
		
		    return true;
	    }
    }
  }

  return false;
}

bool RecordFile::readRecord(int start, int end, std::fstream& fs)
{
    if (debug)
    {
        printf("RecordFile readRecord start=%d end=%d\n", start, end);
    }

    fs.seekg((36 + 1 + start * 80), std::ios::beg);
    
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
    if(ConfigFile::split(line, (char*)("/"), buf, sizeof(buf)) < 7)
    {
        if (debug)
            {
                printf("RecordFile readRecord split error i=%d\n", i);
            }
    
	    return false;
    }

    if ((strlen(buf[4]) != 19) || (strlen(buf[5]) != 19))
    {
        if (debug)
            {
                printf("RecordFile readRecord strlen error i=%d\n", i);
            }
    
        return false;
    }

    //索引号/项目1/项目2/项目3/开始时间/结束时间/当前标志
    Record* record = new Record;
    memset(record, 0, sizeof(Record));
    record->index = atoi(buf[0]);
    record->item1 = atoi(buf[1]);
    record->item2 = atoi(buf[2]);
    record->item3 = atoi(buf[3]);
    strcpy(record->startTime, buf[4]);
    strcpy(record->endTime, buf[5]);
    record->active = (atoi(buf[6]) == 0) ? false : true;
    record->state = 0;
    recordList.push_back(record);
    
    if (debug)
        {
            printf("RecordFile readRecord i=%d index=%d item1=%d item2=%d item3=%d startTime=%s endTime=%s active=%d\n", i, record->index, record->item1, record->item2, record->item3, record->startTime, record->endTime, record->active);
        }
    }

  return true;
}
}


