/*<FH>*************************************************************************
* ��Ȩ����: Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
* �ļ�����: 
* �ļ���ʶ: 
* ����ժҪ: 
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
