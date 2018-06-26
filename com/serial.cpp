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

#include "serial.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/time.h>

//增加对非标准波特率的支持
#include <linux/serial.h>

#define TIOCGRS485  0x542E
#define TIOCSRS485  0x542F
#define CMSPAR      010000000000


namespace charger 
{
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
	Serial::Serial(const char* _port, const char* _settings, int _debug, int _timeout)
	{
		memset(port, 0, sizeof(port));
		if ((_port != 0) && (strlen(_port) > 0))
		{
			strcpy(port, _port);
		}

		memset(settings, 0, sizeof(settings));
		if ((_settings != 0) && (strlen(_settings) > 0))
		{
			strcpy(settings, _settings);
		}
		
		timeout = _timeout;
		handle = -1;
		baudrate = 1;
		
		debug = _debug;
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
    Serial::~Serial()
	{
		close();
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
	bool Serial::open()
	{
		if (handle >= 0)
		{
			return true;
		}
		
		if ((port == 0) || (strlen(port) <= 0) || (settings == 0) || (strlen(settings) <= 0))
		{
			return false;
		}
				
		handle = ::open(port, O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXCL);
			    
	    if (handle < 0)
		{
			return false;
		}
		
		if (!setInternal())
		{
			::close(handle);
			handle = -1;
			return false;
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
	* 完成日期: 2007-11-29 13:35
	* 版    本: V1.0
	* 修改记录: 
	*     修改日期         版 本         修改人               修改摘要
	* -------------------------------------------------------------------------
	**<FUNC>**********************************************************************/
	bool Serial::change(const char* _settings, int _debug, int _timeout)
	{
		if ((_settings == 0) || (strlen(_settings) <= 0) || (_timeout < 0))
		{
			return false;
		}
		
		timeout = _timeout;

		if (strcmp(settings, _settings) == 0)
		{
			return true;
		}
		
		debug = _debug;
		strcpy(settings, _settings);
		return setInternal();
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
	int Serial::send(const char* buffer, int length)
	{
		if (handle < 0)
		{
			return -1;
		}
				
		if ((buffer == 0) || (length <= 0))
		{
			return -1;
		}
		
		int	totalBytesWrite = 0;
		int rc;
		char* pb = (char*)buffer;
		
	        
		if (timeout > 0)
		{
			rc = waitWritable(timeout);
			if (rc <= 0)
			{
				return (rc == 0) ? 0 : -1;
			}
			
			int	retry = 3;
			while (length > 0)
			{
				rc = write(handle, pb, (size_t)length);
				if (rc > 0)
				{
		            length -= rc;
		            pb += rc;
					totalBytesWrite += rc;
		
					if (length == 0)
					{
						break;
					}
				}
				else
				{
					retry--;
					if (retry <= 0)
					{
						break;
					}
				}
				
				rc = waitWritable(50);
				if (rc <= 0)
				{
					break;
				}
			}
		}
		else
		{
			rc = write(handle, pb, (size_t)length);
			if (rc > 0)
			{
				totalBytesWrite += rc;
			}
			else if ((rc < 0) && (errno != EINTR) && (errno != EAGAIN))
			{
				return -1;
			}
		} 
		
		if(debug)
		{
		    printf("Serial Tx: ");
            for(int i = 0; i < totalBytesWrite; i++)
        	{
        		printf("%02X ", (buffer[i]) & 0xFF);	
        	}
        	printf("\n\n");
		}
		
		return totalBytesWrite;
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
	int Serial::recv(char* buffer, int length)
	{
		if (handle < 0)
		{
			return -1;
		}
				
		if ((buffer == 0) || (length <= 0))
		{
			return -1;
		}
		memset(buffer, 0, length);
		
		int	totalBytesRead = 0;
		int rc;
		char* pb = buffer;
		if (timeout > 0)
		{
			rc = waitReadable(timeout);
			if (rc <= 0)
			{
				return (rc == 0) ? 0 : -1;
			}
			
			int	retry = 3;
			while (length > 0)
			{
				rc = read(handle, pb, (size_t)length);
				if (rc > 0)
				{
		            length -= rc;
		            pb += rc;
					totalBytesRead += rc;
		
					if (length == 0)
					{
						break;
					}
				}
				else if (rc < 0)
				{
					retry--;
					if (retry <= 0)
					{
						break;
					}
				}
				
				rc = waitReadable(20);
				if (rc <= 0)
				{
					break;
				}
			}
		}
		else
		{
			rc = read(handle, pb, (size_t)length);
			if (rc > 0)
			{
				totalBytesRead += rc;
			}
			else if ((rc < 0) && (errno != EINTR) && (errno != EAGAIN))
			{
				return -1;
			}
		}
		
		if(debug)
		{
		    printf("Serial Rx: ");
            for(int i = 0; i < totalBytesRead; i++)
        	{
        		printf("%02X ", (buffer[i]) & 0xFF);	
        	}
        	printf("\n\n");
		}
		
		return totalBytesRead;
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
	void Serial::close()
	{
		if (handle < 0)
		{
			return;
		}
		
		//clear the transmit buffer
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 50*1000;
		select(0, NULL, NULL, NULL, &tv);
		tcflush(handle, TCOFLUSH);
		
		//clear the receive buffer
		int	c;
		while (read(handle, &c, 1) > 0);
		tcflush(handle, TCIFLUSH);
		
		::close(handle);
		
		handle = -1;
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
	bool Serial::setInternal()
	{
		if (handle < 0)
		{
			handle = ::open(port, O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXCL);
			
			if (handle < 0)
			{
				return false;
			}
		}
				
		char szBaudrate[8], szParity[8], szDatabits[8], szStopbits[8];
		if (sscanf(settings, "%[^,],%[^,],%[^,],%s", szBaudrate, szParity, szDatabits, szStopbits) < 0)
		{
			return false;
		}
		baudrate = atoi(szBaudrate);
		int parity = 0;
		if ((szParity[0] == 'N') || (szParity[0] == 'n'))
		{
	        parity = 0;
		}
	    else if ((szParity[0] == 'O') || (szParity[0] == 'o'))
		{
	        parity = 1;
		}
	    else if ((szParity[0] == 'E') || (szParity[0] == 'e'))
		{
	        parity = 2;
		}
	    else if ((szParity[0] == 'M') || (szParity[0] == 'm'))
		{
	        parity = 3;
		}
	    else if ((szParity[0] == 'S') || (szParity[0] == 's'))
		{
	        parity = 4;
		}
		int databits = atoi(szDatabits);
		int stopbits = atoi(szStopbits);
		if ((baudrate < 1200) || (parity < 0) || (databits < 5) || (stopbits < 1))
		{
			return false;
		}
		
		struct termios options;
		static int databitsArray[] = {CS5, CS6, CS7, CS8};
		static unsigned long baudrateArray[] = {B1200, B2400, B4800, B9600, B19200, B38400, B115200};
		
		// Get the current options
	    tcgetattr(handle, &options);
	
		// Set the baud rate 1200-115200
		if (baudrate == 1200)
		{
			cfsetispeed(&options, baudrateArray[0]);
			cfsetospeed(&options, baudrateArray[0]);
		}
		else if (baudrate == 2400)
		{
			cfsetispeed(&options, baudrateArray[1]);
			cfsetospeed(&options, baudrateArray[1]);
		}
		else if (baudrate == 4800)
		{
			cfsetispeed(&options, baudrateArray[2]);
			cfsetospeed(&options, baudrateArray[2]);
		}
		else if (baudrate == 9600)
		{
			cfsetispeed(&options, baudrateArray[3]);
			cfsetospeed(&options, baudrateArray[3]);
		}
		else if (baudrate == 19200)
		{
			cfsetispeed(&options, baudrateArray[4]);
			cfsetospeed(&options, baudrateArray[4]);
		}
		else if (baudrate == 38400)
		{
			cfsetispeed(&options, baudrateArray[5]);
			cfsetospeed(&options, baudrateArray[5]);
		}
		else if (baudrate == 115200)
		{
			cfsetispeed(&options, baudrateArray[6]);
			cfsetospeed(&options, baudrateArray[6]);
		}
	    else
	    {
	    	//非标准波特率
	    	struct serial_struct ss,ss_set;
	    	
	    	cfsetispeed(&options, baudrateArray[5]);
	    	cfsetospeed(&options, baudrateArray[5]);
	    	
	    	tcflush(handle, TCIFLUSH);
	    	tcsetattr(handle, TCSANOW, &options);
	    	if((ioctl(handle, TIOCGSERIAL, &ss)) < 0)
	    	{  
				printf("BAUD: error to get the serial_struct info:%s\n",strerror(errno));  
				return false;  
			}

			ss.flags = ASYNC_SPD_CUST;
			ss.custom_divisor = ss.baud_base / baudrate;
			
			if((ioctl(handle, TIOCSSERIAL, &ss)) < 0)
			{  
				printf("BAUD: error to set serial_struct:%s\n",strerror(errno));  
				return false;  
			}
			
			ioctl(handle, TIOCGSERIAL, &ss_set);
			printf("BAUD: success set baud to %d,custom_divisor=%d,baud_base=%d\n", baudrate, ss_set.custom_divisor, ss_set.baud_base);
	    }
	    
	    // parity 0-n, 1-o, 2-e, 3-m, 4-s
	    options.c_cflag &= ~PARENB; // default, no parity
	    
	    if (parity == 1) // odd
	    {
	        options.c_cflag |= PARENB;
	        options.c_cflag |= PARODD;
	    }
	    else if (parity == 2) // even
	    {
	        options.c_cflag |= PARENB;
	        options.c_cflag &= ~PARODD;
	    }
	    else if (parity == 3) // mark
	    {
	        options.c_cflag |= PARENB;
	        options.c_cflag |= PARODD;
	        options.c_cflag |= CMSPAR;
	    }
	    else if (parity == 4) // space
	    {
	        options.c_cflag |= PARENB;
	        options.c_cflag &= ~PARODD;
	        options.c_cflag |= CMSPAR;
	    }
	    
	    // data bits 5-8
	    options.c_cflag &= ~CSIZE;
	    if ((databits >= 5) && (databits <= 8))
	    {
	    	options.c_cflag |= databitsArray[databits - 5];
	    }
	    else
	    {
	    	options.c_cflag |= CS8;
	    }
	    
		// stop bits 1-2
		if (stopbits == 1)
		{
	        options.c_cflag &= ~CSTOPB;    // 1 stop bits
	    }
	    else if (stopbits == 2)
	    {
	        options.c_cflag |= CSTOPB;
	    }
	    else
	    {
	    	options.c_cflag &= ~CSTOPB;    // 1 stop bits
	    }
	    
	    // input modes
	    options.c_iflag = 0;
	    if (parity != 0) // enable parity
		{
			//options.c_iflag |= (INPCK|ISTRIP);
			options.c_iflag |= (INPCK);
		}
		else // disable
		{
			options.c_iflag &= ~INPCK;
		}
		options.c_iflag |= IGNPAR;
	        
	    // control modes
		options.c_cflag |= (CLOCAL | CREAD); //Ignore modem control lines and enable receiver
		options.c_cflag &= ~CRTSCTS; //Disable hardware flow control
		
		options.c_iflag &= ~(IXON | IXOFF | IXANY); //Disable software flow control
				
		// Set raw mode
		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Enable data to be processed as raw input
		options.c_oflag = 0;
	    		
		tcflush(handle, TCIOFLUSH);
	
	    // Set the new options
	    if (tcsetattr(handle, TCSANOW, &options) != 0)
	    {
			return false;
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
	* 完成日期: 2007-11-29 13:35
	* 版    本: V1.0
	* 修改记录: 
	*     修改日期         版 本         修改人               修改摘要
	* -------------------------------------------------------------------------
	**<FUNC>**********************************************************************/
	int Serial::waitReadable(int millis)
	{
		if (handle < 0)
		{
			return -1;
		}
		int serial = handle;
		
		fd_set fdset;
	    struct timeval tv;
	    int rc = 0;
		
		while (millis > 0)
		{
			if (millis < 5000)
			{
				tv.tv_usec = millis % 1000 * 1000;
				tv.tv_sec  = millis / 1000;
	
				millis = 0;
			}
			else
			{
				tv.tv_usec = 0;
				tv.tv_sec  = 5;
	
				millis -= 5000;
			}
	
			FD_ZERO(&fdset);
			FD_SET(serial, &fdset);
			
			rc = select(serial + 1, &fdset, NULL, NULL, &tv);
			if (rc > 0)
			{
				rc = (FD_ISSET(serial, &fdset)) ? 1 : -1;
				break;
			}
			else if (rc < 0)
			{
				rc = -1;
				break;
			}
		}
	
	    return rc;
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
	int Serial::waitWritable(int millis)
	{
		if (handle < 0)
		{
			return -1;
		}
		int serial = handle;
		
		fd_set fdset;
	    struct timeval tv;
	    int rc = 0;
		
		while (millis > 0)
		{
			if (millis < 5000)
			{
				tv.tv_usec = millis % 1000 * 1000;
				tv.tv_sec  = millis / 1000;
	
				millis = 0;
			}
			else
			{
				tv.tv_usec = 0;
				tv.tv_sec  = 5;
	
				millis -= 5000;
			}
	
			FD_ZERO(&fdset);
			FD_SET(serial, &fdset);
			
			rc = select(serial + 1, NULL, &fdset, NULL, &tv);
			if (rc > 0)
			{
				rc = (FD_ISSET(serial, &fdset)) ? 1 : -1;
				break;
			}
			else if (rc < 0)
			{
				rc = -1;
				break;
			}
		}
	
	    return rc;
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
    int Serial::getBaudrate()
    {
        return baudrate;
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
    void Serial::setDebugMode(int val)
    {
        debug = val;
    }
}

