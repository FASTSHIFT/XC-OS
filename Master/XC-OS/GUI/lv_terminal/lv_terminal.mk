CSRCS += lv_terminal.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_apps/lv_terminal
VPATH += :$(LVGL_DIR)/lv_apps/lv_terminal

CFLAGS += "-I$(LVGL_DIR)/lv_apps/lv_terminal"
