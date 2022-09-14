#include <stdint.h>

#ifndef PIXELEXPLORER_COLOR_H_
#define PIXELEXPLORER_COLOR_H_
namespace pixelexplorer {
	struct Color
	{
	public:
		Color(uint32_t color = 0) {
			setColor(color);
		}

		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
			this->setColor(r, g, b, a);
		}

		void setColor(uint32_t color) {
			this->color = color;
		}

		void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
			color = ((r << 24) | (g << 16) | (b << 8) | a);
		}

		uint32_t getColor() const { return color; }

		uint8_t getColorR() const { return (uint8_t)(color >> 24); }

		uint8_t getColorG() const { return (uint8_t)(color >> 16); }

		uint8_t getColorB() const { return (uint8_t)(color >> 8); }

		uint8_t getColorA() const { return (uint8_t)color; }

		void setColorR(uint8_t red) {
			setColor(red, getColorG(), getColorB(), getColorA());
		}

		void setColorG(uint8_t green) {
			setColor(getColorR(), green, getColorB(), getColorA());
		}

		void setColorB(uint8_t blue) {
			setColor(getColorR(), getColorG(), blue, getColorA());
		}

		void setColorA(uint8_t alpha) {
			setColor(getColorR(), getColorG(), getColorB(), alpha);
		}

		uint32_t getColorABGR() const {
			uint32_t value = 0;
			uint8_t* valuePtr = (uint8_t*)&value;
			uint8_t* colorPtr = (uint8_t*)&color;
			valuePtr[0] = colorPtr[3];
			valuePtr[1] = colorPtr[2];
			valuePtr[2] = colorPtr[1];
			valuePtr[3] = colorPtr[0];
			return value;
		}

		uint32_t color;
	};
}
#endif