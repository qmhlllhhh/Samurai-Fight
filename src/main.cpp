#include "core/Game.h"

#include <iostream>

/**
 * @file main.cpp
 * @brief 程序入口点
 */

int main() {
    std::cout << "=== Samurai Fight ===" << std::endl;
    std::cout << "Initializing game..." << std::endl;

    // 获取游戏实例
    auto& game = SamuraiFight::Game::getInstance();

    // 初始化游戏
    if (!game.initialize()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return 1;
    }

    std::cout << "Game initialized successfully!" << std::endl;
    std::cout << "Starting game loop..." << std::endl;

    // 运行游戏
    game.run();

    // 关闭游戏
    game.shutdown();

    std::cout << "Game exited normally." << std::endl;
    return 0;
}
