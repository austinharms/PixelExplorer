#include "common/RefCount.h"

#include <filesystem>
#include <string>

#ifndef PIXELEXPLORER_GAME_WORLD_WORLDDETAILS_H_
#define PIXELEXPLORER_GAME_WORLD_WORLDDETAILS_H_
namespace pixelexplorer::game::world {
	struct WorldDetails : public RefCount
	{
	public:
		inline WorldDetails(const std::filesystem::path& path, std::string name) : Path(path), Name(name) {}

		inline virtual ~WorldDetails() {}

		inline WorldDetails* loadDetails(const std::filesystem::path& path) {
			// validate path is valid
			if (path.has_filename()) return nullptr;
			if (!std::filesystem::exists(path)) return nullptr;
			return new WorldDetails(path, "test");
		}

		std::filesystem::path Path;
		std::string Name;
	};
}
#endif