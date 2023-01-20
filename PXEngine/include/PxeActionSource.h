#ifndef PXENGINE_ACTION_SOURCE
#define PXENGINE_ACTION_SOURCE
#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	namespace nonpublic {
		class NpActionManager;
	}

	// TODO Comment file
	class PxeActionSource : public PxeRefCount
	{
	public:
		PxeActionSource() = default;
		virtual ~PxeActionSource() = default;
		virtual PXE_NODISCARD float getValue() const = 0;
		virtual PXE_NODISCARD PxeActionSourceCode getSourceCode() const = 0;
	};
}
#endif // !PXENGINE_ACTION_SOURCE
