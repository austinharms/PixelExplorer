#ifndef PXENGINE_RINGBUFFER_H_
#define PXENGINE_RINGBUFFER_H_
#include "PxeTypes.h"

namespace pxengine {
	template<typename T, size_t ringCapacity>
	class PxeRingBuffer
	{
	public:
		inline PxeRingBuffer() : _capacity(ringCapacity) {
			_size = 0;
			_headIndex = 0;
			_tailIndex = 0;
		}

		// Returns the number of elements able to be inserted assuming an empty buffer
		inline PXE_NODISCARD size_t capacity() const { return _capacity; }

		// Returns the number of elements in the buffer
		inline PXE_NODISCARD size_t size() const { return _size; }

		// Returns if the buffer is empty
		inline PXE_NODISCARD bool empty() const { return !_size; }

		template<typename T>
		inline PXE_NODISCARD bool insert(const T& value) {
			if (_size == _capacity) return false;
			new (&(reinterpret_cast<T*>(&(_ring[0]))[_tailIndex])) T(value);
			// in theory we should never overwrite our head as we checked size first
			if (++_tailIndex == _capacity) _tailIndex = 0;
			++_size;
			return true;
		}

		// Returns a reference to the element at the head of the container
		// Note: this assumes a non empty container
		template<typename T>
		inline PXE_NODISCARD const T& peek() const {
			return (reinterpret_cast<T*>(&(_ring[0])))[_headIndex];
		}

		// Returns a reference to the element at the head of the container
		// Note: this assumes a non empty container
		template<typename T>
		inline PXE_NODISCARD T& peek() {
			return (reinterpret_cast<T*>(&(_ring[0])))[_headIndex];
		}

		// Removes the element from the head of the container and returns true if successful 
		template<typename T>
		inline PXE_NODISCARD bool pop() {
			if (_size == 0) return false;
			peek<T>().~T();
			if (++_headIndex == _capacity) _headIndex = 0;
			--_size;
			return true;
		}

	private:
		const size_t _capacity;
		uint8_t _ring[ringCapacity * sizeof(T)];
		size_t _size;
		size_t _headIndex;
		size_t _tailIndex;
	};
}
#endif // !PXENGINE_RINGBUFFER_H_
