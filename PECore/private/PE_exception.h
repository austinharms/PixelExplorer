#ifndef PE_EXCEPTION_H_
#define PE_EXCEPTION_H_
#include "PE_defines.h"
#include "PE_errors.h"
#include "PE_log.h"
#include <exception>

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

	extern ErrorCode BoundaryExceptionHandler();
}
#endif // !PE_EXCEPTION_H_
