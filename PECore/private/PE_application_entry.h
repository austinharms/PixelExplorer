#ifndef PE_APPLICATION_ENTRY_H_
#define PE_APPLICATION_ENTRY_H_
#include "PE_defines.h"
namespace pe::internal {
	PE_EXTERN_C PE_API int PE_CALL application_entry(int argc, char** argv);
}
#endif // !PE_APPLICATION_ENTRY_H_
