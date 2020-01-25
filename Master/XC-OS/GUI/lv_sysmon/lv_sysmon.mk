CSRCS += lv_sysmon.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_apps/lv_sysmon
VPATH += :$(LVGL_DIR)/lv_apps/lv_sysmon

CFLAGS += "-I$(LVGL_DIR)/lv_apps/lv_sysmon"
