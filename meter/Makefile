# Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
# All rights reserved.
#
# Author: 
#

include $(SOURCE_DIR)/config_vars.mk

CXXFLAGS += -I../lib -I../lib/esf

EXEC = ac_ammeter1997
OBJS = ac_ammeter.o dlt645-1997Connection.o

ifeq ($(TARGET), x86)
	GLOG = libglog_x86.a
else
	GLOG = libglog.a
endif



all: $(EXEC) clean

$(EXEC): $(OBJS)
	-rm -f $(RELEASE_DIR)/bin/$@
	echo CXX=$(CXX) STRIP=$(STRIP) GLOG=$(GLOG) LIBS=$(LIBS) OBJS=$(OBJS) 
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(GLOG) libesf.a $(LIBS)
ifneq ($(TARGET), x86)
	$(STRIP) -s $@
endif
	cp -f $@ $(RELEASE_DIR)/bin

clean:
	#-rm -f $(EXEC) *.elf *.gdb *.o
	-rm -f *.elf *.gdb *.o


