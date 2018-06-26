#include "Connection.hpp"
#include "Serial.hpp"
#include <string>
#include "DLT6451997Frame.h"

using namespace esf;
using namespace cp;

#define READ_CMD 0x01
        
//namespace cp 
//{
    //class Serial;
	class DLT645Connection
	{
	public:
		DLT645Connection(const char* _config,Serial* _serial,int _keepaliveid = 0,bool _debug = false);
		virtual ~DLT645Connection();
								
		virtual bool open();
		virtual void close();
		virtual bool process();
		
        typedef struct
        {
            int index;
        	unsigned char addr_h[10];
        	unsigned char addr_l[10];
        }DLT645Addr;
        typedef std::map<int, DLT645Addr*> DLT645AddrMap;
        DLT645AddrMap dtl645addrs;
        
        typedef struct
        {
            int index;
        	unsigned char description[64];
        	int frmId;
        	int frmIndex;
        	int frmType;
        	int frmScale;
        }DataSignal;
        typedef std::map<int, DataSignal*> DataSignalMap;
        DataSignalMap datasignals;
        
        typedef struct
        {
            int index;
        	unsigned char description[64];
        	unsigned int cmd;
        }CmdSignal;
        typedef std::map<int, CmdSignal*> CmdSignalMap;
        CmdSignalMap cmdsignals;
        
        enum DLT_State
        { 
        	RS_WAIT = 0,//帧起始符状态
        	RS_ADDER,//接收地址域状态
        	RS_SYNC,//接收帧起始符状态
        	RS_CTRL,//接收控制码状态
        	RS_DATA_LEN,//接收数据长度状态
        	RS_DATA,//接收数据状态
        	RS_CS,//接收校验和状态
        	RS_END//接收校验和状态
        };

        private:
        char config[128];
        int  keepaliveid;
        bool debug;
        int commStateId;//设备通讯状态id
        Serial* serial;
        
        unsigned char ammeter_addr[6];  
        unsigned char Txbuff[258];
        unsigned char Rxbuff[258];
        unsigned char Rxframe[256];
        float  fCollectData[100];
        dlt6451997_ammeter_tx_frame *p_tx_frame; 
        
        int electric_meter_num; //电表个数
        int cmd_num;//命令个数
        int data_signal_num;//字段个数  
        
        void get_ammeter_addr(unsigned char *addr_h,unsigned char *addr_l);
        int parse_recv(int len);  
        float getInfoData(int index, int type);
        void send_cmd(int no);        
	};
//}