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

    auto& vm = spell->GetAssociatedSkill() == associatedSkill ? skillValueModifier : otherValueModifier;

    for (auto effect : spell->effects) {
        auto copy = new RE::Effect();
        copy->effectItem = effect->effectItem;
        copy->baseEffect = effect->baseEffect;
        copy->cost = effect->cost;
        copy->conditions = effect->conditions;

        copy->effectItem.area *= vm.areaMult;
        copy->effectItem.duration *= vm.duartionMult;
        copy->effectItem.magnitude *= vm.magnitudeMult;
        copy->cost *= vm.magnitudeMult;

        enchantment->effects.push_back(copy);
    }


    if (vm.costOverride > 0) {
        enchantment->data.costOverride = vm.costOverride;
        enchantment->data.flags |= RE::EnchantmentItem::EnchantmentFlag::kCostOverride;
    }
    enchantment->data.chargeTime = spell->GetChargeTime() * vm.chargingTimeMult;
    enchantment->data.castingType = spell->GetCastingType();
    enchantment->data.delivery = spell->GetDelivery();
    
}
