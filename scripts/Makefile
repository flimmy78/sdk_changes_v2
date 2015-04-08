#===============================================================================
# export variables
#===============================================================================
ifeq ($(CFG_HI_EXPORT_FLAG),)
SDK_DIR := $(CURDIR)/..
include $(SDK_DIR)/base.mak
endif

#***********************************************************************************
#   varibale
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++

ifeq ($(CFG_HI_ADVCA_SUPPORT),y)
    ADVCA_MOD_NAME := hi_advca.ko
else
    CFG_HI_ADVCA_SUPPORT :=
endif

JPGE_SUPPORT := y
SATA_SUPPORT := y

ifeq ($(CFG_HI_CHIP_TYPE),hi3716mv400)
SATA_SUPPORT := n
endif

ifeq ($(CFG_HI_CHIP_TYPE),hi3718mv100)
JPGE_SUPPORT := n
SATA_SUPPORT := n
endif

ifeq ($(CFG_HI_CHIP_TYPE),hi3719mv100)
JPGE_SUPPORT := n
SATA_SUPPORT := n
endif

#***********************************************************************************
#   rule
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++
.PHONY: load load_install load_uninstall
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++
load_install: load $(if $(CFG_HI_ADVCA_SUPPORT),,unload)

load:
	$(AT)rm -rf load
ifneq ($(CFG_HI_MSP_BUILDIN),y)
    ifeq ($(CFG_HI_MCE_SUPPORT)_$(CFG_HI_KEYLED_CT1642_KERNEL_SUPPORT),_)
		$(AT)echo "insmod hi_media.ko"          >> load
    endif

	$(AT)echo "insmod acc_mod.ko"                >> load
	$(AT)echo "insmod hi_mmz.ko"                >> load
	$(AT)echo "insmod hi_common.ko"             >> load

	$(AT)echo "insmod hi_gpio.ko"               >> load

    ifeq ($(CFG_HI_GPIOI2C_SUPPORT),y)
		$(AT)echo "insmod hi_gpio_i2c.ko i2cmode=2" >> load
    endif

	$(AT)echo "insmod hi_i2c.ko"                >> load

    ifeq ($(CFG_HI_GPU),y)
		$(AT)echo "insmod mali.ko"              >> load
    endif

	$(AT)echo "insmod hi_tde.ko"                >> load
	$(AT)echo "insmod hi_otp.ko"                >> load
	$(AT)echo "insmod hi_demux.ko"              >> load
	$(AT)echo "insmod hi_adec.ko"               >> load
	$(AT)echo "insmod hi_vfmw.ko"               >> load

    ifeq ($(CFG_HI_VDEC_SVDEC_SUPPORT),y)
		$(AT)echo "insmod hi_svdec.ko"          >> load
    endif

	$(AT)echo "insmod hi_pdm.ko"                >> load
	$(AT)echo "insmod hi_pq.ko"                 >> load
	$(AT)echo "insmod hi_vpss.ko"               >> load
	$(AT)echo "insmod hi_vdec.ko"               >> load
	$(AT)echo "insmod hi_cipher.ko"             >> load
	$(AT)echo "insmod hi_hdmi.ko"               >> load
	$(AT)echo "insmod hi_adsp.ko"               >> load
	$(AT)echo "insmod hi_aiao.ko"               >> load
#	$(AT)echo "insmod hi_dmac.ko"               >> load
	$(AT)echo "insmod hi_vou.ko"                >> load
	$(AT)echo "insmod hi_sync.ko"               >> load
	$(AT)echo "insmod hi_pvr.ko"                >> load
	$(AT)echo "insmod hi_avplay.ko"             >> load
	$(AT)echo "insmod hi_mce.ko"                >> load
	$(AT)echo "insmod hi_tuner.ko"              >> load
	$(AT)echo "insmod hi_pwm.ko"                >> load

    ifndef CFG_HI_ADVCA_FUNCTION
		$(AT)echo "# About hi_fb:"                  >> load
		$(AT)echo "#  For 1920*1080, triple buffer, ARGB8888 format, use:" >> load
		$(AT)echo "#  insmod hi_fb.ko video=\"hi_fb:vram0_size:16200, vram4_size:2430\""    >> load
		$(AT)echo "#  For 1280*720, use:"    >> load
		$(AT)echo "#  insmod hi_fb.ko video=\"hi_fb:vram0_size:7200, vram2_size:2430\""     >> load
		$(AT)echo "#  Insmod hi_fb.ko with no args will use defaule value,The defaule value is set by cfg.mak."  >> load
		$(AT)echo "#  vram0 is memory size for HD and vram4 is for SD0. The size is calculated as below:" >> load
		$(AT)echo "#  vram0_size = (1920*1080*4*2/1024)KB; vram4_size = (720*576*2*2/1024)KB" >> load
    endif
	$(AT)echo "insmod hi_fb.ko"                 >> load

    ifneq ($(CFG_HI_IR_TYPE_LIRC),y)
		$(AT)echo "insmod hi_ir.ko"             >> load
    endif

	$(AT)echo "insmod hi_wdg.ko"                >> load
	$(AT)echo "insmod hi_jpeg.ko"               >> load

    ifeq ($(JPGE_SUPPORT),y)
		$(AT)echo "insmod hi_jpge.ko"           >> load
    endif

	$(AT)echo "insmod hi_png.ko"                >> load

    ifeq ($(CFG_HI_ADVCA_SUPPORT),y)
		$(AT)echo "insmod $(ADVCA_MOD_NAME)"    >> load
    endif

    ifeq ($(CFG_HI_VENC_SUPPORT),y)
		$(AT)echo "insmod hi_venc.ko"           >> load
    endif

    ifeq ($(CFG_HI_AENC_SUPPORT),y)
		$(AT)echo "insmod hi_aenc.ko"           >> load
    endif

    ifeq ($(CFG_HI_KEYLED_SUPPORT),y)
		$(AT)echo "insmod hi_keyled.ko"         >> load
    endif

    ifeq ($(CFG_HI_SCI_SUPPORT),y)
		$(AT)echo "insmod hi_sci.ko"            >> load
    endif

    ifeq ($(CFG_HI_VI_SUPPORT),y)
		$(AT)echo "insmod hi_vi.ko"             >> load
    endif

	$(AT)echo "insmod hi_pmoc.ko"               >> load
endif   # End of ifneq ($(CFG_HI_MSP_BUILDIN),y)

ifeq ($(CFG_HI_IR_TYPE_LIRC),y)
	$(AT)echo "insmod lirc_dev.ko"              >> load
	$(AT)echo "insmod lirc_serial.ko"           >> load
	$(AT)echo "mkdir -p /var/run/lirc"          >> load
	$(AT)echo "mkdir -p /var/log"               >> load
	$(AT)echo "cat > /var/log/lircd << EOF"     >> load
	$(AT)echo "EOF"                             >> load
	$(AT)echo "ln -fs /dev/lirc0 /dev/lirc"     >> load
	$(AT)echo "lircd -d /dev/lirc0 -o /dev/lircd -r__up" >> load
endif

ifeq ($(CFG_HI_VOIP_SUPPORT),y)
	$(AT)echo "insmod hi_ndpt.ko"           >> load
endif

ifeq ($(CFG_HI_ADAC_TVL320AIC31_SUPPORT),y)
	$(AT)echo "insmod hi_tlv320.ko"         >> load
endif

ifeq ($(CFG_HI_VOIP_SUPPORT),y)
	$(AT)echo "insmod voip_hme.ko"          >> load
	$(AT)echo "insmod hi_aud_adp.ko"        >> load
endif

ifeq ($(CFG_HI_CIPLUS_SUPPORT),y)
	$(AT)echo "insmod hi_ci.ko"             >> load
endif

ifeq ($(CFG_HI_CI_DEV_CIMAXPLUS),y)
	$(AT)echo "insmod cimax\+usb_driver.ko" >> load
endif

	$(AT)echo "insmod ./usb/ehci-hcd.ko"        >> load
	$(AT)echo "insmod ./usb/ohci-hcd.ko"        >> load

ifeq ($(SATA_SUPPORT),y)
	$(AT)echo "insmod ./sata/libahci.ko"        >> load
	$(AT)echo "insmod ./sata/ahci_platform.ko"  >> load
endif

ifeq ($(CFG_HI_NTFS_MODULE_SUPPORT),y)
	$(AT)echo "insmod ufsd.ko"              >> load
endif

ifndef CFG_HI_ADVCA_FUNCTION
	$(AT)echo "echo 6 > /proc/sys/vm/dirty_ratio"           >> load # see source/msp/component/hifat/how to use.txt for detail
	$(AT)echo "echo 1 > /proc/sys/vm/overcommit_memory"     >> load
	$(AT)echo "echo 8192 > /proc/sys/vm/min_free_kbytes"    >> load
endif

ifeq ($(CFG_HI_DVFS_CPU_SUPPORT),y)
#	$(AT)echo "echo hotplug > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"    >> load
endif
	$(AT)(chmod +x load; mv ./load $(PUB_DIR)/kmod/load)

unload:
	$(AT)rm -rf unload
ifeq ($(CFG_HI_NTFS_MODULE_SUPPORT),y)
	$(AT)echo "rmmod ufsd"                  >> unload
endif

ifeq ($(SATA_SUPPORT),y)
	$(AT)echo "rmmod ahci_platform"         >> unload
	$(AT)echo "rmmod libahci"               >> unload
endif

	$(AT)echo "rmmod ohci-hcd"              >> unload
	$(AT)echo "rmmod ehci-hcd"              >> unload

ifeq ($(CFG_HI_CI_DEV_CIMAXPLUS),y)
	$(AT)echo "rmmod cimax\+usb_driver"     >> unload
endif

ifeq ($(CFG_HI_CIPLUS_SUPPORT),y)
	$(AT)echo "rmmod hi_ci"                 >> unload
endif

ifeq ($(CFG_HI_VOIP_SUPPORT),y)
	$(AT)echo "rmmod hi_aud_adp"            >> unload
	$(AT)echo "rmmod voip_hme"              >> unload
endif

ifeq ($(CFG_HI_ADAC_TVL320AIC31_SUPPORT),y)
	$(AT)echo "rmmod hi_tlv320"             >> unload
endif

ifeq ($(CFG_HI_VOIP_SUPPORT),y)
	$(AT)echo "rmmod hi_ndpt"               >> unload
endif

ifeq ($(CFG_HI_IR_TYPE_LIRC),y)
	$(AT)echo "rmmod lirc_serial"           >> unload
	$(AT)echo "rmmod lirc_dev"              >> unload
endif

ifneq ($(CFG_HI_MSP_BUILDIN),y)
    ifeq ($(CFG_HI_DVFS_CPU_SUPPORT),y)
		$(AT)echo "echo userspace > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"    >> unload
    endif

	$(AT)echo "rmmod hi_pmoc"               >> unload

    ifeq ($(CFG_HI_VI_SUPPORT),y)
		$(AT)echo "rmmod hi_vi"                 >> unload
    endif

    ifeq ($(CFG_HI_SCI_SUPPORT),y)
		$(AT)echo "rmmod hi_sci"                >> unload
    endif

    ifeq ($(CFG_HI_KEYLED_SUPPORT),y)
		$(AT)echo "rmmod hi_keyled"             >> unload
    endif

    ifeq ($(CFG_HI_AENC_SUPPORT),y)
		$(AT)echo "rmmod hi_aenc"               >> unload
    endif

    ifeq ($(CFG_HI_VENC_SUPPORT),y)
		$(AT)echo "rmmod hi_venc"               >> unload
    endif

    ifeq ($(CFG_HI_ADVCA_SUPPORT),y)
		$(AT)echo "rmmod $(ADVCA_MOD_NAME)"     >> unload
    endif

	$(AT)echo "rmmod hi_png"                    >> unload

    ifeq ($(JPGE_SUPPORT),y)
		$(AT)echo "rmmod hi_jpge"               >> unload
    endif

	$(AT)echo "rmmod hi_jpeg"                   >> unload
	$(AT)echo "rmmod hi_wdg"                    >> unload

    ifneq ($(CFG_HI_IR_TYPE_LIRC),y)
		$(AT)echo "rmmod hi_ir"                 >> unload
    endif

	$(AT)echo "rmmod hi_fb"                     >> unload
	$(AT)echo "rmmod hi_pwm"                    >> unload
	$(AT)echo "rmmod hi_tuner"                  >> unload
	$(AT)echo "rmmod hi_mce"                    >> unload
	$(AT)echo "rmmod hi_avplay"                 >> unload
	$(AT)echo "rmmod hi_pvr"                    >> unload
	$(AT)echo "rmmod hi_sync"                   >> unload
	$(AT)echo "rmmod hi_vou"                    >> unload
#	$(AT)echo "rmmod hi_dmac"                   >> unload
	$(AT)echo "rmmod hi_aiao"                   >> unload
	$(AT)echo "rmmod hi_adsp"                   >> unload
	$(AT)echo "rmmod hi_hdmi"                   >> unload

	$(AT)echo "rmmod hi_cipher"                 >> unload
	$(AT)echo "rmmod hi_vdec"                   >> unload
	$(AT)echo "rmmod hi_vpss"                   >> unload

	$(AT)echo "rmmod hi_pq"                     >> unload
	$(AT)echo "rmmod hi_pdm"                    >> unload

    ifeq ($(CFG_HI_VDEC_SVDEC_SUPPORT),y)
		$(AT)echo "rmmod hi_svdec"              >> unload
    endif

	$(AT)echo "rmmod hi_vfmw"                   >> unload
	$(AT)echo "rmmod hi_adec"                   >> unload
	$(AT)echo "rmmod hi_demux"                  >> unload
	$(AT)echo "rmmod hi_otp"                    >> unload
	$(AT)echo "rmmod hi_tde"                    >> unload

    ifeq ($(CFG_HI_GPU),y)
		$(AT)echo "rmmod mali"                  >> unload
    endif

	$(AT)echo "rmmod hi_i2c"                    >> unload

    ifeq ($(CFG_HI_GPIOI2C_SUPPORT),y)
		$(AT)echo "rmmod hi_gpio_i2c"           >> unload
    endif

	$(AT)echo "rmmod hi_gpio"                   >> unload

	$(AT)echo "rmmod hi_common"                 >> unload
	$(AT)echo "rmmod hi_mmz"                    >> unload
	$(AT)echo "rmmod acc_mod"                   >> unload

    ifeq ($(CFG_HI_MCE_SUPPORT)_$(CFG_HI_KEYLED_CT1642_KERNEL_SUPPORT),_)
		$(AT)echo "rmmod hi_media"              >> unload
    endif
endif   # End of ifneq ($(CFG_HI_MSP_BUILDIN),y)
	$(AT)(chmod +x unload; mv ./unload $(PUB_DIR)/kmod/unload)

load_uninstall:
	$(AT)rm -rf $(PUB_DIR)/kmod/load
	$(AT)rm -rf $(PUB_DIR)/kmod/unload
