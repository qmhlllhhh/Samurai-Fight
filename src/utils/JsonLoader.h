#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace SamuraiFight {

/**
 * @file JsonLoader.h
 * @brief JSON文件加载工具
 */

/**
 * @class JsonLoader
 * @brief 提供JSON文件的加载和保存功能
 */
namespace JsonLoader {

/**
 * @brief 从文件加载JSON数据
 *
 * @param path JSON文件路径
 * @param json 输出的JSON对象
 * @return true 加载成功
 * @return false 加载失败
 */
bool loadFromFile(const std::string& path, nlohmann::json& json);

/**
 * @brief 将JSON数据保存到文件
 *
 * @param path JSON文件路径
 * @param json 要保存的JSON对象
 * @return true 保存成功
 * @return false 保存失败
 */
bool saveToFile(const std::string& path, const nlohmann::json& json);

} // namespace JsonLoader

} // namespace SamuraiFight