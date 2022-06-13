#include "GLObject.h"
#include "Logger.h"

namespace pixelexplorer::rendering {
	GLObject::GLObject()
	{
		_currentWindow = nullptr;
		_objectInitialized = false;
	}

	GLObject::~GLObject() {
		if (_currentWindow != nullptr)
			Logger::error("GLObject destroyed without being removed from window");

		if (_objectInitialized)
			Logger::error("GLObject not deallocated");
	}

	RenderWindow* GLObject::getRenderWindow()
	{
		return _currentWindow;
	}

	void GLObject::initGLObjects()
	{
		if (_objectInitialized) { 
			Logger::warn(__FUNCTION__" was already called"); 
			return;
		}

		if (!requiresGLObjects()) {
			Logger::warn(__FUNCTION__" was called on object not requiring GLObjects");
			return;
		}

		_objectInitialized = true;
		createGLObjects();
	}

	void GLObject::uninitGLObjects()
	{
		if (!_objectInitialized) {
			Logger::warn(__FUNCTION__" was called on object without GLObjects");
			return;
		}

		_objectInitialized = false;
		destroyGLObjects();
	}


}
