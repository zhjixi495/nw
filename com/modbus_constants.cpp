/*<FH>*************************************************************************
* 版权所有: Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
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

#include "modbus_constants.h"


namespace charger 
{
const int ModbusConstants::FRAME_BUFFER_SIZE     = 4096;     
const int ModbusConstants::FRAME_HEADER_SIZE     = 2;        
const int ModbusConstants::FRAME_TAIL_SIZE         = 2;        

const unsigned int ModbusConstants::FUNCTION_CODE_01        = 1;        
const unsigned int ModbusConstants::FUNCTION_CODE_02        = 2;        
const unsigned int ModbusConstants::FUNCTION_CODE_03        = 3;        
const unsigned int ModbusConstants::FUNCTION_CODE_04        = 4;        
const unsigned int ModbusConstants::FUNCTION_CODE_05        = 5;        
const unsigned int ModbusConstants::FUNCTION_CODE_06        = 6;        
const unsigned int ModbusConstants::FUNCTION_CODE_0F        = 15;        
const unsigned int ModbusConstants::FUNCTION_CODE_16        = 16;        


const int ModbusConstants::RTN_OK                = 0x00;        


int ModbusConstants::get_check_sum(const unsigned char* buffer, int length, int type)
{
    if ((buffer == 0) || (length <= 0) || (type < 0))
    {
        return -1;
    }
      
    unsigned char crcHi = 0xFF;
    unsigned char crcLo = 0xFF;
    unsigned int index;
    unsigned char jbus_crc;
    
    jbus_crc = (unsigned char)(type&0xff);
    
    for (int i = 0; i< length; i++)
    {
        index = crcHi ^ (*(buffer+i));
          crcHi = crcLo ^ CrcHiTable[index];
          crcLo = CrcLoTable[index];
    }

    if (jbus_crc == 0)
    {
      return (crcLo << 8 | crcHi);
    }
    else
    {
        return (crcHi << 8 | crcLo);
    }
}
}
