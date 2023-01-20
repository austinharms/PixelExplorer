#include "NpAction.h"

#include "NpLogger.h"

namespace pxengine {
	namespace nonpublic {
		PXE_NODISCARD float NpAction::getValue() const
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

		void NpAction::addSource(PxeActionSource& source)
		{
			if (!_sources.remove(&source)) {
				source.grab();
			}
			else {
				PXE_WARN("Attempted readd PxeActionSource to PxeAction");
			}

			_sources.push_back(&source);
		}

		PXE_NODISCARD const std::string& NpAction::getName() const
		{
			return _name;
		}

		PXE_NODISCARD const std::list<PxeActionSource*>& NpAction::getSources() const
		{
			return _sources;
		}

		NpAction::NpAction(const std::string& name) : _name(name) {}

		NpAction::~NpAction()
		{
			for (PxeActionSource* source : _sources)
				source->drop();
			_sources.clear();
		}


	}
}