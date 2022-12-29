#include <stdint.h>

#include "PxeGLAsset.h"
#include "PxeBuffer.h"
#include "GL/glew.h"

#ifndef PXENGINE_GLINDEXBUFFER_H_
#define PXENGINE_GLINDEXBUFFER_H_
namespace pxengine {
	enum class PxeIndexType : uint32_t
	{
		UNSIGNED_8BIT = GL_UNSIGNED_BYTE,
		UNSIGNED_16BIT = GL_UNSIGNED_SHORT,
		UNSIGNED_32BIT = GL_UNSIGNED_INT,
	};

	class PxeIndexBuffer : public PxeGLAsset
	{
	public:

		PxeIndexBuffer(PxeIndexType indexType, PxeBuffer* buffer = nullptr);
		virtual ~PxeIndexBuffer();
		void bind() override;
		void unbind() override;
		void bufferData(PxeBuffer& buffer);

		// returns a pointer to the PxeBuffer last buffered into the internal GlBuffer
		// returns nullptr if no data has been buffered
		PxeBuffer* getBuffer() const;

		// returns a pointer to the PxeBuffer waiting to be buffered into the internal GlBuffer
		// returns nullptr if there is no pending PxeBuffer
		PxeBuffer* getPendingBuffer() const;

		// returns true if a PxeBuffer is waiting to be buffered into the internal GlBuffer
		bool getBufferPending() const;

		// returns the id of the internal GlBuffer
		uint32_t getGlBufferId() const;

		// returns true if the internal GlBuffer is allocated and valid
		bool getGlBufferValid() const;

		// returns the index type the buffer is using
		PxeIndexType getIndexType() const;

		PxeIndexBuffer(const PxeIndexBuffer& other) = delete;
		PxeIndexBuffer operator=(const PxeIndexBuffer& other) = delete;

	protected:
		void initializeGl() override;
		void uninitializeGl() override;

	private:
		PxeBuffer* _currentBuffer;
		PxeBuffer* _pendingBuffer;
		uint32_t _glBufferId;
		const PxeIndexType _glBufferType;

		// this function assumes the buffer is bound
		void updateGlBuffer();
	};
}
#endif