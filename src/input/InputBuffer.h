#pragma once

#include "InputCommand.h"
#include <vector>
#include <deque>
#include <SFML/Graphics.hpp>

namespace SamuraiFight {

/**
 * @file InputBuffer.h
 * @brief 输入缓冲系统
 */

/**
 * @struct BufferedInput
 * @brief 带帧号的输入记录
 */
struct BufferedInput {
    InputAction action;      ///< 输入动作
    bool pressed;            ///< true=按下, false=释放
    int frame;               ///< 输入发生的帧号

    BufferedInput(InputAction a, bool p, int f)
        : action(a), pressed(p), frame(f) {}
};

/**
 * @class InputBuffer
 * @brief 输入缓冲，用于连招检测
 *
 * 记录过去N帧的输入历史，支持取消窗口检测
 */
class InputBuffer {
public:
    /**
     * @brief 构造函数
     *
     * @param bufferSize 缓冲大小（帧数）
     */
    explicit InputBuffer(int bufferSize = 12);

    /**
     * @brief 析构函数
     */
    ~InputBuffer();

    /**
     * @brief 推入输入
     *
     * @param action 输入动作
     * @param pressed 是否按下
     * @param frame 当前帧号
     */
    void pushInput(InputAction action, bool pressed, int frame);

    /**
     * @brief 检查在指定帧范围内是否有某个输入
     *
     * @param action 要检查的输入动作
     * @param startFrame 起始帧（相对当前帧）
     * @param endFrame 结束帧（相对当前帧）
     * @return true 存在该输入
     * @return false 不存在
     */
    bool hasInput(InputAction action, int startFrame, int endFrame) const;

    /**
     * @brief 检查在指定帧范围内是否有某个按下事件
     *
     * @param action 要检查的输入动作
     * @param startFrame 起始帧（相对当前帧）
     * @param endFrame 结束帧（相对当前帧）
     * @return true 存在该按下事件
     * @return false 不存在
     */
    bool hasPressEvent(InputAction action, int startFrame, int endFrame) const;

    /**
     * @brief 获取最近的输入
     *
     * @param count 数量
     * @return std::vector<BufferedInput> 输入列表
     */
    std::vector<BufferedInput> getRecentInputs(int count) const;

    /**
     * @brief 清空缓冲
     */
    void clear();

    /**
     * @brief 获取缓冲大小
     *
     * @return int 缓冲大小
     */
    int getBufferSize() const;

    /**
     * @brief 设置当前帧号
     *
     * @param frame 当前帧号
     */
    void setCurrentFrame(int frame);

private:
    int m_bufferSize;                        ///< 缓冲大小
    int m_currentFrame;                      ///< 当前帧号
    std::deque<BufferedInput> m_buffer;       ///< 输入缓冲
};

} // namespace SamuraiFight