#include "GLObject.h"
#include "Logger.h"
#include "RenderWindow.h"

namespace pixelexplorer::rendering {
	GLObject::GLObject()
	{
		_attachedWindow = nullptr;
		_initialized = false;
		_remove = false;
		_node.prev = nullptr;
		_node.next = nullptr;
		_node.value = this;
	}

	GLObject::~GLObject() {
		if (_attachedWindow != nullptr) {
			if (_initialized) {
				Logger::error("GLObject destroyed without being removed from window, GLObjects not deallocated");
			}
			else {
				Logger::error("GLObject destroyed without being removed from window");
			}
		}
	}
	
	bool GLObject::drop() {
		if (getRefCount() == 2 && _attachedWindow)
			setRemoveFlag();
		return RefCount::drop();
	}

	void GLObject::setRemoveFlag()
	{
		if (_remove) return;
		if(_attachedWindow == nullptr) {
			Logger::warn(__FUNCTION__" was called on uninitialized object");
			return;
		}

		_remove = true;
		_attachedWindow->removeGLObject(this);
	}

	void GLObject::preInit(RenderWindow* window) {
		_attachedWindow = window;
	}

	void GLObject::initialize()
	{
		if (_initialized) {
			Logger::warn(__FUNCTION__" was already called");
			return;
		}

		if (_attachedWindow == nullptr) {
			Logger::warn(__FUNCTION__" was called with NULL RenderWindow, object not initialize");
			return;
		}

		onInitialize();
		_initialized = true;
	}

	void GLObject::terminate()
	{
		if (!_initialized && _attachedWindow == nullptr) {
			Logger::warn(__FUNCTION__" was called on uninitialized object");
			return;
		}

		if (_initialized) onTerminate();
		_initialized = false;
		_attachedWindow = nullptr;
	}

	void GLObject::removeNode()
	{
		if (_node.prev != nullptr)
			_node.prev->next = _node.next;
		if (_node.next != nullptr)
			_node.next->prev = _node.prev;

		_node.next = nullptr;
		_node.prev = nullptr;
	}

	void GLObject::insertNodeBetween(GLObjectNode* prev, GLObjectNode* next)
	{
		_node.prev = prev;
		if (prev != nullptr)
			prev->next = &_node;

		_node.next = next;
		if (next != nullptr)
			next->prev = &_node;
	}
}
