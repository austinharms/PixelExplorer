#ifndef PXESAMPLES_RENDER_DATA_H_
#define PXESAMPLES_RENDER_DATA_H_
#include "PxeRefCount.h"
#include "PxeVertexArray.h"

class PxesRenderData : public pxengine::PxeRefCount
{
public:
	PxesRenderData();
	virtual ~PxesRenderData();
	void draw();

private:
	pxengine::PxeVertexArray* _vertexArray;
};
#endif // !PXESAMPLES_RENDER_DATA_H_
