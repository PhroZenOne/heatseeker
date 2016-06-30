#include "input.h"

GPIOInput::GPIOInput() :
	_shutdown_pin("17"), _shutdown(false),
	_standby_pin("27"), _standby(false),
	_brightness_high_pin("22"), _brightness_high(false),
	_brightness_low_pin("10"), _brightness_low(false),
	_mode_overlay_pin("9"), _mode_overlay(false),
	_mode_ir_pin("11"), _mode_ir(false)
{
	_shutdown_pin.setdir_gpio("in");
	_standby_pin.setdir_gpio("in");
	_brightness_high_pin.setdir_gpio("in");
	_brightness_low_pin.setdir_gpio("in");
	_mode_overlay_pin.setdir_gpio("in");
	_mode_ir_pin.setdir_gpio("in");
}

void GPIOInput::update()
{
	// Invert input as we are using pull-up resistors and active low inputs.
	string buffer;
	_shutdown_pin.getval_gpio(buffer);
	_shutdown = buffer.compare("0") == 0;
	_standby_pin.getval_gpio(buffer);
	_standby = buffer.compare("0") == 0;
	_brightness_high_pin.getval_gpio(buffer);
	_brightness_high = buffer.compare("0") == 0;
	_brightness_low_pin.getval_gpio(buffer);
	_brightness_low = buffer.compare("0") == 0;
	_mode_overlay_pin.getval_gpio(buffer);
	_mode_overlay = buffer.compare("0") == 0;
	_mode_ir_pin.getval_gpio(buffer);
	_mode_ir = buffer.compare("0") == 0;
}

CameraMode GPIOInput::mode()
{
	if (_mode_ir)
		return CameraMode::irOnly;
	if (_mode_overlay)
		return CameraMode::mixedBlend;
	return CameraMode::mixedGrayscale;
}

ScreenBrightness GPIOInput::brightness()
{
	if (_brightness_high)
		return ScreenBrightness::high;
	if (_brightness_low)
		return ScreenBrightness::low;
	return ScreenBrightness::medium;
}