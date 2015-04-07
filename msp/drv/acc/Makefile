MOD_NAME=acc_mod

obj-y    := $(MOD_NAME).o
$(MOD_NAME)-objs := check_acc.o

EXTRA_CFLAGS += -I$(COMMON_UNF_INCLUDE)                 \
                -I$(COMMON_API_INCLUDE)                 \
                -I$(COMMON_DRV_INCLUDE)                 \
                -I$(MSP_UNF_INCLUDE)                    \
                -I$(MSP_API_INCLUDE)                    \
                -I$(MSP_DRV_INCLUDE) 

.PHONY: install
install: all
	$(AT)cp -f $(CURDIR)/$(MOD_NAME).ko $(MODULE_DIR)/

all: clean
	$(AT)make -C $(LINUX_DIR) ARCH=arm CROSS_COMPILE=$(CFG_HI_TOOLCHAINS_NAME)- M=$(CURDIR) modules

clean:
	$(RM) -r *.o *.ko *.mod.c .*.cmd *.order *.symvers .tmp_versions
