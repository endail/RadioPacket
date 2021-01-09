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

#ifndef NETWORK_BUFFER_H_672AB4F1_9E4F_4A2F_B516_2B6864032903
#define NETWORK_BUFFER_H_672AB4F1_9E4F_4A2F_B516_2B6864032903

#include "ExpandingArray.h"
#include "Util.h"

namespace RadioPacket {
template<class StorageType, class IndexType = size_t>
class NetworkBuffer : public ExpandingArray<StorageType, IndexType> {

public:

	void setUInt8(const uint8_t* const bytes, const IndexType len, const IndexType offset) noexcept {
		this->copyFromAt(bytes, len, offset);
	}

	void setUInt16(const uint16_t s, const IndexType offset) noexcept {
		const uint16_t netShort = Util::htons(s);
		this->copyFromAt(&netShort, sizeof(uint16_t), offset);
	}

	void setUInt32(const uint32_t i, const IndexType offset) noexcept {
		const uint32_t netInt = Util::htonl(i);
		this->copyFromAt(&netInt, sizeof(uint32_t), offset);
	}

	void setUInt64(const uint64_t ll, const IndexType offset) noexcept {
		const uint64_t netLong = Util::htonll(ll);
		this->copyFromAt(&netLong, sizeof(uint64_t), offset);
	}

	void getUInt8(uint8_t* const bytes, const IndexType len, const IndexType offset) const noexcept {
		this->copyToAt(bytes, len, offset);
	}

	uint16_t getUInt16(const IndexType offset) const noexcept {
		uint16_t netuint;
		this->copyToAt(&netuint, sizeof(uint16_t), offset);
		return Util::ntohs(netuint);
	}

	uint32_t getUInt32(const IndexType offset) const noexcept {
		uint32_t netuint;
		this->copyToAt(&netuint, sizeof(uint32_t), offset);
		return Util::ntohl(netuint);
	}

	uint64_t getUInt64(const IndexType offset) const noexcept {
		uint64_t netuint;
		this->copyToAt(&netuint, sizeof(uint64_t), offset);
		return Util::ntohll(netuint);
	}

};
};

#endif
