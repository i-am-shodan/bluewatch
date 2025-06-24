#include <LilyGoLib.h>
#include "event.h"
#include "../system.h"
#include <lvgl.h>

// Helper function to set ESP32 time
static void set_esp32_time(int hour, int min) {
    struct tm t = {0};
    t.tm_hour = hour;
    t.tm_min = min;
    t.tm_sec = 0;
    time_t now = mktime(&t);
    struct timeval tv = { .tv_sec = now };
    settimeofday(&tv, nullptr);
}

// OK button event handler
static void ok_btn_event_handler(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *dialog = lv_obj_get_parent(btn);

    // Find spinboxes in dialog
    lv_obj_t *hour_sb = lv_obj_get_child(dialog, 2); // 0:label, 1:hour_sb
    lv_obj_t *min_sb = lv_obj_get_child(dialog, 4);  // 2:colon, 3:min_sb

    int hour = lv_spinbox_get_value(hour_sb);
    int min = lv_spinbox_get_value(min_sb);

    set_esp32_time(hour, min);

    // Optionally close dialog
    lv_obj_del(dialog);
}

class TimeApp : public App {
public:
    void create(lv_obj_t *tile) {
        // Create a modal dialog
        lv_obj_t *dialog = lv_obj_create(tile);
        lv_obj_set_size(dialog, 220, 120);
        lv_obj_center(dialog);

        // Title label
        lv_obj_t *label = lv_label_create(dialog);
        lv_label_set_text(label, "Set Time");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 8);

        // Hour spinbox
        lv_obj_t *hour_sb = lv_spinbox_create(dialog);
        lv_spinbox_set_range(hour_sb, 0, 23);
        lv_spinbox_set_digit_format(hour_sb, 2, 0);
        lv_spinbox_set_value(hour_sb, 12); // Default value
        lv_obj_set_width(hour_sb, 50);
        lv_obj_align(hour_sb, LV_ALIGN_LEFT_MID, 20, 0);

        // Colon label
        lv_obj_t *colon = lv_label_create(dialog);
        lv_label_set_text(colon, ":");
        lv_obj_align_to(colon, hour_sb, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

        // Minute spinbox
        lv_obj_t *min_sb = lv_spinbox_create(dialog);
        lv_spinbox_set_range(min_sb, 0, 59);
        lv_spinbox_set_digit_format(min_sb, 2, 0);
        lv_spinbox_set_value(min_sb, 0); // Default value
        lv_obj_set_width(min_sb, 50);
        lv_obj_align_to(min_sb, colon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

        // OK button
        lv_obj_t *ok_btn = lv_btn_create(dialog);
        lv_obj_set_width(ok_btn, 60);
        lv_obj_align(ok_btn, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
        lv_obj_t *ok_label = lv_label_create(ok_btn);
        lv_label_set_text(ok_label, "OK");

        // Attach event handler to OK button
        lv_obj_add_event_cb(ok_btn, ok_btn_event_handler, LV_EVENT_CLICKED, nullptr);
    }
    void destroy() {
    }
} timeApp;
