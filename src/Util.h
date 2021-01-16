
// MIT License
//
// Copyright (c) 2020 Daniel Robertson
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

#ifndef UTIL_H_AC46D80D_FECC_4494_9EC2_F115004B50DE
#define UTIL_H_AC46D80D_FECC_4494_9EC2_F115004B50DE

#include <stddef.h>
#include <stdint.h>

namespace RadioPacket {
class Util {

protected:

	/**
	 * https://stackoverflow.com/a/15171925/570787
	 */
	static const uint8_t crc8_table[256];

	/**
	 * Protected constructor; do not allow instatiation
	 */
	Util();


public:

	/**
	 * Converts a short (uint16_t) from host byte order to network byte order
	 * @param  {uint16_t} s : 
	 * @return {uint16_t}   : 
	 */
	static inline uint16_t htons(const uint16_t s) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
			return __builtin_bswap16(s);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
			return s;
#endif
	}
	
	/**
	 * Converts a short (uint16_t) from network byte order to host byte order
	 * @param  {uint16_t} s : 
	 * @return {uint16_t}   : 
	 */
	static inline uint16_t ntohs(const uint16_t s) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
			return __builtin_bswap16(s);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
			return s;
#endif
	}

	/**
	 * Converts a long (uint32_t) from host byte order to network byte order
	 * @param  {uint32_t} l : 
	 * @return {uint32_t}   : 
	 */
	static inline uint32_t htonl(const uint32_t l) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		return __builtin_bswap32(l);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		return l;
#endif
	}

	/**
	 * Converts a long (uint32_t) from network byte order to host byte order
	 * @param  {uint32_t} l : 
	 * @return {uint32_t}   : 
	 */
	static inline uint32_t ntohl(const uint32_t l) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		return __builtin_bswap32(l);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		return l;
#endif
	}
	
	/**
	 * Converts a long long (uint64_t) from host byte order to network byte order
	 * @param  {uint64_t} ll : 
	 * @return {uint64_t}    : 
	 */
	static inline uint64_t htonll(const uint64_t ll) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		return __builtin_bswap64(ll);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		return l;
#endif
	}

	/**
	 * Converts a long long (uint64_t) from network byte order to host byte order
	 * @param  {uint64_t} ll : 
	 * @return {uint64_t}    : 
	 */
	static inline uint64_t ntohll(const uint64_t ll) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		return __builtin_bswap64(ll);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		return ll;
#endif
	}

	/**
	 * Zeroes an array of data by setting each element to 0
	 * @param  {void*} const : 
	 * @param  {size_t} len  : 
	 */
	static void zero(void* const data, const size_t len);

	/**
	 * Calculate the CRC8 value of data
	 * Set data to nullptr to return the initial seed value
	 * This is an AVR-optimised function
	 * https://www.nongnu.org/avr-libc/user-manual/group__util__crc.html#gab27eaaef6d7fd096bd7d57bf3f9ba083
	 * @param  {uint8_t} crc    : 
	 * @param  {uint8_t*} const : 
	 * @param  {size_t} len     : 
	 * @return {uint8_t}        : 
	 */
	static uint8_t crc8(uint8_t crc, const uint8_t* const data, const size_t len);

	/**
	 * Calculate the CRC16 value of data
	 * Set data to nullptr to return the initial seed value
	 * This is an AVR-optimised function
	 * https://www.nongnu.org/avr-libc/user-manual/group__util__crc.html#ga1c1d3ad875310cbc58000e24d981ad20
	 * @param  {uint16_t} crc   : 
	 * @param  {uint8_t*} const : 
	 * @param  {size_t} len     : 
	 * @return {uint16_t}       : 
	 */
	static uint16_t crc16(uint16_t crc, const uint8_t* const data, const size_t len);

	/**
	 * Calculate the CRC8 value of data
	 * Set data to nullptr to return the initial seed value
	 * https://stackoverflow.com/a/15171925/570787
	 * @param  {uint8_t} crc   : 
	 * @param  {uint8_t*} data : 
	 * @param  {size_t} len    : 
	 * @return {uint8_t}       : 
	 */
	static uint8_t crc8_slow(uint8_t crc, const uint8_t* data, size_t len);

	/**
	 * Calculate the CRC8 value of data
	 * Set data to nullptr to return the initial seed value
	 * https://stackoverflow.com/a/15171925/570787
	 * @param  {uint8_t} crc   : 
	 * @param  {uint8_t*} data : 
	 * @param  {size_t} len    : 
	 * @return {uint8_t}       : 
	 */
	static uint8_t crc8_fast(uint8_t crc, const uint8_t* data, const size_t len);

};
};

#endif
