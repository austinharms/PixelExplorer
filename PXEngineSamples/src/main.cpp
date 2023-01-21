#include "PxeEngineAPI.h"
#include "LogHandler.h"
#include "SampleApplication.h"

int main(int argc, char* args[]) {
	LogHandle log;
	SampleApplication app;
	pxengine::pxeRunApplication(app, log);
	return 0;
}
