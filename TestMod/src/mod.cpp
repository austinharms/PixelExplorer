#include "PE_mod_interface.h"
#include "TMOD_test_mod_imp.h"

PE_ModInterfaceFactoryFunction() {
	return test_mod::TestModImp::Create();
}
