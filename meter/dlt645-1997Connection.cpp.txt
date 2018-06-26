#include <sys/time.h>
#include "dlt645-1997Connection.h"
#include "ConfigFile.hpp"
#include "ModbusConstants.hpp"
#include "ModbusFrame.hpp"
#include "PThreads.hpp"
#include <string.h>
#include <iostream>

#define BCD_TO_BIN(val) ((val)=(unsigned char)(((val)&15) + ((val)>>4)*10))
#define BIN_TO_BCD(val) ((val)=(((val)/10)<<4) + (val)%10)

#define commOK   0
#define commFAIL 1

using namespace std;
//namespace cp 
//{
    
DLT645Connection::DLT645Connection(const char* _config,Serial* _serial,int _keepaliveid,bool _debug)
{
    int i;
    
    strcpy(config, _config);
    serial = _serial;
    keepaliveid = _keepaliveid;
    debug = _debug;
    commStateId=-1;
    
    dtl645addrs.clear();
    datasignals.clear();
    cmdsignals.clear();
    
    for(i=0;i<(int)sizeof(ammeter_addr);i++)
    {
        ammeter_addr[i]=0x99;
    }
    
    electric_meter_num = 0; //电表个数
    cmd_num = 0;//命令个数
    data_signal_num = 0;//字段个数  
}

DLT645Connection::~DLT645Connection()
{
    close();
}

void DLT645Connection::close()
{    
    memset(config, 0, sizeof(config));
	serial = 0;
	
	for (DLT645AddrMap::iterator i = dtl645addrs.begin(); i != dtl645addrs.end(); ++i)
	{
        delete ((*i).second);
	}
	dtl645addrs.clear();
	
    for (DataSignalMap::iterator i = datasignals.begin(); i != datasignals.end(); ++i)
	{
        delete ((*i).second);
	}
    datasignals.clear();
    
    
    for (CmdSignalMap::iterator i = cmdsignals.begin(); i != cmdsignals.end(); ++i)
	{
        delete ((*i).second);
	}
    cmdsignals.clear();
}
		
bool DLT645Connection::open()
{
    int i;
    ConfigFile configFile;
	if (!configFile.open(config))
	{
		return false;
	}
	
	int num;
	configFile.find("[ELECTRIC_METER_NUM]");
	if (!configFile.read("%d", &num))
	{
		return false;
	}
	electric_meter_num = num;
	//cout<<"ELECTRIC_METER_NUM="<<num<<endl;
	
	if (!configFile.find("[ELECTRIC_METER_ADDR]"))
	{
	    cout<<"Can't find ELECTRIC_METER_ADDR"<<endl;
	    return false;
	}  
	DLT645Addr* dtl645addr;
	for(i=0;i<num;i++)
	{
	    dtl645addr = new DLT645Addr;
	    if (!configFile.read("%d%s%s",&(dtl645addr->index),&(dtl645addr->addr_h),&(dtl645addr->addr_l)))
		{
			return false;
		}
		if((strlen((char *)dtl645addr->addr_h)!=6)||(strlen((char *)dtl645addr->addr_h)!=6))
		{
		    return false;
		}    
		dtl645addrs[i]=dtl645addr;
	}
	
	for (DLT645AddrMap::iterator j = dtl645addrs.begin(); j != dtl645addrs.end(); ++j)
    {
          dtl645addr=((*j).second);
          cout<<" dtl645addr->index ="<<dtl645addr->index<<" dtl645addr->addr_h="<<dtl645addr->addr_h<<" dtl645addr->addr_l="<<dtl645addr->addr_l<<endl;
    }
    
    
    configFile.find("[CMD_NUM]");
	if (!configFile.read("%d", &num))
	{
		return false;
	}
	cmd_num = num;
	//cout<<"CMD_NUM="<<num<<endl;
	
	configFile.find("[CMD_TYPE]");
	CmdSignal *cmdsignal;
	for(i=0;i<num;i++)
	{
	    cmdsignal = new CmdSignal;
	    if (!configFile.read("%d%s%x",&(cmdsignal->index),&(cmdsignal->description),&(cmdsignal->cmd)))
		{
			return false;
		}        
		cmdsignals[i]=cmdsignal;
		//cout<<" cmdsignal->index ="<<cmdsignal->index<<" cmdsignal->description="<<cmdsignal->description<<" cmdsignal->cmd="<<cmdsignal->cmd<<endl;
	}
	
	configFile.find("[DATA_SIGNAL_NUM]");
	if (!configFile.read("%d", &num))
	{
		return false;
	}
	data_signal_num = num;
	//cout<<"DATA_SIGNAL_NUM="<<num<<endl;
	configFile.find("[DATA_SIGNAL]");
	DataSignal *datasignal;
	for(i=0;i<num;i++)
	{
	    datasignal = new DataSignal;
	    if (!configFile.read("%d%s%/d/d/d/d",&(datasignal->index),&(datasignal->description),&(datasignal->frmId),&(datasignal->frmIndex),&(datasignal->frmType),&(datasignal->frmScale)))
		{
			return false;
		}        
		datasignals[i]=datasignal;
		//cout<<" datasignal->index="<<datasignal->index<<" datasignal->description="<<datasignal->description<<" datasignal->frmId="<<datasignal->frmId<<" datasignal->frmIndex="<<datasignal->frmIndex<<" datasignal->frmType="<<datasignal->frmType<<" datasignal->frmScale="<<datasignal->frmScale<<endl;
	}
	
	//倒数第二个数据为设备通讯状态
	//commStateId = datasignals.size() - 2;
	  commStateId = data_signal_num - 2;
	  
		
	configFile.close();	
	return true;
}
float DLT645Connection::getInfoData(int index, int type)
{
    float fdata=0;
    int size = -1;
    
    if(type==0)
    {
        //格式:XX
        size = 1;
    }
    else if(type==1)
    {
        //格式:XXX
        size = 2;
    }
    else if(type==2)
    {
        //格式:XX.XX
        size = 2;
    }
    else if(type==3)
    {
        //格式:XX.XXXX
        size = 3;
    }
    else if(type==4)
    {
        //格式:XXXXXX.XX
        size = 4;
    }
    else if(type==5)
    {
        //格式:MMDDHHmm
        size = 4;
    }
    else if(type==6)
    {
        //格式:YYMMDDHHWW
        size = 4;
    }
    else if(type==7)
    {
        //格式:NNNN
        size = 2;
    }
    else if(type==8)
    {
        //格式:NNNNNN
        size = 3;
    }
    else if(type==9)
    {
        //格式:NN..NN
        size = 6;
    }
    
    if( (index<(int)sizeof(Dlt645_1997_head)) ||(size < 0) || ((index + size)>(int)(Rxframe[9]+sizeof(Dlt645_1997_head))))
    {
        printf("配置文件有误！ config file error！ Rxframe[9]+sizeof(Dlt645_1997_head)=%d,(index + size)=%d,index=%d,size=%d\n",\
        Rxframe[9]+sizeof(Dlt645_1997_head),(index + size),index,size);
        return 0;
    }    
    
    for(int i=index;i<index + size;i++)
    {
        printf("Rxframe[%d]=%d  ",i,Rxframe[i]);
    }
    printf("\n");
    
    if(type==0)
    {
        //格式:XX
        fdata=(float)Rxframe[index];
    }
    else if(type==1)
    {
        //格式:XXX
        fdata=(float)((float)(Rxframe[index+1]*100)+(float)Rxframe[index]);
    }
    else if(type==2)
    {
        //格式:XX.XX
        fdata=(float)((float)(Rxframe[index+1])+((float)Rxframe[index])/100);
    }
    else if(type==3)
    {
        //格式:XX.XXXX
        fdata=(float)((float)(Rxframe[index+2])+((float)Rxframe[index+1])/100+((float)Rxframe[index])/10000);
    }
    else if(type==4)
    {
        //格式:XXXXXX.XX
        fdata=(float)((float)(Rxframe[index+3]*10000)+(float)(Rxframe[index+2]*100)+(float)Rxframe[index+1]+((float)Rxframe[index])/100);
    }
    else if(type==5)
    {
        //格式:MMDDHHmm
        size = 4;
    }
    else if(type==6)
    {
        //格式:YYMMDDHHWW
        size = 4;
    }
    else if(type==7)
    {
        //格式:NNNN
        size = 2;
    }
    else if(type==8)
    {
        //格式:NNNNNN
        size = 3;
    }
    else if(type==9)
    {
        //格式:NN..NN
        size = 6;
    }      
    
    //printf("fdata= %f \n",fdata);
    return fdata;   
}
int DLT645Connection::parse_recv(int len)
{
    unsigned char RecState=RS_WAIT;
    unsigned int star_index=0;
    unsigned int cs;
    unsigned int loopcout=0;
    int data_len=0;//数据域的数据长度
    int i,j;
    int ret=0;
    
    i=0;
    ret=0;
    loopcout=0;
    RecState=RS_WAIT;
    p_tx_frame = (dlt6451997_ammeter_tx_frame *)&Txbuff;
    
    while(len)
    {
        loopcout++;
        //预防程序bug引起死循环
        if(loopcout>256)
        {
            len = 0;
            break;    
        }   
         
        switch(RecState)
        {
            case RS_WAIT://起始状态
                if(Rxbuff[i]==FRAME_START_CHAR) 
				{
				    RecState++;
				    star_index = i;
				}
			    i++;
			    len--;
				break;
		   case RS_ADDER://地址域
		        if(memcmp(&Rxbuff[i],p_tx_frame->addr,6)==0)//验证是否是请求的地址电表回复
		        {
		            RecState++;	
		            i=i+6;
		            len=len-6;	            
		        }
		        else
		        {
		            RecState = RS_WAIT;
		            cout<<"ERROR LINE:"<<__LINE__<<endl;
		            printf("reply addr error\n");
		        }
		       break;
		  case RS_SYNC://帧起始符
                if(Rxbuff[i]==FRAME_START_CHAR) 
                { 
                    RecState++;
                    i++;
                    len--; 	    			          			   
                }
        		else
        		{
                    RecState = RS_WAIT;
                    cout<<"ERROR LINE:"<<__LINE__<<endl;
        		    break;        			    
        		}
		       break; 
		  case RS_CTRL://控制码，直接出队
               RecState++;
               i++;
               len--;  
		       break;
		  case RS_DATA_LEN://数据域的数据长度
		       data_len =Rxbuff[i];//数据域长度
		       i++;
        	   len--;
        	   //数据域之后还有一个校验码和结束符，所以现剩余长度要比数据域数据长才对
               if(len>data_len)
               {
                   RecState++;
               }
               else
               {   //数据长度不对
                   RecState = RS_WAIT;
                   cout<<"ERROR LINE:"<<__LINE__<<endl;
                   printf("reply data length error\n");
                   break;
               } 
		       break;              
		  case RS_DATA://数据域
		       RecState++;
		       len=len-data_len;
		       i=i+data_len;		       
		       break;
          case RS_CS://校验码
               cs = 0;
               for(j=star_index;j<i;j++)
                 cs=cs+Rxbuff[j];
               
               if((cs&0xff)==Rxbuff[i])
               {
                   i++;
                   len--;
                   RecState++; 
               }
               else
               {
                   RecState = RS_WAIT;
                   //cout<<"ERROR LINE:"<<__LINE__<<endl;
                   printf("check sum error cs=%02x i=%d Rxbuff[i]=%02x ",cs,i,Rxbuff[i]);
               }
                    
               break;
          case RS_END: //结束符 
               if(Rxbuff[i]==FRAME_END_CHAR)
               {
                    ret = sizeof(Dlt645_1997_head)+data_len+1+1;
                    //包头+数据域+CS+结束符
                    memcpy(&Rxframe,&Rxbuff[star_index],ret);
                    return ret;
               }     
		       break;      
		  default:
		       break;              		
        }        
    }
    printf("RecState = %02x \n",RecState);
    return ret;
}


bool DLT645Connection::process()
{
    DLT645Addr* dtl645addr;
    for (DLT645AddrMap::iterator j = dtl645addrs.begin(); j != dtl645addrs.end(); j++)
    {          
        dtl645addr=((*j).second);
        get_ammeter_addr(dtl645addr->addr_h,dtl645addr->addr_l);    
        send_cmd((*j).first);            
    }
    return 0;
}

void DLT645Connection::get_ammeter_addr(unsigned char *addr_h,unsigned char *addr_l)
{
    int temp_value;
    temp_value = (addr_h[0]-'0')*10+(addr_h[1]-'0');
    BIN_TO_BCD(temp_value);
    ammeter_addr[5]=(unsigned char)(temp_value&0xff);
    
    temp_value = (addr_h[2]-'0')*10+(addr_h[3]-'0');
    BIN_TO_BCD(temp_value);
    ammeter_addr[4]=(unsigned char)(temp_value&0xff);
    
    temp_value = (addr_h[4]-'0')*10+(addr_h[5]-'0');
    BIN_TO_BCD(temp_value);
    ammeter_addr[3]=(unsigned char)(temp_value&0xff);
    
    temp_value = (addr_l[0]-'0')*10+(addr_l[1]-'0');
    BIN_TO_BCD(temp_value);
    ammeter_addr[2]=(unsigned char)(temp_value&0xff);
    
    temp_value = (addr_l[2]-'0')*10+(addr_l[3]-'0');
    BIN_TO_BCD(temp_value);
    ammeter_addr[1]=(unsigned char)(temp_value&0xff);
    
    temp_value = (addr_l[4]-'0')*10+(addr_l[5]-'0');
    BIN_TO_BCD(temp_value);
    ammeter_addr[0]=(unsigned char)(temp_value&0xff);
}

void DLT645Connection::send_cmd(int no)
{
    int i,j;
    int responseLength = 0;
    int requestLength = 0;
    unsigned int cs=0;
    unsigned char *p=0;
    unsigned char commState;
    CmdSignal *pcmdsignal;
    Dlt645_1997_head *p_head;
    DataSignal *datasignal;
    memset(Txbuff,0,sizeof(Txbuff));
    p_tx_frame = (dlt6451997_ammeter_tx_frame *)&Txbuff;
    
    commState = commFAIL;//初始化通信状态
    
    Txbuff[0]=0xfe;
    Txbuff[1]=0xfe;
    Txbuff[2]=0xfe;
    Txbuff[3]=0xfe;
            
    for(i=0;i<cmd_num;i++)
    {
        for(int k=0;k<3;k++)
        {                
            sleep(1);
            
            if(keepaliveid>0)
            {
                 keepalive(keepaliveid);
            }
             
            
            p_tx_frame->sync1 = FRAME_START_CHAR;
            //电表地址
            p_tx_frame->addr[0] = ammeter_addr[0];
            p_tx_frame->addr[1] = ammeter_addr[1];
            p_tx_frame->addr[2] = ammeter_addr[2];
            p_tx_frame->addr[3] = ammeter_addr[3];
            p_tx_frame->addr[4] = ammeter_addr[4];
            p_tx_frame->addr[5] = ammeter_addr[5];
            
            p_tx_frame->sync2 = FRAME_START_CHAR;
            
            p_tx_frame->control = READ_CMD;
            p_tx_frame->len = 2;
            //DI0、DI1
            pcmdsignal = cmdsignals[i];
            p_tx_frame->data[0] = (unsigned char)((pcmdsignal->cmd&0xff)+ADD_VALUE_CHAR);
            p_tx_frame->data[1] = (unsigned char)(((pcmdsignal->cmd>>8)&0xff)+ADD_VALUE_CHAR);
            
            //校验码是从起始符开始的
            p=(unsigned char *)(&p_tx_frame->sync1);
            cs=0;
            //去掉前面4个FE、一个校验码、一个结束符
            for(j=0;j<(int)(sizeof(dlt6451997_ammeter_tx_frame)-6);j++)
            {
              cs +=*p;
              p++;
              
            }
            p_tx_frame->sum = cs&0xff;
            p_tx_frame->endsync = FRAME_END_CHAR;
                    
            if (!serial->open())
    		{
    			continue;
    		}
    		
            requestLength = sizeof(dlt6451997_ammeter_tx_frame);
            
    		if (serial->send((char *)Txbuff, requestLength) != requestLength)
        	{
        		serial->close();
        		continue;
        	}
            
            save_ac_ammeter_tx_data(no,i,(unsigned char)(requestLength&0xff),Txbuff);
            
        	responseLength = serial->recv((char *)Rxbuff, sizeof(Rxbuff));
        	if (responseLength < 0)
        	{
    
        	    printf("Error---Tx len=%d: ", sizeof(dlt6451997_ammeter_tx_frame));
        	    
    		    for(j=0;j<(int)(sizeof(dlt6451997_ammeter_tx_frame));j++)
                printf("%02x ",Txbuff[j]);
                printf("\n");
    	    	
        	    
        		serial->close();
        		continue;
        	}
            
            save_ac_ammeter_rx_data(no,i,(unsigned char)(responseLength&0xff),Rxbuff);
            
            
            printf("\n\nTx len=%d: ",sizeof(dlt6451997_ammeter_tx_frame));
            for(j=0;j<(int)(sizeof(dlt6451997_ammeter_tx_frame));j++)
            printf("%02x ",Txbuff[j]);
            printf("\n");
                
            if(parse_recv(responseLength)>0)
            {
                printf("Rx len=%d: ",responseLength);
                for(j=0;j<(int)(responseLength);j++)
                printf("%02x ",Rxbuff[j]);
                printf("\n");
                    
                p_head = (Dlt645_1997_head *)&Rxframe;
                //回复包中的DI字段要与发送的一致 DI字段在接收数据的偏移量10位置
                if(memcmp(p_tx_frame->data,&Rxframe[10],2)==0)
                {                       
                    //数据域从偏移量10开始
                    for(j=10;j<10+p_head->len;j++)
                    {
                        //减去0x33h
                        Rxframe[j] = (unsigned char)(Rxframe[j]-ADD_VALUE_CHAR);
                        //把BCD转成十进制数
                        BCD_TO_BIN(Rxframe[j]);
                    }                
                    //cout<<"LINE:"<<__LINE__<<" pAc_ammeter_cmd->index="<<pAc_ammeter_cmd->index<<endl;
                    
                    for(j=0;j<data_signal_num;j++)
                    {
                        datasignal = datasignals[j];
                        if(datasignal->frmId==pcmdsignal->index)
                        {
                            float fdata;
                            
                            fdata = getInfoData(datasignal->frmIndex,datasignal->frmType);
                            if(datasignal->frmScale!=0)
                            {
                                fdata =(float)((fdata)/((float)datasignal->frmScale));
                            }
                            
                            set_ac_ammeter_device_data(no,datasignal->index,fdata);
                            cout<<datasignal->description<<" ="<<fdata<<endl;                             
                            //fCollectData[datasignal->index]=fdata;
                            
                        }
                        else if(datasignal->frmId>i)
                        {
                            //这是为了节省时间，当数据所在的帧大于当个命令索引时，
                            //就直接退出。所以就要求配置文件中的测量数据必须按所
                            //在的命令帧编号从小到大排列！
                            break;
                        }                                
                    }                    
                    //设置通讯状态
		            commState = commOK;
		            //跳出for(int k=0;k<3;k++)
                    break;
                }
                else
                {
                    printf("DI field does not match!\n");
                }                                     
            }
            else
            {               
    	        serial->close();
    	        printf("Error---Rx len=%d: ",responseLength);
                for(j=0;j<(int)(responseLength);j++)
                printf("%02x ",Rxbuff[j]);
                printf("\n");                
            }              
        }
    }
    //设置通讯状态 只要有一条命令通信成功，就标志通信状态是成功的
    set_ac_ammeter_device_data(no,commStateId,commState);
}