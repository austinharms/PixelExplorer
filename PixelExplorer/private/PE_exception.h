// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#ifndef PE_PRIVATE_EXCEPTION_H_
#define PE_PRIVATE_EXCEPTION_H_
#include <exception>
#include "PE_defines.h"
#include "PE_errors.h"

namespace pe::internal {
	extern const char* const error_code_messages[PE_ERROR_ENUM_VALUE_COUNT];
	class ErrorCodeException : public std::exception
	{
	public:
		ErrorCodeException(ErrorCode code);
		virtual ~ErrorCodeException() = default;
		PE_NODISCARD ErrorCode GetErrorCode() const;

	private:
		ErrorCode code_;
	};

	// Handles known/expected exceptions by converting them into an ErrorCode
	// if the exception is not known/expected the applications is aborted
	PE_EXTERN PE_NODISCARD ErrorCode PE_CALL BoundaryExceptionHandler();

	// Handles known/expected exceptions by logging the error
	// if the exception is not known/expected the applications is aborted
	extern void PE_CALL KnownExceptionHandler();
}
#endif // !PE_PRIVATE_EXCEPTION_H_
