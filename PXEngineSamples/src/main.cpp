#include "PxeEngineAPI.h"
#include "LogHandler.h"
#include "SampleApplication.h"

int main(int argc, char* args[]) {
	LogHandle log;
	SampleApplication app;
	app.start(log);
	return 0;
}
