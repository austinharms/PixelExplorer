#ifndef PXENGINE_ACTION_H_
#define PXENGINE_ACTION_H_
#include <string>
#include <list>

#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeActionSource.h"

namespace pxengine {
	// TODO Comment file
	class PxeAction : public PxeRefCount
	{
	public:
		PxeAction() = default;
		virtual ~PxeAction() = default;

		// Returns the actions current value
		// Note: output range is 0-1 inclusive
		virtual PXE_NODISCARD float getValue() const = 0;

		// Add another source to use when calculating action value
		virtual void addSource(PxeActionSource& source) = 0;

		// Returns the action name
		virtual PXE_NODISCARD const std::string& getName() const = 0;

		// Returns all the sources used when calculating the action value
		virtual PXE_NODISCARD const std::list<PxeActionSource*>& getSources() const = 0;

		// Returns if the action has at least one source 
		virtual PXE_NODISCARD bool hasSource() const = 0;
	};
}
#endif // !PXENGINE_ACTION_H_
