#pragma once


namespace Core {
    inline std::atomic<bool> shouldRemoveOnUnequip = false;

    struct StaffEnchantment {
        RE::EnchantmentItem* enchantment = nullptr;
        RE::SpellItem* spell = nullptr;
    };

    inline StaffEnchantment* leftHand = new StaffEnchantment();
    inline StaffEnchantment* rightHand = new StaffEnchantment();

    bool ProcessEquippedSpell(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                              RE::BGSEquipSlot* a_slot);

    void PostLoad();
    void EquipEvent();
    void UnEquipEvent();
}