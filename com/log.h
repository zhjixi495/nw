#ifndef __LOG_H__
#define __LOG_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

namespace charger
{

#define kLOG_PRINTF_NONE_MODE     0x00
#define kLOG_PRINTF_TIME_MODE    (0x01<<0)
#define kLOG_PRINTF_TIME_MASK    0x01

#define kI_LOG_PRINTF_FLAG      (0x00<<1)
#define kE_LOG_PRINTF_FLAG      (0x01<<1)

#define kLOG_FILE_FLAG          (0x01<<2)

    
#define DEBUG_COLOR
#define INFO_COLOR
#define ERROR_COLOR "\033[1;40;31m"     //ºìÉ«
#define CLOSE_DEBUG_COLOR
#define CLOSE_INFO_COLOR
#define CLOSE_ERROR_COLOR   "\033[0m"
    

extern int log_filename_init(char *file_name,int log_file_size);
extern void debug_log_mode(int mode,const char* format, ...);


#define LOG_DEBUG(format, arg...) {\
                                debug_log_mode(kLOG_PRINTF_TIME_MODE|kI_LOG_PRINTF_FLAG|kLOG_FILE_FLAG,"%s() %s:%d]\n" ,__FUNCTION__, __FILE__, __LINE__);\
                                debug_log_mode(kLOG_FILE_FLAG,format,##arg);\
                              }

#define LOG_ERROR(format, arg...) {\
                                debug_log_mode(kLOG_PRINTF_TIME_MODE|kE_LOG_PRINTF_FLAG|kLOG_FILE_FLAG,"%s() %s:%d]\n" ,__FUNCTION__, __FILE__, __LINE__);\
                                debug_log_mode(kLOG_FILE_FLAG,format,##arg);\
                              }
                              
#define LOG_STRING_NATIVE(format, arg...) {\
                                        debug_log_mode(kLOG_FILE_FLAG,format,##arg);\
                                   }

                              
#define PRINTF_DEBUG(format, arg...) {\
                                debug_log_mode(kLOG_PRINTF_TIME_MODE|kI_LOG_PRINTF_FLAG,"%s() %s:%d]\n" ,__FUNCTION__, __FILE__, __LINE__);\
                                debug_log_mode(kLOG_PRINTF_NONE_MODE,format,##arg);\
                              }

#define PRINTF_ERROR(format, arg...) {\
                                debug_log_mode(kLOG_PRINTF_TIME_MODE|kE_LOG_PRINTF_FLAG,"%s() %s:%d]\n" ,__FUNCTION__, __FILE__, __LINE__);\
                                debug_log_mode(kLOG_PRINTF_NONE_MODE,format,##arg);\
                              }                              
#define PRINTF_STRING_NATIVE(format, arg...) {\
                                        debug_log_mode(kLOG_PRINTF_NONE_MODE,format,##arg);\
                                   }
                                                                                                                             
#define debug_log_hex(buf, len)

} // namespace tcu

#endif // __LOG_H__
