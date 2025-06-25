#include <LilyGoLib.h>
#include <WiFi.h>
#include "event.h"

lv_obj_t *watchface, *timeLabel, *dateLabel, *statusBar, *batteryLabel, *chargeLabel, *wifiLabel;

unsigned long timeLastUpdateTime;

void updateTime();
void updateBatteryStatus();
void updateChargeStatus();

void setupWatchface()
{
	extern lv_obj_t *watchfaceTile;
	watchface = lv_obj_create(watchfaceTile);
	lv_obj_set_style_bg_color(watchface, lv_color_black(), LV_PART_MAIN); // Set background to black
	lv_obj_set_style_text_color(watchface, lv_color_white(), LV_PART_MAIN); // Set text to white
	lv_obj_set_style_border_side(watchface, LV_BORDER_SIDE_NONE, LV_PART_MAIN);
	lv_obj_set_style_radius(watchface, 0, LV_PART_MAIN);
	lv_obj_set_size(watchface, LV_PCT(100), LV_PCT(100));
	lv_obj_set_style_pad_all(watchface, 0, 0);

	timeLabel = lv_label_create(watchface);
	lv_obj_center(timeLabel);
	lv_obj_set_style_text_font(timeLabel, &lv_font_montserrat_48, LV_PART_MAIN);
	lv_obj_set_style_text_color(timeLabel, lv_color_white(), LV_PART_MAIN); // Set text to white

	dateLabel = lv_label_create(watchface);
	lv_obj_align(dateLabel, LV_ALIGN_CENTER, 0, 48);
	lv_obj_set_style_text_font(dateLabel, &lv_font_montserrat_24, LV_PART_MAIN);
	lv_obj_set_style_text_color(dateLabel, lv_color_white(), LV_PART_MAIN); // Set text to white

	statusBar = lv_obj_create(watchface);
	lv_obj_set_width(statusBar, LV_PCT(100));
	lv_obj_set_height(statusBar, LV_SIZE_CONTENT);
	lv_obj_set_style_pad_all(statusBar, 4, LV_PART_MAIN);
	lv_obj_set_flex_flow(statusBar, LV_FLEX_FLOW_ROW_REVERSE);
	lv_obj_set_flex_align(statusBar, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_border_width(statusBar, 0, LV_PART_MAIN);
	lv_obj_set_style_text_font(statusBar, &lv_font_montserrat_24, LV_PART_MAIN);
	lv_obj_set_style_bg_color(statusBar, lv_color_black(), LV_PART_MAIN); // Set background to black
	lv_obj_set_style_text_color(statusBar, lv_color_white(), LV_PART_MAIN); // Set text to white
	chargeLabel = lv_label_create(statusBar);
	lv_label_set_text(chargeLabel, LV_SYMBOL_CHARGE);
	batteryLabel = lv_label_create(statusBar);

	auto now = millis();
	updateTime();
	timeLastUpdateTime = now;
	updateBatteryStatus();
	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_UPDATE, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		updateBatteryStatus();
	}, nullptr);
	updateChargeStatus();
	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_VBUS_INSERT, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		updateChargeStatus();
	}, nullptr);
	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_VBUS_REMOVE, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		updateChargeStatus();
	}, nullptr);
	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_CHARGE_DONE, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		updateChargeStatus();
	}, nullptr);
	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_CHARGE_START, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		updateChargeStatus();
	}, nullptr);
}

void watchfaceHandler()
{
	auto now = millis();

	if (now - timeLastUpdateTime >= 1000) {
		updateTime();
		timeLastUpdateTime = now;
	}
}

void updateTime()
{
	struct tm hwTimeinfo;
	watch.getDateTime(&hwTimeinfo);

	char s[16];
	strftime(s, 16, "%X", &hwTimeinfo);
	lv_label_set_text(timeLabel, s);
	strftime(s, 16, "%a %x", &hwTimeinfo);
	lv_label_set_text(dateLabel, s);
}

void updateBatteryStatus()
{
	auto batteryPercentage = watch.getBatteryPercent();
	lv_label_set_text(batteryLabel,
		batteryPercentage > 90 ? LV_SYMBOL_BATTERY_FULL :
		batteryPercentage > 60 ? LV_SYMBOL_BATTERY_3 :
		batteryPercentage > 40 ? LV_SYMBOL_BATTERY_2 :
		batteryPercentage > 10 ? LV_SYMBOL_BATTERY_1 :
		LV_SYMBOL_BATTERY_EMPTY
	);
}

void updateChargeStatus()
{
	(watch.isCharging() ? lv_obj_clear_flag : lv_obj_add_flag)(chargeLabel, LV_OBJ_FLAG_HIDDEN);
}