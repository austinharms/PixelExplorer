#include "PexGui.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui_internal.h"

namespace PexGui {
	bool ImageButton(const char* str_id, ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1)
	{
        using namespace ImGui;
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		if (window->SkipItems)
			return false;

        ImGuiID id = window->GetID(str_id);
        const ImVec2 padding = g.Style.FramePadding;
        const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2.0f);
        ItemSize(bb);
        if (!ItemAdd(bb, id))
            return false;

        bool hovered, held;
        bool pressed = ButtonBehavior(bb, id, &hovered, &held);

        // Render
        const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        RenderNavHighlight(bb, id);
        //RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, g.Style.FrameRounding));
        window->DrawList->AddImage(user_texture_id, bb.Min + padding, bb.Max - padding, uv0, uv1, col);

        return pressed;
	}
}