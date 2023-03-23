#ifndef PXESAMPLES_CUBESTACK_GUI_H_
#define PXESAMPLES_CUBESTACK_GUI_H_
#include "PxeTypes.h"
#include "PxeGuiComponent.h"
#include "imgui.h"

class StackGui : public pxengine::PxeGuiComponent
{
public:
	struct State
	{
		State() { 
			CubeCount = 20;
			CubeSpacing = 1.5f;
			BaseHeight = -8.0f;
			TimeScale = 1.0f;
			PhysicsStep = 0.035f;
			Reset = false;
		}

		int32_t CubeCount;
		float CubeSpacing;
		float BaseHeight;
		float TimeScale;
		float PhysicsStep;
		bool Reset;
	};

	State& getState() { return _state; }
	void setState(const State& state) { _state = state; }
	PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(StackGui, pxengine::PxeGuiComponent);
	StackGui() = default;
	virtual ~StackGui() = default;
	PXE_NOCOPY(StackGui);

protected:
	PXE_DEFAULT_PROTECTED_COMPONENT_IMPLMENTATION(pxengine::PxeGuiComponent);
	void onRender() override {
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Begin("Cube Stack Gui", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground);
		ImGui::Text("FPS: %.0f", io.Framerate);
		ImGui::Text("Controls");
		ImGui::PushItemWidth(75);
		ImGui::DragInt("Cube Count", &_state.CubeCount, 1, 1);
		ImGui::DragFloat("Cube Spacing", &_state.CubeSpacing, 0.05f);
		ImGui::DragFloat("Base Height", &_state.BaseHeight, 0.25f);
		ImGui::DragFloat("Time Scale", &_state.TimeScale, 0.01f, 0, 100);
		ImGui::DragFloat("Physics Step", &_state.PhysicsStep, 0.001f, 0.001, 1);
		ImGui::PopItemWidth();
		if (ImGui::Button("Reset Stack"))
			_state.Reset = true;
		ImGui::End();
		ImGui::PopStyleVar();
	}

private:
	State _state;
};
#endif // !PXESAMPLES_CUBESTACK_GUI_H_
