#include <stdint.h>

#include "PxeGLAsset.h"
#include "PxeBuffer.h"

#ifndef PXENGINE_GLVERTEXBUFFER_H_
#define PXENGINE_GLVERTEXBUFFER_H_
namespace pxengine {
	template<typename DataType = uint8_t, typename LengthType = uint32_t>
	class PxeGlVertexBuffer : public PxeGLAsset
	{
	public:
		typedef PxeBuffer<DataType, LengthType> PxeBufferType;

		PxeGlVertexBuffer(PxeBufferType* buffer = nullptr);
		virtual ~PxeGlVertexBuffer();
		void bind() override;
		void unbind() override;
		void bufferData(PxeBufferType& buffer);

		// returns a pointer to the PxeBuffer last buffered into the GlBuffer
		// returns nullptr if no data has been buffered
		PxeBufferType* getBuffer() const;

		// returns a pointer to the PxeBuffer waiting to be buffered into the GlBuffer
		// returns nullptr if there is no pending PxeBuffer
		PxeBufferType* getPendingBuffer() const;

		// returns true if a PxeBuffer is waiting to be buffered into the GlBuffer
		bool getBufferPending() const;

		// returns the id of the internal GlBuffer
		uint32_t getGlBufferId() const;

		// returns true if the internal GlBuffer is allocated and valid
		bool getGlBufferValid() const;

	protected:
		void initializeGl() override;
		void uninitializeGl() override;

	private:
		uint32_t _glBufferId;
		PxeBufferType* _currentBuffer;
		PxeBufferType* _pendingBuffer;

		void updateGlBuffer();
	};
}
#endif