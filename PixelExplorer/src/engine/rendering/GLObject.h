#include <list>
#include <stdint.h>

#include "RenderWindow.fwd.h"
#include "GLObject.fwd.h"
#include "common/RefCount.h"
#include "GLNode.h"

#ifndef PIXELEXPLORER_ENGINE_RENDERING_GLOBJECT_H_
#define PIXELEXPLORER_ENGINE_RENDERING_GLOBJECT_H_
namespace pixelexplorer::engine::rendering {
	class GLObject : private GLNode<GLObject>, public RefCount
	{
	public:
		GLObject(int16_t priority = -1);
		virtual ~GLObject();
		bool drop() override;

		inline RenderWindow* getRenderWindow() const { return _attachedWindow; }

		inline bool getInitialized() const { return _initialized; }

		inline bool getAttached() const { return _attachedWindow != nullptr; }

		inline int16_t getPriority() const { return _priority; }

	protected:
		// add dependency and override the priority
		void addDependency(GLObject* dependency, int16_t priority);
		void removeDependency(GLObject* dependency);
		// check if this object is dependent on dependency, recursive checks all dependencies for dependency as well
		bool hasDependency(GLObject* dependency, bool recursive = false) const;
		void clearDependencies();

		inline void addDependency(GLObject* dependency) { 
			if (dependency == nullptr) return; 
			addDependency(dependency, dependency->_priority);
		}

		// get the dirty flag (dirty flag unused by GLObject methods)
		bool getDirty() const { return _dirty; }

		// set the dirty flag to true (dirty flag unused by GLObject methods)
		void setDirty() { _dirty = true; }

		// set the dirty flag to false (dirty flag unused by GLObject methods)
		void clearDirty() { _dirty = false; }

		// set the dirty flag to dirty value (dirty flag unused by GLObject methods)
		void setDirty(bool dirty) { _dirty = dirty; }

		// this will not update existing dependencies
		inline void setPriority(int16_t value) { _priority = value; }

		// called after attached window is set, no thread guarantee
		virtual inline void onAttach() {}

		// guaranteed to be called on the render thread by the _attachedWindow object before first update (will not get called if object never updates), called before initialized flag is set
		virtual inline void onInitialize() {}

		// guaranteed to be called on the render thread
		virtual inline void onUpdate() {}

		// guaranteed to be called on the render thread before dependencies and _attachedWindow are cleared, will not be called if onInitialize was never called, called after the initialized flag is cleared
		virtual inline void onTerminate() {}

	private:
		friend class RenderWindow;
		RenderWindow* _attachedWindow;
		std::list<GLObject*> _dependencies;
		// smaller goes first, negative values cause updates pre onUpdate, positive (including 0) numbers cause updates post onUpdate
		int16_t _priority;
		bool _initialized;
		bool _dirty;
		void attach(RenderWindow* window);
		void initialize();
		void terminate();
		void update();
	};
}

#endif // !PIXELEXPLORE_RENDERING_GLOBJECT_H_
