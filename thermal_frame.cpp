//from https://raw.githubusercontent.com/rzva/ThermalView/master/ThermalView/frame.h

#include "thermal_frame.h"
#include <cstring>

using namespace std;

ThermalFrame::ThermalFrame() {
	m_id = 0;
	m_max_val = 0;
	m_min_val = 0xffff;
	m_avg_val = 0;
	valid = false;
}

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

	valid = true;
}

void ThermalFrame::computeMinMax() {
	m_max_val = 0;
	m_min_val = 0xffff;

	uint32_t total = 0;
	uint16_t total_count = 0;

	size_t i = 0;

	for (size_t y = 0; y < height; ++y) {
		for (size_t x = 0; x < width; ++x, ++i) {
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

void ThermalFrame::applyOffsetCalibration(const std::vector<int> & calibration) {
	for (size_t i = 0; i < calibration.size(); ++i)
		m_pixels[i] += calibration[i];
}

bool ThermalFrame::isValid() {
	return valid;
}

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

void ThermalFrame::applyGainCalibration(const std::vector<double> & calibration) {
	for (size_t i = 0; i < calibration.size(); ++i)
		m_pixels[i] = static_cast<uint16_t>(m_pixels[i] * calibration[i]);
}

std::vector<uint16_t> ThermalFrame::getZeroPixels() const {
	std::vector<uint16_t> res;

	for (size_t i = 0; i < m_pixels.size(); ++i) {
		if (!is_pattern_pixel(i) && m_pixels[i] == 0)
			res.push_back(i);
	}

	return res;
}

void ThermalFrame::addBadPixels(const std::vector<uint16_t> & pixels) {
	for (size_t i = 0; i < pixels.size(); ++i) {
		uint16_t pos = pixels[i];

		m_bad_pixels[pos % width][pos / width] = true;
	}
}

void ThermalFrame::fixBadPixels() {
	size_t i = 0;

	for (size_t y = 0; y < height; ++y) {
		for (size_t x = 0; x < width; ++x, ++i) {
			// If it's not a bad pixel, skip it
			if (!m_bad_pixels[x][y])
				continue;

			uint32_t val = 0;
			uint8_t nr = 0;

			if (y > 0 && !m_bad_pixels[x][y - 1]) {
				val += m_pixels[(y - 1) * width + x];
				++nr;
			}

			if (y < height - 1 && !m_bad_pixels[x][y + 1]) {
				val += m_pixels[(y + 1) * width + x];
				++nr;
			}

			if (x > 0 && !m_bad_pixels[x - 1][y]) {
				val += m_pixels[y * width + (x - 1)];
				++nr;
			}

			if (x < width - 1 && !m_bad_pixels[x + 1][y]) {
				val += m_pixels[y * width + x + 1];
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

void ThermalFrame::fixPixels(const std::vector<uint16_t> & pixels, bool use_given_pixel) {
	for (size_t i = 0; i < pixels.size(); ++i) {
		uint32_t pixel = pixels[i];

		size_t x = pixel % width;
		size_t y = pixel / width;


		uint32_t val = use_given_pixel ? m_pixels[pixel] * 2 : 0;
		uint8_t nr = use_given_pixel ? 2 : 0;

		if (y > 0 && !m_bad_pixels[x][y - 1]) {
			val += m_pixels[(y - 1) * width + x];
			++nr;
		}

		if (y < height - 1 && !m_bad_pixels[x][y + 1]) {
			val += m_pixels[(y + 1) * width + x];
			++nr;
		}

		if (x > 0 && !m_bad_pixels[x - 1][y]) {
			val += m_pixels[y * width + (x - 1)];
			++nr;
		}

		if (x < width - 1 && !m_bad_pixels[x + 1][y]) {
			val += m_pixels[y * width + x + 1];
			++nr;
		}

		if (nr) {
			val /= nr;
			m_pixels[pixel] = val;
		} else
			m_pixels[pixel] = m_avg_val;
	}
}

bool ThermalFrame::is_pattern_pixel(int x, int y) const {
	int pattern_start = (10 - y * 4) % 15;

	if (pattern_start < 0)
		pattern_start = 15 + pattern_start;

	return (x >= pattern_start && ((x - pattern_start) % 15) == 0);
}

bool ThermalFrame::is_pattern_pixel(int pos) const {
	return is_pattern_pixel(pos % width, pos / width);
}

void ThermalFrame::copyToImageData() {
	//Copy because I want to do it thread safe in the future and the api should expose a char * to have the same API as the regular camera.
	std::copy(m_pixels.begin(), m_pixels.end(), imageData);
}