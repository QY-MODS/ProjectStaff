#pragma once

struct ValueModifier {
    float magnitudePercentage = 100;
    float areaPercentage = 100;
    float durationPercentage = 100;
    float costPercentage = 100;
    float chargingTimePercentage = 100;
    int32_t costOverride = -1;
    RE::ActorValue costActorValue = RE::ActorValue::kMagicka;
};


inline std::map<std::string, std::map<RE::ActorValue, ValueModifier>> Groups;


class StaffEnchantment {
    bool GetValueModifierGroup(ValueModifier& group);


public:
    static bool GetKeyword(RE::TESForm* weapon, std::string& keyword);
    static bool HasKeyword(RE::TESForm* weapon);

    RE::EnchantmentItem* enchantment = nullptr;
    RE::SpellItem* spell = nullptr;
    RE::ActorValue costActorValue = RE::ActorValue::kNone;

    std::string keyword;

    ~StaffEnchantment() {}
    StaffEnchantment(RE::EnchantmentItem* enchantment, RE::SpellItem* spell):enchantment(enchantment), spell(spell) {}

    bool Valid();

    void CopyEffects();
};

inline std::map<RE::FormID, StaffEnchantment*> dynamicForms;
