#include "Block.h"
namespace pixelexplorer::game::block {
	Block::Block(Shape* shape, uint32_t id, const std::string& name) : _name(name), _id(id)
	{
		_shape = shape;
		_shape->grab();
	}

	Block::~Block()
	{
		_shape->drop();
	}

	Shape* Block::getShape(const BlockInstance& block) const
	{
		return _shape;
	}

	bool Block::isSolid(const BlockInstance& block) const
	{
		return true;
	}

	bool Block::isFull(const BlockInstance& block) const
	{
		return true;
	}

	glm::vec4 Block::getUVMapping(const BlockInstance& block, FaceDirection face) const
	{
		return glm::vec4(0,0,1,1);
	}
}
