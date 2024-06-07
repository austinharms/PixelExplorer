#ifndef TMOD_TEST_MOD_H_
#define TMOD_TEST_MOD_H_
#include "PE_defines.h"
#include "PE_mod_interface.h"

namespace test_mod {
	class TestMod : public pe::ModInterface {
	public:
		virtual ~TestMod();
		PE_NOCOPY(TestMod);

	protected:
		TestMod();
	};
}
#endif // !TMOD_TEST_MOD_H_
