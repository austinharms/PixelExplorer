#ifndef PXENGINE_NONPUBLIC_ACTION_H_
#define PXENGINE_NONPUBLIC_ACTION_H_
#include <string>
#include <list>

#include "PxeTypes.h"
#include "PxeAction.h"
#include "PxeActionSource.h"

namespace pxengine {
	namespace nonpublic {
		class NpAction : public PxeAction
		{
		public:
			//############# PxeAction API ##################

			PXE_NODISCARD float getValue() const override;
			void addSource(PxeActionSource& source) override;
			PXE_NODISCARD const std::string& getName() const override;
			PXE_NODISCARD const std::list<PxeActionSource*>& getSources() const override;
			PXE_NODISCARD bool hasSource() const override;

			//############# PRIVATE API ##################

			NpAction(const std::string& name);
			virtual ~NpAction();

		private:
			const std::string _name;
			std::list<PxeActionSource*> _sources;
		};
	}
}

#endif // !PXENGINE_NONPUBLIC_ACTION_H_
