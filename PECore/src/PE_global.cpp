#include "PE_global.h"

namespace pe::internal {
	PE_NODISCARD GlobalData& GetGlobalData() {
		static GlobalData data{};
		return data;
	}
}
