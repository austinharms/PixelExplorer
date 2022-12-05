#include <unordered_map>

#include "PxeGLAsset.h"
#include "PxeVertexBuffer.h"

#ifndef PXENGINE_VERTEXARRAY_H_
#define PXENGINE_VERTEXARRAY_H_
namespace pxengine {
	class PxeVertexArray : public PxeGLAsset
	{
	public:
		PxeVertexArray();
		virtual ~PxeVertexArray();
		void bind() override;
		void unbind() override;
		bool getValid() const;
		// flag the vertex array to re-setup/rebind vertex buffer attribs
		void updateBufferBindings();

		// adds a PxeVertexBuffer binding
		void addVertexBuffer(PxeVertexBuffer& vertexBuffer, size_t vertexAttribIndex, uint8_t arrayAttribIndex);

		// remove and disable ArrayAttrib at index
		void removeArrayAttrib(uint8_t index);

		// returns true if the ArrayAttrib is bound otherwise returns false
		// optionally sets vertexBuffer and vertexAttribIndex to the bound buffer and attrib index if they are not nullptr and ArrayAttrib is bound
		// note will still return true if an invalid buffer is bound, but the array attrib will be disabled
		bool getAttribBound(uint8_t index, PxeVertexBuffer** vertexBuffer = nullptr, size_t* vertexAttribIndex = nullptr);

	protected:
		void initializeGl() override;
		void uninitializeGl() override;

	private:
		uint32_t _glId;
		std::unordered_map<uint8_t, std::pair<PxeVertexBuffer*, size_t>> _bufferBindings;
		bool _bufferBindingsDirty;

		// this function assumes the vertex array is bound
		void updateGlBindings();
	};
}
#endif // !PXENGINE_VERTEXARRAY_H_
