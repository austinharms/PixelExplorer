#include <stdint.h>

#ifndef PIXELEXPLORER_COLOR_H_
#define PIXELEXPLORER_COLOR_H_
namespace pixelexplorer {
	struct Color
	{
	public:
		inline Color(uint32_t color = 0);
		inline Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		inline void setColor(uint32_t color);
		inline void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		inline uint32_t getColor() const;
		inline uint8_t getColorR() const;
		inline uint8_t getColorG() const;
		inline uint8_t getColorB() const;
		inline uint8_t getColorA() const;
		inline void setColorR(uint8_t red);
		inline void setColorG(uint8_t green);
		inline void setColorB(uint8_t blue);
		inline void setColorA(uint8_t alpha);
		inline uint32_t getColorABGR() const;

		uint32_t color;
	};
}

#include "Color.inl"
#endif