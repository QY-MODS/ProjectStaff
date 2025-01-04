#pragma once


namespace Core {

    struct StaffEnchantment {
        RE::EnchantmentItem* enchantment = nullptr;
        RE::SpellItem* spell = nullptr;
    };

    bool ProcessEquippedSpell(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                              RE::BGSEquipSlot* a_slot);

    void PostLoad();
}