#include "PxeEngineBase.h"

#include "nonpublic/NpEngineBase.h"
#include "PxeLogger.h"

namespace pxengine {
    PxeEngineBase* createPXEEngineBase(PxeLogInterface& logInterface)
    {
        return static_cast<PxeEngineBase*>(nonpublic::NpEngineBase::createInstance(logInterface));
    }

    PxeEngineBase& getPXEEngineBase()
    {
        return static_cast<PxeEngineBase&>(nonpublic::NpEngineBase::getInstance());
    }
}