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

#ifndef MESSAGE_H_E85F6136_F60E_476D_95CA_8461D274B8A7
#define MESSAGE_H_E85F6136_F60E_476D_95CA_8461D274B8A7

#include <stdint.h>

#include "NetworkBuffer.h"

/**
 * Message format:
 * 
 * 	HEADER	| 0x0 - 0x1 			[ BODY LEN, 2 bytes, unsigned ]
 * 			| 0x2 - 0x3 			[ ACTION, 2 bytes, unsigned ]
 * 	BODY	| 0x4 - {BODY LEN - 1}	[ BODY DATA ]
 */
namespace RadioPacket {
class Message {

protected:

	static const uint16_t _HEADER_LEN = 4;
	static const uint8_t _BODYLEN_OFFSET = 0x0;
	static const uint8_t _ACTION_OFFSET = 0x2;
	static const uint8_t _BODY_OFFSET = 0x4;
	static constexpr uint8_t _DEFAULT_HEADER_DATA[_HEADER_LEN] = {
		/* 0x0 - 0x1 */ 0x0, 0x0, /* body length, 2 bytes, unsigned */
		/* 0x2 - 0x3 */ 0x0, 0x0  /* action, 2 bytes, unsigned */
	};

	void _init();

	NetworkBuffer<uint8_t, uint16_t> _data;


public:
	
	static const uint8_t PARSE_OK = 0;
	static const uint8_t PARSE_ERROR_INSUFFICIENT_HEADER_BYTES = 1;
	static const uint8_t PARSE_ERROR_INSUFFICIENT_BUFFER_BYTES = 2;
	static const uint8_t PARSE_ERROR_BODY_LENGTH_EXCEEDED = 3;
	static const uint8_t BODY_LENGTH_EXCEEDED = 4;

	static constexpr uint16_t getMaxMessageLength();
	static constexpr uint16_t getHeaderLength();
	static constexpr uint16_t getMaxBodyLength();

	Message() noexcept;
	Message(const uint8_t* const data, const uint16_t len) noexcept;
	Message(const Message& m) noexcept;
	virtual ~Message() = default;

	uint16_t getRawBodyLength() const noexcept;
	uint16_t getRawAction() const noexcept;
	void setRawBodyLength(const uint16_t len) noexcept;
	void setRawAction(const uint16_t action) noexcept;
	uint16_t fromBaseBodyOffset(const uint16_t offset = 0) const noexcept;
	uint16_t getMessageLength() const noexcept;
	void reset() noexcept;
	void setBodyData(const uint8_t* const data, const uint16_t len) noexcept;
	void resizeBody(const uint16_t bodyLen, const bool copy = true) noexcept;

	const uint8_t* getData() const noexcept;
	const uint8_t* getHeaderData() const noexcept;
	const uint8_t* getBodyData() const noexcept;
	static uint8_t parse(Message** const m, const uint8_t* const buff, const uint16_t len) noexcept;

};
};

#endif
