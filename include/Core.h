#pragma once


namespace Core {
    inline std::atomic<bool> isRegularEquip = false;


    RE::ActorValue ProcessActorValueCost(RE::MagicItem* a1);
    bool IsAttemptingToEquipStaff(RE::Actor* a_actor, RE::BGSEquipSlot* a_slot, RE::SpellItem* a_spell);
    bool ProcessEquippedSpell(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                              RE::BGSEquipSlot* a_slot);

    void PostLoad();
}