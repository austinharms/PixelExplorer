// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#ifndef PE_ERRORS_H_
#define PE_ERRORS_H_
namespace pe {
	enum ErrorCode : unsigned int
	{
		PE_ERROR_NONE = 0,
		PE_ERROR_GENERAL,
		PE_ERROR_OUT_OF_MEMORY,
		PE_ERROR_ALREADY_INITIALIZED,
		PE_ERROR_NOT_FOUND,
		PE_ERROR_EMPTY,
		PE_ERROR_INVALID_PARAMETERS,
		PE_ERROR_TOO_BIG,
		PE_ERROR_NOT_INITIALIZED,
		PE_ERROR_ENUM_VALUE_COUNT,
	};
}
#endif // !PE_ERRORS_H_
