#ifndef PE_ERRORS_H_
#define PE_ERRORS_H_
namespace pecore {
	enum PE_ErrorCode
	{
		PE_ERROR_NONE = 0,
		PE_ERROR_OUT_OF_MEMORY,
		PE_ERROR_ALREADY_INITIALIZED,
	};
}
#endif // !PE_ERRORS_H_