#include <LilyGoLib.h>
#include "event.h"
#include "../system.h"
#include <lvgl.h>

#define BUTTON_HEIGHT 45

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

        lv_obj_set_flex_flow(tile, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(tile, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        // Create a modal dialog
        lv_obj_t *dialog = lv_obj_create(tile);
        lv_obj_set_size(dialog, LV_PCT(100), LV_PCT(100));
        lv_obj_center(dialog);

        // Title label
        lv_obj_t *label = lv_label_create(dialog);
        lv_label_set_text(label, "Set Time");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 4); // Less top margin

        // Hour spinbox
        lv_obj_t *hour_sb = lv_spinbox_create(dialog);
        lv_spinbox_set_range(hour_sb, 0, 23);
        lv_spinbox_set_digit_format(hour_sb, 2, 0);
        lv_spinbox_set_value(hour_sb, 12);
        lv_obj_set_size(hour_sb, 60, 40); // Larger touch area
        lv_obj_align(hour_sb, LV_ALIGN_LEFT_MID, 10, 0);

        // Hour up button
        lv_obj_t *hour_up = lv_btn_create(dialog);
        lv_obj_set_size(hour_up, 60, BUTTON_HEIGHT);
        lv_obj_align_to(hour_up, hour_sb, LV_ALIGN_OUT_TOP_MID, 0, -2);
        lv_obj_t *hour_up_label = lv_label_create(hour_up);
        lv_label_set_text(hour_up_label, LV_SYMBOL_PLUS);
        lv_obj_add_event_cb(hour_up, [](lv_event_t *e){
            lv_obj_t *sb = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_increment(sb);
        }, LV_EVENT_CLICKED, hour_sb);

        // Hour down button
        lv_obj_t *hour_down = lv_btn_create(dialog);
        lv_obj_set_size(hour_down, 60, BUTTON_HEIGHT);
        lv_obj_align_to(hour_down, hour_sb, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
        lv_obj_t *hour_down_label = lv_label_create(hour_down);
        lv_label_set_text(hour_down_label, LV_SYMBOL_MINUS);
        lv_obj_add_event_cb(hour_down, [](lv_event_t *e){
            lv_obj_t *sb = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_decrement(sb);
        }, LV_EVENT_CLICKED, hour_sb);

        // Colon label
        lv_obj_t *colon = lv_label_create(dialog);
        lv_label_set_text(colon, ":");
        lv_obj_align_to(colon, hour_sb, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

        // Minute spinbox
        lv_obj_t *min_sb = lv_spinbox_create(dialog);
        lv_spinbox_set_range(min_sb, 0, 59);
        lv_spinbox_set_digit_format(min_sb, 2, 0);
        lv_spinbox_set_value(min_sb, 0);
        lv_obj_set_size(min_sb, 60, 40);
        lv_obj_align_to(min_sb, colon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

        // Minute up button
        lv_obj_t *min_up = lv_btn_create(dialog);
        lv_obj_set_size(min_up, 60, BUTTON_HEIGHT);
        lv_obj_align_to(min_up, min_sb, LV_ALIGN_OUT_TOP_MID, 0, -2);
        lv_obj_t *min_up_label = lv_label_create(min_up);
        lv_label_set_text(min_up_label, LV_SYMBOL_PLUS);
        lv_obj_add_event_cb(min_up, [](lv_event_t *e){
            lv_obj_t *sb = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_increment(sb);
        }, LV_EVENT_CLICKED, min_sb);

        // Minute down button
        lv_obj_t *min_down = lv_btn_create(dialog);
        lv_obj_set_size(min_down, 60, BUTTON_HEIGHT);
        lv_obj_align_to(min_down, min_sb, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
        lv_obj_t *min_down_label = lv_label_create(min_down);
        lv_label_set_text(min_down_label, LV_SYMBOL_MINUS);
        lv_obj_add_event_cb(min_down, [](lv_event_t *e){
            lv_obj_t *sb = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_decrement(sb);
        }, LV_EVENT_CLICKED, min_sb);

        // OK button
        lv_obj_t *ok_btn = lv_btn_create(dialog);
        lv_obj_set_size(ok_btn, 100, 40); // Larger button
        //lv_obj_align(ok_btn, LV_ALIGN_BOTTOM_RIGHT, -10, -6);
        lv_obj_align(ok_btn, LV_ALIGN_BOTTOM_RIGHT, -2, -2);
        lv_obj_t *ok_label = lv_label_create(ok_btn);
        lv_label_set_text(ok_label, "OK");

        // Attach event handler to OK button
        lv_obj_add_event_cb(ok_btn, ok_btn_event_handler, LV_EVENT_CLICKED, nullptr);
    }
    void destroy() {
    }
} timeApp;
