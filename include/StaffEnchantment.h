#pragma once

struct ValueModifier {
    float magnitudeMult = 1;
    float areaMult = 1;
    float duartionMult = 1;
    float costMult = 1;
    float chargingTimeMult = 1;
    int32_t costOverride = -1;
    int32_t chargeOverride = 10;
};

class StaffEnchantment {
public:

    RE::EnchantmentItem* enchantment = nullptr;
    RE::SpellItem* spell = nullptr;

    RE::ActorValue associatedSkill = RE::ActorValue::kDestruction;

    ValueModifier skillValueModifier;
    ValueModifier otherValueModifier;

    ~StaffEnchantment() {}
    StaffEnchantment(RE::EnchantmentItem* enchantment, RE::SpellItem* spell):enchantment(enchantment), spell(spell) {}

    bool Valid();

    void CopyEffects();
};

inline std::map<RE::FormID, StaffEnchantment*> dynamicForms;
