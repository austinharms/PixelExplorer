#include "NpWindow.h"

#include "NpEngineBase.h"
#include "NpScene.h"
#include "SDL.h"
#include "PxeShader.h"
#include "PxeRenderMaterial.h"
#include "PxeRenderBase.h"
#include "PxeRenderElement.h"
#include "PxeRenderObject.h"
#include "NpLogger.h"
#include "imgui.h"

namespace pxengine {
	namespace nonpublic {
		NpWindow::NpWindow(SDL_Window& window) : _sdlWindow(window)
		{
			_scene = nullptr;
			_swapInterval = 1;
			_shouldClose = false;
			NpEngineBase::getInstance().grab();
		}

		NpWindow::~NpWindow()
		{
			if (_scene) {
				_scene->drop();
				_scene = nullptr;
			}

			NpEngineBase::getInstance().destroyWindow(*this);
			NpEngineBase::getInstance().drop();
		}

		void NpWindow::acquireGlContext()
		{
			NpEngineBase::getInstance().acquireGlContext(*this);
		}

		void NpWindow::releaseGlContext()
		{
			NpEngineBase::getInstance().releaseGlContext(*this);
		}

		bool NpWindow::acquiredGlContext() const
		{
			return NpEngineBase::getInstance().acquiredGlContext(*this);
		}

		void NpWindow::swapFrameBuffer()
		{
			SDL_GL_SwapWindow(&_sdlWindow);
		}

		const SDL_Window& NpWindow::getSDLWindow() const
		{
			return _sdlWindow;
		}

		SDL_Window& NpWindow::getSDLWindow()
		{
			return _sdlWindow;
		}

		void NpWindow::setSwapInterval(int8_t interval)
		{
			_swapInterval = interval;
		}

		int8_t NpWindow::getSwapInterval()
		{
			return _swapInterval;
		}

		bool NpWindow::getShouldClose() const
		{
			return _shouldClose;
		}

		void NpWindow::resetShouldClose()
		{
			_shouldClose = false;
		}

		// TODO does changing window visibility require the GL Context?
		void NpWindow::setWindowShown(bool show)
		{
			if (show) {
				SDL_ShowWindow(&_sdlWindow);
			}
			else {
				SDL_HideWindow(&_sdlWindow);
			}
		}

		bool NpWindow::getWindowShown() const
		{
			return SDL_GetWindowFlags(&_sdlWindow) & SDL_WINDOW_SHOWN;
		}

		void NpWindow::setShouldClose()
		{
			_shouldClose = true;
		}

		bool NpWindow::pollEvents(SDL_Event* event)
		{
			if (_eventBuffer.size() == 0)
				NpEngineBase::getInstance().pollEvents();
			if (_eventBuffer.size() == 0) return false;
			if (event) {
				*event = _eventBuffer.peek<SDL_Event>();
				_eventBuffer.pop<SDL_Event>();
			}

			return true;
			//SDL_Event e;
			//while (_eventBuffer.size() > 0)
			//{
			//	e = _eventBuffer.peek<SDL_Event>();
			//	_eventBuffer.pop<SDL_Event>();
			//	if (e.type == SDL_QUIT || e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
			//		_shouldClose = true;
			//	}

			//	switch (e.type)
			//	{
			//	case SDL_KEYDOWN:
			//	case SDL_KEYUP:
			//	{
			//		char buf[512] = "";
			//		sprintf_s(buf, "Key Event Key: %c Pressed: %i, Repeat: %i", (char)e.key.keysym.sym, e.key.state, e.key.repeat);
			//		PXE_INFO(buf);
			//	}
			//	break;

			//	case SDL_MOUSEMOTION:
			//	{
			//		char buf[512] = "";
			//		sprintf_s(buf, "Mouse Move: %i,%i", e.motion.x, e.motion.y);
			//		PXE_INFO(buf);
			//	}
			//	break;
			//	}

			//	// TODO manage other events
			//}
		}

		void NpWindow::setScene(PxeScene* scene)
		{
			if (_scene)
				_scene->drop();
			// TODO add "magic" values check like SDL?
			_scene = dynamic_cast<NpScene*>(scene);
			if (_scene) {
				_scene->grab();
			}
			else if (scene) {
				PXE_WARN("Scene parameter did not inherit from NpScene, Scene set to nullptr");
			}
		}

		void NpWindow::drawScene(PxeScene* scene)
		{
			// TODO add "magic" values check like SDL?
			NpScene* activeScene = dynamic_cast<NpScene*>(scene);
			if (!activeScene) activeScene = _scene;
			if (!activeScene) {
				PXE_WARN("Attempted to draw null scene");
				return;
			}

			acquireGlContext();
			int width;
			int height;
			SDL_GetWindowSizeInPixels(&_sdlWindow, &width, &height);
			glViewport(0, 0, width, height);
			std::list<PxeRenderBase*>& renderList = activeScene->getRenderList();
			if (renderList.empty()) return;
			glm::mat4 vp(glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 100.0f)* glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(0), glm::vec3(0, 1, 0)));
			PxeShader* activeShader = nullptr;
			PxeRenderMaterial* activeMaterial = nullptr;
			int32_t mvpLocation = -1;
			for (auto it = renderList.begin(); it != renderList.end(); ++it) {
				if ((*it)->getRenderSpace() == PxeRenderBase::RenderSpace::WORLD_SPACE) {
					PxeRenderObject& renderObject = static_cast<PxeRenderObject&>(**it);
					if (&renderObject.getRenderMaterial() != activeMaterial) {
						activeMaterial = &renderObject.getRenderMaterial();
						if (&(activeMaterial->getShader()) != activeShader) {
							activeShader = &(activeMaterial->getShader());
							activeShader->bind();
							mvpLocation = activeShader->getUniformLocation("u_MVP");
						}

						activeMaterial->applyMaterial();
					}

					activeShader->setUniformM4f(mvpLocation, vp * renderObject.getPositionMatrix());
					renderObject.render();
				}
#ifdef PXE_DEBUG
				else if ((*it)->getRenderSpace() == PxeRenderBase::RenderSpace::SCREEN_SPACE)
#else
				else
#endif // PXE_DEBUG
				{
					//TODO implement scene screen space rendering
				}
#ifdef PXE_DEBUG
			else {
				PXE_ERROR("Failed to render PxeRenderBase in Invalid RenderSpace, How did this manage to get into the render queue?");
			}
#endif // PXE_DEBUG


			}
			releaseGlContext();
		}

		PxeScene* NpWindow::getScene() const
		{
			return _scene;
		}

		const NpWindow::EventBuffer& NpWindow::getEventBuffer() const
		{
			return _eventBuffer;
		}

		NpWindow::EventBuffer& NpWindow::getEventBuffer()
		{
			return _eventBuffer;
		}

		uint32_t NpWindow::getSDLWindowId() const
		{
			return SDL_GetWindowID(&_sdlWindow);
		}
	}
}