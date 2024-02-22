#include "PE_event_loop.h"
#include "PE_log.h"
#include "SDL_events.h"
#include <thread>
#include <mutex>
#include <condition_variable>

namespace {
	struct PE_EventLoopFunctionData {
		PE_EventLoopFunction function;
		void* data;
		PE_EventLoopFunctionData* next;
		bool executed;
	};

	PE_EventLoopFunctionData* l_functionQueueHead = nullptr;
	PE_EventLoopFunctionData* l_functionQueueTail = nullptr;
	std::mutex l_functionQueueMutex;
	std::condition_variable l_functionQueueCondition;
	bool l_eventThreadFlag = false;
}

PE_EXTERN_C PE_API void PE_CALL PE_PrepareSDLEventLoop()
{
	l_eventThreadFlag = true;
}

PE_EXTERN_C PE_API void PE_CALL PE_RunSDLEventLoop() {
	PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Entry"));
	while (l_eventThreadFlag)
	{
		l_functionQueueMutex.lock();
		PE_EventLoopFunctionData* fnData = l_functionQueueHead;
		if (fnData)
		{
			if (l_functionQueueTail == l_functionQueueHead) {
				l_functionQueueTail = nullptr;
			}

			l_functionQueueHead = fnData->next;
			l_functionQueueMutex.unlock();
			fnData->data = fnData->function(fnData->data);
			fnData->executed = true;
			l_functionQueueCondition.notify_all();
		}
		else {
			l_functionQueueMutex.unlock();
		}

		SDL_PumpEvents();
	}

	PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Exit"));
}

PE_EXTERN_C PE_API void PE_CALL PE_StopSDLEventLoop()
{
	l_eventThreadFlag = false;
}

PE_EXTERN_C PE_API void* PE_CALL PE_RunEventLoopFunction(PE_EventLoopFunction fn, void* userdata)
{
	PE_EventLoopFunctionData fnData{ fn, userdata, nullptr, false };
	std::unique_lock lock(l_functionQueueMutex);
	if (l_functionQueueTail) {
		l_functionQueueTail->next = &fnData;
	}
	else {
		l_functionQueueHead = &fnData;
		l_functionQueueTail = &fnData;
	}

	l_functionQueueCondition.wait(lock, [&] { return fnData.executed; });
	lock.unlock();
	return fnData.data;
}
