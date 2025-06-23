#include <WiFi.h>
#include "../system.h"

#include "esp_ota_ops.h"

static void event_handler(lv_event_t * e)
{
	//Serial.println("In button handler");

	lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);

	if (code != LV_EVENT_CLICKED) {
		return;
	}

	const esp_partition_t *partition = esp_ota_get_next_update_partition(NULL);

	if (partition == nullptr)
	{
		Serial.println("Partition was null");
		return;
	}

    if (esp_ota_set_boot_partition(partition) == ESP_OK) {
		Serial.println("Setting partition worked");
		Serial.flush();
        ESP.restart();
    }
	else
	{
		Serial.println("Setting partition failed");
	}
}

class UakApp : public App {
public:
	void create(lv_obj_t *tile) {

		//static lv_style_t toggleBtnStyle;
		//lv_style_init(&toggleBtnStyle);
		//lv_style_set_bg_color(&toggleBtnStyle, lv_color_white());
		//lv_style_set_bg_opa(&toggleBtnStyle, LV_OPA_20);

		auto btn1  = lv_btn_create(tile);
		lv_obj_add_event_cb(btn1 , event_handler, LV_EVENT_ALL, NULL);
    	lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
		lv_obj_set_width(btn1 , LV_PCT(100));
		//lv_obj_add_style(btn1, &toggleBtnStyle, LV_PART_MAIN);

		auto buttonLabel = lv_label_create(btn1);
		lv_label_set_text(buttonLabel, "UAK");
		lv_obj_center(buttonLabel);
	}
	void destroy() {}
} uakApp;
