#

include $(SOURCE_DIR)/config_vars.mk

CXXFLAGS += -I../com 


ifneq ($(TARGET), x86)
	EXEC = libcmlrecord.so
else
	EXEC = libcmlrecord_x86.so
endif

	CFLAGS += -fpic
	OBJS = sqlite_db.o 
	OBJS += sqlite_operate_db.o 
	OBJS += charger_record_db.o 
	OBJS += card_list_db.o 
	
all: $(EXEC) 

$(EXEC): $(OBJS)
	$(CC) -shared  $(OBJS) -o $@   	
	

ifneq ($(TARGET), x86)
#	$(STRIP) -s $@
endif
	cp -f $@ $(RELEASE_DIR)/

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o


