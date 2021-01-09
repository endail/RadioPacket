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

#include "Message.h"
#include "Util.h"

/**
 * Message format:
 * 
 * 	HEADER	| 0x0 - 0x1 			[ BODY LEN, 2 bytes, unsigned ]
 * 			| 0x2 - 0x3 			[ ACTION, 2 bytes, unsigned ]
 * 	BODY	| 0x4 - {BODY LEN - 1}	[ BODY DATA ]
 */
namespace RadioPacket {
class Message {

void Message::_init() {
	this->_data.clear();
	this->_data.resize(Message::getHeaderLength(), false);
	this->_data.copyFrom(Message::_DEFAULT_HEADER_DATA, Message::getHeaderLength());
}

constexpr uint16_t Message::getMaxMessageLength() {
	return 0xffff;
}

constexpr uint16_t Message::getHeaderLength() {
	return Message::_HEADER_LEN;
}

constexpr uint16_t Message::getMaxBodyLength() {
	return Message::getMaxMessageLength() - Message::getHeaderLength();
}

Message::Message() noexcept {
	this->_init();
}

Message::Message(const uint8_t* const data, const uint16_t len) noexcept
	: Message() {
		this->setBodyData(data, len);
}

Message::Message(const Message& m) noexcept {
	this->_data = m._data;
}

uint16_t Message::getRawBodyLength() const noexcept {
	return this->_data.getUInt16(Message::_BODYLEN_OFFSET);
}

uint16_t Message::getRawAction() const noexcept {
	return this->_data.getUInt16(Message::_ACTION_OFFSET);
}

void Message::setRawBodyLength(const uint16_t len) noexcept {
	this->_data.setUInt16(len, Message::_BODYLEN_OFFSET);
}

void Message::setRawAction(const uint16_t action) noexcept {
	this->_data.setUInt16(action, Message::_ACTION_OFFSET);
}

uint16_t Message::fromBaseBodyOffset(const uint16_t offset) const noexcept {
	return Message::getHeaderLength() + offset;
}

uint16_t Message::getMessageLength() const noexcept {
	return this->_data.length();
}

void Message::reset() noexcept {
	this->_init();
}

void Message::setBodyData(const uint8_t* const data, const uint16_t len) noexcept {

	//[re]allocate, keep the data (ie. header)
	this->resizeBody(len, true);

	//then copy the new data, skipping the header
	this->_data.copyFromAt(data, len, Message::getHeaderLength());

}

void Message::resizeBody(const uint16_t bodyLen, const bool copy) noexcept {

	if(bodyLen > Message::getMaxBodyLength()) {
		return;
	}

	//if copy == false, store a copy of the header
	//must store a copy of the header before resizing the body
	//, then copy it back in
	if(!copy) {
		uint8_t headerCopy[Message::getHeaderLength()];
		this->_data.copyTo(headerCopy, Message::getHeaderLength());
		this->_data.resize(Message::getHeaderLength() + bodyLen, false);
		this->_data.copyFrom(headerCopy, Message::getHeaderLength());
	}
	else {
		this->_data.resize(Message::getHeaderLength() + bodyLen, true);
	}

	this->setRawBodyLength(bodyLen);

}

const uint8_t* Message::getData() const noexcept {
	return &this->_data[0];
}

const uint8_t* Message::getHeaderData() const noexcept {
	return &this->_data[0];
}

const uint8_t* Message::getBodyData() const noexcept {
	return &this->_data[Message::getHeaderLength()];
}

uint8_t Message::parse(Message** const m, const uint8_t* const buff, const uint16_t len) noexcept {

	if(len < Message::getHeaderLength()) {
		return Message::PARSE_ERROR_INSUFFICIENT_HEADER_BYTES;
	}

	*m = new Message;

	(*m)->_data.copyFrom(buff, Message::getHeaderLength());

	//insufficient bytes
	if((*m)->getRawBodyLength() < (len - Message::getHeaderLength())) {
		delete *m;
		return Message::PARSE_ERROR_INSUFFICIENT_BUFFER_BYTES;
	}

	//too many bytes
	if((*m)->getRawBodyLength() > Message::getMaxBodyLength()) {
		delete *m;
		return Message::PARSE_ERROR_BODY_LENGTH_EXCEEDED;
	}

	(*m)->setBodyData(buff + Message::getHeaderLength(), (*m)->getRawBodyLength());

	return Message::PARSE_OK;

}

};

constexpr uint8_t Message::_DEFAULT_HEADER_DATA[];

};
