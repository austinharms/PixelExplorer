#ifndef PXENGINE_VERTEXARRAY_H_
#define PXENGINE_VERTEXARRAY_H_
#include <unordered_map>

#include "PxeTypes.h"
#include "PxeGLAsset.h"
#include "PxeGlBindable.h"
#include "PxeVertexBuffer.h"
#include "PxeIndexBuffer.h"

namespace pxengine {
	// Wrapper class for glVertexArray
	class PxeVertexArray : public PxeGLAsset, public PxeGlBindable
	{
	public:
		PxeVertexArray(bool delayAssetInitialization = false);
		virtual ~PxeVertexArray();

		// Binds the glVertexArray
		void bind() override;

		// Binds 0 as glVertexArray
		void unbind() override;

		// Rebinds and updates all vertex buffer attribs
		// Note: only happens after the next call to bind()
		void updateBufferBindings();

		// Adds a PxeVertexBuffer binding
		// {arrayAttribIndex} is glVertexAttribPointer index
		// {vertexAttribIndex} is the index of the PxeVertexBufferAttrib to use from the {vertexBuffer}
		void addVertexBuffer(PxeVertexBuffer& vertexBuffer, size_t vertexAttribIndex, uint8_t arrayAttribIndex);

		// Adds a PxeVertexBuffer binding
		void setIndexBuffer(PxeIndexBuffer* indexBuffer);

		// Remove and disable ArrayAttrib at {index}
		void removeArrayAttrib(uint8_t index);

		// Returns true if the ArrayAttrib is bound and sets vertexBuffer and vertexAttribIndex to the current values if passed
		// Note: will still return true if an invalid buffer is bound, but the array attrib will be disabled
		PXE_NODISCARD bool getAttribBound(uint8_t index, PxeVertexBuffer** vertexBuffer = nullptr, size_t* vertexAttribIndex = nullptr);

		// Returns if there was an error updating the buffer bindings
		PXE_NODISCARD bool getBindingError() const;

	protected:
		void initializeGl() override;
		void uninitializeGl() override;

	private:
		uint32_t _glId;
		std::unordered_map<uint8_t, std::pair<PxeVertexBuffer*, size_t>> _bufferBindings;
		PxeIndexBuffer* _indexBuffer;
		bool _bufferBindingsDirty;
		bool _bufferBindingError;

		// Rebind vertex buffer attribs
		// Note: this function assumes the glVertexArray is bound and a valid OpenGl context
		void updateGlBindings();
	};
}
#endif // !PXENGINE_VERTEXARRAY_H_
