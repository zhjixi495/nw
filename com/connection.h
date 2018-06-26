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

#ifndef _ESF_CONNECTION_HPP
#define _ESF_CONNECTION_HPP

#include <map>


namespace charger 
{
	class Connection
	{
	public:
		virtual ~Connection() {};
		
		typedef std::map<int, float> DataMap;
		typedef std::map<int, int> EventMap;
	    typedef std::map<int, char*> WaveMap;
		typedef std::map<int, int> WaveSizeMap;
		
		virtual bool open() = 0;
		virtual void close() = 0;
		virtual bool process(bool waveFlag) = 0;
		virtual const char* info() = 0;
		virtual bool init() = 0;
		
		virtual void setDebugMode(int val) = 0;
		
		virtual void getWave(WaveMap& waveMap) = 0;
		virtual int getWaveSize(WaveSizeMap& waveSizeMap) = 0;
		virtual void getData(DataMap& dataMap) = 0;
		virtual void setEvent(EventMap& eventMap) = 0;
		virtual void getEvent(EventMap& eventMap) = 0;
		virtual int getEventLevel(int index) = 0;
		virtual bool control(int index, float var, char* batch, int length,bool broadcastFlag = false)=0;
	};
}

#endif /*_ESF_CONNECTION_HPP*/
