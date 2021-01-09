// MIT License
//
// Copyright (c) 2021 Daniel Robertson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "RadioPacket.h"

#include <math.h>
#include <string.h>
#include "Util.h"

namespace RadioPacket {
	
void RadioPacket::_init() noexcept {
	this->_data.clear();
	this->_data.resize(RadioPacket::getHeaderLength(), false);
	this->_data.copyFrom(RadioPacket::_DEFAULT_HEADER, RadioPacket::getHeaderLength());
}

constexpr uint8_t RadioPacket::getMaxPacketLength() noexcept {
	return 0xff;
}

constexpr uint8_t RadioPacket::getHeaderLength() noexcept {
	return _HEADER_LEN;
}

constexpr uint8_t RadioPacket::getMaxBodyLength() noexcept {
	return RadioPacket::getMaxPacketLength() - RadioPacket::getHeaderLength();
}

RadioPacket::RadioPacket() noexcept {
	this->_init();
}

RadioPacket::RadioPacket(const uint8_t* const body, const uint8_t len) noexcept 
	: RadioPacket() {
		this->setBodyData(body, len);
}

RadioPacket::RadioPacket(const RadioPacket& p) noexcept {
	this->_data = p._data;
}

void RadioPacket::setRawPacketLength(const uint8_t len) noexcept {
	this->_data[0] = len;
}

void RadioPacket::setRawVersion(const uint8_t version) noexcept {
	this->_data[1] = version;
}

void RadioPacket::setRawTransmitterId(const uint16_t id) noexcept {
	this->_data.setUInt16(id, 2);
}

void RadioPacket::setRawReceiverId(const uint16_t id) noexcept {
	this->_data.setUInt16(id, 4);
}

void RadioPacket::setRawFragmentNumber(const uint8_t n) noexcept {
	this->_data[6] = n;
}

void RadioPacket::setRawBodyLength(const uint8_t len) noexcept {
	this->_data[7] = len;
}

void RadioPacket::setRawCrc8(const uint8_t crc) noexcept {
	this->_data[8] = crc;
}

uint8_t RadioPacket::getRawPacketLength() const noexcept {
	return this->_data[0];
}

uint8_t RadioPacket::getRawVersion() const noexcept {
	return this->_data[1];
}

uint16_t RadioPacket::getRawTransmitterId() const noexcept {
	return this->_data.getUInt16(2);
}

uint16_t RadioPacket::getRawReceiverId() const noexcept {
	return this->_data.getUInt16(4);
}

uint8_t RadioPacket::getRawFragmentNumber() const noexcept {
	return this->_data[6];
}

uint8_t RadioPacket::getRawBodyLength() const noexcept {
	return this->_data[7];
}

uint8_t RadioPacket::getRawCrc8() const noexcept {
	return this->_data[8];
}

const uint8_t* RadioPacket::getData() const noexcept {
	return &this->_data[0];
}

const uint8_t* RadioPacket::getHeaderData() const noexcept {
	return &this->_data[0];
}

const uint8_t* RadioPacket::getBodyData() const noexcept {
	return &this->_data[RadioPacket::getHeaderLength()];
}

void RadioPacket::copyHeader(void* const data) const noexcept {
	this->_data.copyTo(data, this->getHeaderLength(), RadioPacket::getHeaderLength());
}

void RadioPacket::copyBody(void* const data) const noexcept {
	this->_data.copyToAt(data, this->getRawBodyLength())
}

void RadioPacket::setBodyData(const uint8_t* const data, const uint8_t len) {
	//resize the body, but don't bother copying the existing body
	this->resizeBody(len, false);
	this->_data.copyFromAt(data, len, RadioPacket::getHeaderLength());
}

void RadioPacket::resizeBody(const uint8_t bodyLen, const bool copy) noexcept {
	
	if(bodyLen > RadioPacket::getMaxBodyLength()) {
		return;
	}

	if(!copy) {
		uint8_t headerCopy[RadioPacket::getHeaderLength()];
		this->_data.copyTo(headerCopy, RadioPacket::getHeaderLength());
		this->_data.resize(RadioPacket::getHeaderLength() + bodyLen, false);
		this->_data.copyFrom(headerCopy, RadioPacket::getHeaderLength());
	}
	else {
		this->_data.resize(RadioPacket::getHeaderLength() + bodyLen, true);
	}

	//set new length
	this->setRawPacketLength(RadioPacket::getHeaderLength() + bodyLen);
	this->setRawBodyLength(bodyLen);

}

Message* RadioPacket::getMessage() const noexcept {
	
	Message* m;
	
	if(Message::parse(
		&m,
		this->getBodyData(),
		this->getRawBodyLength()) != Message::PARSE_OK) {
			m = nullptr;
	}

	return m;

}

uint8_t RadioPacket::generateChecksum() const noexcept {
	
	uint8_t crc = Util::crc8(0, nullptr, 0);

	//calculate the crc for the header (without the crc)
	crc = Util::crc8(
		crc,
		this->getHeaderData(),
		this->getHeaderLength() - sizeof(uint8_t));

	//calculate the crc for the body
	crc = Util::crc8(
		crc,
		this->getBodyData(),
		this->getRawBodyLength());

	return crc;

}

void RadioPacket::reset() noexcept {
	this->_init();
}

uint8_t RadioPacket::parse(RadioPacket** const p, const uint8_t* const buff, const uint16_t len) noexcept {

	if(len < RadioPacket::getHeaderLength()) {
		return RadioPacket::PARSE_ERROR_INCOMPLETE_HEADER;
	}

	*p = new RadioPacket;
	
	//copy the header first
	(*p)->_data.copyFrom(buff, RadioPacket::getHeaderLength());

	//make sure there are sufficient bytes in the buffer before copying
	if((*p)->getRawBodyLength() > (len - RadioPacket::getHeaderLength())) {
		delete *p;
		return RadioPacket::PARSE_ERROR_INSUFFICIENT_BYTES;
	}

	//check if the body length exceeds the maximum permitted
	if((*p)->getRawBodyLength() > RadioPacket::getMaxBodyLength()) {
		delete *p;
		return RadioPacket::PARSE_ERROR_MAX_LENGTH_EXCEEDED;
	}

	//copy of the rest based on the body length in the header
	(*p)->setBodyData(
		buff + RadioPacket::getHeaderLength(),
		(*p)->getRawBodyLength());

	return RadioPacket::PARSE_OK;

}

uint8_t RadioPacket::calculateFragmentNumber(const uint16_t len) {
	const double maxBodyLenDbl = static_cast<double>(RadioPacket::getMaxBodyLength());
	return ceil(len / maxBodyLenDbl);
}

uint8_t RadioPacket::fragment2(RadioPacket** packets, const uint8_t* const data, const uint16_t len) {

	RadioPacket* p = nullptr;
	const uint16_t fragments = RadioPacket::calculateFragmentNumber(len);

	for(uint16_t f = 0; f < fragments; ++f) {

		p = new RadioPacket;

		p->setRawFragmentNumber(f);
		p->setBodyData(
			data + (f * RadioPacket::getMaxBodyLength()),
			RadioPacket::getMaxBodyLength()); //BUG: calculation is wrong!

		packets[f] = p;

	}

	return fragments;

}

uint8_t RadioPacket::fragment(RadioPacket** packets, const uint8_t* const data, const uint16_t len) {

	RadioPacket* p = nullptr;
	uint8_t fragmentNumber = 0;
	uint16_t bytesPacketised = len;

	while(bytesPacketised > 0) {

		p = new RadioPacket;

		p->setRawFragmentNumber(fragmentNumber);
		p->setBodyData(
			data + (bytesPacketised - RadioPacket::getMaxBodyLength()),
			RadioPacket::getMaxBodyLength()); //BUG: calculation is wrong

		packets[fragmentNumber] = p;

		bytesPacketised -= RadioPacket::getMaxBodyLength();
		fragmentNumber++;

	}

	return fragmentNumber;

}

uint16_t RadioPacket::defragment(RadioPacket** packets, const uint8_t packetsLen, uint8_t* const data) {

	uint16_t byteOffset = 0;

	for(uint8_t i = 0; i < packetsLen; ++i) {
		
		::memcpy(
			data + byteOffset,
			packets[i]->getBodyData(),
			packets[i]->getRawBodyLength());
		
		byteOffset += packets[i]->getRawBodyLength();

	}

	//return the number of bytes defragmented
	return byteOffset + 1;

}

constexpr uint8_t RadioPacket::_DEFAULT_HEADER[];

};

#endif
