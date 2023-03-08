#ifndef PXENGINE_ACTION_H_
#define PXENGINE_ACTION_H_
#include <string>
#include <list>

#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeActionSource.h"

namespace pxengine {
	class PxeInputManager;

	// TODO Comment file
	class PxeAction : public PxeRefCount
	{
	public:
		// Returns the actions current value
		// Note: output range is 0-1 inclusive
		PXE_NODISCARD float getValue() const;

		// Add another source to use when calculating action value
		void addSource(PxeActionSource& source);

		// Returns the action name
		PXE_NODISCARD const std::string& getName() const;

		// Returns all the sources used when calculating the action value
		PXE_NODISCARD const std::list<PxeActionSource*>& getSources() const;

		// Returns if the action has at least one source 
		PXE_NODISCARD bool hasSource() const;

		virtual ~PxeAction();
		PXE_NOCOPY(PxeAction);

	private:
		friend class PxeInputManager;
		PxeAction(const std::string& name);

		const std::string _name;
		std::list<PxeActionSource*> _sources;
	};
}
#endif // !PXENGINE_ACTION_H_
