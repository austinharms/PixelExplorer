namespace pixelexplorer {
	template<class T>
	DataBuffer<T>::DataBuffer(uint64_t length) : _length(length) {
		_buffer = (T*)malloc(length * sizeof(T));
	}

	template<class T>
	DataBuffer<T>::~DataBuffer() {
		free(_buffer);
	}

	template<class T>
	uint64_t DataBuffer<T>::getLength() const { return _length; }

	template<class T>
	uint64_t DataBuffer<T>::getSize() const { return _length * sizeof(T); }

	template<class T>
	T* DataBuffer<T>::getBufferPtr() const { return _buffer; }

	template<class T>
	T DataBuffer<T>::operator [] (int i) const { return _buffer[i]; }

	template<class T>
	T& DataBuffer<T>::operator [] (int i) { return _buffer[i]; }
}