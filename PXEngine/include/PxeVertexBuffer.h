#ifndef PXENGINE_GLVERTEXBUFFER_H_
#define PXENGINE_GLVERTEXBUFFER_H_
#include "PxeTypes.h"
#include "PxeGLAsset.h"
#include "PxeGlBindable.h"
#include "PxeBuffer.h"
#include "PxeVertexBufferFormat.h"

namespace pxengine {
	// Wrapper class for GL_ARRAY_BUFFER
	class PxeVertexBuffer : public PxeGLAsset, public PxeGlBindable
	{
	public:
		PxeVertexBuffer(const PxeVertexBufferFormat& bufferFormat, PxeBuffer* buffer = nullptr, bool delayAssetInitialization = false);
		virtual ~PxeVertexBuffer();

		// Binds GL_ARRAY_BUFFER
		void bind() override;

		// Binds 0 to GL_ARRAY_BUFFER
		void unbind() override;

		// Sets the data to be buffered into the GL_ARRAY_BUFFER
		// Note: the data will only be updated after the next call to bind()
		void bufferData(PxeBuffer& buffer);

		// TODO Is this function needed?
		// Returns a pointer to the PxeBuffer last buffered into the internal GlBuffer
		// returns nullptr if no data has been buffered
		//PxeBuffer* getBuffer() const;

		// Returns a pointer to the PxeBuffer waiting to be buffered or nullptr if nothing is pending
		PXE_NODISCARD PxeBuffer* getPendingBuffer() const;

		// Returns true if a PxeBuffer is waiting to be buffered
		PXE_NODISCARD bool getBufferPending() const;

		// Returns the id of the internal GlBuffer
		PXE_NODISCARD uint32_t getGlBufferId() const;

		// Returns the format of the vertex buffer
		PXE_NODISCARD const PxeVertexBufferFormat& getFormat() const;

		// Sets the format of the vertex buffer
		void setFormat(const PxeVertexBufferFormat& format);

		PxeVertexBuffer(const PxeVertexBuffer& other) = delete;
		PxeVertexBuffer& operator=(const PxeVertexBuffer& other) = delete;

	protected:
		void initializeGl() override;
		void uninitializeGl() override;

	private:
		uint32_t _glBufferId;
		PxeBuffer* _pendingBuffer;
		PxeVertexBufferFormat _format;

		// Uploads the pending buffer into the GL_ARRAY_BUFFER
		// Note: this function assumes the buffer is bound and a valid OpenGl context
		void updateGlBuffer();
	};
}
#endif