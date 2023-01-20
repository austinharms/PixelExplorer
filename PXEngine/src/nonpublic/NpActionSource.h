#ifndef PXENGINE_NONPUBLIC_ACTION_SOURCE_H_
#define PXENGINE_NONPUBLIC_ACTION_SOURCE_H_
#include "PxeTypes.h"
#include "PxeActionSource.h"

namespace pxengine {
	namespace nonpublic {
		class NpActionSource : public PxeActionSource
		{
		public:
			//############# PxeActionSource API ##################

			PXE_NODISCARD float getValue() const override;
			PXE_NODISCARD PxeActionSourceCode getSourceCode() const override;


			//############# PRIVATE API ##################

			NpActionSource(PxeActionSourceCode code);
			virtual ~NpActionSource() = default;
			void setValue(float value);

		private:
			const PxeActionSourceCode _sourceCode;
			float _value;
		};
	}
}

#endif // !PXENGINE_NONPUBLIC_ACTION_SOURCE_H_
