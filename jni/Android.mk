LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -fPIE -std=c11
LOCAL_LDFLAGS += -fPIE -pie
LOCAL_SRC_FILES := recmas.c
LOCAL_MODULE := recmas

include $(BUILD_EXECUTABLE)
