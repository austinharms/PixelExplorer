#include "PxeEngineBase.h"

#include "NpEngineBase.h"

namespace pxengine {
    PxeEngineBase* createPXEEngineBase(PxeLogHandler& logHandler)
    {
        return (PxeEngineBase*)nonpublic::NpEngineBase::createInstance(logHandler);
    }

    PxeEngineBase& getPXEEngineBase()
    {
        return (PxeEngineBase&)nonpublic::NpEngineBase::getInstance();
    }
}