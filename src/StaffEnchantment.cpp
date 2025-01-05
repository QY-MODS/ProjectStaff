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
        auto copy = new RE::Effect();
        copy->effectItem = effect->effectItem;
        copy->baseEffect = effect->baseEffect;
        copy->cost = effect->cost;
        copy->conditions = effect->conditions;

        copy->effectItem.area *= areaMult;
        copy->effectItem.duration *= duartionMult;
        copy->effectItem.magnitude *= magnitudeMult;
        copy->cost *= magnitudeMult;



        enchantment->effects.push_back(copy);
    }

    if (costOverride > 0) {
        enchantment->data.costOverride = costOverride;
        enchantment->data.flags |= RE::EnchantmentItem::EnchantmentFlag::kCostOverride;
    }
    enchantment->data.chargeTime = spell->GetChargeTime() * chargingTimeMult;
    enchantment->data.castingType = spell->GetCastingType();
    enchantment->data.delivery = spell->GetDelivery();
}
