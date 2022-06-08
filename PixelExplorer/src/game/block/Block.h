#include <stdint.h>

#include "RefCount.h"
#include "Shape.h"

#ifndef PIXELEXPLORER_GAME_BLOCK_BLOCK_H_
#define PIXELEXPLORER_GAME_BLOCK_BLOCK_H_
namespace pixelexplorer::game::block {
	class Block : public RefCount
	{
	public:
		Block(Shape* shape, uint32_t id, const std::string& name);
		virtual ~Block();
		// get current shape
		virtual Shape* getShape() const;
		// is a full non transparent block
		virtual bool isSolid() const;
		// is a full block
		virtual bool isFull() const;
	protected:
		Shape* _shape;
		const std::string _name;
	};
}
#endif // !PIXELEXPLORER_GAME_BLOCK_BLOCK_H_
