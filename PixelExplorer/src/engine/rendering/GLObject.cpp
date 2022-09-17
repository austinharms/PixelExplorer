#include "GLObject.h"
#include "common/Logger.h"
#include "RenderWindow.h"

namespace pixelexplorer::engine::rendering {
	GLObject::GLObject(int16_t priority)
	{
		_priority = priority;
		_attachedWindow = nullptr;
		_initialized = false;
		_dirty = false;
	}

	GLObject::~GLObject() {
		// this should never happen, but just in case log it
		if (_attachedWindow != nullptr) {
			if (_initialized) {
				Logger::error(__FUNCTION__" GLObject destroyed without being removed from window, GLObjects not deallocated");
			}
			else {
				Logger::error(__FUNCTION__" GLObject destroyed without being removed from window");
			}
		}
	}

	bool GLObject::drop() {
		bool dropped = RefCount::drop();
		if (!dropped && getRefCount() == 1 && _attachedWindow)
			return _attachedWindow->terminateGLObject(*this);
		return dropped;
	}

	void GLObject::addDependency(GLObject& dependency, int16_t priority)
	{
		dependency.grab();
		if (hasDependency(dependency)) {
			Logger::warn(__FUNCTION__" called with duplicate dependency");
			dependency.drop();
			return;
		}

		if (!dependency.getAttached()) {
			Logger::warn(__FUNCTION__" added unregistering dependency, registering dependency");
			getRenderWindow()->registerGLObject(dependency);
		}
		else if (dependency.getRenderWindow() != getRenderWindow()) {
			if (getAttached()) {
				Logger::error(__FUNCTION__" called with dependency registered to a different RenderWindow, dependency not added");
				dependency.drop();
				return;
			}
			else {
				Logger::warn(__FUNCTION__" called with dependency registered to a RenderWindow without being registered to a RenderWindow");
			}
		}

		auto it = _dependencies.begin();
		while (it != _dependencies.end() && (*it)->_priority < priority) ++it;
		_dependencies.emplace(it, &dependency);
	}

	void GLObject::removeDependency(GLObject& dependency) {
		for (auto it = _dependencies.begin(); it != _dependencies.end(); ++it) {
			if ((*it) == &dependency) {
				(*it)->drop();
				_dependencies.erase(it);
				return;
			}
		}

		Logger::warn(__FUNCTION__" failed to remove dependency, dependency did not exist");
		return;
	}

	void GLObject::clearDependencies() {
		auto it = _dependencies.begin();
		while (it != _dependencies.end()) {
			(*it)->drop();
			it = _dependencies.erase(it);
		}
	}

	bool GLObject::hasDependency(const GLObject& dependency, bool recursive) const {
		for (const GLObject* dep : _dependencies) {
			if (dep == &dependency) return true;
		}

		if (recursive) {
			for (const GLObject* dep : _dependencies) {
				if (dep->hasDependency(dependency, true)) return true;
			}
		}

		return false;
	}

	void GLObject::attach(RenderWindow& window) {
		_attachedWindow = &window;
		onAttach();
	}

	void GLObject::initialize()
	{
		if (_initialized) {
			Logger::warn(__FUNCTION__" was already called");
			return;
		}

		// this should never happen, but just in case log it
		if (_attachedWindow == nullptr) {
			Logger::error(__FUNCTION__" was called with NULL RenderWindow, object not initialize");
			return;
		}

		onInitialize();
		_initialized = true;
	}

	void GLObject::terminate()
	{
		// this should never happen, but just in case log it
		if (!_initialized && !getAttached()) {
			Logger::error(__FUNCTION__" was called on uninitialized object");
			return;
		}

		
		if (_initialized)
		{
			_initialized = false;
			onTerminate();
		}

		clearDependencies();
		_attachedWindow = nullptr;
	}

	void GLObject::update() {
		// this should never happen, but just in case log it
		if (!getAttached()) {
			Logger::error(__FUNCTION__" was called on uninitialized object");
			return;
		}

		auto it = _dependencies.begin();
		while (it != _dependencies.end() && (*it)->_priority < 0)
		{
			(*it)->update();
			++it;
		}

		if (!_initialized) initialize();
		onUpdate();
		while (it != _dependencies.end())
		{
			(*it)->update();
			++it;
		}
	}
}
