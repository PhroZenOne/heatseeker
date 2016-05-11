//from https://raw.githubusercontent.com/rzva/ThermalView/master/ThermalView/frame.h
#pragma once

#include <cstdint>
#include <vector>
#include <array>

class ThermalFrame {
public:
	const int m_with_pixels = 206;
	const int m_height_pixels = 156;

	std::vector<uint16_t>						m_pixels;
	std::array<std::array<bool, m_height_pixels>, m_with_pixels>	m_bad_pixels;

	uint8_t m_id;

	uint16_t m_max_val;
	uint16_t m_min_val;
	uint16_t m_avg_val;

	ThermalFrame();
	ThermalFrame(const std::vector<uint16_t> & data);

	void computeMinMax();

	// Offset calibration
	std::vector<int> getOffsetCalibration() const;
	void applyOffsetCalibration(const std::vector<int> & calibration);

	// Gain calibration
	std::vector<double> getGainCalibration() const;
	void applyGainCalibration(const std::vector<double> & calibration);

	// Get the zero value pixels, that are not pattern pixels
	std::vector<uint16_t> getZeroPixels() const;

	// Add to the bad pixels matrix
	void addBadPixels(const std::vector<uint16_t> & pixels);

	// Fix bad pixels
	void fixBadPixels();

	// Fix the given pixels
	void fixPixels(const std::vector<uint16_t> & pixels, bool use_given_pixel = false);

	void subtract(const ThermalFrame & frame);
	void add(const ThermalFrame & frame);
};

bool is_pattern_pixel(int x, int y);
bool is_pattern_pixel(int pos);