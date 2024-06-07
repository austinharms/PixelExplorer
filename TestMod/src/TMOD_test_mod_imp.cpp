#include "TMOD_test_mod_imp.h"
#include <new>
#include "PE_log.h"

namespace test_mod {
	TestMod::TestMod() { }
	TestMod::~TestMod() { }

	PE_NODISCARD TestModImp* TestModImp::Create()
	{
		return new(std::nothrow) TestModImp();
	}

	TestModImp::TestModImp() :
		details_{ 0, PE_TEXT("TestMod"), PE_TEXT("An example test mod") } {
	}

	TestModImp::~TestModImp() { }

	const TestModImp::Details& TestModImp::GetDetails() {
		return details_;
	}

	void TestModImp::Init() {
		PE_LogDebug(PE_TEXT("TestMod Init"));
	}

	void TestModImp::OnDrop() {
		PE_LogDebug(PE_TEXT("TestMod Drop"));
		delete this;
	}
}
