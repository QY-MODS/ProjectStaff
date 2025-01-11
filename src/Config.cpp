#include "Config.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "StaffEnchantment.h"

const std::string pattern = "_ISTV.json";
const std::string directory = "Data/";

std::map<std::string, int> avMap = {
    {"charge", -1},
    {"aggression", 0},
    {"confidence", 1},
    {"energy", 2},
    {"morality", 3},
    {"mood", 4},
    {"assistance", 5},
    {"onehanded", 6},
    {"twohanded", 7},
    {"archery", 8},
    {"block", 9},
    {"smithing", 10},
    {"heavyarmor", 11},
    {"lightarmor", 12},
    {"pickpocket", 13},
    {"lockpicking", 14},
    {"sneak", 15},
    {"alchemy", 16},
    {"speech", 17},
    {"alteration", 18},
    {"conjuration", 19},
    {"destruction", 20},
    {"illusion", 21},
    {"restoration", 22},
    {"enchanting", 23},
    {"health", 24},
    {"magicka", 25},
    {"stamina", 26},
    {"healrate", 27},
    {"magickarate", 28},
    {"staminarate", 29},
    {"speedmult", 30},
    {"inventoryweight", 31},
    {"carryweight", 32},
    {"criticalchance", 33},
    {"meleedamage", 34},
    {"unarmeddamage", 35},
    {"mass", 36},
    {"voicepoints", 37},
    {"voicerate", 38},
    {"damageresist", 39},
    {"poisonresist", 40},
    {"resistfire", 41},
    {"resistshock", 42},
    {"resistfrost", 43},
    {"resistmagic", 44},
    {"resistdisease", 45},
    {"perceptioncondition", 46},
    {"endurancecondition", 47},
    {"leftattackcondition", 48},
    {"rightattackcondition", 49},
    {"leftmobilitycondition", 50},
    {"rightmobilitycondition", 51},
    {"braincondition", 52},
    {"paralysis", 53},
    {"invisibility", 54},
    {"nighteye", 55},
    {"detectliferange", 56},
    {"waterbreathing", 57},
    {"waterwalking", 58},
    {"ignorecrippledlimbs", 59},
    {"fame", 60},
    {"infamy", 61},
    {"jumpingbonus", 62},
    {"wardpower", 63},
    {"rightitemcharge", 64},
    {"armorperks", 65},
    {"shieldperks", 66},
    {"warddeflection", 67},
    {"variable01", 68},
    {"variable02", 69},
    {"variable03", 70},
    {"variable04", 71},
    {"variable05", 72},
    {"variable06", 73},
    {"variable07", 74},
    {"variable08", 75},
    {"variable09", 76},
    {"variable10", 77},
    {"bowspeedbonus", 78},
    {"favoractive", 79},
    {"favorsperday", 80},
    {"favorsperdaytimer", 81},
    {"leftitemcharge", 82},
    {"absorbchance", 83},
    {"blindness", 84},
    {"weaponspeedmult", 85},
    {"shoutrecoverymult", 86},
    {"bowstaggerbonus", 87},
    {"telekinesis", 88},
    {"favorpointsbonus", 89},
    {"lastbribedintimidated", 90},
    {"lastflattered", 91},
    {"movementnoisemult", 92},
    {"bypassvendorstolencheck", 93},
    {"bypassvendorkeywordcheck", 94},
    {"waitingforplayer", 95},
    {"onehandedmodifier", 96},
    {"twohandedmodifier", 97},
    {"marksmanmodifier", 98},
    {"blockmodifier", 99},
    {"smithingmodifier", 100},
    {"heavyarmormodifier", 101},
    {"lightarmormodifier", 102},
    {"pickpocketmodifier", 103},
    {"lockpickingmodifier", 104},
    {"sneakingmodifier", 105},
    {"alchemymodifier", 106},
    {"speechcraftmodifier", 107},
    {"alterationmodifier", 108},
    {"conjurationmodifier", 109},
    {"destructionmodifier", 110},
    {"illusionmodifier", 111},
    {"restorationmodifier", 112},
    {"enchantingmodifier", 113},
    {"onehandedskilladvance", 114},
    {"twohandedskilladvance", 115},
    {"marksmanskilladvance", 116},
    {"blockskilladvance", 117},
    {"smithingskilladvance", 118},
    {"heavyarmorskilladvance", 119},
    {"lightarmorskilladvance", 120},
    {"pickpocketskilladvance", 121},
    {"lockpickingskilladvance", 122},
    {"sneakingskilladvance", 123},
    {"alchemyskilladvance", 124},
    {"speechcraftskilladvance", 125},
    {"alterationskilladvance", 126},
    {"conjurationskilladvance", 127},
    {"destructionskilladvance", 128},
    {"illusionskilladvance", 129},
    {"restorationskilladvance", 130},
    {"enchantingskilladvance", 131},
    {"leftweaponspeedmultiply", 132},
    {"dragonsouls", 133},
    {"combathealthregenmultiply", 134},
    {"onehandedpowermodifier", 135},
    {"twohandedpowermodifier", 136},
    {"marksmanpowermodifier", 137},
    {"blockpowermodifier", 138},
    {"smithingpowermodifier", 139},
    {"heavyarmorpowermodifier", 140},
    {"lightarmorpowermodifier", 141},
    {"pickpocketpowermodifier", 142},
    {"lockpickingpowermodifier", 143},
    {"sneakingpowermodifier", 144},
    {"alchemypowermodifier", 145},
    {"speechcraftpowermodifier", 146},
    {"alterationpowermodifier", 147},
    {"conjurationpowermodifier", 148},
    {"destructionpowermodifier", 149},
    {"illusionpowermodifier", 150},
    {"restorationpowermodifier", 151},
    {"enchantingpowermodifier", 152},
    {"dragonrend", 153},
    {"attackdamagemult", 154},
    {"healratemult", 155},
    {"magickaratemult", 156},
    {"staminaratemult", 157},
    {"werewolfperks", 158},
    {"vampireperks", 159},
    {"grabactoroffset", 160},
    {"grabbed", 161},
    {"deprecated05", 162},
    {"reflectdamage", 163}
};
std::string toLowerCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}
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
std::string TryReadString(json obj, std::string key, std::string def) {
    if (obj.contains(key) && obj[key].is_string()) {
        return obj[key].get<std::string>();
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

    auto avRaw = toLowerCase(TryReadString(obj, "CostActorValue", "magicka"));

    auto it = avMap.find(avRaw);

    if (it != avMap.end()) {
        result.costActorValue = static_cast<RE::ActorValue>(it->second);
    }

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
            auto [hasDestruction, destruction] = ReadObject(item, "Destruction");

            if (hasDestruction) {
                Groups[keyword][RE::ActorValue::kDestruction] = ReadValueModifier(destruction);
            }

            auto [hasIllusion, illusion] = ReadObject(item, "Illusion");

            if (hasIllusion) {
                Groups[keyword][RE::ActorValue::kIllusion] = ReadValueModifier(illusion);
            }

            auto [hasRestoration, restoration] = ReadObject(item, "Restoration");

            if (hasRestoration) {
                Groups[keyword][RE::ActorValue::kRestoration] = ReadValueModifier(restoration);
            }

            auto [hasDefault, _default] = ReadObject(item, "Default");

            if (hasDefault) {
                Groups[keyword][RE::ActorValue::kNone] = ReadValueModifier(_default);
            }
 
        }
    });
}
