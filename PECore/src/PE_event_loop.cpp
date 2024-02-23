#include "PE_event_loop.h"
#include "PE_log.h"
#include "SDL_events.h"
#include <thread>
#include <mutex>
#include <condition_variable>

namespace pecore {
	struct PE_EventLoopFunctionData {
		PE_EventLoopFunction function;
		void* data;
		PE_EventLoopFunctionData* next;
		bool executed;
	};

	static PE_EventLoopFunctionData* s_function_queue_head = nullptr;
	static PE_EventLoopFunctionData* s_function_queue_tail = nullptr;
	static std::mutex s_function_queue_mutex;
	static std::condition_variable s_function_queue_condition;
	static bool s_event_thread_flag = false;

	void PE_PrepareSDLEventLoop()
	{
		s_event_thread_flag = true;
	}

	void PE_RunSDLEventLoop() {
		PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Entry"));
		while (s_event_thread_flag)
		{
			s_function_queue_mutex.lock();
			PE_EventLoopFunctionData* fnData = s_function_queue_head;
			if (fnData)
			{
				if (s_function_queue_tail == s_function_queue_head) {
					s_function_queue_tail = nullptr;
				}

				s_function_queue_head = fnData->next;
				s_function_queue_mutex.unlock();
				fnData->data = fnData->function(fnData->data);
				fnData->executed = true;
				s_function_queue_condition.notify_all();
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

	void* PE_RunEventLoopFunction(PE_EventLoopFunction fn, void* userdata)
	{
		PE_EventLoopFunctionData fn_data{ fn, userdata, nullptr, false };
		std::unique_lock lock(s_function_queue_mutex);
		if (s_function_queue_tail) {
			s_function_queue_tail->next = &fn_data;
		}
		else {
			s_function_queue_head = &fn_data;
			s_function_queue_tail = &fn_data;
		}

		s_function_queue_condition.wait(lock, [&] { return fn_data.executed; });
		lock.unlock();
		return fn_data.data;
	}
}
