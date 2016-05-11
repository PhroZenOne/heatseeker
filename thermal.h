//taken from https://raw.githubusercontent.com/rzva/ThermalView

#include <libusb.h>
#include <cstdint>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

class SeekThermal {
private:

	const int m_with_pixels = 206;
	const int m_height_pixels = 156;

	libusb_device_handle * m_handle;
	bool m_ep_claimed;

	typedef std::unique_ptr<ColorProfile> PColorProfile;
	typedef std::unique_ptr<GradientProfile> PGradientProfile;


public:
	SeekThermal();
	~SeekThermal();

	bool connect();
	void close();

	ThermalFrame* getFrame();

private:

	std::vector<double>			m_gain_cal;				// Gain calibration data - Frame ID 4
	std::vector<uint16_t>		m_unknown_gain;			// Unknown gain pixels - Frame ID 4
	std::vector<int>			m_offset_cal;			// Offset calibration - Frame ID 1

	std::vector<uint16_t> getRawFrame();
	bool initialize();

	std::vector<uint8_t> ctrlIn(uint8_t req, uint16_t nr_bytes);

	void startThread(bool stream);
	void stopThread();
	void workerThread();
};