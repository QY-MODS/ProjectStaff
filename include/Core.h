#pragma once


namespace Core {
    inline std::atomic<bool> isRegularEquip = false;



    bool IsAttemptingToEquipStaff(RE::Actor* a_actor, RE::BGSEquipSlot* a_slot);
    bool ProcessEquippedSpell(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                              RE::BGSEquipSlot* a_slot);

    void PostLoad();
}