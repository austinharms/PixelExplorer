#include <stdint.h>

#include "RefCount.h"

#ifndef PIXELEXPLORER_DATABUFFER_H_
#define PIXELEXPLORER_DATABUFFER_H_
namespace pixelexplorer {
	template<class T>
	class DataBuffer : public RefCount
	{
	public:
		inline DataBuffer(uint64_t length);
		inline virtual ~DataBuffer();
		inline uint64_t getLength() const;
		inline uint64_t getSize() const;
		inline T* getBufferPtr() const;
		inline T operator [] (int i) const;
		inline T& operator [] (int i);

	private:
		const uint64_t _length;
		T* _buffer;
	};
}

#include "DataBuffer.inl"
#endif // !PIXELEXPLORER_DATABUFFER_H_
