################################################################################
#    ./udev/Makefile
#    Create By CaiZhiYong
#    2011.06.19
#
######################export val################################################
#  CFG_HI_KERNEL_VERSION
#  CFG_HI_KERNEL_CFG
#  CROSS_COMPILE
#######################inside val###############################################
#  PREFIX
#  CROSS_COMPILE
#  OUTPUT_DIR
#  KERNEL
#  OUTPUT_DIR
#  KERNEL_CONFIG

ifeq ($(CFG_HI_EXPORT_FLAG),)
SDK_DIR := $(shell pwd)/../..
include $(SDK_DIR)/base.mak
endif

PREFIX           ?= $(shell pwd)
CROSS_COMPILE            := \
	$(if $(CROSS_COMPILE),$(CROSS_COMPILE),arm-hisiv200-linux-)
OUTPUT_DIR       ?= \
	$(if $(PUB_DIR),$(PUB_DIR),$(shell pwd)/../../pub)
KERNEL           ?= \
	$(if $(CFG_HI_KERNEL_VERSION),$(CFG_HI_KERNEL_VERSION),linux-3.4.y)
KERNEL_CONFIG    ?= \
	$(if $(CFG_HI_KERNEL_CFG),$(CFG_HI_KERNEL_CFG),hi3716cv200es_defconfig)

CFG_MSP  := $(CFG_HI_MSP_BUILDIN)
CFG_FS_BUILDIN := $(CFG_HI_FS_BUILDIN)

SATA_SUPPORT := y

ifeq ($(CFG_HI_CHIP_TYPE),hi3716mv400)
SATA_SUPPORT := n
endif

ifeq ($(CFG_HI_CHIP_TYPE),hi3718mv100)
SATA_SUPPORT := n
endif

ifeq ($(CFG_HI_CHIP_TYPE),hi3719mv100)
SATA_SUPPORT := n
endif

################################################################################

BUILD_DIR        := $(shell pwd)/$(KERNEL)

################################################################################

KERNEL_PATCH_DIR ?= $(shell pwd)/$(KERNEL).patch

ifeq ($(CFG_HI_KERNEL_VERSION),linux-3.4.y)
##################### Advca config#############################################
ifdef CFG_HI_ADVCA_SUPPORT
KERNEL_ADVCA_RUNTIME_DIR ?= $(shell pwd)/$(KERNEL).hica/runtime

ifeq ($(CFG_HI_LOADER_APPLOADER),y)
KERNEL_ADVCA_CONFIG_DIR ?= $(shell pwd)/$(KERNEL).hica/loader_configs
else
KERNEL_ADVCA_CONFIG_DIR ?= $(shell pwd)/$(KERNEL).hica/system_configs
endif
endif


AFLAGS_KERNEL = -DHI_ADVCA_SUPPORT
CFG_ADVCA := $(CFG_HI_ADVCA_SUPPORT)
endif

ifdef CFG_HI_ADVCA_FUNCTION
ifeq ($(CFG_HI_LOADER_APPLOADER),y)
INITRAMFS_SOURCE = $(shell pwd)/initramfs.cpio.gz
endif
else
INITRAMFS_SOURCE = $(ROOTBOX_DIR)
endif
###############################################################################

all: $(PREFIX)/hi_kernel.bin

$(PREFIX)/hi_kernel.bin: $(BUILD_DIR)/arch/arm/boot/uImage
	cp -fv $(BUILD_DIR)/arch/arm/boot/uImage $@

$(BUILD_DIR)/arch/arm/boot/uImage: $(BUILD_DIR)/.config
	make -C $(BUILD_DIR) ARCH=arm \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		CONFIG_MSP=$(CFG_MSP) \
		CONFIG_ADVCA=$(CFG_ADVCA) \
		CFLAGS_KERNEL=$(CFLAGS_KERNEL) AFLAGS_KERNEL=$(AFLAGS_KERNEL) -j 20 uImage
	make -C $(BUILD_DIR) ARCH=arm \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		CONFIG_MSP=$(CFG_MSP) \
		CONFIG_ADVCA=$(CFG_ADVCA) \
		CFLAGS_KERNEL=$(CFLAGS_KERNEL) AFLAGS_KERNEL=$(AFLAGS_KERNEL) -j 20 modules

menuconfig: $(BUILD_DIR)/.config
	make -C $(BUILD_DIR) ARCH=arm \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		CONFIG_MSP=$(CFG_MSP) \
		CFLAGS_KERNEL=$(CFLAGS_KERNEL) AFLAGS_KERNEL=$(AFLAGS_KERNEL) -j 20 menuconfig

prepare: patch_kernel
	make -C $(BUILD_DIR) ARCH=arm \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		CFLAGS_KERNEL=$(CFLAGS_KERNEL) AFLAGS_KERNEL=$(AFLAGS_KERNEL) $(KERNEL_CONFIG)

$(BUILD_DIR)/.config:
	make -C $(BUILD_DIR) ARCH=arm \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		CFLAGS_KERNEL=$(CFLAGS_KERNEL) AFLAGS_KERNEL=$(AFLAGS_KERNEL) $(KERNEL_CONFIG)

patch_kernel: $(BUILD_DIR)/tar
	@test ! -d $(KERNEL_PATCH_DIR) || cp -rf $(KERNEL_PATCH_DIR)/* $(BUILD_DIR)/
ifeq ($(CFG_HI_KERNEL_VERSION),linux-3.4.y)	
##################### Patch for Advca##########################
	-@cp -f $(BUILD_DIR)/arch/arm/Kconfig.org          $(BUILD_DIR)/arch/arm/Kconfig
	-@cp -f $(BUILD_DIR)/arch/arm/Kconfig.debug.org    $(BUILD_DIR)/arch/arm/Kconfig.debug
	-@cp -f $(BUILD_DIR)/drivers/base/Kconfig.org      $(BUILD_DIR)/drivers/base/Kconfig
	-@cp -f $(BUILD_DIR)/init/Kconfig.org             $(BUILD_DIR)/init/Kconfig
ifdef CFG_HI_ADVCA_SUPPORT
	-@test ! -d $(KERNEL_ADVCA_RUNTIME_DIR) || cp -rf $(KERNEL_ADVCA_RUNTIME_DIR)/* $(BUILD_DIR)/
ifdef CFG_HI_ADVCA_FUNCTION
	-@test ! -d $(KERNEL_ADVCA_CONFIG_DIR) || cp -rf $(KERNEL_ADVCA_CONFIG_DIR)/* $(BUILD_DIR)/
ifneq ($(CFG_HI_ADVCA_FUNCTION),DEBUG)
	-@cp -f $(BUILD_DIR)/arch/arm/Kconfig.ca          $(BUILD_DIR)/arch/arm/Kconfig
	-@cp -f $(BUILD_DIR)/arch/arm/Kconfig.debug.ca    $(BUILD_DIR)/arch/arm/Kconfig.debug
	-@cp -f $(BUILD_DIR)/drivers/base/Kconfig.ca      $(BUILD_DIR)/drivers/base/Kconfig
	-@cp -f $(BUILD_DIR)/init/Kconfig.ca              $(BUILD_DIR)/init/Kconfig
endif
else
	-@test ! -d $(KERNEL_ADVCA_CONFIG_DIR) || cp -rf $(KERNEL_ADVCA_CONFIG_DIR)/arch/arm/configs/hi3716cv200-common-ca_defconfig $(BUILD_DIR)/arch/arm/configs/
endif
endif
endif
	#AUTELAN autelan
	if [ ! -f $(BUILD_DIR)/autelan.config ]; then \
		${hisitopdir}/autelan_scripts/autelan.private kernel; \
	fi


$(BUILD_DIR)/tar:
	@test -f $@ || test ! -f $(notdir $(@:/tar=)).tar.bz2 || tar -xjf $(notdir $(@:/tar=)).tar.bz2
	@test ! -f $(BUILD_DIR)/.config || rm -f $(BUILD_DIR)/.config
	@touch $@

unprepare:
	@test ! -d $(BUILD_DIR) || test ! -f $(shell pwd)/$(KERNEL).tar.bz2 \
        || test ! -d $(KERNEL_PATCH_DIR) || rm -rf $(BUILD_DIR)

install: all headers_install
	@cp -fv $(PREFIX)/hi_kernel.bin $(OUTPUT_DIR)/image
	@test -d $(OUTPUT_DIR)/kmod/usb || mkdir -p $(OUTPUT_DIR)/kmod/usb
	-@cp -fv $(BUILD_DIR)/drivers/usb/host/*.ko $(OUTPUT_DIR)/kmod/usb
ifeq ($(SATA_SUPPORT),y)
	@test -d $(OUTPUT_DIR)/kmod/sata || mkdir -p $(OUTPUT_DIR)/kmod/sata
	-@cp -fv $(BUILD_DIR)/drivers/ata/*.ko $(OUTPUT_DIR)/kmod/sata
endif

ifdef CFG_HI_ADVCA_FUNCTION
ifeq ($(CFG_HI_LOADER_APPLOADER),y)
$(INITRAMFS_SOURCE): force
	@rm -f $(INITRAMFS_SOURCE)
	@cd $(ROOTBOX_DIR) && \
            find . | cpio -o -H newc | gzip > $(INITRAMFS_SOURCE) && \
        cd -

force: ;

endif
endif

fs_buildin:$(INITRAMFS_SOURCE)
	make -C $(BUILD_DIR) ARCH=arm \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		CONFIG_MSP=$(CFG_MSP) \
		CONFIG_ADVCA=$(CFG_ADVCA) \
		CFLAGS_KERNEL=$(CFLAGS_KERNEL) \
		AFLAGS_KERNEL=$(AFLAGS_KERNEL) \
		CONFIG_CC_OPTIMIZE_FOR_SIZE=$(CONFIG_CC_OPTIMIZE_FOR_SIZE) \
		CONFIG_INITRAMFS_SOURCE=$(INITRAMFS_SOURCE) \
		-j 20 uImage
	@cp -fv $(BUILD_DIR)/arch/arm/boot/uImage $(PREFIX)/hi_kernel.bin
ifeq ($(CFG_HI_LOADER_APPLOADER),y)
	@cp -fv $(PREFIX)/hi_kernel.bin $(OUTPUT_DIR)/image/apploader.bin
	@rm -rf $(OUTPUT_DIR)/image/hi_kernel.bin
else
	@cp -fv $(PREFIX)/hi_kernel.bin $(OUTPUT_DIR)/image/
endif

uninstall:
	-@rm -rf $(OUTPUT_DIR)/kmod/usb/*.ko
	-@rm -rf $(OUTPUT_DIR)/kmod/sata/*.ko
	-@rm -rf $(OUTPUT_DIR)/image/hi_kernel.bin
	-@rm -rf $(OUTPUT_DIR)/image/apploader.bin

################################################################################
headers_install:
	cp -fv $(BUILD_DIR)/include/linux/vinput.h $(SDK_DIR)/source/msp/drv/include/hi_drv_vinput.h

################################################################################

distclean: clean
	@test ! -d $(BUILD_DIR) || make -C $(BUILD_DIR) ARCH=arm \
		CROSS_COMPILE=$(CROSS_COMPILE) distclean

clean:
	@test ! -d $(BUILD_DIR) || make -C $(BUILD_DIR) ARCH=arm \
		CROSS_COMPILE=$(CROSS_COMPILE) clean
	@test ! -f $(PREFIX)/hi_kernel.bin || rm -rf  $(PREFIX)/hi_kernel.bin
ifdef CFG_HI_ADVCA_FUNCTION
ifeq ($(CFG_HI_LOADER_APPLOADER),y)
	@test ! -f $(INITRAMFS_SOURCE) || rm -f $(INITRAMFS_SOURCE)
endif
endif

################################################################################
.PHONY: clean distclean prepare
################################################################################
