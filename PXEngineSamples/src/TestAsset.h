#include <iostream>

#include "PxeGLAsset.h"

#ifndef PXENGINESAMPLES_TEST_ASSET_H_
#define PXENGINESAMPLES_TEST_ASSET_H_

class TestAsset : public pxengine::PxeGLAsset
{
public:
	TestAsset() { std::cout << "Test Asset Create" << std::endl; }
	virtual ~TestAsset() { std::cout << "Test Asset Destroy" << std::endl; }
	void bind() { std::cout << "Test Asset Bound" << std::endl; }
	void unbind() { std::cout << "Test Asset Unbound" << std::endl; }
protected:
	void initializeGl() { std::cout << "Test Asset Init" << std::endl; }
	void uninitializeGl() { std::cout << "Test Asset Uninit" << std::endl; }
};

#endif // !PXENGINESAMPLES_TEST_ASSET_H_
