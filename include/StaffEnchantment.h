#pragma once


class StaffEnchantment {
public:
    RE::EnchantmentItem* enchantment = nullptr;
    RE::SpellItem* spell = nullptr;

    ~StaffEnchantment() {}

    bool Valid();

    void CopyEffects();
};

inline std::map<RE::FormID, StaffEnchantment*> dynamicForms;
