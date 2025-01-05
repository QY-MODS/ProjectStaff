#include "StaffEnchantment.h"




bool StaffEnchantment::Valid() {
    if (!enchantment || !spell) {
        logger::trace("did not copy");
        return false;
    }
    return true;
}

void StaffEnchantment::CopyEffects() {
    if (!Valid()) {
        return;
    }
    logger::trace("copied");

    enchantment->effects.clear();
    enchantment->avEffectSetting = nullptr;
    enchantment->data.spellType = RE::MagicSystem::SpellType::kStaffEnchantment;

    for (auto effect : spell->effects) {
        enchantment->effects.push_back(effect);
    }
    enchantment->data.castingType = spell->GetCastingType();
    enchantment->data.delivery = spell->GetDelivery();
}
