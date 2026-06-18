#include "JsonLoader.h"

#include <fstream>
#include <iostream>

namespace SamuraiFight {

bool JsonLoader::loadFromFile(const std::string& path, nlohmann::json& json) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "JsonLoader: Failed to open file for reading: " << path << std::endl;
        return false;
    }

    try {
        file >> json;
        file.close();
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JsonLoader: JSON parse error in file " << path << ": " << e.what() << std::endl;
        file.close();
        return false;
    } catch (const std::exception& e) {
        std::cerr << "JsonLoader: Error reading file " << path << ": " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool JsonLoader::saveToFile(const std::string& path, const nlohmann::json& json) {
    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "JsonLoader: Failed to open file for writing: " << path << std::endl;
        return false;
    }

    try {
        file << json.dump(4);  // 格式化输出，缩进为4空格
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "JsonLoader: Error writing to file " << path << ": " << e.what() << std::endl;
        file.close();
        return false;
    }
}

} // namespace SamuraiFight