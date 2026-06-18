#pragma once
namespace SamuraiFight {

enum class MenuItem {
    StartGame, ///< 开始游戏
    Settings,  ///< 设置
    Exit,      ///< 退出游戏
    Resume,    ///< 继续游戏（用于暂停场景）
    MainMenu   ///< 返回主菜单（用于暂停场景）
};
}