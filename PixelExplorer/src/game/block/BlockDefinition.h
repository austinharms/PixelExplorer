#include <stdint.h>
#include <string>

#include "common/RefCount.h"
#include "BlockFaceDefinition.h"
#include "FaceDirection.h"

#ifndef PIXELEXPLORER_GAME_BLOCK_BLOCKDEFINITION_H_
#define PIXELEXPLORER_GAME_BLOCK_BLOCKDEFINITION_H_
namespace pixelexplorer::game::block {
	class BlockDefinition : public RefCount
	{
	public:
		inline BlockDefinition(BlockFaceDefinition* faces[6], uint32_t id, const std::string& name) : _name(name), _id(id) {
			for (uint8_t i = 0; i < 6; ++i) {
				faces[i]->grab();
				_faces[i] = faces[i];
			}
		}

		inline virtual ~BlockDefinition() {
			for (uint8_t i = 0; i < 6; ++i) _faces[i]->drop();
		}

		inline const uint32_t getId() const { return _id; }

		inline const std::string getName() const { return _name; }

		inline const BlockFaceDefinition* getBlockFace(const FaceDirection direction) const {
			if ((uint32_t)direction < 0 || (uint32_t)direction > 5) return nullptr;
			return _faces[(uint32_t)direction];
		}

		inline const BlockFaceDefinition** getBlockFaces() const {
			return (const BlockFaceDefinition**)_faces;
		}

	protected:
		const uint32_t _id;
		const std::string _name;
		BlockFaceDefinition* _faces[6];
	};
}
#endif // !PIXELEXPLORER_GAME_BLOCK_BLOCKDEFINITION_H_
