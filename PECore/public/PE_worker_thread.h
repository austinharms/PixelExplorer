//#ifndef PE_WORKER_THREAD_H_
//#define PE_WORKER_THREAD_H_
//#include "PE_defines.h"
//
//typedef void* (*PE_WorkerThreadFunction)(void*);
//struct PE_WorkerThread;
//
//// Creates a new PE_WorkerThread instance or nullptr on failure
//PE_EXTERN_C PE_NODISCARD PE_API PE_WorkerThread* PE_CALL PE_CreateWorkerThread();
//
//// Runs function on the worker and waits for the function to completed and returns the result
//PE_EXTERN_C PE_NODISCARD PE_API void* PE_CALL PE_WorkerThreadRunBlocking(PE_WorkerThread* worker, PE_WorkerThreadFunction function, void* userdata);
//
//// Runs function on the worker and returns immediately
//PE_EXTERN_C PE_API void PE_CALL PE_WorkerThreadRun(PE_WorkerThread* worker, PE_WorkerThreadFunction function, void* userdata);
//
//// Waits for remaining functions to complete and destroys the worker
//PE_EXTERN_C PE_API void PE_CALL PE_DestroyWorkerThread(PE_WorkerThread* worker);
//#endif // !PE_WORKER_THREAD_H_
