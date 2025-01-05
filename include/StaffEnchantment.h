#pragma once



class StaffEnchantment {
public:

    RE::EnchantmentItem* enchantment = nullptr;
    RE::SpellItem* spell = nullptr;

    float magnitudeMult = 100;
    float areaMult = 1;
    float duartionMult = 1;
    float costMult = 1;
    float chargingTimeMult = 1;
    int32_t costOverride = -1;

    ~StaffEnchantment() {}
    StaffEnchantment(RE::EnchantmentItem* enchantment, RE::SpellItem* spell):enchantment(enchantment), spell(spell) {}

    bool Valid();

    void CopyEffects();
};

inline std::map<RE::FormID, StaffEnchantment*> dynamicForms;
