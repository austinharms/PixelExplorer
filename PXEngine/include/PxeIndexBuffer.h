#ifndef PXENGINE_GLINDEXBUFFER_H_
#define PXENGINE_GLINDEXBUFFER_H_
#include "PxeTypes.h"
#include "PxeGLAsset.h"
#include "PxeBuffer.h"

namespace pxengine {
	// Wrapper class for GL_ELEMENT_ARRAY_BUFFER
	class PxeIndexBuffer : public PxeGLAsset
	{
	public:
		PxeIndexBuffer(PxeIndexType indexType, PxeBuffer* buffer = nullptr);
		virtual ~PxeIndexBuffer();

		// Binds GL_ELEMENT_ARRAY_BUFFER
		void bind() override;

		// Binds 0 to GL_ELEMENT_ARRAY_BUFFER
		void unbind() override;
		
		// Sets the data to be buffered into the GL_ELEMENT_ARRAY_BUFFER
		// Note: the data will only be updated after the next call to bind()
		void bufferData(PxeBuffer& buffer);

		// Returns a pointer to the PxeBuffer waiting to be buffered or nullptr if nothing is pending
		PXE_NODISCARD PxeBuffer* getPendingBuffer() const;

		// Returns true if a PxeBuffer is waiting to be buffered
		PXE_NODISCARD bool getBufferPending() const;

		// Returns the id of the internal GlBuffer
		PXE_NODISCARD uint32_t getGlBufferId() const;

		// Returns if the internal GlBuffer is created/valid
		PXE_NODISCARD bool getValid() const;

		// Returns the index type the buffer is using
		PXE_NODISCARD PxeIndexType getIndexType() const;

		PxeIndexBuffer(const PxeIndexBuffer& other) = delete;
		PxeIndexBuffer operator=(const PxeIndexBuffer& other) = delete;

	protected:
		void initializeGl() override;
		void uninitializeGl() override;

	private:
		PxeBuffer* _pendingBuffer;
		uint32_t _glBufferId;
		const PxeIndexType _glBufferType;

		// Uploads the pending buffer into the GL_ELEMENT_ARRAY_BUFFER
		// Note: this function assumes the buffer is bound and a valid OpenGl context
		void updateGlBuffer();
	};
}
#endif