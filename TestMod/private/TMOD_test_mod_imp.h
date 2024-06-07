#ifndef TMOD_TEST_MOD_IMP_H_
#define TMOD_TEST_MOD_IMP_H_
#include "PE_defines.h"
#include "TMOD_test_mod.h"

namespace test_mod {
	class TestModImp : public TestMod{
	public:
		static PE_NODISCARD TestModImp* Create();

		const Details& GetDetails() PE_OVERRIDE;
		void Init() PE_OVERRIDE;
		void OnDrop() PE_OVERRIDE;
		virtual ~TestModImp();
		PE_NOCOPY(TestModImp);

	protected:
		TestModImp();

	private:
		const Details details_;
	};
}
#endif // !TMOD_TEST_MOD_IMP_H_
