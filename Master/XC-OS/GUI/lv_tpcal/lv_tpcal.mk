CSRCS += lv_tpcal.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_apps/lv_tpcal
VPATH += :$(LVGL_DIR)/lv_apps/lv_tpcal

CFLAGS += "-I$(LVGL_DIR)/lv_apps/lv_tpcal"
