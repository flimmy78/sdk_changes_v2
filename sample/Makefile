#===============================================================================
# export variable
#===============================================================================
ifeq ($(CFG_HI_EXPORT_FLAG),)
SDK_DIR := $(CURDIR)/..
include $(SDK_DIR)/base.mak
endif

include $(SAMPLE_DIR)/base.mak

#===============================================================================
# local variable
#===============================================================================
objects:= tuner demux dvbplay esplay ao tsplay es_ts_switch ipplay psi mosaic pip log memdev userproc \
	  iframe_dec pvr dolby mce resampler\
          capture seperateoutput hdmi_tsplay fb higo tde jpeg\
          gpio wdg pmoc ir e2prom i2c \
          otp uti1201 flash uart network fs cipher factory_detect  snapshot 3dtv pwm edid fullband 

ifeq ($(CFG_HI_KEYLED_SUPPORT),y)
objects += keyled
endif

ifeq ($(CFG_HI_CIPLUS_SUPPORT),y)
objects += ci
endif

ifeq ($(CFG_HI_HDMI_SUPPORT_HDCP),y)
objects += hdcpkey
endif

ifeq ($(CFG_HI_AENC_SUPPORT),y)
objects += aenc
endif

ifeq ($(CFG_HI_SND_CAST_SUPPORT),y)
objects += audiocast
endif

ifeq ($(CFG_HI_AI_SUPPORT),y)
objects += ai
endif

ifeq ($(CFG_HI_VOIP_SUPPORT),y)
objects += voip
endif

ifeq ($(CFG_HI_LOADER_SUPPORT),y)
objects += loader
endif

ifeq ($(CFG_HI_WIFI_SUPPORT),y)
objects += wifi
endif

ifeq ($(CFG_HI_3G_SUPPORT),y)
objects += 3g
endif

#AUTELAN start
ifeq ($(CFG_HI_GPS_SUPPORT),y)
objects += gps
endif

ifeq ($(CFG_HI_LED_SUPPORT),y)
objects += led
endif

ifeq ($(CFG_HI_ACC_SUPPORT),y)
objects += acc
endif
#AUTELAN end

ifeq ($(CFG_HI_GPU),y)
objects += gpu 
endif

ifeq ($(CFG_HI_PES_SUPPORT),y)
objects += pesplay
endif

ifeq ($(CFG_HI_SCI_SUPPORT),y)
objects += sci
endif

ifeq ($(CFG_HI_AENC_SUPPORT),y)
ifeq ($(CFG_HI_VENC_SUPPORT),y)
objects += transcode
objects += avcast
endif
endif

ifeq ($(CFG_HI_VI_SUPPORT),y)
objects += vi
ifeq ($(CFG_HI_VENC_SUPPORT),y)
ifneq ($(CFG_HI_VO_SINGLE_VIDEO_SUPPORT),y)
objects += vi_venc
ifeq ($(CFG_HI_VP_SUPPORT),y)
objects += vp
endif
endif
endif
ifeq ($(CFG_HI_AI_SUPPORT),y)
objects += hdmi_rx
endif
endif

ifeq ($(CFG_HI_VENC_SUPPORT),y)
objects += venc
endif

ifeq ($(CFG_HI_PLAYER_SUPPORT),y)
objects += localplay
endif

ifeq ($(CFG_HI_CAPTION_CC_SUPPORT),y)
objects += cc
endif

ifeq ($(CFG_HI_CAPTION_TTX_SUPPORT),y)
objects += teletext
endif

ifeq ($(CFG_HI_CAPTION_SUBT_SUPPORT),y)
objects += subtitle
endif

ifeq ($(CFG_HI_ADVCA_SUPPORT),y)
ifneq ($(CFG_HI_ADVCA_TYPE), NAGRA)
objects += advca
else
objects += nagra
endif
endif

ifeq ($(CFG_HI_PQ_SUPPORT),y)
objects += pq
endif

ifeq ($(CFG_HI_DLNA_SUPPORT),y)
objects += hidlna
endif

ifeq ($(CFG_HI_TRANSCODER_SUPPORT),y)
objects += hitranscoder
endif

objects_clean:=${addsuffix _clean,${objects}}
objects_install:=${addsuffix _install,${objects}}
objects_uninstall:=${addsuffix _uninstall,${objects}}

ifeq (${obj},)
depends:=${objects}
else
depends:=${obj}
endif
depends_clean:=$(addsuffix _clean,${depends})
depends_install:=$(addsuffix _install,${depends})
depends_uninstall:=$(addsuffix _uninstall,${depends})

#===============================================================================
# rules
#===============================================================================
.PHONY: prepare unprepare all clean install uninstall ${objects} ${objects_clean} ${objects_install} ${objects_uninstall} test

all: ${depends}

clean:${depends_clean}

install:${depends_install}

uninstall:${depends_uninstall}

${objects}:
	make -C $@

${objects_clean}:
	make -C ${patsubst %_clean,%,$@} clean

${objects_install}:
	make -C ${patsubst %_install,%,$@} install

${objects_uninstall}:
	make -C ${patsubst %_uninstall,%,$@} uninstall
