#pragma once


namespace Core {
    bool ProcessEquippedSpell(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                              RE::BGSEquipSlot* a_slot);
}