#include <stdint.h>

#include "PxeRefCount.h"

#ifndef PXENGINE_BUFFER_H_
#define PXENGINE_BUFFER_H_
namespace pxengine {
	template<typename DataType = uint8_t, typename LengthType = uint32_t>
	class PxeBuffer : public PxeRefCount
	{
	public:
		PxeBuffer(LengthType size);
		virtual ~PxeBuffer();
		bool getAllocated() const;
		LengthType getLength() const;
		DataType* getBuffer() const;

	private:
		LengthType _length;
		DataType* _buffer;
	};
}
#endif // !PXENGINE_BUFFER_H_
