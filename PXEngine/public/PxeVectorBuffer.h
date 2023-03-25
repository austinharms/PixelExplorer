#ifndef PXENGINE_VECTOR_BUFFER_H_
#define PXENGINE_VECTOR_BUFFER_H_
#include <vector>
#include <new>

#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeBuffer.h"
#include "PxeLogger.h"

namespace pxengine {
	// Helper class for storing and sharing a std::vector<T>
	template<typename T>
	class PxeVectorBuffer : public PxeBuffer
	{
	public:
		// Returns a new PxeVectorBuffer instance with vector of {capacity} bytes or nullptr on failure
		template<typename T>
		static PxeVectorBuffer<T>* create(PxeSize capacity = 0) {
			PxeVectorBuffer<T>* buf = new(std::nothrow) PxeVectorBuffer<T>(capacity);
			return buf;
		}

		// Returns the byte size of the allocated buffer
		PXE_NODISCARD virtual PxeSize getSize() const override { return _vector.size() * sizeof(_vector[0]); }

		// Returns a pointer to the allocated memory
		// Note: this will return nullptr if the buffer size is 0
		PXE_NODISCARD virtual void* getBuffer() const override {
			if (_vector.size() == 0) return nullptr;
			return const_cast<void*>(static_cast<const void*>(&_vector[0]));
		}

		// Returns the vector used to store data
		template<typename T>
		PXE_NODISCARD std::vector<T>& getVector() { return _vector; }

		// Returns the vector used to store data
		template<typename T>
		PXE_NODISCARD const std::vector<T>& getVector() const { return _vector; }

		virtual ~PxeVectorBuffer() = default;
		PXE_NOCOPY(PxeVectorBuffer);

	private:
		PxeVectorBuffer(PxeSize capacity) {
			if (capacity) _vector.reserve(capacity);
		}

		std::vector<T> _vector;
	};
}
#endif // !PXENGINE_VECTOR_BUFFER_H_
