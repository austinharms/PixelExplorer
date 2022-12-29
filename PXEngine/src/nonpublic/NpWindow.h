#include <stdint.h>

#include "PxeWindow.h"
#include "PxeRingBuffer.h"
#include "SDL.h"
#include "PxeScene.h"

#ifndef PXENGINE_NONPUBLIC_WINDOW_H_
#define PXENGINE_NONPUBLIC_WINDOW_H_
namespace pxengine {
	namespace nonpublic {
		class NpScene;

		class NpWindow : public PxeWindow
		{
		public:
			static const uint32_t WINDOW_EVENT_QUEUE_SIZE = 128;

			typedef PxeRingBuffer<SDL_Event, NpWindow::WINDOW_EVENT_QUEUE_SIZE> EventBuffer;

			//############# PUBLIC API ##################

			void acquireGlContext() override;

			void releaseGlContext() override;

			bool acquiredGlContext() const override;

			void swapFrameBuffer() override;

			void setSwapInterval(int8_t interval) override;

			int8_t getSwapInterval() override;

			bool getShouldClose() const override;

			void resetShouldClose() override;

			void setWindowShown(bool show) override;

			bool getWindowShown() const override;

			// TODO add some sort of event system
			bool pollEvents(SDL_Event* event) override;

			void setScene(PxeScene* scene) override;

			void drawScene(PxeScene* scene) override;

			PxeScene* getScene() const override;


			//############# PRIVATE API ##################

			NpWindow(SDL_Window& window);

			virtual ~NpWindow();

			const EventBuffer& getEventBuffer() const;

			const SDL_Window& getSDLWindow() const;

			SDL_Window& getSDLWindow();

			uint32_t getSDLWindowId() const;

			EventBuffer& getEventBuffer();

			void setShouldClose();

		private:
			SDL_Window& _sdlWindow;
			NpScene* _scene;
			EventBuffer _eventBuffer;
			int8_t _swapInterval;
			bool _shouldClose;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_WINDOW_H_
