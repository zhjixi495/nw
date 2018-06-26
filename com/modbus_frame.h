/*<FH>*************************************************************************
* ��Ȩ����: Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
* �ļ�����: 
* �ļ���ʶ: 
* ����ժҪ: 
* ����˵��: 
* ��ǰ�汾: V1.0
* ��    ��: 
* �������: 2012-5-2 11:53:47
* �޸ļ�¼1: 
*     �޸�����: 
*     �� �� ��: 
*     �� �� ��: 
*     �޸�����:
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
                
        unsigned char frameBuffer[4096];							//������
        int frameLength;											//����
        int dataIndex;												//������ָ��
        
        static const int DATA_BASE_INDEX;							//����������ʼλ��
        
        bool request;												//����֡��־
        
        int crc;													//CRCУ��˳��
        
        unsigned int node;											//��ַ
        unsigned int fun;											//������
        unsigned int id;											//�׵�ַ�������ֽڸ�����nr_byte��
        unsigned int nr;											//�Ĵ������������ݣ�data��
        unsigned int bytes;											//�����������ݸ���
        unsigned int value;											//ң����������ֵ
        unsigned int error;											//���ϴ���
	};
}

#endif /*_CP_MODBUSFRAME_HPP*/

