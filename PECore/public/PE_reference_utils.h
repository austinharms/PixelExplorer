#ifndef PE_REFERENCE_UTILS_H_
#define PE_REFERENCE_UTILS_H_
#include "PE_defines.h"
#include "PE_referenced.h"
#include "PE_type_asserts.h"

namespace pe {
	// Utility class that automatically calls grab and drop on the stored value, similar to std::shared_ptr
	// type T must inherit the Referenced class
	template<class T>
	class ReferencePtr {
		typedef T ValueType;
		ReferencePtr() : value_ptr_(nullptr) {
			PE_INHERIT_ASSERT(Referenced, ValueType);
		}

		ReferencePtr(ValueType* value) : value_ptr_(value) {
			PE_INHERIT_ASSERT(Referenced, ValueType);
			if (value_ptr_) value_ptr_->grab();
		}

		// When preventInitialGrab is true grab will not be called on value
		// Use with caution as this may cause unexpected behavior if used incorrectly
		ReferencePtr(ValueType* value, bool preventInitialGrab) : value_ptr_(value) {
			PE_INHERIT_ASSERT(Referenced, ValueType);
			if (value_ptr_ && !preventInitialGrab) value_ptr_->grab();
		}

		ReferencePtr(const ReferencePtr& other) : value_ptr_(nullptr) {
			*this = other;
		}

		ReferencePtr(ReferencePtr&& other) : value_ptr_(other.value_ptr_) {
			other.value_ptr_ = nullptr;
		}

		~ReferencePtr() {
			*this = nullptr;
		}

		PE_NODISCARD ValueType* GetValue() const { return value_ptr_; }

		explicit operator ValueType* () const { return value_ptr_; }

		ValueType* operator->() const { return value_ptr_; }

		ValueType& operator*() const { return *value_ptr_; }

		ReferencePtr& operator=(ReferencePtr& other) {
			if (this != &other) {
				if (value_ptr_) value_ptr_->drop();
				if ((value_ptr_ = other.value_ptr_)) value_ptr_->grab();
			}

			return *this;
		}

		ReferencePtr& operator=(ValueType* other) {
			if (value_ptr_) value_ptr_->drop();
			if ((value_ptr_ = other)) value_ptr_->grab();
			return *this;
		}

		ReferencePtr& operator=(ReferencePtr&& other) PE_NOEXCEPT {
			if (this != &other) {
				value_ptr_ = other.value_ptr_;
				other.value_ptr_ = nullptr;
			}

			return *this;
		}

	private:
		ValueType* value_ptr_;
	};
}
#endif // !PE_REFERENCE_UTILS_H_