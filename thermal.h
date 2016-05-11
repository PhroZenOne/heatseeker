//taken from https://raw.githubusercontent.com/rzva/ThermalView

#include <libusb-1.0/libusb.h>
#include <cstdint>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include "thermal_frame.h"

class SeekThermal {
private:

	const static int m_with_pixels = 206;
	const static int m_height_pixels = 156;

	libusb_device_handle * m_handle;
	bool m_ep_claimed;

public:
	SeekThermal();
	~SeekThermal();

	ThermalFrame* getFrame();

private:

	ThermalFrame * current_frame = NULL;

	std::vector<double>			m_gain_cal;				// Gain calibration data - Frame ID 4
	std::vector<uint16_t>		m_unknown_gain;			// Unknown gain pixels - Frame ID 4
	std::vector<int>			m_offset_cal;			// Offset calibration - Frame ID 1

	bool connect();
	bool initialize();
	void close();

	std::vector<uint16_t> getRawFrame();
	std::vector<uint8_t> ctrlIn(uint8_t req, uint16_t nr_bytes);
};