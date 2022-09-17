namespace pixelexplorer::engine::rendering {
	template<class T>
	GLNode<T>::GLNode() {
		Next = nullptr;
		Prev = nullptr;
	}

	template<class T>
	void GLNode<T>::removeNode() {
		if (Prev != nullptr)
			Prev->Next = Next;
		if (Next != nullptr)
			Next->Prev = Prev;
		Prev = nullptr;
		Next = nullptr;
	}

	template<class T>
	void GLNode<T>::insertNodeAfter(GLNode<T>* node) {
		removeNode();
		if (node == nullptr) return;
		Prev = node;
		Next = node->Next;
		node->Next = this;
		if (Next != nullptr)
			Next->Prev = this;
	}
}