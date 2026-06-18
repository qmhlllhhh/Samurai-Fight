#include "Scene.h"

namespace SamuraiFight {

Scene::Scene()
    : m_popSceneCount(0) {
}

Scene::~Scene() {
}

void Scene::onEnter() {
    // 默认实现：空操作
}

void Scene::onExit() {
    // 默认实现：空操作
}

void Scene::onPause() {
    // 默认实现：空操作
}

void Scene::onResume() {
    // 默认实现：空操作
}

int Scene::getPopSceneCount() const {
    return m_popSceneCount;
}

std::unique_ptr<Scene> Scene::getNextScene() {
    if (m_nextScene) {
        return std::move(m_nextScene);
    }
    return nullptr;
}

} // namespace SamuraiFight