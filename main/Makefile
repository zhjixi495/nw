#

include $(SOURCE_DIR)/config_vars.mk

CXXFLAGS += -I../com 
CXXFLAGS += -I../record

ifneq ($(TARGET), x86)
	EXEC = nw_charger
else
	EXEC = nw_charger_x86
endif

	CFLAGS += -fpic
	OBJS = main.o 

ifeq ($(TARGET), x86)
	LIBSOURCE += -lcmlcom_x86
	LIBSOURCE += -lcmlrecord_x86
	LIBSOURCE += -lsqlite3_x86
else
	LIBSOURCE += -lcmlcom
	LIBSOURCE += -lcmlrecord
	LIBSOURCE += -lsqlite3
endif

all: $(EXEC) 

$(EXEC): $(OBJS)
	$(CXX)  $(LDFLAGS) -o $@ $(OBJS) $(LIBS)  -L../lib -L../release $(LIBSOURCE)
	cp -f $@ ../release/

ifneq ($(TARGET), x86)
#	$(STRIP) -s $@
endif
	cp -f $@ $(RELEASE_DIR)/

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o *.so
	-rm -f *.elf *.gdb *.o *.so




