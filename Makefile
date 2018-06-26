#



DIRS = ./com
DIRS += ./record
DIRS += ./main

all:check_setenv
	SOURCE_DIR=$(PWD)
	RELEASE_DIR=$(PWD)/release
	export SOURCE_DIR RELEASE_DIR
	for i in $(DIRS) ; do make -C $$i $@ || exit $?; done



clean:
	for i in $(DIRS) ; do make -C $$i clean; done
	rm $(RELEASE_DIR)/*.so $(RELEASE_DIR)/nw_charger_x86
	
define do_check_setenv
	SOURCE_DIR=$(PWD)
	RELEASE_DIR=$(PWD)/release
if [ ! $(SOURCE_DIR) ] ; then echo -e $(ERROR_COLOR) error!! ��������δ����!!���Ƚ�����붥��Ŀ¼��Ȼ��ִ�� source ./env.sh !!$(CLOSE_ERROR_COLOR); exit 1; fi
endef
	
# ��黷�������Ƿ�����
check_setenv:
	@$(do_check_setenv)
