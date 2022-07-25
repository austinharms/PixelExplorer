#include <assert.h>

#include "RenderWindow.fwd.h"
#include "GLObject.fwd.h"
#include "RefCount.h"

#ifndef PIXELEXPLORE_RENDERING_GLOBJECT_H_
#define PIXELEXPLORE_RENDERING_GLOBJECT_H_
namespace pixelexplorer::rendering {
	struct GLObjectNode {
		GLObjectNode* prev;
		GLObjectNode* next;
		GLObject* value;
	};

	class GLObject : public RefCount
	{
	public:
		GLObject();
		virtual ~GLObject();
		bool drop() override;
		inline RenderWindow* getRenderWindow() const { return _attachedWindow; }
		inline bool getInitialized() const { return _initialized; }
		virtual inline bool getRemoveFlag() const { return _remove; }
		virtual void setRemoveFlag();

	protected:
		virtual inline void onInitialize() {}
		virtual inline void onTerminate() {}

	private:
		friend class RenderWindow;
		RenderWindow* _attachedWindow;
		GLObjectNode _node;
		bool _initialized;
		bool _remove;
		void preInit(RenderWindow* window);
		void initialize();
		void terminate();
		void removeNode();
		void insertNodeBetween(GLObjectNode* prev, GLObjectNode* next);
	};
}

#endif // !PIXELEXPLORE_RENDERING_GLOBJECT_H_
