#include <LilyGoLib.h>
#include "event.h"
#include "../system.h"
#include <lvgl.h>

#define BUTTON_HEIGHT 45

char buf[64];

static int hourState = -1;
static int minuteState = -1;

// Helper function to set ESP32 time
static void set_esp32_time() {

    Serial.printf("in set_esp32_time %d %d\n", hourState, minuteState);

    if (hourState < 0 || hourState > 23 || minuteState < 0 || minuteState > 59) {
        Serial.println("Invalid time provided");
        return;
    }

    struct tm timeinfo;
    // Get the time C library structure
    watch.getDateTime(&timeinfo);
    auto written = strftime(buf, 64, "Before %A, %d. %B %Y %I:%M:%S %p\n", &timeinfo);
    Serial.println(buf);

    timezone tz_utc = {0,0};

    struct tm t = {0};
    t.tm_year = 2025 - 1900;
    t.tm_mon = 5; // January (0-based)
    t.tm_mday = 24; // 1st of the month
    t.tm_hour = hourState;
    t.tm_min = minuteState;
    t.tm_sec = 0;
    time_t now = mktime(&t);
    struct timeval tv = { .tv_sec = now };

    Serial.printf("Setting time to %02d:%02d\n", hourState, minuteState);

    settimeofday(&tv, &tz_utc);

    time_t now2;
    struct tm  info;
    time(&now2);
    localtime_r(&now2, &info);

    written = strftime(buf, 64, "localtime_r %A, %d. %B %Y %I:%M:%S %p\n", &info);
    Serial.println(buf);



    Serial.println("Time set, writing to hardware clock");

    watch.hwClockWrite();

    // Get the time C library structure
    memset(&timeinfo, 0, sizeof(timeinfo));
    watch.getDateTime(&timeinfo);
    written = strftime(buf, 64, "After %A, %d. %B %Y %I:%M:%S %p\n", &timeinfo);
    Serial.println(buf);

    Serial.println("Hardware clock updated");
}

// OK button event handler
static void ok_btn_event_handler(lv_event_t *e) {

    Serial.println("in ok_btn_event_handler");
    
    Serial.printf("hourState:minuteState %d:%d\n", hourState, minuteState);
    set_esp32_time();

    // Send LV_EVENT_SCROLL_END to the tile (parent of the dialog)
    //lv_obj_t *tile = lv_obj_get_parent(dialog);
    //lv_event_send(tile, LV_EVENT_SCROLL_END, nullptr);
}

class TimeApp : public App {
public:
    void create(lv_obj_t *tile) {

        // Get current time
        struct tm hwTimeinfo;
	    watch.getDateTime(&hwTimeinfo);
        int current_hour = hwTimeinfo.tm_hour;
        int current_min = hwTimeinfo.tm_min;

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
        lv_spinbox_set_value(hour_sb, current_hour); // Set to current hour
        lv_obj_set_size(hour_sb, 60, 40); // Larger touch area
        lv_obj_align(hour_sb, LV_ALIGN_LEFT_MID, 10, 0);
        lv_obj_set_style_text_font(hour_sb, &lv_font_montserrat_24, LV_PART_MAIN); // Larger text

        // Hour up button
        lv_obj_t *hour_up = lv_btn_create(dialog);
        lv_obj_set_size(hour_up, 60, BUTTON_HEIGHT);
        lv_obj_align_to(hour_up, hour_sb, LV_ALIGN_OUT_TOP_MID, 0, -2);
        lv_obj_t *hour_up_label = lv_label_create(hour_up);
        lv_label_set_text(hour_up_label, LV_SYMBOL_PLUS);
        lv_obj_add_event_cb(hour_up, [](lv_event_t *e){
            lv_obj_t *sb = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_increment(sb);
            hourState = lv_spinbox_get_value(sb);
            Serial.printf("Hour incremented to %d\n", hourState);
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
            hourState = lv_spinbox_get_value(sb);
            Serial.printf("Hour decremented to %d\n", hourState);
        }, LV_EVENT_CLICKED, hour_sb);

        // Colon label
        lv_obj_t *colon = lv_label_create(dialog);
        lv_label_set_text(colon, ":");
        lv_obj_align_to(colon, hour_sb, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

        // Minute spinbox
        lv_obj_t *min_sb = lv_spinbox_create(dialog);
        lv_spinbox_set_range(min_sb, 0, 59);
        lv_spinbox_set_digit_format(min_sb, 2, 0);
        lv_spinbox_set_value(min_sb, current_min); // Set to current minute
        lv_obj_set_size(min_sb, 60, 40);
        lv_obj_align_to(min_sb, colon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        lv_obj_set_style_text_font(min_sb, &lv_font_montserrat_24, LV_PART_MAIN); // Larger text

        // Minute up button
        lv_obj_t *min_up = lv_btn_create(dialog);
        lv_obj_set_size(min_up, 60, BUTTON_HEIGHT);
        lv_obj_align_to(min_up, min_sb, LV_ALIGN_OUT_TOP_MID, 0, -2);
        lv_obj_t *min_up_label = lv_label_create(min_up);
        lv_label_set_text(min_up_label, LV_SYMBOL_PLUS);
        lv_obj_add_event_cb(min_up, [](lv_event_t *e){
            lv_obj_t *sb = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_increment(sb);
            minuteState = lv_spinbox_get_value(sb);
            Serial.printf("Minute incremented to %d\n", minuteState);
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
            minuteState = lv_spinbox_get_value(sb);
            Serial.printf("Minute decremented to %d\n", minuteState);
        }, LV_EVENT_CLICKED, min_sb);

        // OK button
        lv_obj_t *ok_btn = lv_btn_create(dialog);
        lv_obj_set_size(ok_btn, 100, 35); // Larger button
        lv_obj_align(ok_btn, LV_ALIGN_BOTTOM_RIGHT, -2, -2);
        lv_obj_t *ok_label = lv_label_create(ok_btn);
        lv_label_set_text(ok_label, "Save");

        // Attach event handler to OK button
        lv_obj_add_event_cb(ok_btn, ok_btn_event_handler, LV_EVENT_CLICKED, nullptr);

        Serial.println("Added time UI elements");
    }
    void destroy() {
    }
} timeApp;
