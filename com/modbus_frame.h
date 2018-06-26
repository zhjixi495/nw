/*<FH>*************************************************************************
* 版权所有: Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
* 文件名称: 
* 文件标识: 
* 内容摘要: 
* 其它说明: 
* 当前版本: V1.0
* 作    者: 
* 完成日期: 2012-5-2 11:53:47
* 修改记录1: 
*     修改日期: 
*     版 本 号: 
*     修 改 人: 
*     修改内容:
**<FH>************************************************************************/

#ifndef _CP_MODBUSFRAME_HPP
#define _CP_MODBUSFRAME_HPP

namespace charger 
{
	class ModbusFrame
	{
	public:
		ModbusFrame(bool request = true);
		virtual ~ModbusFrame();
				
		virtual void setCrc(int crc);
		virtual void setNode(int node);
		virtual int getNode();
		virtual void setFun(int fun, bool request = true);
		virtual int getFun();
		virtual void setId(int id);
		virtual int getId();
		virtual void setNr(int nr);
		virtual int getNr();
		virtual void setError(int error);
		virtual int getError();
		virtual void setBytes(int bytes);
		virtual int getBytes();		
		virtual void setValue(int value);
		virtual int getValue();
		virtual void setData(int data);
		
		virtual void clearBuffer();
		virtual const unsigned char* getBuffer();
		virtual int getLength();
		virtual int parse(const unsigned char* buffer, int length);
		
		virtual float getInfoData(int index, int type, int bit);
		virtual bool getInfoData(int index, char* string, int length);
		virtual void addInfoData(float value, int type = 0);
		virtual void addInfoData(unsigned char* string, int length);
		
		union Eudata
        {
            unsigned char buff_temp[4];
            int ivar;
            float fvar;
        };
        
	private:
		ModbusFrame(const ModbusFrame& other);
        ModbusFrame& operator=(const ModbusFrame& other);
                
        unsigned char frameBuffer[4096];							//缓冲区
        int frameLength;											//长度
        int dataIndex;												//数据区指针
        
        static const int DATA_BASE_INDEX;							//数据区的起始位置
        
        bool request;												//请求帧标志
        
        int crc;													//CRC校验顺序
        
        unsigned int node;											//地址
        unsigned int fun;											//功能码
        unsigned int id;											//首地址或数据字节个数（nr_byte）
        unsigned int nr;											//寄存器个数或数据（data）
        unsigned int bytes;											//设置命令数据个数
        unsigned int value;											//遥控命令设置值
        unsigned int error;											//故障代码
	};
}

#endif /*_CP_MODBUSFRAME_HPP*/

