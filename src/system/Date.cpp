#include <LilyGoLib.h>
#include "event.h"
#include "../system.h"
#include <lvgl.h>

#define BUTTON_HEIGHT 45

static int yearState = -1;
static int monthState = -1;
static int dayState = -1;

// Helper function to set ESP32 date
static void set_esp32_date() {

    if (yearState < 1970 || monthState < 1 || monthState > 12 || dayState < 1 || dayState > 31) {
        Serial.println("Invalid date provided");
        return;
    }

    struct tm timeinfo;
    // Get the time C library structure
    watch.getDateTime(&timeinfo);

    timezone tz_utc = {0,0};

    struct tm t = {0};
    t.tm_year = yearState - 1900;
    t.tm_mon = monthState - 1;
    t.tm_mday = dayState;
    t.tm_hour = timeinfo.tm_hour;
    t.tm_min = timeinfo.tm_min;
    t.tm_sec = 0;
    time_t now = mktime(&t);
    struct timeval tv = { .tv_sec = now };

    settimeofday(&tv, &tz_utc);
    watch.hwClockWrite();
}

// OK button event handler
static void ok_btn_event_handler(lv_event_t *e) {
    set_esp32_date();
}

class DateApp : public App {
public:
    void create(lv_obj_t *tile) {

        // Get current date
        struct tm hwTimeinfo;
        watch.getDateTime(&hwTimeinfo);
        int current_year = hwTimeinfo.tm_year + 1900;
        int current_month = hwTimeinfo.tm_mon + 1;
        int current_day = hwTimeinfo.tm_mday;

        lv_obj_set_flex_flow(tile, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(tile, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        // Create a modal dialog
        lv_obj_t *dialog = lv_obj_create(tile);
        lv_obj_set_size(dialog, LV_PCT(100), LV_PCT(100));
        lv_obj_center(dialog);

        // Title label
        lv_obj_t *label = lv_label_create(dialog);
        lv_label_set_text(label, "Set Date");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 4); // Less top margin

        // Year spinbox
        lv_obj_t *year_sb = lv_spinbox_create(dialog);
        lv_spinbox_set_range(year_sb, 1970, 2099);
        lv_spinbox_set_digit_format(year_sb, 4, 0);
        lv_spinbox_set_value(year_sb, current_year);
        lv_obj_set_size(year_sb, 80, 40);
        lv_obj_align(year_sb, LV_ALIGN_LEFT_MID, 10, 0);
        lv_obj_set_style_text_font(year_sb, &lv_font_montserrat_24, LV_PART_MAIN);

        // Year up button
        lv_obj_t *year_up = lv_btn_create(dialog);
        lv_obj_set_size(year_up, 60, BUTTON_HEIGHT);
        lv_obj_align_to(year_up, year_sb, LV_ALIGN_OUT_TOP_MID, 0, -2);
        lv_obj_t *year_up_label = lv_label_create(year_up);
        lv_label_set_text(year_up_label, LV_SYMBOL_PLUS);
        lv_obj_add_event_cb(year_up, [](lv_event_t *e){
            lv_obj_t *sb = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_increment(sb);
            yearState = lv_spinbox_get_value(sb);
        }, LV_EVENT_CLICKED, year_sb);

        // Year down button
        lv_obj_t *year_down = lv_btn_create(dialog);
        lv_obj_set_size(year_down, 60, BUTTON_HEIGHT);
        lv_obj_align_to(year_down, year_sb, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
        lv_obj_t *year_down_label = lv_label_create(year_down);
        lv_label_set_text(year_down_label, LV_SYMBOL_MINUS);
        lv_obj_add_event_cb(year_down, [](lv_event_t *e){
            lv_obj_t *sb = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_decrement(sb);
            yearState = lv_spinbox_get_value(sb);
            Serial.printf("Year decremented to %d\n", yearState);
        }, LV_EVENT_CLICKED, year_sb);

        // Month spinbox
        lv_obj_t *month_sb = lv_spinbox_create(dialog);
        lv_spinbox_set_range(month_sb, 1, 12);
        lv_spinbox_set_digit_format(month_sb, 2, 0);
        lv_spinbox_set_value(month_sb, current_month);
        lv_obj_set_size(month_sb, 60, 40);
        lv_obj_align_to(month_sb, year_sb, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        lv_obj_set_style_text_font(month_sb, &lv_font_montserrat_24, LV_PART_MAIN);

        // Month up button
        lv_obj_t *month_up = lv_btn_create(dialog);
        lv_obj_set_size(month_up, 60, BUTTON_HEIGHT);
        lv_obj_align_to(month_up, month_sb, LV_ALIGN_OUT_TOP_MID, 0, -2);
        lv_obj_t *month_up_label = lv_label_create(month_up);
        lv_label_set_text(month_up_label, LV_SYMBOL_PLUS);
        lv_obj_add_event_cb(month_up, [](lv_event_t *e){
            lv_obj_t *sb = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_increment(sb);
            monthState = lv_spinbox_get_value(sb);
        }, LV_EVENT_CLICKED, month_sb);

        // Month down button
        lv_obj_t *month_down = lv_btn_create(dialog);
        lv_obj_set_size(month_down, 60, BUTTON_HEIGHT);
        lv_obj_align_to(month_down, month_sb, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
        lv_obj_t *month_down_label = lv_label_create(month_down);
        lv_label_set_text(month_down_label, LV_SYMBOL_MINUS);
        lv_obj_add_event_cb(month_down, [](lv_event_t *e){
            lv_obj_t *sb = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_decrement(sb);
            monthState = lv_spinbox_get_value(sb);
        }, LV_EVENT_CLICKED, month_sb);

        // Day spinbox
        lv_obj_t *day_sb = lv_spinbox_create(dialog);
        lv_spinbox_set_range(day_sb, 1, 31);
        lv_spinbox_set_digit_format(day_sb, 2, 0);
        lv_spinbox_set_value(day_sb, current_day);
        lv_obj_set_size(day_sb, 60, 40);
        lv_obj_align_to(day_sb, month_sb, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        lv_obj_set_style_text_font(day_sb, &lv_font_montserrat_24, LV_PART_MAIN);

        // Day up button
        lv_obj_t *day_up = lv_btn_create(dialog);
        lv_obj_set_size(day_up, 60, BUTTON_HEIGHT);
        lv_obj_align_to(day_up, day_sb, LV_ALIGN_OUT_TOP_MID, 0, -2);
        lv_obj_t *day_up_label = lv_label_create(day_up);
        lv_label_set_text(day_up_label, LV_SYMBOL_PLUS);
        lv_obj_add_event_cb(day_up, [](lv_event_t *e){
            lv_obj_t *sb = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_increment(sb);
            dayState = lv_spinbox_get_value(sb);
        }, LV_EVENT_CLICKED, day_sb);

        // Day down button
        lv_obj_t *day_down = lv_btn_create(dialog);
        lv_obj_set_size(day_down, 60, BUTTON_HEIGHT);
        lv_obj_align_to(day_down, day_sb, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
        lv_obj_t *day_down_label = lv_label_create(day_down);
        lv_label_set_text(day_down_label, LV_SYMBOL_MINUS);
        lv_obj_add_event_cb(day_down, [](lv_event_t *e){
            lv_obj_t *sb = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_decrement(sb);
            dayState = lv_spinbox_get_value(sb);
        }, LV_EVENT_CLICKED, day_sb);

        // OK button
        lv_obj_t *ok_btn = lv_btn_create(dialog);
        lv_obj_set_size(ok_btn, 100, 35); // Larger button
        lv_obj_align(ok_btn, LV_ALIGN_BOTTOM_RIGHT, -2, -2);
        lv_obj_t *ok_label = lv_label_create(ok_btn);
        lv_label_set_text(ok_label, "Save");

        // Attach event handler to OK button
        lv_obj_add_event_cb(ok_btn, ok_btn_event_handler, LV_EVENT_CLICKED, nullptr);

        Serial.println("Added date UI elements");
    }
    void destroy() {
    }
} dateApp;
