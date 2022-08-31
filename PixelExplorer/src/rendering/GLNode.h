#ifndef PX_RENDERING_GLNODE_H_
#define PX_RENDERING_GLNODE_H_

template<class T>
class GLNode
{
public:
	inline GLNode() {
		next = nullptr;
		prev = nullptr;
	}

	template<class T>
	inline void removeNode() {
		if (prev != nullptr)
			prev->next = next;
		if (next != nullptr)
			next->prev = prev;
		prev = nullptr;
		next = nullptr;
	}

	template<class T>
	inline void insertNodeAfter(GLNode<T>* node) {
		removeNode<T>();
		if (node == nullptr) return;
		prev = node;
		next = node->next;
		node->next = this;
		if (next != nullptr)
			next->prev = this;
	}

	GLNode<T>* next;
	GLNode<T>* prev;
};
#endif 