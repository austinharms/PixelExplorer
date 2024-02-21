#include "PE_log.h"

int main()
{
	PE_InitLog();
	PE_LogInfo(PE_LOG_CATEGORY_TEST, PE_TEXT("Hello World (PETest)"));
	return 0;
}
