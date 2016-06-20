// taken from https://raw.githubusercontent.com/rzva/ThermalView

#include "thermal.h"
#include <string>
#include <iostream>

#define USB_TIMEOUT 1000

#define CTRL_OUT(req, ...) \
{\
	uint8_t data[] = {__VA_ARGS__};\
\
	if (libusb_control_transfer(m_handle, 0x41 | LIBUSB_ENDPOINT_OUT, req, 0, 0, reinterpret_cast<unsigned char *>(data), sizeof(data), USB_TIMEOUT) != sizeof(data))\
		throw usb_failure();\
}

using namespace std;

//Just a local throwable
struct usb_failure {
};

SeekThermal::SeekThermal() {
	std::cout << " Booting up ir camera" << std::endl;
	m_handle = 0;
	m_ep_claimed = false;
	if (!connect()) {
		std::cout << " could not connect " << std::endl;
		return;
	};

	startCapture();
}

SeekThermal::~SeekThermal() {
	stopCapture();
	closeUsb();
}

std::vector<uint8_t> SeekThermal::ctrlIn(uint8_t req, uint16_t nr_bytes) {
	std::vector<uint8_t> data;
	data.resize(nr_bytes);

	if (libusb_control_transfer(m_handle, 0xC1 | LIBUSB_ENDPOINT_IN, req, 0, 0, reinterpret_cast<unsigned char *>(&data[0]), nr_bytes, USB_TIMEOUT) != nr_bytes)
		throw usb_failure();

	return data;
}

bool SeekThermal::connect() {
	// If it's already connected, try disconnect
	if (m_handle)
		closeUsb();

	// Let's find the device and connect to it
	libusb_device ** list;

	size_t len = libusb_get_device_list(0, &list);

	if (len > 0) {
		libusb_device_handle * handle;

		for (size_t i = 0; i < len; ++i) {
			libusb_device_descriptor descr;

			if (libusb_get_device_descriptor(list[i], &descr) == 0) {	// 0 ok, LIBUSB_ERROR - not ok
				if (libusb_open(list[i], &handle) == 0) {
					if (descr.idVendor == 0x289D && descr.idProduct == 0x0010) {
						m_handle = handle;
						break;
					}

					libusb_close(handle);
				}
			}
		}

		libusb_free_device_list(list, 1);
	}


	if (m_handle) {
		if (initialize()) {
			return true;
		}
	}

	return false;
}

bool SeekThermal::initialize() {
	if (m_handle) {
		try {
			// Claim endpoint
			if (!libusb_claim_interface(m_handle, 0) == 0)
				throw usb_failure();

			m_ep_claimed = true;


			// Now let's go through the initialization routine
			CTRL_OUT(0x56, 1);
			CTRL_OUT(0x56, 0, 0);

			vector<uint8_t> data1 = ctrlIn(0x4e, 4);
			vector<uint8_t> data2 = ctrlIn(0x36, 12);

			CTRL_OUT(0x56, 0x06, 0x00, 0x08, 0x00, 0x00, 0x00);
			vector<uint8_t> data3 = ctrlIn(0x58, 0x0c);

			CTRL_OUT(0x56, 0x01, 0x00, 0x00, 0x06, 0x00, 0x00);
			vector<uint8_t> data4 = ctrlIn(0x58, 0x02);

			CTRL_OUT(0x56, 0x01, 0x00, 0x01, 0x06, 0x00, 0x00);
			vector<uint8_t> data5 = ctrlIn(0x58, 0x02);

			CTRL_OUT(0x56, 0x20, 0x00, 0x30, 0x00, 0x00, 0x00);
			vector<uint8_t> data6 = ctrlIn(0x58, 0x40);

			CTRL_OUT(0x56, 0x20, 0x00, 0x50, 0x00, 0x00, 0x00);
			vector<uint8_t> data7 = ctrlIn(0x58, 0x40);

			CTRL_OUT(0x56, 0x0C, 0x00, 0x70, 0x00, 0x00, 0x00);
			vector<uint8_t> data8 = ctrlIn(0x58, 0x18);

			CTRL_OUT(0x3e, 0x08, 0x00);
			vector<uint8_t> data9 = ctrlIn(0x3d, 0x02);

			CTRL_OUT(0x3c, 0x01, 0x00);
			vector<uint8_t> data10 = ctrlIn(0x3d, 2);

			return true;
		} catch (usb_failure &) {
			closeUsb();
		}
	}

	return false;
}

void SeekThermal::closeUsb() {
	if (m_handle) {
		try {
			CTRL_OUT(0x3c, 0, 0);
			CTRL_OUT(0x3c, 0, 0);
			CTRL_OUT(0x3c, 0, 0);
		} catch (usb_failure &) {
		}

		if (m_ep_claimed) {
			libusb_release_interface(m_handle, 0);
			m_ep_claimed = false;
		}

		libusb_close(m_handle);

		m_handle = 0;
	}
}

void SeekThermal::startCapture() {
	alive = true;
	cameraThread = std::thread(&SeekThermal::captureFrame, this);
}

void SeekThermal::stopCapture() {
	alive = false;
	cameraThread.join();
}

void SeekThermal::captureFrame() {
	while (alive) {

		ThermalFrame frame = ThermalFrame(getRawData());
		std::cout << "getting frame" << std::endl;
		// See if it's a key frame
		if (frame.m_id != 3) {
			switch (frame.m_id) {
			// Gain calibration
			case 4:
				m_gain_cal = frame.getGainCalibration();
				m_unknown_gain = frame.getZeroPixels();
				break;

			// Offset calibration (every time the shutter is heard)
			case 1:
				frame.applyGainCalibration(m_gain_cal);
				frame.computeMinMax();

				m_offset_cal = frame.getOffsetCalibration();
				break;
			}
		} else {
			// It's a regular frame, so let's process it
			frame.addBadPixels(frame.getZeroPixels());
			frame.addBadPixels(m_unknown_gain);

			frame.applyGainCalibration(m_gain_cal);
			frame.applyOffsetCalibration(m_offset_cal);

			frame.computeMinMax();

			frame.fixBadPixels();

			frame.copyToImageData();

			frameBuffer.write(frame);
		}
	}
}



std::vector<uint16_t> SeekThermal::getRawData() {

	std::vector<uint8_t> data;
	std::vector<uint16_t> frame;

	data.resize(0x7ec0 * 2); // 64896

	try {
		CTRL_OUT(0x53, 0xc0, 0x7e, 0, 0);

		// Do the bulk in transfer
		int total = 0;
		int transferred;

		while (true) {
			int res = libusb_bulk_transfer(m_handle, 0x81, &data[0] + total, data.size() - total, &transferred, USB_TIMEOUT);

			if (res == 0) {
				total += transferred;

				if (total >= static_cast<int>(data.size()))
					break;
			} else
				throw usb_failure();
		}

		if (total != static_cast<int>(data.size()))
			throw usb_failure();


		const int m_with_pixels = 206;
		const int m_height_pixels = 156;


		// Let's interpret the data
		frame.resize(m_with_pixels * m_height_pixels);

		for (size_t y = 0; y < m_height_pixels; ++y) {
			for (size_t x = 0; x < m_with_pixels; ++x) {
				uint16_t frame_pixel = y * m_with_pixels + x;
				uint16_t data_pixel = y * (m_with_pixels + 2) + x;
				uint16_t val = (data[data_pixel * 2 + 1] << 8) | data[data_pixel * 2];

				frame[frame_pixel] = val;
			}
		}
	} catch (usb_failure &) {
		closeUsb();
	}

	return frame;
}

bool SeekThermal::hasFrame() {
	return frameBuffer.hasNext();
}

ThermalFrame SeekThermal::getFrame() {
	return frameBuffer.read();
}
