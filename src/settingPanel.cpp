#include <LilyGoLib.h>
#include "event.h"
#include "sleep.h"
#include "Modal.h"
#include <tuple>
#include <utility>

using std::tuple;

lv_obj_t *settingPanel;
static lv_coord_t touchY0;
static lv_coord_t panelY0;
static bool isDragging;

static const lv_coord_t stickOut = 10;

void setupSettingPanel() {
	settingPanel = lv_obj_create(lv_scr_act());
	lv_obj_set_size(settingPanel, LV_PCT(100), LV_PCT(100));
	lv_obj_set_style_pad_all(settingPanel, 20, LV_PART_MAIN);
	lv_obj_set_style_border_side(settingPanel, LV_BORDER_SIDE_NONE, LV_PART_MAIN);
	lv_obj_set_style_radius(settingPanel, 0, LV_PART_MAIN);
	lv_obj_set_style_bg_color(settingPanel, lv_color_hex(0x333333), LV_PART_MAIN);
	lv_obj_set_style_bg_opa(settingPanel, LV_OPA_70, LV_PART_MAIN);
	lv_obj_set_pos(settingPanel, 0, -240 + stickOut);
	lv_obj_set_style_opa(settingPanel, LV_OPA_0, LV_PART_MAIN);

	lv_obj_add_event_cb(settingPanel, [](lv_event_t *e) {
		auto indev = lv_event_get_indev(e);
		lv_point_t point;
		lv_indev_get_point(indev, &point);
		isDragging = true;
		touchY0 = point.y;
		panelY0 = lv_obj_get_y(settingPanel);
		lv_obj_set_style_opa(settingPanel, LV_OPA_100, LV_PART_MAIN);
	}, LV_EVENT_PRESSED, nullptr);

	lv_obj_add_event_cb(settingPanel, [](lv_event_t *e) {
		if (!isDragging) return;
		auto indev = lv_event_get_indev(e);
		lv_point_t point;
		lv_indev_get_point(indev, &point);
		auto y = panelY0 + (point.y - touchY0);
		if (y > 0) y = 0;
		lv_obj_set_y(settingPanel, y);
	}, LV_EVENT_PRESSING, nullptr);

	lv_obj_add_event_cb(settingPanel, [](lv_event_t *e) {
		isDragging = false;
		lv_anim_t a;
		lv_anim_init(&a);
		lv_anim_set_var(&a, settingPanel);
		lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
		lv_anim_set_time(&a, 200);
		auto y = lv_obj_get_y(settingPanel);
		lv_anim_set_values(&a, y, y > -120 ? 0 : -240 + stickOut);
		lv_anim_set_ready_cb(&a, [](lv_anim_t *a) {
			if (a->end_value < 0)
				lv_obj_set_style_opa(settingPanel, LV_OPA_0, LV_PART_MAIN);
		});
		lv_anim_start(&a);
	}, LV_EVENT_RELEASED, nullptr);

	lv_obj_set_style_text_color(settingPanel, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_flex_flow(settingPanel, LV_FLEX_FLOW_COLUMN);

	static lv_style_t toggleBtnStyle;
	lv_style_init(&toggleBtnStyle);
	lv_style_set_bg_color(&toggleBtnStyle, lv_color_white());
	lv_style_set_bg_opa(&toggleBtnStyle, LV_OPA_20);
	static lv_style_t toggleBtnCheckedStyle;
	lv_style_init(&toggleBtnCheckedStyle);
	auto colorPrimary = lv_theme_get_color_primary(nullptr);
	lv_style_set_bg_color(&toggleBtnCheckedStyle, colorPrimary);
	lv_style_set_bg_opa(&toggleBtnCheckedStyle, LV_OPA_100);

	auto batteryBtn = lv_btn_create(settingPanel);
	lv_obj_set_width(batteryBtn, LV_PCT(100));
	lv_obj_add_style(batteryBtn, &toggleBtnStyle, LV_PART_MAIN);
	lv_obj_set_flex_flow(batteryBtn, LV_FLEX_FLOW_ROW);

	auto batteryLabel = lv_label_create(batteryBtn);
	lv_label_set_text_fmt(batteryLabel, "%s %d%%", LV_SYMBOL_BATTERY_FULL, watch.getBatteryPercent());

	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_UPDATE, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		auto batteryLabel = (lv_obj_t *)event_handler_arg;
		lv_label_set_text_fmt(batteryLabel, "%s %d%%", LV_SYMBOL_BATTERY_FULL, watch.getBatteryPercent());
	}, batteryLabel);

	auto chargeLabel = lv_label_create(batteryBtn);
	lv_label_set_text(chargeLabel, LV_SYMBOL_CHARGE);
	(watch.isCharging() ? lv_obj_clear_flag : lv_obj_add_flag)(chargeLabel, LV_OBJ_FLAG_HIDDEN);

	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_VBUS_INSERT, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		auto chargeLabel = (lv_obj_t *)event_handler_arg;
		(watch.isCharging() ? lv_obj_clear_flag : lv_obj_add_flag)(chargeLabel, LV_OBJ_FLAG_HIDDEN);
	}, chargeLabel);
	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_VBUS_REMOVE, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		auto chargeLabel = (lv_obj_t *)event_handler_arg;
		(watch.isCharging() ? lv_obj_clear_flag : lv_obj_add_flag)(chargeLabel, LV_OBJ_FLAG_HIDDEN);
	}, chargeLabel);
	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_CHARGE_DONE, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		auto chargeLabel = (lv_obj_t *)event_handler_arg;
		(watch.isCharging() ? lv_obj_clear_flag : lv_obj_add_flag)(chargeLabel, LV_OBJ_FLAG_HIDDEN);
	}, chargeLabel);
	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_BATTERY_CHARGE_START, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		auto chargeLabel = (lv_obj_t *)event_handler_arg;
		(watch.isCharging() ? lv_obj_clear_flag : lv_obj_add_flag)(chargeLabel, LV_OBJ_FLAG_HIDDEN);
	}, chargeLabel);

	auto disableSleepBtn = lv_btn_create(settingPanel);
	lv_obj_set_width(disableSleepBtn, LV_PCT(100));
	lv_obj_add_style(disableSleepBtn, &toggleBtnStyle, LV_PART_MAIN);
	lv_obj_add_style(disableSleepBtn, &toggleBtnCheckedStyle, LV_PART_MAIN | LV_STATE_CHECKED);
	auto disableSleepLabel = lv_label_create(disableSleepBtn);
	lv_label_set_text(disableSleepLabel, "Disable Sleep");

	esp_event_handler_register(BLUEWATCH_EVENTS, BLUEWATCH_EVENT_DISABLE_SLEEP_CHANGE, [](void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
		auto disableSleepBtn = (lv_obj_t *)event_handler_arg;
		(disableSleep ? lv_obj_add_state : lv_obj_clear_state)(disableSleepBtn, LV_STATE_CHECKED);
	}, disableSleepBtn);

	lv_obj_add_event_cb(disableSleepBtn, [](lv_event_t *e) {
		setDisableSleep(!disableSleep);
	}, LV_EVENT_CLICKED, nullptr);
}
