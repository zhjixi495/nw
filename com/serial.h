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

#ifndef _BMS_SERIAL_HPP
#define _BMS_SERIAL_HPP

namespace charger 
{
	class Serial
	{
	public:	
		Serial(const char* _port, const char* _settings, int _debug = 0, int _timeout = 1000);
		virtual ~Serial();
		
		bool open();
		bool change(const char* _settings, int _debug = -1, int _timeout = 1000);
		int send(const char* buffer, int length);
		int recv(char* buffer, int length);
		void close();
		
		int getDelay();
		int getBaudrate();
		void setDebugMode(int val);
		
	private:
		Serial(const Serial& other);
        Serial& operator=(const Serial& other);
        
        bool setInternal();
        
		int waitReadable(int millis);
		int waitWritable(int millis);
        
        char port[128];
        char settings[128];
        int timeout;
        int handle;
        int baudrate;
        
        int debug;
        int portid;
	};
}

#endif /*_BMS_SERIAL_HPP*/
