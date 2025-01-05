#include "Core.h"

RE::TESObjectWEAP* GetEquipedStaff(RE::Actor* a_actor, RE::BGSEquipSlot* a_slot) {
    if (a_actor && a_slot) {
        if (auto item = a_actor->GetEquippedObjectInSlot(a_slot)) {
            if (auto weapon = item->As<RE::TESObjectWEAP>()) {
                if (weapon->GetWeaponType() == RE::WEAPON_TYPE::kStaff) {
                    return weapon;
                }
            }
        }
    }
    return nullptr;
}

RE::TESObjectWEAP* GetEquipedStaffLeftHand(RE::Actor* a_actor) {
    if (auto lefthand_weapon = a_actor->GetEquippedObject(true)) {
        if (lefthand_weapon->Is(RE::FormType::Weapon)) {
            auto weapon = skyrim_cast<RE::TESObjectWEAP*>(lefthand_weapon);
            if (weapon->GetWeaponType() == RE::WEAPON_TYPE::kStaff) {
                return weapon;
            }
        }
    }
    return nullptr;
}

RE::TESObjectWEAP* GetEquipedStaffRightHand(RE::Actor* a_actor) {
    if (auto lefthand_weapon = a_actor->GetEquippedObject(false)) {
        if (lefthand_weapon->Is(RE::FormType::Weapon)) {
            auto weapon = skyrim_cast<RE::TESObjectWEAP*>(lefthand_weapon);
            if (weapon->GetWeaponType() == RE::WEAPON_TYPE::kStaff) {
                return weapon;
            }
        }
    }
    return nullptr;
}

RE::ExtraDataList* GetInventoryExtraList(RE::Actor* actor, RE::TESBoundObject* object) {
    auto inv = actor->GetInventory();

    auto it = inv.find(object);

    if (it != inv.end()) {
        if (auto& data = it->second.second) {
            if (!data->extraLists->empty()) {
                if (auto extra = data->extraLists->front()) {
                    return extra;
                }
            }
        }
    }
    return nullptr;
}
void RefreshEquipedItem(RE::Actor* a_actor, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraData,
                        RE::BGSEquipSlot* a_slot) {
    Core::shouldRemoveOnUnequip.store(false);

    RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, a_object, nullptr, 1, a_slot, false, false, false,
                                                         true, nullptr);
    RE::ActorEquipManager::GetSingleton()->EquipObject(a_actor, a_object, a_extraData, 1, a_slot, false, false, false,
                                                       true);

    Core::shouldRemoveOnUnequip.store(true);
}

enum class WornSlot { None, Left, Right };
Core::StaffEnchantment* GetHand(WornSlot slot) {
    if (slot == WornSlot::Right) {
        return Core::rightHand;
    }
    return Core::leftHand;
}
RE::ActorValue GetChargeValue(WornSlot slot) {
    if (slot == WornSlot::Right) {
        return RE::ActorValue::kRightItemCharge;
    }
    return RE::ActorValue::kLeftItemCharge;
}

WornSlot GetWornSlot(RE::ExtraDataList* a_extraData) {
    if (a_extraData->HasType(RE::ExtraDataType::kWorn)) {
        return WornSlot::Right;
    }
    if (a_extraData->HasType(RE::ExtraDataType::kWornLeft)) {
        return WornSlot::Left;
    }
    return WornSlot::None;
}

void CopyEffects(RE::SpellItem* spell, RE::EnchantmentItem* ench) {
    ench->effects.clear();
    ench->avEffectSetting = nullptr;

    for (auto effect : spell->effects) {
        ench->effects.push_back(effect);
    }
    ench->data.castingType = spell->GetCastingType();
    ench->data.delivery = spell->GetDelivery();
}

RE::EnchantmentItem* GetEnchantment(RE::SpellItem* spell, RE::ExtraDataList* extra) {

    auto wornSlot = GetWornSlot(extra);
    auto hand = GetHand(wornSlot);

    hand->spell = spell;

    if (hand->enchantment) {
        CopyEffects(spell, hand->enchantment);
        return hand->enchantment;
    }
    
    auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::EnchantmentItem>();
    if (auto ench = factory->Create()) {
        hand->enchantment = ench;
        ench->data.spellType = RE::MagicSystem::SpellType::kStaffEnchantment;
        CopyEffects(spell, ench);
        // ench->data.costOverride = 0;
        // ench->data.flags |= RE::EnchantmentItem::EnchantmentFlag::kCostOverride;
        return ench;
    }
    return nullptr;
}

void EnchantStaff(RE::Actor* a_actor, RE::TESObjectWEAP* staff, RE::ExtraDataList* extra,
                  RE::BGSEquipSlot* a_slot) {


    if (auto actor = a_actor->AsActorValueOwner()) {
        auto mana = actor->GetBaseActorValue(RE::ActorValue::kMagicka);
        auto wornSlot = GetWornSlot(extra);

        auto hand = GetHand(wornSlot);

        auto ench = hand->enchantment;

        if (extra->HasType<RE::ExtraEnchantment>()) {
            auto e = extra->GetByType<RE::ExtraEnchantment>();
            e->enchantment = ench;
            e->charge = mana;
        } else {
            auto enchantment_fake = RE::BSExtraData::Create<RE::ExtraEnchantment>();
            enchantment_fake->charge = mana;
            enchantment_fake->enchantment = ench;

            extra->Add(enchantment_fake);


            if (wornSlot != WornSlot::None) {
                auto actorValue = GetChargeValue(wornSlot);
                actor->ModActorValue(actorValue, mana);
            }
        }
    }
    RefreshEquipedItem(a_actor, staff, extra, a_slot);
}

bool Core::ProcessEquippedSpell(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                                RE::BGSEquipSlot* a_slot) {
    if (auto staff = GetEquipedStaff(a_actor, a_slot)) {
        logger::trace("{} Attempted to replace staff staff {} equiped with spell {}", a_actor->GetName(),
                      staff->GetName(), a_spell->GetName());

        if (auto extra = GetInventoryExtraList(a_actor, staff)) {
            if (auto ench = GetEnchantment(a_spell, extra)) {
                EnchantStaff(a_actor, staff, extra, a_slot);
            } else {
                logger::trace("Enchantment not found for spell {}", a_spell->GetName());
                return false;
            }
        }

        return false;
    }
    return true;
}

void Core::PostLoad() {}

void Core::EquipEvent() {}

void Core::UnEquipEvent() {
    if (!shouldRemoveOnUnequip.load()) {
        return;
    }

    auto player = RE::PlayerCharacter::GetSingleton();

    if (auto weapon = player->GetEquippedEntryData(true)) {
        if (weapon->extraLists && !weapon->extraLists->empty()) {
            if (auto extra = weapon->extraLists->front()) {
                if (extra->HasType<RE::ExtraEnchantment>()) {
                    extra->RemoveByType(RE::ExtraDataType::kEnchantment);
                }
            }
        }
    }
    if (auto weapon = player->GetEquippedEntryData(false)) {
        if (weapon->extraLists && !weapon->extraLists->empty()) {
            if (auto extra = weapon->extraLists->front()) {
                if (extra->HasType<RE::ExtraEnchantment>()) {
                    extra->RemoveByType(RE::ExtraDataType::kEnchantment);
                }
            }
        }
    }
}
