/*<FH>*************************************************************************
* 版权所有: Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
* 文件名称: 
* 文件标识: 
* 内容摘要: 
* 其它说明: 
* 当前版本: 
* 作    者: 
* 完成日期: 
* 修改记录1: 
*     修改日期: 
*     版 本 号: 
*     修 改 人: 
*     修改内容:
**<FH>************************************************************************/

#include "modbus_frame.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>

#include "modbus_constants.h"



namespace charger 
{

const int ModbusFrame::DATA_BASE_INDEX    = 2000;    //数据区的起始位置

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/
ModbusFrame::ModbusFrame(bool request)
{
    memset(frameBuffer, 0, sizeof(frameBuffer));
    frameLength = 0;
    dataIndex = DATA_BASE_INDEX;
    
    this->request = request;
    
    crc = 0;
    
    node = 0;
    fun = 0;
    id = 0;
    nr = 0;
    bytes = 0;
    value = 0;
    error = 0;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/
ModbusFrame::~ModbusFrame()
{
  
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
void ModbusFrame::setCrc(int crc)
{
    this->crc = crc;
}


/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
void ModbusFrame::setNode(int node)
{
    this->node = node;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2012-9-4
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
int ModbusFrame::getNode()
{
    return this->node;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
void ModbusFrame::setFun(int fun, bool request)
{
    this->fun = fun;
    this->request = request;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
int ModbusFrame::getFun()
{
    return fun;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
void ModbusFrame::setId(int id)
{
    this->id = id;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
int ModbusFrame::getId()
{
    return id;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
void ModbusFrame::setNr(int nr)
{
    this->nr = nr;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
int ModbusFrame::getNr()
{
    return nr;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/  
void ModbusFrame::setError(int error)
{
    this->error = error;
  
    fun = fun | 0x80;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2007-11-29 13:35
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/
int ModbusFrame::getError()
{
    return error;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/
void ModbusFrame::setBytes(int bytes)
{
    this->bytes = bytes;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/  
int ModbusFrame::getBytes()
{
    return bytes;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/  
void ModbusFrame::setValue(int value)
{
    this->value = value;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/  
int ModbusFrame::getValue()
{
    return value;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
void ModbusFrame::setData(int data)
{
    this->nr = data;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
void ModbusFrame::clearBuffer()
{
    memset(frameBuffer, 0, sizeof(frameBuffer));
    frameLength = 0;
    dataIndex = DATA_BASE_INDEX;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
const unsigned char* ModbusFrame::getBuffer()
{
    //帧打包过程
    frameLength = 0;
    
    //组装帧头
    sprintf((char *)frameBuffer, "%c%c", (unsigned char)node, (unsigned char)fun);
    
    
    if ((dataIndex - DATA_BASE_INDEX) > 0xFF)
    {
        return 0;
    }
    int lengthId = dataIndex - DATA_BASE_INDEX;
    
    if (request) //作为客户端，发送请求帧
    {
        if ((fun == ModbusConstants::FUNCTION_CODE_03) || (fun == ModbusConstants::FUNCTION_CODE_04))
        {
            int idIndex = ModbusConstants::FRAME_HEADER_SIZE;
            sprintf((char *)(frameBuffer + idIndex), "%c%c", (unsigned char)(id >> 8), (unsigned char)(id & 0xFF));
            idIndex += 2;
            sprintf((char *)(frameBuffer + idIndex), "%c%c", (unsigned char)(nr >> 8), (unsigned char)(nr & 0xFF));
            idIndex += 2;
                  
            lengthId = idIndex - ModbusConstants::FRAME_HEADER_SIZE;
        }
        if ((fun == ModbusConstants::FUNCTION_CODE_01) || (fun == ModbusConstants::FUNCTION_CODE_02))
        {
            int idIndex = ModbusConstants::FRAME_HEADER_SIZE;
            sprintf((char *)(frameBuffer + idIndex), "%c%c", (unsigned char)(id >> 8), (unsigned char)(id & 0xFF));
            idIndex += 2;
            sprintf((char *)(frameBuffer + idIndex), "%c%c", (unsigned char)(nr >> 8), (unsigned char)(nr & 0xFF));
            idIndex += 2;
                  
            lengthId = idIndex - ModbusConstants::FRAME_HEADER_SIZE;  
        }
        else if ((fun == ModbusConstants::FUNCTION_CODE_05) || (fun == ModbusConstants::FUNCTION_CODE_06))
        {
            int idIndex = ModbusConstants::FRAME_HEADER_SIZE;
            sprintf((char *)(frameBuffer + idIndex), "%c%c", (unsigned char)(id >> 8), (unsigned char)(id & 0xFF));
            idIndex += 2;
            sprintf((char *)(frameBuffer + idIndex), "%c%c", (unsigned char)(value >> 8), (unsigned char)(value & 0xFF));
            idIndex += 2;
            
            lengthId = idIndex - ModbusConstants::FRAME_HEADER_SIZE;  
        }
        else if ((fun == ModbusConstants::FUNCTION_CODE_16))
        {
            int idIndex = ModbusConstants::FRAME_HEADER_SIZE;
            sprintf((char *)(frameBuffer + idIndex), "%c%c", (unsigned char)(id >> 8), (unsigned char)(id & 0xFF));
            idIndex += 2;
            sprintf((char *)(frameBuffer + idIndex), "%c%c", (unsigned char)(nr >> 8), (unsigned char)(nr & 0xFF));
            idIndex += 2;
            sprintf((char *)(frameBuffer + idIndex), "%c", (unsigned char)(bytes & 0x000000FF));
            idIndex += 1;
            memmove((void *)(frameBuffer + idIndex), (void *)(frameBuffer + DATA_BASE_INDEX), lengthId);
            idIndex += lengthId;
            
            lengthId = idIndex - ModbusConstants::FRAME_HEADER_SIZE;
        }
    }
    else //作为服务端，发送应答帧
    {
        if ((fun & 0x80) == 0x80) //异常帧
        {
            int idIndex = ModbusConstants::FRAME_HEADER_SIZE;
            sprintf((char *)(frameBuffer + idIndex), "%c", (unsigned char)error);
            idIndex += 1;
            lengthId = idIndex - ModbusConstants::FRAME_HEADER_SIZE;
        }
        else if ((fun == ModbusConstants::FUNCTION_CODE_03) || (fun == ModbusConstants::FUNCTION_CODE_04))
        {
            int idIndex = ModbusConstants::FRAME_HEADER_SIZE;
            sprintf((char *)(frameBuffer + idIndex), "%c", (unsigned char)lengthId);
            idIndex += 1;
            memmove((void *)(frameBuffer + idIndex), (void *)(frameBuffer + DATA_BASE_INDEX), lengthId);
            idIndex += lengthId;
            
            lengthId = idIndex - ModbusConstants::FRAME_HEADER_SIZE;
        }
        else if ((fun == ModbusConstants::FUNCTION_CODE_05) || (fun == ModbusConstants::FUNCTION_CODE_06))
        {
            int idIndex = ModbusConstants::FRAME_HEADER_SIZE;
            sprintf((char *)(frameBuffer + idIndex), "%c%c", (unsigned char)(id >> 8), (unsigned char)(id & 0xFF));
            idIndex += 2;
            sprintf((char *)(frameBuffer + idIndex), "%c%c", (unsigned char)(value >> 8), (unsigned char)(value & 0xFF));
            idIndex += 2;
            lengthId = idIndex - ModbusConstants::FRAME_HEADER_SIZE;
        }
    }
    
    //设置校验和
    int checkSum = ModbusConstants::get_check_sum(frameBuffer, ModbusConstants::FRAME_HEADER_SIZE + lengthId, crc);
    
    //组装帧尾
    sprintf((char *)(frameBuffer + ModbusConstants::FRAME_HEADER_SIZE + lengthId), "%c%c", (unsigned char)(checkSum >> 8), (unsigned char)(checkSum & 0xFF));
      
    frameLength = ModbusConstants::FRAME_HEADER_SIZE + lengthId + ModbusConstants::FRAME_TAIL_SIZE;
    
    return frameBuffer;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
int ModbusFrame::getLength()
{
    return frameLength;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
int ModbusFrame::parse(const unsigned char* buffer, int length)
{
    if ((buffer == 0) || (length < (ModbusConstants::FRAME_HEADER_SIZE + ModbusConstants::FRAME_TAIL_SIZE)) || (length > ModbusConstants::FRAME_BUFFER_SIZE))
    {
    return -1;
    }
    
      
    //帧解包过程
    clearBuffer();
    
    //寻找起始标志位
    int startIndex = 0;
    
    //寻找结束码
    int endIndex = length - 1;
    
    //检查帧长度
    if ((endIndex - startIndex + 1) < (ModbusConstants::FRAME_HEADER_SIZE + ModbusConstants::FRAME_TAIL_SIZE))
    {
        return -2;
    }
    
    error = 0;
    
    if (request) //作为客户端，解析应答帧
    {
        int state = 0;
        bool okFlag = false;
        
        while(1)
        {
            if(okFlag)
            {
            break;
            }
        
            if(startIndex >= length)
            {
                break;
            }
        
            unsigned char rc = buffer[startIndex];
            startIndex++;
            switch(state)
            {
                case 0:
                {
                if(rc == (unsigned char)node)
                {
                    frameBuffer[0] = rc;
                    state = 1;
                }
                else
                {
                    state = 0;
                }
                }
                break;
        
                case 1:
                {
                if((rc == ModbusConstants::FUNCTION_CODE_01) || (rc == ModbusConstants::FUNCTION_CODE_02)
                    || (rc == ModbusConstants::FUNCTION_CODE_03) || (rc == ModbusConstants::FUNCTION_CODE_04) 
                    || (rc == ModbusConstants::FUNCTION_CODE_05) || (rc == ModbusConstants::FUNCTION_CODE_06))
                {
                    frameBuffer[1] = rc;
                    fun = rc;
                    state = rc | 0x80;
                }
                else
                {
                    state = 0;      
                    startIndex--;
                }
                }
            break;
        
            case 0x81:
            case 0x82://add by zhengjixiang 2012-9-3
            {
                if(rc != nr/8)
                {
                    state = 0;
                    startIndex--;
                    //printf("rc=%d nr*2=%d \n",rc,nr*2);  
                }
                else
                {
                    if(length - startIndex < (rc + ModbusConstants::FRAME_TAIL_SIZE))
                    {
                        state = 0;
                        startIndex--;
                    }
                    else
                    {
                        frameBuffer[2] = rc;
                        memcpy((void*)(frameBuffer+3), (const void*)(buffer + startIndex), rc + ModbusConstants::FRAME_TAIL_SIZE);
                        frameLength = rc + ModbusConstants::FRAME_HEADER_SIZE + ModbusConstants::FRAME_TAIL_SIZE +1;
                  
                        int checkSum = ((*(frameBuffer +  frameLength - 2)) << 8) | (unsigned char)(*(frameBuffer + frameLength - 1));
                        if (checkSum != ModbusConstants::get_check_sum(frameBuffer, frameLength-2, crc))
                        {
                            state = 0;
                            startIndex--;
                        }
                        else
                        {
                            nr = rc;
                            okFlag = true;
                        }
                    }
                  }
            }
            break;
        
            case 0x83:
            case 0x84:
            {
                if(rc != nr*2)
                {
                    state = 0;
                    startIndex--;
                }
                else
                {
                    if(length - startIndex < (rc + ModbusConstants::FRAME_TAIL_SIZE))
                    {
                      state = 0;
                      startIndex--;
                    }
                    else
                    {
                        frameBuffer[2] = rc;
                        memcpy((void*)(frameBuffer+3), (const void*)(buffer + startIndex), rc + ModbusConstants::FRAME_TAIL_SIZE);
                        frameLength = rc + ModbusConstants::FRAME_HEADER_SIZE + ModbusConstants::FRAME_TAIL_SIZE +1;
                  
                        int checkSum = ((*(frameBuffer +  frameLength - 2)) << 8) | (unsigned char)(*(frameBuffer + frameLength - 1));
                        if (checkSum != ModbusConstants::get_check_sum(frameBuffer, frameLength-2, crc))
                        {
                            state = 0;
                            startIndex--;
                        }
                        else
                        {
                            nr = rc;
                            okFlag = true;
                        }  
                    }
                }
            }
            break;
            
            case 0x85:
            case 0x86:
            {
              if(length - startIndex < 5)
                  {
                    state = 0;
                    startIndex--;
                  }
                  else
                  {
                    frameBuffer[2] = rc;
                memcpy((void*)(frameBuffer+3), (const void*)(buffer + startIndex), 5);
                int checkSum = ((*(frameBuffer + 6)) << 8) | (unsigned char)(*(frameBuffer + 7));
                frameLength = 8;
                
                        if (checkSum != ModbusConstants::get_check_sum(frameBuffer, 6, crc))
                        {
                          state = 0;
                      startIndex--;
                        }
                    else
                    {
                      id = ((*(frameBuffer + 2)) << 8) | (unsigned char)(*(frameBuffer + 3));
                      value = ((*(frameBuffer + 4)) << 8) | (unsigned char)(*(frameBuffer + 5));
                      okFlag = true;
                    }  
                  }
            }
            break;
            
            default:
            break;
            }
        }
        
        if(okFlag)
        {
          return 0;
        }
        else
        {
          return -3;
        }
    }
    else //作为服务端，解析请求帧
    {
        if ((endIndex - startIndex + 1) > ModbusConstants::FRAME_BUFFER_SIZE)
        {
            return -4;
        }
        
        int state = 0;
        bool okFlag = false;
        
        unsigned char rxBuf[16];
        memset(rxBuf, 0, sizeof(rxBuf));
    
        while(1)
        {
        if(okFlag)
        {
            break;
        }
        
        if(startIndex >= length)
        {
            break;
        }
        
          unsigned char rc = buffer[startIndex];
          startIndex++;
          switch(state)
          {
            case 0:
            {
                if(rc == (unsigned char)node)
                {
                  rxBuf[0] = rc;
                  state = 1;
                }
                else
                {
                  state = 0;
                }
            }
            break;
        
            case 1:
            {
              if((rc == ModbusConstants::FUNCTION_CODE_03) ||(rc == ModbusConstants::FUNCTION_CODE_04) 
                || (rc == ModbusConstants::FUNCTION_CODE_05) || (rc == ModbusConstants::FUNCTION_CODE_06))
              {
                rxBuf[1] = rc;
                fun = rc;
                state = 2;
              }
              else
              {
                state = 0;      
                startIndex--;
              }
            }
            break;
          
            case 2:
            {
                rxBuf[2] = rc;
                if(length - startIndex < 5)
                {
                    state = 0;
                    startIndex--;
                }
                else
                {
                    memcpy((void*)(rxBuf+3), (const void*)(buffer + startIndex), 5);
                    int checkSum = ((*(rxBuf + 6)) << 8) | (unsigned char)(*(rxBuf + 7));
                    if (checkSum != ModbusConstants::get_check_sum(rxBuf, 6, crc))
                    {
                        state = 0;
                        startIndex--;
                    }
                    else
                    {
                        if((rxBuf[1] == ModbusConstants::FUNCTION_CODE_03) ||(rxBuf[1] == ModbusConstants::FUNCTION_CODE_04))
                        {
                            id = ((*(rxBuf + 2)) << 8) | (unsigned char)(*(rxBuf + 3));
                            nr = ((*(rxBuf + 4)) << 8) | (unsigned char)(*(rxBuf + 5));
                        }
                        else if((rxBuf[1] == ModbusConstants::FUNCTION_CODE_05) ||(rxBuf[1] == ModbusConstants::FUNCTION_CODE_06))
                        {
                            id = ((*(rxBuf + 2)) << 8) | (unsigned char)(*(rxBuf + 3));
                            value = ((*(rxBuf + 4)) << 8) | (unsigned char)(*(rxBuf + 5));  
                        }
                        okFlag = true;
                    }  
              }
            }
            break;
        
            default:
            break;    
          }
          
        }
    
        if(okFlag)
        {
            return 0;
        }
        else
        {
            return -5;
        }
    }
    
    return 0;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
float ModbusFrame::getInfoData(int index, int type, int bit)
{    
    int size = -1;
    if (type == 0) //2字节有符号数
    {
        size = 2;
    }
    else if (type == 1) //位数据
    {
        size = 1;
    }
    else if (type == 2) //四字节整型数数据
    {
        size = 4;
    }
    else if(type == 3)  //四字节浮点数数据
    {
        size = 4;  
    }
    else if(type == 4)  //单字节数据
    {
        size = 1;  
    }
    else if (type == 5) //有符号普通数据
    {
        size = 2;
    }
    else if (type == 6) //当数据为FFFF时更改成特殊值
    {
        size = 2;
    }
    else if(type == 7) //2bit数据
    {
        size = 1;
    }
    else if(type == 8) //2字节无符号数
    {
        size = 2;
    }
    else if(type == 9) //高16位与低16位顺序以小端方式发送 四字节整型数数据 ZJX 
    {
        size = 4;
    }
    else if(type == 10) //高16位与低16位顺序以小端方式发送 四字节浮点数数据 ZJX
    {
        size = 4;
    }
    else if(type == 11) //高位--》低位 四字节浮点数数据 ZJX
    {
        size = 4;
    }
      
    if ((index < ModbusConstants::FRAME_HEADER_SIZE) || (size < 0) || ((index + size) > (frameLength - ModbusConstants::FRAME_TAIL_SIZE)) || (bit > 7))
    {
        return 0;
    }
    
    if (type == 0) //2字节有符号数
    {
        //考虑数据可能为-10000,表示不支持
        short var = 0;   
      
        var = (short)(((*(frameBuffer + index)) << 8) | (unsigned char)(*(frameBuffer + index + 1)));
      
        return var;
    }
    else if (type == 8) //2字节无符号数
    {
        //用32位有符号数存16位数即为16位无符号数
         int var = 0;   
      
         var = (int)((*(frameBuffer + index)) << 8) | (unsigned char)(*(frameBuffer + index + 1));
      
        return (float)var;
    }
    else if (type == 1) //位数据
    {
        if (bit < 0)
        {
            return 0;
        }
        
        int result = 0;
        unsigned char onebyte = (unsigned char)(*(frameBuffer + index));
        if (onebyte & (1 << bit))
        {
            result = 1;
        }
        
        return (float)result;
    }
    else if (type == 7) //2位数据
    {
        if (bit < 0)
        {          
            return 0;
        }
        
        int result = 0;
        unsigned char onebyte = (unsigned char)(*(frameBuffer + index));
        
        result = (onebyte & (3 << bit))>>bit;    
        
        return (float)result;
    }
    else if (type == 2) //四字节整型数数据
    {
        int var = 0;
        var = ((*(frameBuffer + index)) << 24) | ((*(frameBuffer + index + 1)) << 16) | ((*(frameBuffer + index + 2)) << 8) | (unsigned char)(*(frameBuffer + index + 3));
              
        return (float)var;
    }
    else if(type == 3)  //四字节浮点数数据
    {
        //仿照电总协议解析浮点数的方法，先将每个4bit数据转换成ascii码
        unsigned short tmpBuffer[8];
        int j = 0;
        for(int i = 3; i >= 0; i--)
        {
            //通过sprintf转换成ascii时原始数据不能以0开始，否则类似0x00的数据会转换失败
            tmpBuffer[j++] = (unsigned short)(0xFF<<8 | *(frameBuffer + index+i));
        }
        unsigned char changeBuffer[4][8];
        for(int i = 0; i < 4; i++)
        {
            //changeBuffer[i]中已经转换后的ascii字符，其中有两个FF是为了sprintf转换成功填充的字节
            sprintf((char *)changeBuffer[i],  "%X",  tmpBuffer[i]);
        }
        
        unsigned char ascBuffer[16];
        for(int i = 0; i < 4; i++)
        {
            ascBuffer[i*2] = changeBuffer[i][2];
            ascBuffer[i*2+1] = changeBuffer[i][3];
        }
        
        float result = 0;
        unsigned char* presult = (unsigned char*)(&result);
        int var;
        for (int i = 0; i < 4; i++)
        {
            var = 0;
            if (sscanf((const char*)(ascBuffer + i * 2), "%02x", &var) < 0)
            {
                return 0;
            }
            *(presult + i) = (unsigned char)var;
        }
        
        return result;
    }
    else if(type == 4)  //单字节数据
    {
        return *(frameBuffer + index);
    }
    else if (type == 5) //有符号普通数据
    {
        short  tmp = (short)(((*(frameBuffer + index)) << 8) | (unsigned char)(*(frameBuffer + index + 1)));
        return tmp;
    }
    else if (type == 6) //有符号普通数据
    {
        int tmp = ((*(frameBuffer + index)) << 8) | (unsigned char)(*(frameBuffer + index + 1));
        if(tmp == 0xFFFF)
        {
            return -10000;  
        }
        else
        {
            return (float)tmp;
        }
    }
    else if (type == 9) //高16位与低16位顺序以小端方式发送 四字节整型数数据 ZJX 
    {
        /*
        int var = 0;
        var = ((*(frameBuffer + index)) << 8) | ((*(frameBuffer + index + 1))) | ((*(frameBuffer + index + 2)) << 24) | ((*(frameBuffer + index + 3)<<16));                
        return var;
        */

        
        union Eudata edata;
        
        edata.buff_temp[0]=*(frameBuffer + index+1);
        edata.buff_temp[1]=*(frameBuffer + index);
        edata.buff_temp[2]=*(frameBuffer + index+3);
        edata.buff_temp[3]=*(frameBuffer + index+2);
        return (float)edata.ivar;
      
    }
    else if (type == 10) //高16位与低16位顺序以小端方式发送 四字节浮点数数据 ZJX
    {
        union Eudata edata;
        
        edata.buff_temp[0]=*(frameBuffer + index+1);
        edata.buff_temp[1]=*(frameBuffer + index);
        edata.buff_temp[2]=*(frameBuffer + index+3);
        edata.buff_temp[3]=*(frameBuffer + index+2);

        return edata.fvar;         
    }
    else if (type == 11) //四字节浮点数数据 ZJX
    {
        union Eudata edata;
        
        edata.buff_temp[0]=*(frameBuffer + index+3);
        edata.buff_temp[1]=*(frameBuffer + index+2);
        edata.buff_temp[2]=*(frameBuffer + index+1);
        edata.buff_temp[3]=*(frameBuffer + index);

        return edata.fvar;         
    }
    
    return 0;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
bool ModbusFrame::getInfoData(int index, char* string, int length)
{
    if ((index < ModbusConstants::FRAME_HEADER_SIZE) || ((index + length) > (frameLength - ModbusConstants::FRAME_TAIL_SIZE)))
    {
        return false;
    }
    
    memset(string, 0, length);
    
    for (int i = 0; i < length; i++)
    {
        string[i] = (unsigned char)(*(frameBuffer + index + i));
    }
    
    return true;
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2008-12-30 17:28
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/    
void ModbusFrame::addInfoData(float value, int type)
{
    if (type == 0) //2字节有符号数
    {
        sprintf((char *)(frameBuffer + dataIndex), "%c%c", (unsigned char)(((int)value) >> 8), (unsigned char)(((int)value) & 0xFF));
        dataIndex += 2;
    }
    else if (type == 1) //位数据
    {
      
    }
    else if (type == 2) //四字节整型数据
    {

    }
    else if (type == 3) //四字节浮点数数据
    {
        unsigned char* pdata = (unsigned char*)(&value);
        sprintf((char *)(frameBuffer + dataIndex), "%c%c%c%c", *(pdata + 3), *(pdata + 2), *(pdata + 1), *pdata);
        dataIndex += 4;
    }
    else if(type == 4)  //单字节数据
    {
        sprintf((char *)(frameBuffer + dataIndex), "%c", (unsigned char)(value));
        dataIndex += 1;        
    }
}

/*<FUNC>***********************************************************************
* 函数名称: 
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
* 作    者: 
* 完成日期: 2007-11-29 13:35
* 版    本: V1.0
* 修改记录: 
*     修改日期         版 本         修改人               修改摘要
* -------------------------------------------------------------------------
**<FUNC>**********************************************************************/
void ModbusFrame::addInfoData(unsigned char* string, int length)
{
    if ((string == 0) || (length < 0))
    {
        return;
    }
    
    for (int i = 0; i < length; i++)
    {
        sprintf((char *)(frameBuffer + dataIndex), "%c", string[i]);
        dataIndex += 1;
    }
}

}

