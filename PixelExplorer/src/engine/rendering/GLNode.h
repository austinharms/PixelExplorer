#ifndef PIXELEXPLORER_ENGINE_RENDERING_GLNODE_H_
#define PIXELEXPLORER_ENGINE_RENDERING_GLNODE_H_
namespace pixelexplorer::engine::rendering {
	template<class T>
	class GLNode
	{
	public:
		inline GLNode();
		inline void removeNode();
		inline void insertNodeAfter(GLNode<T>* node);
		GLNode<T>* Next;
		GLNode<T>* Prev;
	};
}

#include "GLNode.inl"
#endif 