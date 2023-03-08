#include "PxeAction.h"

#include "PxeLogger.h"

namespace pxengine {
	PXE_NODISCARD float PxeAction::getValue() const
	{
		float sum = 0;
		for (const PxeActionSource* source : _sources)
			sum += source->getValue();
		if (sum < 0)
		{
			sum = 0;
		}
		else if (sum > 1)
		{
			sum = 1;
		}

		return sum;
	}

	void PxeAction::addSource(PxeActionSource& source)
	{
		if (!_sources.remove(&source)) {
			source.grab();
		}
		else {
			PXE_WARN("Attempted readd PxeActionSource to PxeAction");
		}

		_sources.push_back(&source);
	}

	PXE_NODISCARD const std::string& PxeAction::getName() const
	{
		return _name;
	}

	PXE_NODISCARD const std::list<PxeActionSource*>& PxeAction::getSources() const
	{
		return _sources;
	}

	PXE_NODISCARD bool PxeAction::hasSource() const
	{
		return !_sources.empty();
	}

	PxeAction::PxeAction(const std::string& name) : _name(name) {}

	PxeAction::~PxeAction()
	{
		for (PxeActionSource* source : _sources)
			source->drop();
		_sources.clear();
	}
}