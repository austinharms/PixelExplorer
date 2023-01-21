#ifndef PXENGINESAMPLES_SAMPLE_SCENE_BASE_H_
#define PXENGINESAMPLES_SAMPLE_SCENE_BASE_H_
#include "PxeWindow.h"

class SampleSceneBase
{
public:
	SampleSceneBase() = default;
	virtual ~SampleSceneBase() = default;
	virtual bool update() = 0;
};
#endif // !PXENGINESAMPLES_SAMPLE_SCENE_BASE_H_
