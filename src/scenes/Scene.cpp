#include "Scene.h"

namespace SamuraiFight {

Scene::Scene()
    : m_finished(false) {
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

bool Scene::isFinished() const {
    return m_finished;
}

std::unique_ptr<Scene> Scene::getNextScene() const {
    // 默认实现：返回空指针
    return nullptr;
}

} // namespace SamuraiFight