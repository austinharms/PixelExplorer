#include <stdint.h>

#include "RefCount.h"
#include "Shape.h"
#include "glm/vec4.hpp"
#include "FaceDirection.h"
#include "BlockInstance.h"

#ifndef PIXELEXPLORER_GAME_BLOCK_BLOCK_H_
#define PIXELEXPLORER_GAME_BLOCK_BLOCK_H_
namespace pixelexplorer::game::block {
	class Block : public RefCount
	{
	public:
		Block(Shape* shape, uint32_t id, const std::string& name);
		virtual ~Block();
		// get current shape
		virtual Shape* getShape(const BlockInstance& block) const;
		// is a full non transparent block
		virtual bool isSolid(const BlockInstance& block) const;
		// is a full block
		virtual bool isFull(const BlockInstance& block) const;
		uint32_t getId() const { return _id; }

		// return the x,y offset and the width and height
		virtual glm::vec4 getUVMapping(const BlockInstance& block, FaceDirection face) const;

	protected:
		Shape* _shape;
		const uint32_t _id;
		const std::string _name;
	};
}
#endif // !PIXELEXPLORER_GAME_BLOCK_BLOCK_H_
