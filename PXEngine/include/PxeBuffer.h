#include <stdint.h>

#include "PxeRefCount.h"

#ifndef PXENGINE_BUFFER_H_
#define PXENGINE_BUFFER_H_
namespace pxengine {
	class PxeBuffer : public PxeRefCount
	{
	public:
		// creates and allocates a buffer with a byte length of size
		PxeBuffer(size_t size);
		PxeBuffer(const PxeBuffer& other);
		PxeBuffer& operator=(const PxeBuffer& other);
		virtual ~PxeBuffer();

		// returns true if the buffer is allocated
		// returns false if there was an allocation error
		bool getAllocated() const;

		// returns the byte size of the buffer
		// note: if there was an allocation error this will return 0
		size_t getSize() const;

		// returns a pointer to the internal buffer
		// note: buffer may be nullptr if there was an allocation error
		void* getBuffer() const;

	private:
		size_t _size;
		void* _buffer;
	};
}
#endif // !PXENGINE_BUFFER_H_
