#include "Config.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "StaffEnchantment.h"

const std::string pattern = "_ISTV.json";
const std::string directory = "Data/";

namespace {
    void EachConfigFile(const std::function<void(std::string, json)>& callback) {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                const std::string filename = entry.path().filename().string();

                if (filename.size() >= pattern.size() && filename.substr(filename.size() - pattern.size()) == pattern) {
                    std::ifstream f(entry.path());
                    if (!f) {
                        continue;
                    }

                    try {
                        const json data = json::parse(f);

                        callback(filename, data);

                    } catch (const json::parse_error&) {  // NOLINT(bugprone-empty-catch)
                    }
                }
            }
        }
    }
}

int TryReadInt(json obj, std::string key, int def) {
    if (obj.contains(key) && obj[key].is_number_integer()) {
        return obj[key].get<int>();
    }
    if (obj.contains(key) && obj[key].is_number_float()) {
        return obj[key].get<float>();
    }
    return def;
}
float TryReadFloat(json obj, std::string key, float def) {
    if (obj.contains(key) && obj[key].is_number_integer()) {
        return obj[key].get<int>();
    }
    if (obj.contains(key) && obj[key].is_number_float()) {
        return obj[key].get<float>();
    }
    return def;
}

std::pair<bool, std::string> ReadString(json obj, std::string key) {
    if (obj.contains(key) && obj[key].is_string()) {
        return {true, obj[key].get<std::string>()};
    }
    return {false, ""};
}

std::pair<bool, json> ReadObject(json obj, std::string key) {
    if (obj.contains(key) && obj[key].is_object()) {
        return {true, obj[key]};
    }
    return {false, json{}};
}

ValueModifier ReadValueModifier(json& obj) {
    ValueModifier result;
    result.magnitudePercentage = TryReadFloat(obj, "MagnitudePercentage", result.magnitudePercentage);
    result.areaPercentage = TryReadFloat(obj, "AreaPercentage", result.areaPercentage);
    result.durationPercentage = TryReadFloat(obj, "DurationPercentage", result.durationPercentage);
    result.costPercentage = TryReadFloat(obj, "CostPercentage", result.costPercentage);
    result.chargingTimePercentage = TryReadFloat(obj, "ChargingTimePercentage", result.chargingTimePercentage);
    result.costOverride = TryReadInt(obj, "CostOverride", result.costOverride);
    return result;
}


void Config::Load() {
    EachConfigFile([](std::string fileName, json json) {
        if (!json.is_array()) {
            logger::error("Skipping file {}, as it is not a json array", fileName);
            return;
        }
        for (auto item : json) {
            if (!item.is_object()) {
                logger::error("Item is not object skipping it at file: {}", fileName);
                continue;
            }

            auto [hasKeyword, keyword] = ReadString(item, "Keyword");

            if (!hasKeyword) {
                logger::error("missing keyword it at file: {}", fileName);
                continue;
            }

            auto [hasAlteration, alteration] = ReadObject(item, "Alteration");

            if (hasAlteration) {
                Groups[keyword][RE::ActorValue::kAlteration] = ReadValueModifier(alteration);
            }

            auto [hasConjuration, conjuration] = ReadObject(item, "Conjuration");

            if (hasConjuration) {
                Groups[keyword][RE::ActorValue::kConjuration] = ReadValueModifier(conjuration);
            }
            auto [hasDestruction, destruction] = ReadObject(item, "Desruction");

            if (hasDestruction) {
                Groups[keyword][RE::ActorValue::kDestruction] = ReadValueModifier(destruction);
            }

            auto [hasIllusion, illusion] = ReadObject(item, "Illusion");

            if (hasIllusion) {
                Groups[keyword][RE::ActorValue::kIllusion] = ReadValueModifier(illusion);
            }

            auto [hasDefault, _default] = ReadObject(item, "Default");

            if (hasDefault) {
                Groups[keyword][RE::ActorValue::kNone] = ReadValueModifier(_default);
            }
 
        }
    });
}
