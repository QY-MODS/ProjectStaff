#pragma once


namespace Core {
    inline std::atomic<bool> shouldRemoveOnUnequip = false;

    struct StaffEnchantment {
        RE::EnchantmentItem* enchantment = nullptr;
        RE::SpellItem* spell = nullptr;

        void CopyEffects() {
            enchantment->effects.clear();
            enchantment->avEffectSetting = nullptr;

            for (auto effect : spell->effects) {
                enchantment->effects.push_back(effect);
            }
            enchantment->data.castingType = spell->GetCastingType();
            enchantment->data.delivery = spell->GetDelivery();
        }

    };

    inline StaffEnchantment* leftHand = new StaffEnchantment();
    inline StaffEnchantment* rightHand = new StaffEnchantment();

    bool ProcessEquippedSpell(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                              RE::BGSEquipSlot* a_slot);

    void PostLoad();
    void EquipEvent();
    void UnEquipEvent();
}