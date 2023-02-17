#include "NpRenderCollection.h"

#include "NpLogger.h"
#include "NpEngine.h"

namespace pxengine {
	namespace nonpublic {
		NpRenderCollection::NpRenderCollection(NpScene* scene) {
			_scene = scene;
		}

		NpRenderCollection::~NpRenderCollection() {
			clear();
		}

		void NpRenderCollection::addObject(PxeObject& obj)
		{
			if (obj.getObjectFlags() & (PxeObjectFlagsType)PxeObjectFlags::RENDER_OBJECT) {
				PxeRenderObjectInterface* renderObj = dynamic_cast<PxeRenderObjectInterface*>(&obj);
				if (!renderObj) {
					PXE_WARN("Attempted to add PxeObject to RenderCollection with RENDER_OBJECT flag that did not inherit from PxeRenderObjectInterface");
					return;
				}

				PxeRenderPass renderPass = renderObj->getRenderMaterial().getRenderPass();
				if ((int32_t)renderPass >= PxeRenderPassCount) {
					PXE_WARN("Attempted to add PxeRenderObjectInterface to RenderCollection with invalid PxeRenderPass");
					return;
				}

				if (renderPass == PxeRenderPass::GUI && &renderObj->getRenderMaterial() != NpEngine::getInstance().getGuiRenderMaterial()) {
					PXE_WARN("Attempted to add PxeRenderObjectInterface to RenderCollection with GUI PxeRenderPass that did not use gui PxeRenderMaterial");
					return;
				}

				obj.grab();
				// TODO Sort this based on shader and material
				_renderLists[(int32_t)renderPass].emplace_back(renderObj);
			}
			else {
				PXE_WARN("Attempted to add PxeObject to RenderCollection without RENDER_OBJECT flag");
			}
		}

		void NpRenderCollection::removeObject(PxeObject& obj)
		{
			if (obj.getObjectFlags() & (PxeObjectFlagsType)PxeObjectFlags::RENDER_OBJECT) {
				PxeRenderObjectInterface* renderObj = dynamic_cast<PxeRenderObjectInterface*>(&obj);
				if (!renderObj) {
					PXE_WARN("Attempted to remove PxeObject from RenderCollection with RENDER_OBJECT flag that did not inherit from PxeRenderObjectInterface");
					return;
				}

				PxeRenderPass renderPass = renderObj->getRenderMaterial().getRenderPass();
				if ((int32_t)renderPass >= PxeRenderPassCount) {
					PXE_WARN("Attempted to remove PxeRenderObjectInterface from RenderCollection with invalid PxeRenderPass");
					return;
				}

				if (_renderLists[(int32_t)renderPass].remove(renderObj)) {
					obj.drop();
				}
				else {
					PXE_WARN("Attempted to remove PxeRenderObjectInterface that was not added to the RenderCollection");
				}
			}
			else {
				PXE_WARN("Attempted to remove PxeObject from RenderCollection without RENDER_OBJECT flag");
			}
		}

		void NpRenderCollection::clear()
		{
			for (int32_t pass = 0; pass < PxeRenderPassCount; ++pass) {
				for (PxeRenderObjectInterface* obj : _renderLists[pass]) {
					dynamic_cast<PxeObject*>(obj)->drop();
				}

				_renderLists[pass].clear();
			}
		}
	}
}