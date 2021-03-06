################################################################################
#    ./e2fsprogs/Makefile
#    Create By CaiZhiYong 
#    2011.09.05
################################################################################
PREFIX         ?= $(shell pwd)/rootfs
CROSS_COMPILE  ?= arm-hisiv200-linux-

################################################################################
BUILD_DIR      := $(shell pwd)/tmp
HOST           := $(patsubst %-,%,$(CROSS_COMPILE))
STRIP          := $(CROSS_COMPILE)strip
E2FSPROGS      := e2fsprogs-1.41.11

CFG_HI_LOADER_TYPE_CFG_HI_BUSYBOX_CFG_NAME := \
	$(join $(CFG_HI_LOADER_TYPE),_$(CFG_HI_BUSYBOX_CFG_NAME))

#
#autelan changed
#
TARGETS        := bin/chattr bin/lsattr bin/uuidgen \
		etc/mke2fs.conf \
		lib/e2initrd_helper \
		sbin/badblocks  sbin/dumpe2fs   sbin/e2image  sbin/filefrag \
		sbin/fsck.ext2  sbin/fsck.ext4dev  sbin/mkfs.ext2  sbin/mkfs.ext4dev \
		sbin/tune2fs sbin/blkid sbin/e2freefrag sbin/e2label sbin/findfs \
		sbin/fsck.ext3 sbin/logsave sbin/mkfs.ext3 sbin/mklost+found sbin/uuidd \
		sbin/debugfs sbin/e2fsck sbin/e2undo sbin/fsck sbin/fsck.ext4 sbin/mke2fs \
		sbin/mkfs.ext4 sbin/resize2fs

#ifeq ($(CFG_HI_LOADER_TYPE_CFG_HI_BUSYBOX_CFG_NAME),OS_arm-hisiv200-linux_lowest_emmc.config)
#TARGETS        := sbin/mkfs.ext3 sbin/mke2fs
#else
#TARGETS        := sbin/mkfs.ext2 sbin/mkfs.ext3 sbin/mkfs.ext4 sbin/mke2fs sbin/blkid
#endif

################################################################################

all: $(addprefix $(PREFIX)/,$(TARGETS))

$(PREFIX)/%: $(BUILD_DIR)/%
ifeq ($(CFG_HI_LOADER_TYPE_CFG_HI_BUSYBOX_CFG_NAME),OS_arm-hisiv200-linux_lowest_emmc.config)
	@test -d $(@D) || mkdir -p $(@D)
	cp -f $(BUILD_DIR)/$(E2FSPROGS)/misc/mke2fs.static $(PREFIX)/sbin/mke2fs
	$(CROSS_COMPILE)strip $(PREFIX)/sbin/mke2fs
	-ln -sf $(PREFIX)/sbin/mke2fs $(PREFIX)/sbin/mkfs.ext3
else
	@test -d $(@D) || mkdir -p $(@D)
	@install -v -t $(@D) $<
endif

$(addprefix $(BUILD_DIR)/,$(TARGETS)): \
    $(BUILD_DIR)/$(E2FSPROGS)/tar
ifeq ($(CFG_HI_LOADER_TYPE_CFG_HI_BUSYBOX_CFG_NAME),OS_arm-hisiv200-linux_lowest_emmc.config)
	@cd $(BUILD_DIR)/$(E2FSPROGS) && \
		./configure --host=$(HOST) --prefix=$(BUILD_DIR) --disable-tls && \
		make -j 16 && \
		make  -C misc/ mke2fs.static &&\
	cd -
else
	@cd $(BUILD_DIR)/$(E2FSPROGS) && \
		./configure --host=$(HOST) --prefix=$(BUILD_DIR) --disable-tls && \
		make -j 16 && \
		make install && \
	cd -
endif

$(BUILD_DIR)/%/tar:
	test -d $(@D) || mkdir -p $(@D)
	tar -xjf $(notdir $(@:/tar=)).tar.bz2 -C $(BUILD_DIR)
	@touch $@

################################################################################

clean:
	@test ! -d $(BUILD_DIR) || rm -rf $(BUILD_DIR)
	@(for ix in $(TARGETS); do \
		test ! -e $(PREFIX)/$$ix || rm -rf $(PREFIX)/$$ix; \
	done)

menuconfig:
	@echo "not support"

distclean: clean

install:
	@echo "not support"

################################################################################
.PHONY: clean menuconfig distclean install
################################################################################
