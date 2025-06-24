#include <LilyGoLib.h>
#include <LV_Helper.h>
#include "ui.h"
#include "sleep.h"
#include "battery.h"
#include "watchface.h"

SET_LOOP_TASK_STACK_SIZE(16 * 1024);	// Duktape compile may use up default (8K) stack space

void setup()
{
	Serial.begin(115200);
	watch.begin();

	beginLvglHelper();
	esp_event_loop_create_default();

	setupUi();
	setupSleep();
	setupBattery();

	watch.configAccelerometer();
	watch.enableAccelerometer();
}

void loop()
{
	lv_task_handler();
	sleepHandler();
	batteryHandler();
	watchfaceHandler();

	delay(5);
}
