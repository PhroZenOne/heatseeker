#ifndef GPIO_INPUT_H
#define GPIO_INPUT_H

#include "GPIO/gpio_port.h"

enum CameraMode { irOnly, mixed, webcamOnly };
enum ScreenBrightness { low, medium, high };

class GPIOInput
{
private:
	GPIOPort _shutdown_pin;
	GPIOPort _standby_pin;
	GPIOPort _brightness_high_pin;
	GPIOPort _brightness_low_pin;
	GPIOPort _mode_overlay_pin;
	GPIOPort _mode_ir_pin;

	bool _shutdown;
	bool _standby;
	bool _brightness_high;
	bool _brightness_low;
	bool _mode_overlay;
	bool _mode_ir;

public:
	GPIOInput();
	void update();
	CameraMode mode();
	ScreenBrightness brightness();
	bool shutdown() { return _shutdown; }
	bool standby() { return _standby; }
};

#endif