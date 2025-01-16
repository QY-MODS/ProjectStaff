#include "StaffEnchantment.h"


bool StaffEnchantment::GetValueModifierGroup(ValueModifier& group) {
    auto it = Groups.find(keyword);
    if (it == Groups.end()) {
        return false;
    }

    auto groupIterator = it->second;

    auto valueModifierIterator = groupIterator.find(spell->GetAssociatedSkill());

    if (valueModifierIterator == groupIterator.end()) {
        valueModifierIterator = groupIterator.find(RE::ActorValue::kNone);

        if (valueModifierIterator == groupIterator.end()) {
            return false;
        }
    }

    group = valueModifierIterator->second;

    return true;
}

bool StaffEnchantment::GetKeyword(RE::TESForm* weapon, std::string& keyword) {
    for (auto& [key, value] : Groups) {
        if (weapon->HasKeywordByEditorID(key.c_str())) {
            keyword = key;
            return true;
        }
    }
    return false;
}

bool StaffEnchantment::HasKeyword(RE::TESForm* weapon) {
    std::string dummy;
    return GetKeyword(weapon, dummy);
}


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

    ValueModifier vm;

    if (!GetValueModifierGroup(vm)) {
        return;
    }

    enchantment->effects.clear();
    enchantment->avEffectSetting = nullptr;

    enchantment->data.spellType = RE::MagicSystem::SpellType::kStaffEnchantment;

    for (auto effect : spell->effects) {
        auto copy = new RE::Effect();
        copy->effectItem = effect->effectItem;

        auto mag = copy->effectItem.magnitude * vm.magnitudePercentage / 100;

        if (
            effect->baseEffect && 
            effect->baseEffect->data.archetype == RE::EffectArchetype::kValueModifier &&
            effect->baseEffect->data.primaryAV == costActorValue &&
            effect->baseEffect->data.delivery == RE::MagicSystem::Delivery::kSelf
        )
        {
            copy->effectItem.magnitude = std::min(0.f, mag);
        } else {
            copy->effectItem.magnitude = mag;
        }

        copy->baseEffect = effect->baseEffect;
        copy->cost = effect->cost * vm.costPercentage / 100;
        copy->conditions = effect->conditions;

        copy->effectItem.area *= vm.areaPercentage / 100;
        copy->effectItem.duration *= vm.durationPercentage / 100;

        enchantment->effects.push_back(copy);
    }


    auto& costMap = spell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration ? costEffectMapPerSecound : costEffectMap;

    auto it = costMap.find(vm.costActorValue);
    if (it != costMap.end()) {
        if (auto effect = it->second->Get()) {
            auto copy = new RE::Effect();
            copy->baseEffect = effect;
            copy->effectItem.magnitude = enchantment->CalculateMagickaCost(nullptr);
            enchantment->effects.push_back(copy);
        }
    }

    if (vm.costOverride >= 0) {
        enchantment->data.costOverride = vm.costOverride;
        enchantment->data.flags |= RE::EnchantmentItem::EnchantmentFlag::kCostOverride;
    }

    enchantment->data.chargeTime = spell->GetChargeTime() * vm.chargingTimePercentage / 100;
    enchantment->data.castingType = spell->GetCastingType();
    enchantment->data.delivery = spell->GetDelivery();

    costActorValue = vm.costActorValue;
}
