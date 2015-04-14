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
ifeq ($(CFG_HI_GPS_SUPPORT),y)
image=rgps
endif

################################################################################

all:$(image) install

$(image) : rgps.c
	@$(CC) -static -o $@ $^ -lpthread
	@$(STRIP) $@
	
clean:
	-@rm -f $(image)

install:
	cp rgps $(ROOTBOX_DIR)/usr/sbin
uninstall:

