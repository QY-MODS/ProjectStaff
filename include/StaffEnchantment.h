#pragma once


struct StaffEnchantment {
    RE::EnchantmentItem* enchantment = nullptr;
    RE::SpellItem* spell = nullptr;

    bool Valid();

    void CopyEffects();
};


inline StaffEnchantment* leftHand = new StaffEnchantment();
inline StaffEnchantment* rightHand = new StaffEnchantment();