#include "PxeEngineAPI.h"
#include "LogHandler.h"
#include "TestApplication.h"

int main(int argc, char* args[]) {
	LogHandle log;
	TestApplication app;
	pxengine::pxeRunApplication(app, log);
}
