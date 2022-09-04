#include <stdint.h>

#include "RefCount.h"

#ifndef PIXELEXPLORER_DATABUFFER_H_
#define PIXELEXPLORER_DATABUFFER_H_
namespace pixelexplorer {
	template<class T>
	class DataBuffer : public RefCount
	{
	public:
		inline DataBuffer(uint64_t length) : _length(length) {
			_buffer = (T*)malloc(length * sizeof(T));
		}

		inline virtual ~DataBuffer() {
			free(_buffer);
		}

		inline uint64_t getLength() const { return _length; }

		inline uint64_t getSize() const { return _length * sizeof(T); }

		inline T* getBufferPtr() const { return _buffer; }

	private:
		const uint64_t _length;
		T* _buffer;
	};
}
#endif // !PIXELEXPLORER_DATABUFFER_H_
