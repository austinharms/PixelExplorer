#include "NpEngineBase.h"

#include <new>

#include "NpLogger.h"

namespace pxengine::nonpublic {
	NpEngineBase* NpEngineBase::s_instance = nullptr;

	NpEngineBase* NpEngineBase::createInstance(PxeLogHandler& logHandler)
	{
		if (s_instance) {
			PXE_WARN("PxeEngineBase Already Exist");
			return nullptr;
		}

		s_instance = new(std::nothrow) NpEngineBase(logHandler);
		PXE_ASSERT(s_instance, "Failed to allocate PxeEngineBase");
		PXE_INFO("PxeEngineBase Created");
		return s_instance;
	}

	NpEngineBase& NpEngineBase::getInstance()
	{
		PXE_ASSERT(s_instance, "PxeEngineBase Not Created");
		return *s_instance;
	}

	NpEngineBase::NpEngineBase(PxeLogHandler& logHandler) : _logHandler(logHandler)
	{

	}

	NpEngineBase::~NpEngineBase()
	{
	}

	void NpEngineBase::onLog(const char* msg, uint8_t level, const char* file, uint64_t line, const char* function)
	{
		_logHandler.onLog(msg, level, file, line, function);
	}

	void NpEngineBase::onDelete()
	{
		PXE_INFO("PxeEngineBase Destroyed");
		s_instance = nullptr;
	}
}