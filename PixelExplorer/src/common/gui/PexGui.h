#ifndef PIXELEXPLORER_GUI_EXTENSIONS_H_
#define PIXELEXPLORER_GUI_EXTENSIONS_H_
#include "imgui.h"

namespace PexGui {
	bool ImageButton(const char* str_id, ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
}
#endif // !PIXELEXPLORER_GUI_EXTENSIONS_H_
