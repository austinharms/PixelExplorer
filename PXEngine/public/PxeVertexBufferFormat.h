#ifndef PXENGINE_GLVERTEXTBUFFERFORMAT_H_
#define PXENGINE_GLVERTEXTBUFFERFORMAT_H_
#include<vector>

#include "PxeTypes.h"
#include "PxeVertexBufferAttrib.h"

namespace pxengine {
	// Helper class to manage PxeVertexBuffer and PxeVertexArray data format/layout
	class PxeVertexBufferFormat
	{
	public:
		PxeVertexBufferFormat() { _stride = 0; }

		// Initializes the format with a list of attribs, equivalent to calling addAttrib() with each attrib
		// Note: attrib Offset value is ignored and recalculated when inserted and does not change the passed attrib Offset value
		PxeVertexBufferFormat(const std::vector<PxeVertexBufferAttrib>& attribs) {
			_stride = 0;
			for (const PxeVertexBufferAttrib& a : attribs)
				addAttrib(a);
		}

		// Initializes the format with an attrib, equivalent to calling addAttrib({attrib})
		// Note: attrib Offset value is ignored and recalculated when inserted and does not change the passed attrib Offset value
		PxeVertexBufferFormat(const PxeVertexBufferAttrib& attrib) {
			_stride = 0;
			addAttrib(attrib);
		}

		PxeVertexBufferFormat& operator=(const PxeVertexBufferFormat& other) {
			_attribs.clear();
			_stride = 0;
			for (const PxeVertexBufferAttrib& a : other._attribs)
				addAttrib(a);
			return *this;
		}

		// Inserts an PxeVertexBufferAttrib and returns the index it was inserted at
		// Note: attrib Offset value is ignored and recalculated when inserted and does not change the passed attrib Offset value
		// Note: the return is not a size_t because glVertexAttribPointer only supports a GLuint
		uint8_t addAttrib(const PxeVertexBufferAttrib& attrib) {
			PxeVertexBufferAttrib& a = _attribs.emplace_back(attrib);
			a.Offset = getStride();
			recalculateStride();
			return static_cast<uint8_t>(_attribs.size() - 1);
		}

		// Returns true if {index} was valid and {attrib} was set to the current value at {index}
		PXE_NODISCARD bool getAttrib(uint8_t index, PxeVertexBufferAttrib& attrib) const {
			if (index >= _attribs.size()) return false;
			attrib = _attribs[index];
			return true;
		}

		// Returns the byte stride between attribs
		inline PXE_NODISCARD uint32_t getStride() const { return _stride; }

		// Returns the number of attribs stored 
		PXE_NODISCARD uint8_t getAttribCount() const { return static_cast<uint8_t>(_attribs.size()); }

	private:
		// Recalculates the _stride value
		inline void recalculateStride() {
			_stride = 0;
			for (const PxeVertexBufferAttrib& a : _attribs)
				_stride += a.getByteWidth();
		}

		std::vector<PxeVertexBufferAttrib> _attribs;
		uint32_t _stride;
	};
}
#endif // !PXENGINE_GLVERTEXTBUFFERFORMAT_H_
