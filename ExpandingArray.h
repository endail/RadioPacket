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

#ifndef EXPANDING_ARRAY_H_8D9D2562_5029_4A2A_9646_B848C6701624
#define EXPANDING_ARRAY_H_8D9D2562_5029_4A2A_9646_B848C6701624

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "Util.h"

namespace RadioPacket {
template<class StorageType = uint8_t, class IndexType = size_t>
class ExpandingArray {

protected:

	/**
	 * Underlying contiguous array of data
	 */
	StorageType* _data = nullptr;

	/**
	 * Length of data within the array
	 */
	IndexType _currentLength = 0;

	/**
	 * Length of the underlying array; hidden from calling code
	 */
	IndexType _arrayLength = 0;

	
	inline bool _indexInRange(const IndexType i) const noexcept {
		return this->_data != nullptr &&
			i >= 0 &&
			i < this->_currentLength;
	}

public:

	/**
	 * Create with given length
	 */
	ExpandingArray(const IndexType len = 0) noexcept {
		this->resize(len, false);
	}

	/**
	 * Create from existing array
	 */
	ExpandingArray(const void* const data, const size_t len) noexcept {
		this->resize(len, false);
		this->copyFrom(data, len);
	}

	/**
	 * Copy constructor - deep copy
	 */
	ExpandingArray(const ExpandingArray& a) noexcept {
		this->resize(a._currentLength, false);
		this->copyFrom(a._data, a._currentLength);
	}

	/**
	 * Destructor
	 */
	~ExpandingArray() noexcept {
		this->dispose();
	}

	StorageType& operator[](const IndexType i) {
		return this->_data[i];
	}

	const StorageType& operator[](const IndexType i) const {
		return this->_data[i];
	}

	/**
	 * Allocate memory without increasing the current length, optionally
	 * copying existing data
	 */
	void allocate(const IndexType len, const bool copy = true, const bool zero = false) noexcept {

		//lengths < 0 not permitted
		if(len < 0) {
			return;
		}

		//cannot allocate less memory than amount of existing data
		//but allocation is allowed if internal array is currently unallocated
		if(len <= this->_currentLength && this->_data != nullptr) {
			return;
		}

		//allocate
		//no need to initialise the elements
		StorageType* arr = new StorageType[len];

		//copy existing data if requested
		if(copy) {
			this->copyTo(arr, this->_currentLength);
		}

		//zero existing data if requested
		if(zero) {
			Util::zero(this->_data, this->_arrayLength);
		}

		delete[] this->_data;

		this->_data = arr;
		this->_arrayLength = len;

	}
	
	/**
	 * Clears the array of elements
	 * 
	 * If zero is true, the entire array is zero'd-out
	 */
	void clear(const bool zero = false) noexcept {
		
		this->_currentLength = 0;

		if(zero) {
			Util::zero(this->_data, this->_arrayLength);
		}

	}

	/**
	 * Clean up this array; deallocate elements
	 * 
	 * If safe == true, the array is zero'd-out before being deleted
	 */
	void dispose(const bool safe = false) noexcept {
		this->clear(safe);
		delete[] this->_data;
	}

	/**
	 * Reallocate the internal array to match the current number of elements
	 * Useful after decreasing the size of a large array
	 * 
	 * If zero is true, the to-be-deallocated-internal-array is zero'd-out before
	 * being deleted
	 */
	void reclaim(const bool zero = false) noexcept {

		//do nothing if already at capacity
		if(this->_arrayLength <= this->_currentLength) {
			return;
		}

		StorageType* arr = new StorageType[this->_currentLength];
		this->copyTo(arr);

		if(zero) {
			Util::zero(this->_data, this->_currentLength);
		}

		delete[] this->_data;

		this->_data = arr;
		this->_arrayLength = this->_currentLength;

	}

	/**
	 * Current number of elements in the array
	 */
	IndexType length() const noexcept {
		return this->_currentLength;
	}

	/**
	 * Returns a pointer to the element by its index
	 */
	StorageType* ptr(const IndexType i = 0) noexcept {
		return this->_data != nullptr ? &this->_data[i] : nullptr;
	}

	/**
	 * Returns a constant pointer to the element by its index
	 */
	const StorageType* ptr(const IndexType i = 0) const noexcept {
		return this->_data != nullptr ? &this->_data[i] : nullptr;
	}

	/**
	 * Returns a pointer to the end of the array
	 */
	StorageType* end() noexcept {
		return &this->_data[this->_currentLength];
	}

	/**
	 * Returns a constant pointer to the end of the array
	 */
	const StorageType* end() const noexcept {
		return &this->_data[this->_currentLength];
	}

	/**
	 * Returns the index of an element from its pointer
	 * 
	 * -1 if this array is uninitialised or pointer is null
	 */
	IndexType idx(const StorageType* const ptr) const noexcept {
		
		if(this->_data == nullptr || ptr == nullptr) {
			return -1;
		}

		return ptr - this->_data;

	}

	/**
	 * Copies data to this array from the beginning
	 */
	void copyFrom(const void* const data, const size_t len) {
		this->copyFromAt(data, len, 0);
	}

	/**
	 * Copies data to this array at a given index
	 */
	void copyFromAt(const void* const data, const size_t len, const size_t i = 0) {

		if(data == nullptr || len <= 0) {
			return;
		}

		//bug?
		if(!this->_indexInRange(i)) {
			return;
		}

		//if copying len data will exceed the size of the array, resize
		if((i + len) > this->_currentLength) {
			this->resize(this->_currentLength + (this->_currentLength - (i + len)), true);
		}

		::memcpy(&this->_data[i], data, len);

	}

	/**
	 * Copies len amount of data from this array from the beginning
	 * 
	 * If len is <= 0, the entire array is copied
	 */
	void copyTo(void* const data, const size_t len = 0) const noexcept {
		this->copyToAt(data, len > 0 ? len : this->_currentLength, 0);
	}

	/**
	 * Copies data from this array at a given index
	 */
	void copyToAt(void* const data, size_t len = 0, const size_t i = 0) const noexcept {

		if(data == nullptr || len < 0 || len > this->_currentLength) {
			return;
		}

		if(!this->_indexInRange(i)) {
			return;
		}

		if((i + len) > this->_currentLength) {
			return;
		}

		::memcpy(data, &this->_data[i], len);

	}

	/**
	 * Resize the array to the given length, optionally copying existing data
	 * to the newly resized array
	 */
	void resize(const IndexType len, const bool copy = true) noexcept {

		//lengths < 0 not permitted
		if(len < 0) {
			return;
		}

		//do nothing if same size AND unallocated
		if(len == this->_currentLength && this->_data != nullptr) {
			return;
		}
		else if(len < this->_arrayLength) {
			//if the array is currently able to hold len amount of data,
			//just update the length and return
			this->_currentLength = len;
			return;
		}

		//otherwise, allocate len amount of data
		//and set the new length
		this->allocate(len, copy);
		this->_currentLength = len;

	}

};
};

#endif
