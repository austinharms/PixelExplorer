#ifndef PXENGINE_ACTION_SOURCE
#define PXENGINE_ACTION_SOURCE
#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	class PxeInputManager;

	// TODO Comment file
	class PxeActionSource : public PxeRefCount
	{
	public:
		PXE_NODISCARD float getValue() const;
		PXE_NODISCARD PxeActionSourceCode getSourceCode() const;
		virtual ~PxeActionSource();
		PXE_NOCOPY(PxeActionSource);

	private:
		friend class PxeInputManager;
		PxeActionSource(PxeActionSourceCode code);
		PXE_PRIVATE_IMPLEMENTATION_START
		void setValue(float value);
		PXE_PRIVATE_IMPLEMENTATION_END

		const PxeActionSourceCode _sourceCode;
		float _value;
	};
}
#endif // !PXENGINE_ACTION_SOURCE
