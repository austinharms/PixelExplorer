#include "PE_event_loop.h"
#include "PE_log.h"
#include "PE_memory.h"
#include "SDL_events.h"
#include "PE_errors.h"
#include <thread>
#include <mutex>
#include <condition_variable>

namespace pecore {
	enum class EventLoopWorkTypeEnum
	{
		PE_EVENT_WORK_TYPE_DEFAULT,
		PE_EVENT_WORK_TYPE_BLOCKING,
	};

	struct EventLoopWorkDataBase {
		EventLoopWorkDataBase* next;
		PE_EventLoopFunction function;
		void* userdata;
		EventLoopWorkTypeEnum work_type;

		EventLoopWorkDataBase() {
			next = nullptr;
		}
	};

	struct EventLoopWorkData : public EventLoopWorkDataBase
	{
		EventLoopWorkData() {
			work_type = EventLoopWorkTypeEnum::PE_EVENT_WORK_TYPE_DEFAULT;
		}
	};

	struct BlockingEventLoopWorkData : public EventLoopWorkDataBase {
		std::mutex result_mutex;
		std::condition_variable result_condition;
		void* result;
		bool function_executed;

		BlockingEventLoopWorkData() {
			function_executed = false;
			work_type = EventLoopWorkTypeEnum::PE_EVENT_WORK_TYPE_BLOCKING;
			result = nullptr;
		}
	};

	static EventLoopWorkDataBase* s_function_queue_head = nullptr;
	static EventLoopWorkDataBase* s_function_queue_tail = nullptr;
	static std::mutex s_function_queue_mutex;
	static std::condition_variable s_function_queue_condition;
	static bool s_event_thread_flag = false;

	void PE_PrepareSDLEventLoop()
	{
		s_event_thread_flag = true;
	}

	void PE_RunSDLEventLoop() {
		PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Entry"));
		// Ensure all work is completed before exiting
		// Note: if there is recursive work this thread may never exit and cause a softlock
		bool has_work = false;
		while (s_event_thread_flag || has_work)
		{
			has_work = false;
			s_function_queue_mutex.lock();
			if (s_function_queue_head != nullptr) {
				has_work = true;
				EventLoopWorkDataBase* work = s_function_queue_head;
				if (s_function_queue_head == s_function_queue_tail) {
					s_function_queue_tail = nullptr;
				}

				s_function_queue_head = work->next;
				s_function_queue_mutex.unlock();
				void* res = work->function(work->userdata);
				switch (work->work_type)
				{
				case EventLoopWorkTypeEnum::PE_EVENT_WORK_TYPE_BLOCKING: {
					BlockingEventLoopWorkData* blocking_work = static_cast<BlockingEventLoopWorkData*>(work);
					blocking_work->result = res;
					blocking_work->result_mutex.lock();
					blocking_work->function_executed = true;
					blocking_work->result_mutex.unlock();
					blocking_work->result_condition.notify_all();
				} break;

				case EventLoopWorkTypeEnum::PE_EVENT_WORK_TYPE_DEFAULT:
					PE_free(work);
					break;

				default:
					PE_DEBUG_ASSERT(false, PE_TEXT("Unknown EventLoopWorkTypeEnum value"));
					break;
				}
			}
			else {
				s_function_queue_mutex.unlock();
			}

			SDL_PumpEvents();
		}

		PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Exit"));
	}

	void PE_StopSDLEventLoop()
	{
		s_event_thread_flag = false;
	}

	void* PE_RunEventLoopFunctionBlocking(PE_EventLoopFunction fn, void* userdata)
	{
		BlockingEventLoopWorkData work;
		work.function = fn;
		work.userdata = userdata;
		std::unique_lock work_lock(work.result_mutex);
		{
			std::lock_guard worker_lock(s_function_queue_mutex);
			if (s_function_queue_tail) {
				s_function_queue_tail->next = &work;
			}
			else {
				s_function_queue_head = &work;
			}

			s_function_queue_tail = &work;
		}

		work.result_condition.wait(work_lock, [&] { return work.function_executed; });
		return work.result;
	}

	int PE_RunEventLoopFunction(PE_EventLoopFunction fn, void* userdata)
	{
		EventLoopWorkData* work = static_cast<EventLoopWorkData*>(PE_malloc(sizeof(EventLoopWorkData)));
		if (work == nullptr) {
			return PE_ERROR_OUT_OF_MEMORY;
		}

		work->function = fn;
		work->userdata = userdata;
		std::lock_guard worker_lock(s_function_queue_mutex);
		if (s_function_queue_tail) {
			s_function_queue_tail->next = work;
		}
		else {
			s_function_queue_head = work;
		}

		s_function_queue_tail = work;
		return PE_ERROR_NONE;
	}
}
