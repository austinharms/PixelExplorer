#include "ChunkRenderMeshFactory.h"

#include "common/Logger.h"
#include "common/OSHelpers.h"
#include "engine/rendering/Shader.h"

namespace pixelexplorer::game::chunk {
	ChunkRenderMeshFactory::ChunkRenderMeshFactory(engine::rendering::RenderWindow& window) : _window(window)
	{
		_window.grab();

		_chunkMaterial = new(std::nothrow) engine::rendering::Material();
		if (_chunkMaterial == nullptr)
			Logger::fatal(__FUNCTION__" failed to allocate chunk Material");
		_window.registerGLObject(*_chunkMaterial);

		engine::rendering::Shader* chunkShader = _window.loadShader(OSHelper::getAssetPath("shaders") / "chunk.shader");
		if (chunkShader == nullptr) Logger::fatal(__FUNCTION__" failed to allocate chunk Shader");
		_chunkMaterial->addDependency(*chunkShader);
		chunkShader->drop();
		chunkShader = nullptr;
	}

	ChunkRenderMeshFactory::~ChunkRenderMeshFactory()
	{
		emptyRenderMeshQueue();
		if (!_chunkMaterial->drop())
			Logger::warn(__FUNCTION__" chunk render mesh factory Material not dropped, make sure all other references to the Material are dropped");

		_window.drop();
	}

	ChunkRenderMesh* ChunkRenderMeshFactory::getRenderMesh()
	{
		ChunkRenderMesh* renderMesh = nullptr;
		_renderMeshQueueMutex.lock();
		if (!_renderMeshQueue.empty()) {
			renderMesh = _renderMeshQueue.front();
			_renderMeshQueue.pop();
		}

		_renderMeshQueueMutex.unlock();
		if (renderMesh == nullptr) {
			renderMesh = new(std::nothrow) ChunkRenderMesh(*_chunkMaterial, &_window);
			if (renderMesh == nullptr)
				Logger::error(__FUNCTION__" failed to allocate ChunkRenderMesh");
		}

		if (renderMesh) _window.addGLRenderObject(*renderMesh);
		return renderMesh;
	}

	void ChunkRenderMeshFactory::addRenderMesh(ChunkRenderMesh& mesh)
	{
		mesh.grab();
		if (mesh.inRenderQueue())
			mesh.getRenderWindow()->removeGLRenderObject(mesh);
		_renderMeshQueueMutex.lock();
		_renderMeshQueue.emplace(&mesh);
		_renderMeshQueueMutex.unlock();
	}

	engine::rendering::Material& ChunkRenderMeshFactory::getMaterial() const
	{
		return *_chunkMaterial;
	}

	void ChunkRenderMeshFactory::emptyRenderMeshQueue()
	{
		_renderMeshQueueMutex.lock();
		while (!_renderMeshQueue.empty()) {
			if (!_renderMeshQueue.front()->drop())
				Logger::warn(__FUNCTION__" chunk render mesh factory ChunkRenderMesh not dropped, make sure all other references to the ChunkRenderMesh are dropped");
			_renderMeshQueue.pop();
		}

		_renderMeshQueueMutex.unlock();
	}
}
