MODULE := engines/vileVN

MODULE_OBJS := \
	detection.o \

MODULE_DIRS += \
	engines/vileVN

# This module can be built as a plugin
ifeq ($(ENABLE_VILEVN), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
