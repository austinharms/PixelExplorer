#include <stdint.h>

#include "PxeGLAsset.h"
#include "PxeBuffer.h"
#include "PxeVertexBufferFormat.h"

#ifndef PXENGINE_GLVERTEXBUFFER_H_
#define PXENGINE_GLVERTEXBUFFER_H_
namespace pxengine {
	template<typename DataType = uint8_t, typename LengthType = uint32_t>
	class PxeVertexBuffer : public PxeGLAsset
	{
	public:
		typedef PxeBuffer<DataType, LengthType> PxeBufferType;
		PxeVertexBuffer();
		PxeVertexBuffer(PxeBufferType* buffer = nullptr);
		PxeVertexBuffer(const PxeVertexBufferFormat& bufferFormat, PxeBufferType* buffer = nullptr);
		virtual ~PxeVertexBuffer();
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

		PxeVertexBufferFormat& getFormat();

		const PxeVertexBufferFormat& getFormat() const;

		void setFormat(const PxeVertexBufferFormat& format);

		PxeVertexBuffer(const PxeVertexBuffer& other) = delete;
		PxeVertexBuffer& operator=(const PxeVertexBuffer& other) = delete;

	protected:
		void initializeGl() override;
		void uninitializeGl() override;

	private:
		uint32_t _glBufferId;
		PxeBufferType* _currentBuffer;
		PxeBufferType* _pendingBuffer;
		PxeVertexBufferFormat _format;

		void updateGlBuffer();
	};
}
#endif