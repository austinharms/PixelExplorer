#include <stdint.h>

#include "PxeGLAsset.h"
#include "PxeBuffer.h"
#include "PxeVertexBufferFormat.h"

#ifndef PXENGINE_GLVERTEXBUFFER_H_
#define PXENGINE_GLVERTEXBUFFER_H_
namespace pxengine {
	class PxeVertexBuffer : public PxeGLAsset
	{
	public:
		PxeVertexBuffer(const PxeVertexBufferFormat& bufferFormat, PxeBuffer* buffer = nullptr);
		virtual ~PxeVertexBuffer();
		void bind() override;
		void unbind() override;

		// sets the pending buffer to buffer
		void bufferData(PxeBuffer& buffer);

		// returns a pointer to the PxeBuffer last buffered into the GlBuffer
		// returns nullptr if no data has been buffered
		PxeBuffer* getBuffer() const;

		// returns a pointer to the PxeBuffer waiting to be buffered into the GlBuffer
		// returns nullptr if there is no pending PxeBuffer
		PxeBuffer* getPendingBuffer() const;

		// returns true if a PxeBuffer is waiting to be buffered into the GlBuffer
		bool getBufferPending() const;

		// returns the id of the internal GlBuffer
		uint32_t getGlBufferId() const;

		// returns true if the internal GlBuffer is allocated and valid
		bool getGlBufferValid() const;

		// returns the format of the vertex buffer
		const PxeVertexBufferFormat& getFormat() const;

		// sets the format of the vertex buffer
		void setFormat(const PxeVertexBufferFormat& format);

		PxeVertexBuffer(const PxeVertexBuffer& other) = delete;
		PxeVertexBuffer& operator=(const PxeVertexBuffer& other) = delete;

	protected:
		void initializeGl() override;
		void uninitializeGl() override;

	private:
		uint32_t _glBufferId;
		PxeBuffer* _currentBuffer;
		PxeBuffer* _pendingBuffer;
		PxeVertexBufferFormat _format;

		void updateGlBuffer();
	};
}
#endif