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
