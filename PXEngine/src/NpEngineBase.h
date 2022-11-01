#include "PxeEngineBase.h"
#include "PxeLogger.h"

#ifndef PXENGINE_NONPUBLIC_ENGINEBASE_H_
#define PXENGINE_NONPUBLIC_ENGINEBASE_H_
namespace pxengine::nonpublic {
	// TODO add NpEngineBase class description (copy EngineBase description)
	class NpEngineBase : PxeEngineBase, PxeLogHandler
	{
	public:
		// creates and returns an NpEngineBase instance
		// there should only be one instance per process
		// returns nullptr on error or if an NpEngineBase instance already exists
		static NpEngineBase* createInstance(PxeLogHandler& logHandler);

		// returns the NpEngineBase instance
		static NpEngineBase& getInstance();

		virtual ~NpEngineBase();

		void onLog(const char* msg, uint8_t level, const char* file, uint64_t line, const char* function) override;

		uint32_t testFn(uint32_t val) override { return val + 10; }

	protected:
		void onDelete() override;

	private:
		static NpEngineBase* s_instance;

		NpEngineBase(PxeLogHandler& logHandler);

		PxeLogHandler& _logHandler;
	};
}
#endif // !PXENGINE_NONPUBLIC_NPENGINEBASE_H_
