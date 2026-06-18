#include "SceneManager.h"

#include <iostream>

namespace SamuraiFight {

SceneManager::SceneManager() {
    std::cout << "SceneManager: Initialized" << std::endl;
}

SceneManager::~SceneManager() {
    clearScenes();
    std::cout << "SceneManager: Destroyed" << std::endl;
}

void SceneManager::pushScene(std::unique_ptr<Scene> scene) {
    if (!scene) {
        std::cerr << "SceneManager: Cannot push null scene" << std::endl;
        return;
    }

    // 如果有当前场景，暂停它
    if (!m_sceneStack.empty()) {
        m_sceneStack.back()->onPause();
    }

    // 压入新场景并调用进入
    scene->onEnter();
    m_sceneStack.push_back(std::move(scene));

    std::cout << "SceneManager: Pushed scene, stack size: " << m_sceneStack.size() << std::endl;
}

void SceneManager::popScene() {
    if (m_sceneStack.empty()) {
        std::cerr << "SceneManager: Cannot pop from empty stack" << std::endl;
        return;
    }

    // 调用退出并移除
    m_sceneStack.back()->onExit();
    m_sceneStack.pop_back();

    // 如果还有场景，恢复它
    if (!m_sceneStack.empty()) {
        m_sceneStack.back()->onResume();
    }

    std::cout << "SceneManager: Popped scene, stack size: " << m_sceneStack.size() << std::endl;
}

void SceneManager::changeScene(std::unique_ptr<Scene> scene) {
    if (!scene) {
        std::cerr << "SceneManager: Cannot change to null scene" << std::endl;
        return;
    }

    // 先移除当前场景
    while (!m_sceneStack.empty()) {
        m_sceneStack.back()->onExit();
        m_sceneStack.pop_back();
    }

    // 压入新场景
    scene->onEnter();
    m_sceneStack.push_back(std::move(scene));

    std::cout << "SceneManager: Changed scene, stack size: " << m_sceneStack.size() << std::endl;
}

void SceneManager::clearScenes() {
    while (!m_sceneStack.empty()) {
        m_sceneStack.back()->onExit();
        m_sceneStack.pop_back();
    }

    std::cout << "SceneManager: All scenes cleared" << std::endl;
}

Scene *SceneManager::getCurrentScene() const {
    if (m_sceneStack.empty()) {
        return nullptr;
    }
    return m_sceneStack.back().get();
}

bool SceneManager::isEmpty() const {
    return m_sceneStack.empty();
}

size_t SceneManager::getSceneCount() const {
    return m_sceneStack.size();
}

void SceneManager::handleEvents(sf::RenderWindow &window) {
    if (m_sceneStack.empty()) {
        return;
    }

    m_sceneStack.back()->handleEvents(window);
}

void SceneManager::update(float deltaTime) {
    if (m_sceneStack.empty()) {
        return;
    }

    Scene *currentScene = m_sceneStack.back().get();

    // 更新当前场景
    currentScene->update(deltaTime);

    // 检查场景是否完成，需要切换
    std::unique_ptr<Scene> nextScene = currentScene->getNextScene();
    int popCount = currentScene->getPopSceneCount();
    while (!m_sceneStack.empty() && popCount > 0) {
        popScene();
        std::cout << "SceneManager: popCount: " << popCount << std::endl;
        --popCount;
    }
    if (nextScene) {
        pushScene(std::move(nextScene));
    }
}

void SceneManager::render(sf::RenderWindow &window) {
    // 渲染所有场景（从底部到顶部，用于叠加场景如暂停菜单）
    for (const auto &scene : m_sceneStack) {
        scene->render(window);
    }
}

} // namespace SamuraiFight