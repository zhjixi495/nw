ifeq ($(TARGET), x86)
    AR = ar
    CC = gcc
    CFLAGS = -fsigned-char -W -Wall -Wfloat-equal -Wcast-align -Wconversion -Wundef -Wpointer-arith -D_REENTRANT -I.
    CXX = g++
    LDFLAGS =  -rdynamic
    LIBS = -lm -lrt -ldl -lpthread
    RANLIB = ranlib
    STRIP = strip
else
    AR = arm-linux-ar
    CC = arm-linux-gcc
    CFLAGS = -fsigned-char -W -Wfloat-equal -Wcast-align -Wconversion -Wpointer-arith -mapcs-frame -D_REENTRANT -DNDEBUG -I.
    CXX = arm-linux-g++
    LDFLAGS =  -rdynamic
    LIBS = -lm -lrt -ldl -lpthread
    RANLIB = arm-linux-ranlib
    STRIP = arm-linux-strip
endif

ECHO="echo -e"
ERROR_COLOR="\033[1;40;31m"
CLOSE_ERROR_COLOR="\033[0m"
SI_CURR_DIR:=$(shell cd ./; pwd)

MAPFILE=${EXEC}.map
LDFLAGS += -Wl,-Map,${MAPFILE}

#add by LRY
CFLAGS += -I $(SOURCE_DIR)/../comLib/include

#去掉文件名前面的路径，只保留最后的文件名称，log日志会用到
CFLAGS += -D__FILENAME__='"$(subst $(shell dirname $<)/,,$<)"'


ifeq ($(DEBUG_ENABLE),YES)
    CFLAGS += -g -O0
else
    CFLAGS += -O2
#    CFLAGS += -s
#    LDFLAGS += -s
endif

CXXFLAGS = $(CFLAGS)
#支持C++11
CXXFLAGS += -std=gnu++0x -std=c++11

############################################################################
### 用-D选项向GCC传入宏定义，支持代码中的条件编译
############################################################################
CXXFLAGS += -D DEBUG_ENABLE=$(DEBUG_ENABLE)
CXXFLAGS += -D PRINT_RECORD_TIME_ENABLE=$(PRINT_RECORD_TIME_ENABLE)
CXXFLAGS += -D SAVE_DEBUG_LOG=$(SAVE_DEBUG_LOG)
CXXFLAGS += -D SAVE_INFO_LOG=$(SAVE_INFO_LOG)
CXXFLAGS += -D SAVE_ERROR_LOG=$(SAVE_ERROR_LOG)
CXXFLAGS += -D MALLOC_DEBUG_ENABLE=$(MALLOC_DEBUG_ENABLE)
CXXFLAGS += -D FREE_DEBUG_ENABLE=$(FREE_DEBUG_ENABLE)
CXXFLAGS += -D LOCK_DEBUG_ENABLE=$(LOCK_DEBUG_ENABLE)

################################################################
### PROJECT_TYPE 向GCC传宏，选择编译成支持哪个公司的协议(或哪个项目)的程序，值为公司名/项目名
################################################################
CXXFLAGS += -D PROJECT_TYPE=PROJECT_$(PROJECT_TYPE)
