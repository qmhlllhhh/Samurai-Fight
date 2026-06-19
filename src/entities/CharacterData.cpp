#include "CharacterData.h"
#include "../utils/JsonLoader.h"
#include <iostream>

namespace SamuraiFight {

bool CharacterData::loadFromJson(const std::string& jsonPath) {
    nlohmann::json json;
    if (!JsonLoader::loadFromFile(jsonPath, json)) {
        std::cerr << "CharacterData: Failed to load " << jsonPath << std::endl;
        return false;
    }

    try {
        // 加载基本信息
        id = json.value("id", "");
        name = json.value("name", "");
        description = json.value("description", "");

        // 加载属性
        if (json.contains("stats")) {
            const auto& statsJson = json["stats"];
            stats.maxHp = statsJson.value("maxHp", 100.0f);
            stats.maxStamina = statsJson.value("maxStamina", 100.0f);
            stats.moveSpeed = statsJson.value("moveSpeed", 300.0f);
            stats.jumpHeight = statsJson.value("jumpHeight", 400.0f);
            stats.staminaRecovery = statsJson.value("staminaRecovery", 0.5f);
            stats.moveStaminaCost = statsJson.value("moveStaminaCost", 0.1f);
            stats.jumpStaminaCost = statsJson.value("jumpStaminaCost", 10.0f);
        }

        // 加载动画
        if (json.contains("animations")) {
            for (const auto& [key, animJson] : json["animations"].items()) {
                AnimationData anim;
                anim.spriteSheet = animJson.value("spriteSheet", "");
                anim.frameCount = animJson.value("frameCount", 1);
                anim.frameDuration = animJson.value("frameDuration", 0.1f);
                anim.loop = animJson.value("loop", true);
                anim.frameWidth = animJson.value("frameWidth", 128);
                anim.frameHeight = animJson.value("frameHeight", 128);
                animations[key] = anim;
            }
        }

        // 加载碰撞框
        if (json.contains("hitboxes")) {
            for (const auto& [state, hitboxArray] : json["hitboxes"].items()) {
                std::vector<HitboxData> hitboxList;
                for (const auto& hitboxJson : hitboxArray) {
                    HitboxData hitbox;
                    hitbox.x = hitboxJson.value("x", 0.0f);
                    hitbox.y = hitboxJson.value("y", 0.0f);
                    hitbox.width = hitboxJson.value("width", 48.0f);
                    hitbox.height = hitboxJson.value("height", 100.0f);
                    hitbox.type = hitboxJson.value("type", "hurtbox");
                    if (hitboxJson.contains("activeFrames")) {
                        hitbox.activeFrames = hitboxJson["activeFrames"].get<std::vector<int>>();
                    }
                    hitboxList.push_back(hitbox);
                }
                hitboxes[state] = hitboxList;
            }
        }

        // 加载攻击数据
        if (json.contains("attacks")) {
            for (const auto& [key, attackJson] : json["attacks"].items()) {
                AttackData attack;
                attack.damage = attackJson.value("damage", 10.0f);
                attack.startupFrames = attackJson.value("startupFrames", 3);
                attack.activeFrames = attackJson.value("activeFrames", 2);
                attack.recoveryFrames = attackJson.value("recoveryFrames", 5);
                attack.stunFrames = attackJson.value("stunFrames", 8);
                attack.staminaCost = attackJson.value("staminaCost", 0.0f);
                attack.canCancel = attackJson.value("canCancel", true);
                attack.cancelWindowStart = attackJson.value("cancelWindowStart", 2);
                attack.cancelWindowEnd = attackJson.value("cancelWindowEnd", 5);
                attack.hitCount = attackJson.value("hitCount", 1);
                attacks[key] = attack;
            }
        }

        std::cout << "CharacterData: Loaded character " << id << " from " << jsonPath << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "CharacterData: Error parsing " << jsonPath << ": " << e.what() << std::endl;
        return false;
    }
}

} // namespace SamuraiFight