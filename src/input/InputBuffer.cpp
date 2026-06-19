#include "InputBuffer.h"

namespace SamuraiFight {

InputBuffer::InputBuffer(int bufferSize)
    : m_bufferSize(bufferSize)
    , m_currentFrame(0) {
}

InputBuffer::~InputBuffer() {
}

void InputBuffer::pushInput(InputAction action, bool pressed, int frame) {
    // 添加输入到缓冲
    m_buffer.emplace_back(action, pressed, frame);

    // 移除过期的输入（超过缓冲大小的）
    while (m_buffer.size() > static_cast<size_t>(m_bufferSize) * 2) { // *2 因为有按下和释放两个事件
        m_buffer.pop_front();
    }
}

bool InputBuffer::hasInput(InputAction action, int startFrame, int endFrame) const {
    int absoluteStart = m_currentFrame - startFrame;
    int absoluteEnd = m_currentFrame - endFrame;

    for (const auto& input : m_buffer) {
        if (input.action == action &&
            input.frame >= absoluteEnd &&
            input.frame <= absoluteStart) {
            return true;
        }
    }
    return false;
}

bool InputBuffer::hasPressEvent(InputAction action, int startFrame, int endFrame) const {
    int absoluteStart = m_currentFrame - startFrame;
    int absoluteEnd = m_currentFrame - endFrame;

    for (const auto& input : m_buffer) {
        if (input.action == action &&
            input.pressed &&
            input.frame >= absoluteEnd &&
            input.frame <= absoluteStart) {
            return true;
        }
    }
    return false;
}

std::vector<BufferedInput> InputBuffer::getRecentInputs(int count) const {
    std::vector<BufferedInput> result;

    int startIndex = static_cast<int>(m_buffer.size()) - count;
    if (startIndex < 0) {
        startIndex = 0;
    }

    for (size_t i = startIndex; i < m_buffer.size(); ++i) {
        result.push_back(m_buffer[i]);
    }

    return result;
}

void InputBuffer::clear() {
    m_buffer.clear();
}

int InputBuffer::getBufferSize() const {
    return m_bufferSize;
}

void InputBuffer::setCurrentFrame(int frame) {
    m_currentFrame = frame;
}

} // namespace SamuraiFight