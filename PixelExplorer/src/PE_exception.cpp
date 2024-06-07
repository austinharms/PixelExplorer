// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#include "PE_exception.h"
#include "PE_log.h"

namespace pe::internal {
	PE_STATIC_ASSERT(PE_ERROR_ENUM_VALUE_COUNT == 9, PE_TEXT("error_code_messages array out of date"));
	const char* const error_code_messages[PE_ERROR_ENUM_VALUE_COUNT] = {
		"", // PE_ERROR_NONE
		"GENERAL_ERROR", // PE_ERROR_GENERAL
		"OUT_OF_MEMORY", // OUT_OF_MEMORY
		"ALREADY_INITIALIZED", // ALREADY_INITIALIZED
		"NOT_FOUND", // NOT_FOUND
		"EMPTY", // EMPTY
		"INVALID_PARAMETERS", // INVALID_PARAMETERS
		"TOO_BIG", // TOO_BIG
		"NOT_INITIALIZED", // NOT_INITIALIZED
	};

	ErrorCodeException::ErrorCodeException(ErrorCode code) :
		std::exception(error_code_messages[code < PE_ERROR_ENUM_VALUE_COUNT ? code : PE_ERROR_GENERAL]),
		code_(code) {
		PE_DebugAssert(code < PE_ERROR_ENUM_VALUE_COUNT, PE_TEXT("Invalid ErrorCode passed to ErrorCodeException"));
	}

	PE_NODISCARD ErrorCode ErrorCodeException::GetErrorCode() const
	{
		return code_;
	}

	ErrorCode BoundaryExceptionHandler() {
		try {
			throw;
		}
		catch (const ErrorCodeException& e) {
			PE_LogWarn(PE_TEXT("Boundary error code exception: %s"), e.what());
			return e.GetErrorCode();
		}
		catch (const std::bad_alloc& e) {
			PE_LogWarn(PE_TEXT("Boundary bad alloc exception: %s"), e.what());
			return PE_ERROR_OUT_OF_MEMORY;
		}
		catch (const std::exception& e) {
			PE_LogCritical(PE_TEXT("Unhandled exception occurred: %s"), e.what());
			return PE_ERROR_GENERAL;
		}
		catch (...) {
			PE_LogCritical(PE_TEXT("Unknown exception occurred"));
			return PE_ERROR_GENERAL;
		}

		return PE_ERROR_NONE;
	}

	void KnownExceptionHandler() {
		try {
			throw;
		}
		catch (const ErrorCodeException& e) {
			PE_LogError(PE_TEXT("Exception occurred (ErrorCode): %s"), e.what());
		}
		catch (const std::bad_alloc& e) {
			PE_LogError(PE_TEXT("Exception occurred (bad_alloc): %s"), e.what());
		}
		catch (const std::exception& e) {
			PE_LogCritical(PE_TEXT("Unhandled exception occurred: %s"), e.what());
		}
		catch (...) {
			PE_LogCritical(PE_TEXT("Unknown exception occurred"));
		}
	}
}
