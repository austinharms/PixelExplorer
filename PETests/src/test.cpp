#include "PE_log.h"
#include "PE_event_loop.h"
#include "PE_graphics.h"
#include "SDL.h"
#include <thread>

using namespace pecore;

graphics::RenderMesh* CreateRenderMesh() {
	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	uint16_t indices[] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	graphics::MeshFormatAttrib format{ graphics::PE_MESH_ATR_FLOAT, 0, 3, 0, 3 * sizeof(float), 0};
	return graphics::CreateMesh(vertices, sizeof(vertices), indices, sizeof(indices), graphics::PE_INDEX_U16, &format, 1);
}

void PET_main(int* returnValue, int argc, char** argv) {
	SDL_Window* window = graphics::CreateWindow(PE_TEXT("Test Window"), 1000, 800, 0);
	PE_ASSERT(window, PE_TEXT("Failed to create SDL window"));
	graphics::Shader* shader = graphics::LoadShader("test");
	PE_ASSERT(shader, PE_TEXT("Failed to load shader"));
	graphics::CommandQueue* cmd_queue = graphics::CreateCommandQueue();
	PE_ASSERT(cmd_queue, PE_TEXT("Failed to create graphics command queue"));
	graphics::RenderMesh* test_mesh = CreateRenderMesh();
	PE_ASSERT(test_mesh, PE_TEXT("Failed to create graphics render mesh"));
	for (int i = 0; i < 10000; ++i) {
		graphics::ClearCommandQueue(cmd_queue);
		PE_ASSERT(graphics::CommandSetShader(cmd_queue, shader) == 0, PE_TEXT("Failed to push graphics shader command"));
		PE_ASSERT(graphics::CommandSetMesh(cmd_queue, test_mesh) == 0, PE_TEXT("Failed to push graphics mesh command"));
		PE_ASSERT(graphics::CommandDrawMesh(cmd_queue, 6, 0) == 0, PE_TEXT("Failed to push graphics draw command"));
		PE_ASSERT(graphics::RenderToWindow(window, &cmd_queue, 1) == 0, PE_TEXT("Failed to execute command queue"));
	}

	graphics::DestroyMesh(test_mesh);
	graphics::DestroyCommandQueue(cmd_queue);
	graphics::UnloadShader(shader);
	graphics::DestroyWindow(window);
	event_loop::Stop();
	*returnValue = 0;
}

int main(int argc, char** argv)
{
	constexpr Uint32 SDL_SYSTEMS = SDL_INIT_EVENTS | SDL_INIT_GAMEPAD | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
	PE_InitLog();
	PE_LogInfo(PE_LOG_CATEGORY_TEST, PE_TEXT("Hello World"));
	int returnRes = SDL_Init(SDL_SYSTEMS);
	PE_ASSERT(returnRes == 0, PE_TEXT("Failed to init SDL %") SDL_PRIs32, returnRes);
	returnRes = graphics::Init();
	PE_ASSERT(returnRes == 0, PE_TEXT("Failed to init PE_Graphics %") SDL_PRIs32, returnRes);
	event_loop::Prepare();
	std::thread appThread(PET_main, &returnRes, argc, argv);
	event_loop::Start();
	appThread.join();
	graphics::Quit();
	return returnRes;
}
