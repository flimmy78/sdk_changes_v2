################################################################################
#LIB = -Wl,--start-group -lpthread -lm   -Wl,--end-group
################################################################################
ifeq ($(CFG_HI_EXPORT_FLAG),)
SDK_DIR       := $(shell pwd)/../..
LIB_DIR       := $(SDK_DIR)/pub/lib
SHARED_LIB_DIR:= $(LIB_DIR)/share
STATIC_LIB_DIR:= $(LIB_DIR)/static
INCLUDE_DIR   := $(SDK_DIR)/pub/include
ROOTFS_DIR    := $(SDK_DIR)/pub/rootfs
include $(SDK_DIR)/cfg.mak
include $(SDK_DIR)/base.mak
endif

################################################################################
image=
ifeq ($(CFG_HI_3G_SUPPORT),y)
image=3g_sample
endif

################################################################################

all:$(image) install

3g_sample: 3g_sample.c 
	@$(CC) -static -o $@ $^ -I$(INCLUDE_DIR) -L$(STATIC_LIB_DIR) -lhi_3g -lhi_common $(LIB) -lpthread
clean:
	-@rm -f $(image)

install:
	cp 3g_sample $(ROOTBOX_DIR)/usr/sbin
uninstall:

