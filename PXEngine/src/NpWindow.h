#include <stdint.h>

#include "PxeWindow.h"
#include "PxeRingBuffer.h"
#include "SDL.h"
#include "PxeScene.h"

#ifndef PXENGINE_NONPUBLIC_WINDOW_H_
#define PXENGINE_NONPUBLIC_WINDOW_H_
namespace pxengine {
	namespace nonpublic {
		class NpEngineBase;
		class NpScene;

		class NpWindow : public PxeWindow
		{
		public:
			static const uint32_t WINDOW_EVENT_QUEUE_SIZE = 128;

			//############# PUBLIC API ##################

			// blocks the calling thread until the OpenGl context is ready for use with this window on the calling thread
			// note you MUST call releaseGlContext after you are done making OpenGl calls
			void acquireGlContext() override;

			// releases the OpenGl context for use on other threads and windows
			void releaseGlContext() override;

			// returns true if this window has currently acquired the OpenGl context
			bool acquiredGlContext() const override;

			// swaps the windows front and back frame buffer
			void swapFrameBuffer() override;

			// returns the internal SDL Window object
			const SDL_Window& getSDLWindow() const;

			// returns the internal SDL Window object
			SDL_Window& getSDLWindow();

			void setSwapInterval(int8_t interval) override;

			int8_t getSwapInterval() override;

			bool getShouldClose() const override;

			void resetShouldClose() override;

			void setWindowHidden(bool hidden) override;

			bool getWindowHidden() const override;

			// TODO add some sort of event system
			void pollEvents() override;

			void setScene(PxeScene* scene) override;

			void drawScene(PxeScene* scene) override;

			PxeScene* getScene() const override;

			void initializeAsset(PxeGLAsset& asset) override;

			//############# PRIVATE API ##################

			NpWindow(SDL_Window& window);
			virtual ~NpWindow();

			const PxeRingBuffer<SDL_Event, WINDOW_EVENT_QUEUE_SIZE>& getEventBuffer() const;

		protected:
			void onDelete() override;
			PxeRingBuffer<SDL_Event, WINDOW_EVENT_QUEUE_SIZE>& getEventBuffer();

		private:
			friend class NpEngineBase;

			SDL_Window& _sdlWindow;
			NpEngineBase* _engineBase;
			NpScene* _scene;
			PxeRingBuffer<SDL_Event, WINDOW_EVENT_QUEUE_SIZE> _eventBuffer;
			int8_t _swapInterval;
			bool _acquiredContext;
			bool _shouldClose;
			bool _hidden;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_WINDOW_H_
