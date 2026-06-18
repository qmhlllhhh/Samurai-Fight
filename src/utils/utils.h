#pragma once

#include <random>
#include <ctime>

namespace SamuraiFight {

/**
 * @file utils.h
 * @brief 工具函数
 */

// 随机数生成器
static std::mt19937 gen(static_cast<unsigned>(time(0)));

/**
 * @brief 生成指定范围内的随机整数
 *
 * @param min 最小值
 * @param max 最大值
 * @return int 随机整数
 */
inline int randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

/**
 * @brief 生成指定范围内的随机长整数
 *
 * @param min 最小值
 * @param max 最大值
 * @return long long 随机长整数
 */
inline long long randomLongLong(long long min, long long max) {
    std::uniform_int_distribution<long long> dist(min, max);
    return dist(gen);
}

} // namespace SamuraiFight