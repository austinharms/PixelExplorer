#include<vector>

#include "PxeVertexBufferAttrib.h"

#ifndef PXENGINE_GLVERTEXTBUFFERFORMAT_H_
#define PXENGINE_GLVERTEXTBUFFERFORMAT_H_
namespace pxengine {
	class PxeVertexBufferFormat
	{
	public:
		PxeVertexBufferFormat() {}

		PxeVertexBufferFormat(const std::vector<PxeVertexBufferAttrib>& attribs) {
			for (const PxeVertexBufferAttrib& a : attribs)
				addAttrib(a);
		}

		PxeVertexBufferFormat(const PxeVertexBufferAttrib& attrib) {
				addAttrib(attrib);
		}

		PxeVertexBufferFormat& operator=(const PxeVertexBufferFormat& other) {
			_attribs.clear();
			for (const PxeVertexBufferAttrib& a : other._attribs)
				addAttrib(a);
			return *this;
		}

		// inserts an Attrib and returns the attribs index
		// the inserted attrib's Stride and Offset will also be calculated and set, the passed attrib will be unchanged
		size_t addAttrib(const PxeVertexBufferAttrib& attrib) {
			uint32_t offset = getBufferStride();
			PxeVertexBufferAttrib& a = _attribs.emplace_back(attrib);
			uint32_t totalStride = getBufferStride();
			for (PxeVertexBufferAttrib& a : _attribs)
				a.Stride = totalStride;
			a.Offset = offset;
			return _attribs.size() - 1;
		}

		// set's attrib to the PxeVertexBufferAttrib at index
		// returns true if the attrib was set
		// returns false if index is out of bounds of the array, attrib is unmodified 
		bool getAttrib(size_t index, PxeVertexBufferAttrib& attrib) const {
			if (index >= _attribs.size()) return false;
			attrib = _attribs[index];
			return true;
		}

		// returns the byte stride between all attribs
		uint32_t getBufferStride() const {
			uint32_t sum = 0;
			for (const PxeVertexBufferAttrib& a : _attribs)
				sum += a.getByteWidth();
			return sum;
		}

		size_t getAttribCount() const {
			return _attribs.size();
		}

	private:
		std::vector<PxeVertexBufferAttrib> _attribs;
	};
}
#endif // !PXENGINE_GLVERTEXTBUFFERFORMAT_H_
