//from https://raw.githubusercontent.com/rzva/ThermalView/master/ThermalView/frame.h
#pragma once

#include <cstdint>
#include <vector>
#include <array>

class ThermalFrame {
public:
	static const int m_width = 206;
	static const int m_height = 156;

	std::vector<uint16_t> m_pixels;
	std::array<std::array<bool, m_height>, m_width> m_bad_pixels;

	static const int width = 256;
	static const int height = 256;

	char imageData[width * height]; //will contain the final output but padded to power of two.

	uint8_t m_id;

	uint16_t m_max_val;
	uint16_t m_min_val;
	uint16_t m_avg_val;

	bool valid;

	ThermalFrame();
	ThermalFrame(const std::vector<uint16_t> & data);

	void computeMinMax();

	// Offset calibration
	std::vector<int> getOffsetCalibration() const;
	void applyOffsetCalibration(const std::vector<int> & calibration);

	// Gain calibration
	std::vector<double> getGainCalibration() const;
	void applyGainCalibration(const std::vector<double> & calibration);

	std::vector<uint16_t> getZeroPixels() const;

	void addBadPixels(const std::vector<uint16_t> & pixels);
	void fixBadPixels();
	void fixPixels(const std::vector<uint16_t> & pixels, bool use_given_pixel = false);

	bool isValid();

	bool is_pattern_pixel(int x, int y) const;
	bool is_pattern_pixel(int pos) const;

	void copyToImageData();
};