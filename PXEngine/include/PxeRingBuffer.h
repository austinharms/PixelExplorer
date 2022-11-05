#include <stdint.h>

#ifndef PXENGINE_RINGBUFFER_H_
#define PXENGINE_RINGBUFFER_H_
namespace pxengine {
	template<typename T, uint32_t ringCapacity>
	class PxeRingBuffer
	{
	public:
		PxeRingBuffer() {
			_size = 0;
			_headIndex = 0;
			_tailIndex = 0;
		}

		const uint32_t capacity() const { return ringCapacity; }

		uint32_t size() const { return _size; }

		template<typename T>
		bool insert(const T& value) {
			if (_size == ringCapacity) return false;
			new (_ring + (_tailIndex * sizeof(T))) T(value);
			// in theory we should never overwrite our head as we checked size first
			if (++_tailIndex == ringCapacity) _tailIndex = 0;
			++_size;
			return true;
		}

		// this assumes a non empty container
		template<typename T>
		const T& peek() const {
			return ((T*)_ring)[_headIndex];
		}

		// this assumes a non empty container
		template<typename T>
		T& peek() {
			return ((T*)_ring)[_headIndex];
		}

		// remove the element from the head of the container
		// returns false if the container is empty
		template<typename T>
		bool pop() {
			if (_size == 0) return false;
			peek<T>().~T();
			if (++_headIndex == ringCapacity) _headIndex = 0;
			--_size;
			return true;
		}

	private:
		uint8_t _ring[ringCapacity * sizeof(T)];
		uint32_t _size;
		uint32_t _headIndex;
		uint32_t _tailIndex;
	};
}
#endif // !PXENGINE_RINGBUFFER_H_
