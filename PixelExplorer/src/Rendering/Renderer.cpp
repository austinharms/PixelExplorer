#include "Renderer.h"

#include <iostream>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/vec3.hpp"

std::unordered_map<GLFWwindow*, Renderer*> Renderer::s_renderers;

Renderer::Renderer(int32_t width, int32_t height, const char* title, float FOV, uint32_t FPSLimit) {
	//Init Renderer Members
	_position = glm::vec3(0);
	_rotation = glm::vec3(0);
	_forward = glm::vec3(0);
	_screenHeight = height;
	_screenWidth = width;
	_FOV = FOV;
	_lastFrame = 0;
	_FPSTimer = 0;
	_FPS = 0;
	_frameCounter = 0;
	_deltaTime = 0;
	_cursorX = 0;
	_cursorY = 0;
	_cursorChangeX = 0;
	_cursorChangeY = 0;
	_windowActive = true;
	updatePrjectionMatrix();
	updateForwardVector();

	//Init GLFW/Render Window
	assert(glfwInit());
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	_window = glfwCreateWindow(width, height, title, NULL, NULL);
	assert(_window);
	glfwMakeContextCurrent(_window);
	glfwSetWindowFocusCallback(_window, Renderer::s_windowFocus);
	glfwSetFramebufferSizeCallback(_window, Renderer::s_windowResize);
	glfwRequestWindowAttention(_window);

	//Need to do this after GLFW init as they depend on it
	setFPSLimit(FPSLimit);
	setCursorHidden(false);

	//Init OpenGL
	assert(glewInit() == GLEW_OK);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(Renderer::GLErrorCallback, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Enable Wireframe
	// glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); //Disable Wireframe

	//Add Renderer Instance to List
	Renderer::s_renderers.emplace(_window, this);
}

void Renderer::addRenderable(Renderable* renderable) {
	std::lock_guard<std::mutex> locker(_renderLock);
	renderable->grab();
	_renderableObjects.emplace_back(renderable);
	_renderableObjects.sort([](const Renderable* r1, const Renderable* r2) {
		if (r1->getMaterial()->getShader()->getGLID() > r2->getMaterial()->getShader()->getGLID()) {
			return true;
		}
		else if (r1->getMaterial()->getShader()->getGLID() == r2->getMaterial()->getShader()->getGLID()) {
			return r1->getMaterial()->getId() > r2->getMaterial()->getId();
		}

		return false;
		});
}

void Renderer::removeRenderable(uint32_t id) {
	std::lock_guard<std::mutex> locker(_renderLock);
	std::remove_if(_renderableObjects.begin(), _renderableObjects.end(),
		[&id](Renderable* renderable) {
			if (renderable->getId() == id) {
				renderable->drop();
				return true;
			}

			return false;
		});
}

void Renderer::drawFrame() {
	double currentFrame = glfwGetTime();
	_deltaTime = currentFrame - _lastFrame;
	_lastFrame = currentFrame;
	if (currentFrame - _FPSTimer >= 0.250) {
		_FPS = _frameCounter * 4;
		_frameCounter = 0;
		_FPSTimer = currentFrame;
		std::cout << "FPS: " << _FPS << std::endl;
	}

	if (_windowActive) {
		if (_cursorHidden) {
			glfwGetCursorPos(_window, &_cursorChangeX, &_cursorChangeY);
			glfwSetCursorPos(_window, 0, 0);
		}
		else {
			_cursorChangeX = _cursorX;
			_cursorChangeY = _cursorY;
			glfwGetCursorPos(_window, &_cursorX, &_cursorY);
			_cursorChangeX = _cursorX - _cursorChangeX;
			_cursorChangeY = _cursorY - _cursorChangeY;
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
		Material* currentMaterial = nullptr;
		Shader* currentShader = nullptr;
		std::lock_guard<std::mutex> drawLock(_renderLock);
		std::list<Renderable*>::iterator iter = _renderableObjects.begin();
		std::list<Renderable*>::iterator end = _renderableObjects.end();

		glm::mat4 vp = _projection * glm::lookAt(_position, _position + _forward,
			glm::vec3(0, 1, 0));
		while (iter != end) {
			Renderable* renderable = *iter;
			if (renderable->getRendererDropFlag()) {
				iter = _renderableObjects.erase(iter);
				renderable->drop();
			}
			else {
				if (renderable->onPreRender(_deltaTime, glm::value_ptr(_position),
					glm::value_ptr(_rotation))) {
					Material* renderableMaterial = renderable->getMaterial();
					if (renderableMaterial != currentMaterial) {
						if (currentMaterial != nullptr) currentMaterial->drop();
						renderableMaterial->grab();
						currentMaterial = renderableMaterial;
						Shader* renderableShader = renderableMaterial->getShader();
						if (renderableShader != currentShader) {
							if (currentShader != nullptr) {
								currentShader->unbind();
								currentShader->drop();
							}

							renderableShader->grab();
							currentShader = renderableShader;
							renderableShader->bind();
						}
						renderableMaterial->onPostBind();
					}

					currentShader->setUniformm4("u_MVP", vp * renderable->getTransform());
					renderable->onRender();
				}
				++iter;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		if (currentShader != nullptr) {
			currentShader->unbind();
			currentShader->drop();
		}

		if (currentMaterial != nullptr) currentMaterial->drop();
	}

	glfwSwapBuffers(_window);
	glfwPollEvents();
	++_frameCounter;
}

bool Renderer::getKeyPressed(int32_t key) {
	return glfwGetKey(_window, key) == GLFW_PRESS;
}

void Renderer::setFPSLimit(int32_t limit) {
	_FPSLimit = limit;
	glfwSwapInterval(_FPSLimit);
}

void Renderer::setCursorHidden(bool hidden) {
	_cursorHiddenTarget = hidden;
	if (_windowActive) {
		_cursorHidden = _cursorHiddenTarget;
		if (_cursorHidden) {
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		glfwSetCursorPos(_window, 0, 0);
		_cursorChangeX = 0;
		_cursorChangeY = 0;
		_cursorX = 0;
		_cursorY = 0;
	}
}

Renderer::~Renderer() {
	std::lock_guard<std::mutex> locker(_renderLock);
	for (Renderable* r : _renderableObjects) r->drop();
	Renderer::s_renderers.erase(_window);
	glfwDestroyWindow(_window);
}

void Renderer::updateForwardVector() {
	float cosY = cosf(_rotation.y * 0.5f);
	float sinY = sinf(_rotation.y * 0.5f);
	float cosX = cosf(_rotation.x * 0.5f);
	float sinX = sinf(_rotation.x * 0.5f);

	float x = cosX * cosY;
	float y = -sinX * sinY;
	float z = cosX * sinY;
	float w = sinX * cosY;

	_forward.x = 2 * (x * z + w * y);
	_forward.y = 2 * (y * z - w * x);
	_forward.z = 1 - 2 * (x * x + y * y);
}

void Renderer::updatePrjectionMatrix()
{
	_projection = glm::perspective(glm::radians(_FOV), (float)_screenWidth / (float)_screenHeight, 0.1f, 1000.0f);
}

void Renderer::windowResize(int width, int height)
{
	std::cout << "Window Size Changed: " << width << " " << height << std::endl;
	_screenWidth = width;
	_screenHeight = height;
	updatePrjectionMatrix();
}

void Renderer::windowFocus(bool focused) {
	_windowActive = focused;
	if (_windowActive) {
		setCursorHidden(_cursorHiddenTarget);
	}
	else if (_cursorHidden) {
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		_cursorHidden = false;
	}
}

void GLAPIENTRY Renderer::GLErrorCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* message,
	const void* userParam) {
	if (type != 0x8251)
		fprintf(stderr,
			"GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
			severity, message);
}

void Renderer::s_windowFocus(GLFWwindow* window, int focused)
{
	Renderer::s_renderers[window]->windowFocus(focused);
}

void Renderer::s_windowResize(GLFWwindow* window, int width, int height)
{
	Renderer::s_renderers[window]->windowResize(width, height);
}
