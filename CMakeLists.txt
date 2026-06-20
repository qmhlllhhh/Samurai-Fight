cmake_minimum_required(VERSION 3.16)
project(SamuraiFight VERSION 1.0.0 LANGUAGES CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 设置 MSYS2 UCRT64 路径（如果需要）
if(EXISTS "C:/msys64/ucrt64")
    list(APPEND CMAKE_PREFIX_PATH "C:/msys64/ucrt64")
endif()

# 查找 SFML 3.0
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

# 查找 nlohmann_json
find_package(nlohmann_json REQUIRED)

# 源文件
set(SOURCES
    # Core
    src/main.cpp
    src/core/Game.cpp
    src/core/GameLoop.cpp
    src/core/FrameTimer.cpp

    # Managers
    src/managers/ResourceManager.cpp
    src/managers/SceneManager.cpp
    src/managers/ConfigManager.cpp

    # Scenes
    src/scenes/Scene.cpp
    src/scenes/MainMenuScene.cpp
    src/scenes/CharacterSelectScene.cpp
    src/scenes/BattleScene.cpp
    src/scenes/PauseScene.cpp

    # Entities
    src/entities/Entity.cpp
    src/entities/CharacterData.cpp
    src/entities/Character.cpp
    src/entities/CharacterFactory.cpp

    # Components
    src/components/AnimationComponent.cpp
    src/components/HitboxComponent.cpp
    src/components/HealthComponent.cpp
    src/components/StaminaComponent.cpp

    # States
    src/states/CharacterState.cpp
    src/states/StateMachine.cpp
    src/states/IdleState.cpp
    src/states/MoveState.cpp
    src/states/JumpState.cpp
    src/states/CrouchState.cpp
    src/states/AttackState.cpp
    src/states/HurtState.cpp
    src/states/BlockState.cpp
    src/states/HurtState.cpp
    src/states/RollState.cpp
    src/states/DeadState.cpp
    src/states/VictoryState.cpp

    # Battle
    src/battle/RoundManager.cpp
    src/battle/MatchManager.cpp
    src/battle/MatchHud.cpp

    # Utils
    src/utils/JsonLoader.cpp

    # Input
    src/input/InputManager.cpp
    src/input/InputBuffer.cpp

    # UI
    src/ui/HealthBar.cpp
    src/ui/StaminaBar.cpp
)

# 头文件目录
set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# 可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

# 链接 SFML 和 nlohmann_json
target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    nlohmann_json::nlohmann_json
)

# 复制资源文件到构建目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "Copying assets to build directory"
)

# Windows 特定设置
if(WIN32)
    # 复制 SFML DLL 到输出目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SFML::Graphics>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SFML::Window>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SFML::System>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SFML::Audio>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying SFML DLLs to output directory"
    )
endif()

# 编译选项
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug 定义
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

# Print configuration info
message(STATUS "=== Samurai Fight Configuration ===")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "SFML version: ${SFML_VERSION}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
