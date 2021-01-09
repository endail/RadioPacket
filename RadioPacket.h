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

#ifndef RADIO_PACKET_H_D3C3A8BD_BB6E_46A5_A992_9286C892C492
#define RADIO_PACKET_H_D3C3A8BD_BB6E_46A5_A992_9286C892C492

#include <stdint.h>

#include "NetworkBuffer.h"
#include "Message.h"

/** 
 * A RadioPacket is limited in length to UINT8_MAX, which is the maximum
 * permitted by the Manchester library.
 * 
 * Therefore, the maximum body length of a RadioPacket will always
 * be <= UINT8_MAX - header length.
 * 
 * Formatted as such:
 * 
 * 	HEADER	| 0x0 - 0x0				[ PACKET LENGTH, 1 byte, unsigned ]
 * 			| 0x1 - 0x1				[ VERSION, 1 byte, unsigned ]
 * 			| 0x2 - 0x3				[ TRANSMITTER ID, 2 bytes, unsigned ]
 * 			| 0x4 - 0x5				[ RECEIVER ID, 2 bytes, unsigned ]
 * 			| 0x6 - 0x6				[ FRAGMENT, 1 byte, unsigned ]
 * 			| 0x7 - 0x7				[ BODY LENGTH, 1 byte, unsigned ]
 * 			| 0x8 - 0x8				[ CRC8, 1 byte, unsigned ]
 * 	BODY	| 0x9 - {BODY LENGTH-1}	[ BODY DATA ]
 */
namespace RadioPacket {
class RadioPacket {

protected:

	static const uint8_t _HEADER_LEN = 9;

	/**
	 * Stored in network byte order (MSB first)
	 * R/W through getter/setter methods
	 */
	static constexpr uint8_t _DEFAULT_HEADER[_HEADER_LEN] = {
		/* 0x0 - 0x0 */ _HEADER_LEN, 	/* packet length, 1 byte, unsigned (REQURED BY MANCHESTER LIB) 
											by default, the length of the packet will be the length of the header */
		/* 0x1 - 0x1 */ 1,				/* version, 1 byte, unsigned */
		/* 0x2 - 0x3 */ 0x00, 0x00,		/* transmitter id, 2 bytes, unsigned */
		/* 0x4 - 0x5 */ 0xff, 0xff,		/* receiver id, 2 bytes, unsigned */
		/* 0x6 - 0x6 */ 1,				/* fragment number, 1 byte, unsigned */
		/* 0x7 - 0x7 */ 0,				/* body length, 1 byte, unsigned */
		/* 0x8 - 0x8 */ 0				/* crc8, 1 byte, unsigned */
	};
	
	void _init() noexcept;

	NetworkBuffer<uint8_t, uint8_t> _data;


public:

	static const uint8_t PARSE_OK = 0;
	static const uint8_t PARSE_ERROR_INCOMPLETE_HEADER = 1;
	static const uint8_t PARSE_ERROR_INSUFFICIENT_BYTES = 2;
	static const uint8_t PARSE_ERROR_MAX_LENGTH_EXCEEDED = 3;

	static constexpr uint8_t getMaxPacketLength() noexcept;
	static constexpr uint8_t getHeaderLength() noexcept;
	static constexpr uint8_t getMaxBodyLength() noexcept;

	RadioPacket() noexcept;
	RadioPacket(const uint8_t* const body, const uint8_t len) noexcept;
	RadioPacket(const RadioPacket& p) noexcept;
	virtual ~RadioPacket() = default;

	void setRawPacketLength(const uint8_t len) noexcept;
	void setRawVersion(const uint8_t version) noexcept;
	void setRawTransmitterId(const uint16_t id) noexcept;
	void setRawReceiverId(const uint16_t id) noexcept;
	void setRawFragmentNumber(const uint8_t n) noexcept;
	void setRawBodyLength(const uint8_t len) noexcept;
	void setRawCrc8(const uint8_t crc) noexcept;

	uint8_t getRawPacketLength() const noexcept;
	uint8_t getRawVersion() const noexcept;
	uint16_t getRawTransmitterId() const noexcept;
	uint16_t getRawReceiverId() const noexcept;
	uint8_t getRawFragmentNumber() const noexcept;
	uint8_t getRawBodyLength() const noexcept;
	uint8_t getRawCrc8() const noexcept;

	/**
	 * Returns a pointer to this packet's entire data
	 * @return {uint8_t*}  : 
	 */
	const uint8_t* getData() const noexcept;

	/**
	 * Returns a pointer to this packet's header data
	 * @return {uint8_t*}  : 
	 */
	const uint8_t* getHeaderData() const noexcept;

	/**
	 * Returns a pointer to this packet's body data
	 * @return {uint8_t*}  : 
	 */
	const uint8_t* getBodyData() const noexcept;

	void copyHeader(void* const data) const noexcept;
	void copyBody(void* const data) const noexcept;

	void setBodyData(const uint8_t* const data, const uint8_t len) noexcept;
	void resizeBody(const uint8_t bodyLen, const bool copy = true) noexcept;
	
	/**
	 * Parse this packet's body into a Message
	 * @return {Message*}  : 
	 */
	Message* getMessage() const noexcept;

	/**
	 * Generate a CRC8 checksum across the packet.
	 * This EXCLUDES the CRC value in the header
	 * @return {uint8_t}  : 
	 */
	uint8_t generateChecksum() const noexcept;
		
	/**
	 * Resets this packet to default header data with no body
	 */
	void reset() noexcept;

	/**
	 * Parse arbitrary bytes into a packet. Returns RadioPacket::PARSE_OK
	 * on success.
	 * @param  {RadioPacket**} const : pointer to pointer to RadioPacket
	 * @param  {uint8_t*} const      : array of bytes
	 * @param  {uint16_t} len        : length of byte array
	 * @return {uint8_t}             : one of the RadioPacket::PARSE_* constants
	 */
	static uint8_t parse(
		RadioPacket** const p,
		const uint8_t* const buff,
		const uint16_t len) noexcept;
	
	/**
	 * Fragmentation functions are bugged; do not use
	 */
	static uint8_t calculateFragmentNumber(const uint16_t len) noexcept;
	static uint8_t fragment2(RadioPacket** packets, const uint8_t* const data, const uint16_t len) noexcept;
	static uint8_t fragment(RadioPacket** packets, const uint8_t* const data, const uint16_t len) noexcept;
	static uint16_t defragment(RadioPacket** packets, const uint8_t packetsLen, uint8_t* const data) noexcept;

};
};

#endif
