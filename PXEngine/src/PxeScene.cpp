#include "PxeScene.h"

#include <shared_mutex>

#include "PxeEngine.h"
#include "PxePhysicsComponent.h"
#include "PxeUpdateComponent.h"
#include "PxeRenderComponent.h"

namespace pxengine {
	struct PxeScene::Impl {
		Impl(physx::PxScene& physScene) : _physScene(physScene) {}
		mutable std::shared_mutex _sceneMutex;
		mutable std::shared_mutex _objectsMutex;
		std::list<PxeObject*> _objects;
		std::list<PxeRenderComponent*> _renderComponents;
		std::list<PxePhysicsComponent*> _physicsComponents;
		std::list<PxeUpdateComponent*> _updateComponents;

		PxeSceneFlags _flags;
		physx::PxScene& _physScene;
		float _physTimeScale;
		float _physAccumulator;
		float _physStep;
	};

	PxeScene::PxeScene(physx::PxScene& physicsScene)
	{
		static_assert(sizeof(Impl) <= STORAGE_SIZE, "PxeScene::STORAGE_SIZE need be changed");
		static_assert(STORAGE_ALIGN == alignof(Impl), "PxeScene::STORAGE_ALIGN need be changed");

		userData = nullptr;
		new(&_storage) Impl(physicsScene);
		Impl& imp = impl();
		imp._flags = PxeSceneFlag::NONE;
		imp._physTimeScale = 1.0f;
		imp._physStep = 0.5f;
		imp._physAccumulator = 0.0f;
	}

	PXE_NODISCARD bool PxeScene::getFlag(PxeSceneFlags flag) const
	{
		const Impl& imp = impl();
		std::shared_lock lock(imp._sceneMutex);
		return imp._flags & flag;
	}

	void PxeScene::onDelete()
	{
		PxeEngine::getInstance().removeScene(*this);
	}

	PxeScene::~PxeScene()
	{
		Impl& imp = impl();
		imp._physScene.release();
		imp.~Impl();
	}

	void PxeScene::updatePhysics(float deltaTime) {
		Impl& imp = impl();
		if (!getFlag(PxeSceneFlag::PHYSICS_UPDATE)) return;
		imp._sceneMutex.lock();
		imp._physAccumulator += deltaTime * imp._physTimeScale;
		while (imp._physAccumulator >= imp._physStep)
		{
			imp._physAccumulator -= imp._physStep;
			imp._sceneMutex.unlock();

			{
				std::shared_lock objLock(imp._objectsMutex);
				for (PxePhysicsComponent* component : imp._physicsComponents)
					component->onPhysics();
			}

			imp._physScene.lockWrite(__FILE__, __LINE__);
			imp._physScene.simulate(imp._physStep);
			imp._physScene.unlockWrite();
			while (!imp._physScene.checkResults());
			imp._physScene.lockWrite(__FILE__, __LINE__);
			imp._physScene.fetchResults(true);
			imp._physScene.unlockWrite();
			imp._sceneMutex.lock();
		}

		imp._sceneMutex.unlock();
	}

	void PxeScene::updateComponents()
	{
		Impl& imp = impl();
		if (!getFlag(PxeSceneFlag::COMPONENT_UPDATE)) return;
		std::shared_lock lock(imp._objectsMutex);
		for (PxeUpdateComponent* component : imp._updateComponents)
			component->onUpdate();
	}

	void PxeScene::addComponent(PxeComponent& component)
	{
		Impl& imp = impl();
		if (component.componentOfType<PxeUpdateComponent>()) {
			component.grab();
			imp._updateComponents.emplace_back(static_cast<PxeUpdateComponent*>(&component));
		}
		else if (component.componentOfType<PxePhysicsComponent>()) {
			component.grab();
			imp._physicsComponents.emplace_back(static_cast<PxePhysicsComponent*>(&component));
		}
		else if (component.componentOfType<PxeRenderComponent>()) {
			component.grab();
			imp._renderComponents.emplace_back(static_cast<PxeRenderComponent*>(&component));
		}
	}

	void PxeScene::removeComponent(PxeComponent& component)
	{
		Impl& imp = impl();
		if (component.componentOfType<PxeUpdateComponent>()) {
			if (imp._updateComponents.remove(static_cast<PxeUpdateComponent*>(&component)))
				component.drop();
		}
		else if (component.componentOfType<PxePhysicsComponent>()) {
			if (imp._physicsComponents.remove(static_cast<PxePhysicsComponent*>(&component)))
				component.drop();
		}
		else if (component.componentOfType<PxeRenderComponent>()) {
			if (imp._renderComponents.remove(static_cast<PxeRenderComponent*>(&component)))
				component.drop();
		}
	}

	PXE_NODISCARD const std::list<PxeRenderComponent*>& PxeScene::acquireRenderComponents()
	{
		Impl& imp = impl();
		imp._objectsMutex.lock_shared();
		return imp._renderComponents;
	}

	void PxeScene::releaseRenderComponents()
	{
		impl()._objectsMutex.unlock_shared();
	}

	bool PxeScene::addObject(PxeObject& obj)
	{
		if (!obj.setScene(*this)) return false;
		obj.addComponentsToScene();
		return true;
	}

	void PxeScene::removeObject(PxeObject& obj)
	{
		obj.removeComponentsFromScene();
		obj.clearScene(*this);
	}

	void PxeScene::setFlags(PxeSceneFlags flags)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._sceneMutex);
		imp._flags = flags;
	}

	PXE_NODISCARD PxeSceneFlags PxeScene::getFlags() const
	{
		const Impl& imp = impl();
		std::shared_lock lock(imp._sceneMutex);
		return imp._flags;
	}

	void PxeScene::setFlag(PxeSceneFlags flag)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._sceneMutex);
		imp._flags |= flag;
	}

	void PxeScene::clearFlag(PxeSceneFlags flag)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._sceneMutex);
		imp._flags &= ~flag;
	}

	PXE_NODISCARD physx::PxScene& PxeScene::getPhysicsScene() const
	{
		return impl()._physScene;
	}

	void PxeScene::setPhysicsStepSize(float step)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._sceneMutex);
		imp._physStep = step;
	}

	void PxeScene::setPhysicsTimeScale(float speed)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._sceneMutex);
		if (speed < 0) {
			PXE_WARN("Attempted to set PxeScene's physics time scale to a value less then 0");
			return;
		}

		imp._physTimeScale = speed;
	}

	PXE_NODISCARD float PxeScene::getPhysicsTimeScale() const
	{
		const Impl& imp = impl();
		std::shared_lock lock(imp._sceneMutex);
		return imp._physTimeScale;
	}

	PXE_NODISCARD float PxeScene::getPhysicsAccumulator() const
	{
		const Impl& imp = impl();
		std::shared_lock lock(imp._sceneMutex);
		return imp._physAccumulator;
	}

	void PxeScene::setPhysicsSimulationAccumulator(float time)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._sceneMutex);
		imp._physAccumulator = time;
	}

	PXE_NODISCARD float PxeScene::getPhysicsStepSize() const
	{
		const Impl& imp = impl();
		std::shared_lock lock(imp._sceneMutex);
		return imp._physStep;
	}
}