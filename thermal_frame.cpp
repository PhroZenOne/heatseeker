//from https://raw.githubusercontent.com/rzva/ThermalView/master/ThermalView/frame.h

#include "frame.h"
#include <cstring>

using namespace std;

//////////////////////////////////////////////////////////////////////////
/// Default Constructor
//////////////////////////////////////////////////////////////////////////
ThermalFrame::ThermalFrame() {
	m_id = 0;
	m_max_val = 0;
	m_min_val = 0xffff;
	m_avg_val = 0;
}


//////////////////////////////////////////////////////////////////////////
/// Main Constructor
//////////////////////////////////////////////////////////////////////////
ThermalFrame::ThermalFrame(const std::vector<uint16_t> & data) {
	if (data.size() != 32136)
		return;

	// Initialize the dead pixel matrix
	for (size_t x = 0; x < m_bad_pixels.size(); ++x) {
		for (size_t y = 0; y < m_bad_pixels[x].size(); ++y) {
			m_bad_pixels[x][y] = is_pattern_pixel(x, y);
		}
	}

	// Make room for the data
	m_pixels.resize(data.size());

	// Copy it
	memcpy(&m_pixels[0], &data[0], data.size() * 2);

	// Get the ID of the frame
	m_id = static_cast<uint8_t>(m_pixels[10]);

	// Comput min/max values
	computeMinMax();
}


//////////////////////////////////////////////////////////////////////////
/// computeMinMax - Figure out the min, max and average values
//////////////////////////////////////////////////////////////////////////
void ThermalFrame::computeMinMax() {
	m_max_val = 0;
	m_min_val = 0xffff;

	uint32_t total = 0;
	uint16_t total_count = 0;

	size_t i = 0;

	for (size_t y = 0; y < m_height_pixels; ++y) {
		for (size_t x = 0; x < m_with_pixels; ++x, ++i) {
			uint16_t val = m_pixels[i];

			if (!m_bad_pixels[x][y]) {
				m_max_val = std::max(m_max_val, val);
				m_min_val = std::min(m_min_val, val);

				total += val;
				++total_count;
			}
		}
	}

	m_avg_val = total / total_count;
}


//////////////////////////////////////////////////////////////////////////
/// getOffsetCalibration
//////////////////////////////////////////////////////////////////////////
std::vector<int> ThermalFrame::getOffsetCalibration() const {
	std::vector<int> calibration;

	calibration.resize(m_pixels.size());

	for (size_t i = 0; i < calibration.size(); ++i) {
		uint16_t val = m_pixels[i];

		if (val != 0)
			calibration[i] = m_avg_val - val;
		else
			calibration[i] = 0;
	}

	return calibration;
}


//////////////////////////////////////////////////////////////////////////
/// applyOffsetCalibration
//////////////////////////////////////////////////////////////////////////
void ThermalFrame::applyOffsetCalibration(const std::vector<int> & calibration) {
	for (size_t i = 0; i < calibration.size(); ++i)
		m_pixels[i] += calibration[i];
}


//////////////////////////////////////////////////////////////////////////
/// getGainCalibration
//////////////////////////////////////////////////////////////////////////
std::vector<double> ThermalFrame::getGainCalibration() const {
	std::vector<double> calibration;

	calibration.resize(m_pixels.size());

	const uint16_t min_val = 0;

	for (size_t i = 0; i < calibration.size(); ++i) {
		uint16_t val = m_pixels[i];

		if (val != 0 && !is_pattern_pixel(i))
			calibration[i] = (double)(m_avg_val - min_val) / (m_pixels[i] - min_val);
		else
			calibration[i] = 1;
	}

	return calibration;
}


//////////////////////////////////////////////////////////////////////////
/// applyGainCalibration
//////////////////////////////////////////////////////////////////////////
void ThermalFrame::applyGainCalibration(const std::vector<double> & calibration) {
	for (size_t i = 0; i < calibration.size(); ++i)
		m_pixels[i] = static_cast<uint16_t>(m_pixels[i] * calibration[i]);
}


//////////////////////////////////////////////////////////////////////////
/// getZeroPixels - Returns the non-pattern pixels that have a value of 0
//////////////////////////////////////////////////////////////////////////
std::vector<uint16_t> ThermalFrame::getZeroPixels() const {
	std::vector<uint16_t> res;

	for (size_t i = 0; i < m_pixels.size(); ++i) {
		if (!is_pattern_pixel(i) && m_pixels[i] == 0)
			res.push_back(i);
	}

	return res;
}


//////////////////////////////////////////////////////////////////////////
/// addBadPixels - Adds the given pixels to bad pixel matrix
//////////////////////////////////////////////////////////////////////////
void ThermalFrame::addBadPixels(const std::vector<uint16_t> & pixels) {
	for (size_t i = 0; i < pixels.size(); ++i) {
		uint16_t pos = pixels[i];

		m_bad_pixels[pos % m_with_pixels][pos / m_with_pixels] = true;
	}
}


//////////////////////////////////////////////////////////////////////////
/// fixBadPixels - Attempts to fix the pixels in the bad pixel matrix
//////////////////////////////////////////////////////////////////////////
void ThermalFrame::fixBadPixels() {
	size_t i = 0;

	for (size_t y = 0; y < m_height_pixels; ++y) {
		for (size_t x = 0; x < m_with_pixels; ++x, ++i) {
			// If it's not a bad pixel, skip it
			if (!m_bad_pixels[x][y])
				continue;

			uint32_t val = 0;
			uint8_t nr = 0;

			if (y > 0 && !m_bad_pixels[x][y - 1]) {
				val += m_pixels[(y - 1) * m_with_pixels + x];
				++nr;
			}

			if (y < m_height_pixels - 1 && !m_bad_pixels[x][y + 1]) {
				val += m_pixels[(y + 1) * m_with_pixels + x];
				++nr;
			}

			if (x > 0 && !m_bad_pixels[x - 1][y]) {
				val += m_pixels[y * m_with_pixels + (x - 1)];
				++nr;
			}

			if (x < m_with_pixels - 1 && !m_bad_pixels[x + 1][y]) {
				val += m_pixels[y * m_with_pixels + x + 1];
				++nr;
			}

			if (nr) {
				val /= nr;
				m_pixels[i] = val;
			} else
				m_pixels[i] = m_avg_val;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
/// fixPixels - Attempts to fix the given pixels
//////////////////////////////////////////////////////////////////////////
void ThermalFrame::fixPixels(const std::vector<uint16_t> & pixels, bool use_given_pixel) {
	for (size_t i = 0; i < pixels.size(); ++i) {
		uint32_t pixel = pixels[i];

		size_t x = pixel % m_with_pixels;
		size_t y = pixel / m_with_pixels;


		uint32_t val = use_given_pixel ? m_pixels[pixel] * 2 : 0;
		uint8_t nr = use_given_pixel ? 2 : 0;

		if (y > 0 && !m_bad_pixels[x][y - 1]) {
			val += m_pixels[(y - 1) * m_with_pixels + x];
			++nr;
		}

		if (y < m_height_pixels - 1 && !m_bad_pixels[x][y + 1]) {
			val += m_pixels[(y + 1) * m_with_pixels + x];
			++nr;
		}

		if (x > 0 && !m_bad_pixels[x - 1][y]) {
			val += m_pixels[y * m_with_pixels + (x - 1)];
			++nr;
		}

		if (x < m_with_pixels - 1 && !m_bad_pixels[x + 1][y]) {
			val += m_pixels[y * m_with_pixels + x + 1];
			++nr;
		}

		if (nr) {
			val /= nr;
			m_pixels[pixel] = val;
		} else
			m_pixels[pixel] = m_avg_val;
	}
}


//////////////////////////////////////////////////////////////////////////
/// subtract - Subtracts the value of the pixels from the given frame
//////////////////////////////////////////////////////////////////////////
void ThermalFrame::subtract(const ThermalFrame & frame) {
	for (size_t i = 0; i < frame.m_pixels.size(); ++i) {
		if (m_pixels[i] >= frame.m_pixels[i])
			m_pixels[i] -= frame.m_pixels[i];
		else
			m_pixels[i] = 0;
	}
}


//////////////////////////////////////////////////////////////////////////
/// add - Adds the values of the pixels from the given frame
//////////////////////////////////////////////////////////////////////////
void ThermalFrame::add(const ThermalFrame & frame) {
	for (size_t i = 0; i < frame.m_pixels.size(); ++i) {
		if (m_pixels[i] < 0xffff - frame.m_pixels[i])
			m_pixels[i] += frame.m_pixels[i];
		else
			m_pixels[i] = 0xffff;
	}
}


//////////////////////////////////////////////////////////////////////////
/// Utility Stuff
/////////////////////////////////////////////////////////////////////////////
bool is_pattern_pixel(int x, int y) {
	int pattern_start = (10 - y * 4) % 15;

	if (pattern_start < 0)
		pattern_start = 15 + pattern_start;

	return (x >= pattern_start && ((x - pattern_start) % 15) == 0);
}

bool is_pattern_pixel(int pos) {
	return is_pattern_pixel(pos % m_with_pixels, pos / m_with_pixels);
}