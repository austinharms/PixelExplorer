#include <string>
#include <stdint.h>
#include <xhash>

#include "RefCount.h"

#ifndef PIXELEXPLORER_GAME_SHAPE_H_
#define PIXELEXPLORER_GAME_SHAPE_H_
namespace pixelexplorer::game::block {
	class Shape : public RefCount
	{
	public:
		Shape(const std::string& path);
		virtual ~Shape();
		uint8_t* indices[6];
		// this includes uvs! format: x,y,z,u,v
		float* vertices[6];
		uint16_t indexCount[6];
		uint16_t vertexCount[6];
		const std::string name;

	private:
		//struct Vec5 {
		//public:
		//	float x;
		//	float y;
		//	float z;
		//	float u;
		//	float v;

		//	bool operator==(const Vec5& other) const {
		//		return (x == other.x && y == other.y && z == other.z && u == other.u && v == other.v);
		//	}
		//};

		std::string getFileName(const std::string& path);
		void freeShape();
	};
}
#endif // !PIXELEXPLORER_GAME_SHAPE_H_
