#

include $(SOURCE_DIR)/config_vars.mk



ifneq ($(TARGET), x86)
	EXEC = libcmlcom.so
else
	EXEC = libcmlcom_x86.so
endif

	CFLAGS += -fpic
	OBJS = inifile.o
	OBJS += base_type.o  
	OBJS += config_file.o 
	OBJS += serial.o 
	OBJS += modbus_frame.o 
	OBJS += modbus_constants.o 
	OBJS += modbus_connection.o 
	OBJS += timer.o 
	OBJS += log.o 
	OBJS += mutex_and_condvar.o 
	OBJS += mixed_c.o 
	
all: $(EXEC) 

$(EXEC): $(OBJS)
	$(CC) -shared  $(OBJS) -o $@   	
	

ifneq ($(TARGET), x86)
#	$(STRIP) -s $@
endif
	cp -f $@ $(RELEASE_DIR)/

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o


