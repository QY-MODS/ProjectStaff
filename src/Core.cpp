#include "Core.h"
#include "StaffEnchantment.h"

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
enum class WornSlot { None, Left, Right };

RE::InventoryEntryData* GetEquipedStaff(RE::Actor* a_actor, WornSlot hand) {
    if (auto entry = a_actor->GetEquippedEntryData(hand == WornSlot::Left)) {
        if (entry->object) {
            if (entry->object->Is(RE::FormType::Weapon)) {
                auto weapon = skyrim_cast<RE::TESObjectWEAP*>(entry->object);
                if (weapon->GetWeaponType() == RE::WEAPON_TYPE::kStaff) {
                    return entry;
                }
            }
        }
    }
    return nullptr;
}
RE::ExtraDataList* GetEquipedExtraList(RE::InventoryEntryData* entry) {
    if (entry->extraLists && !entry->extraLists->empty()) {
        if (auto extra = entry->extraLists->front()) {
            return extra;
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
    Core::isRegularEquip.store(false);

    RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, a_object, nullptr, 1, a_slot, false, false, false,
                                                         true, nullptr);
    RE::ActorEquipManager::GetSingleton()->EquipObject(a_actor, a_object, a_extraData, 1, a_slot, false, false, false,
                                                       true);

    Core::isRegularEquip.store(true);
}

StaffEnchantment* GetHand(WornSlot slot) {
    if (slot == WornSlot::Right) {
        return rightHand;
    }
    return leftHand;
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


RE::EnchantmentItem* GetEnchantment(RE::SpellItem* spell, RE::ExtraDataList* extra) {

    auto wornSlot = GetWornSlot(extra);
    auto hand = GetHand(wornSlot);

    hand->spell = spell;

    if (hand->enchantment) {
        hand->CopyEffects();
        return hand->enchantment;
    }
    
    auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::EnchantmentItem>();
    if (auto ench = factory->Create()) {
        hand->enchantment = ench;
        hand->CopyEffects();
        ench->AddChange(1);
        // ench->data.costOverride = 0;
        // ench->data.flags |= RE::EnchantmentItem::EnchantmentFlag::kCostOverride;
        return ench;
    }
    return nullptr;
}

void EnchantStaff(RE::Actor* a_actor, RE::TESObjectWEAP* staff, RE::ExtraDataList* extra) {
    auto wornSlot = GetWornSlot(extra);
    auto hand = GetHand(wornSlot);

    if (!hand->Valid()) {
        return;
    }


    if (auto actor = a_actor->AsActorValueOwner()) {
   
        auto mana = actor->GetBaseActorValue(RE::ActorValue::kMagicka);

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
        }
        if (wornSlot != WornSlot::None) {
            auto actorValue = GetChargeValue(wornSlot);
            actor->ModActorValue(actorValue, mana);
        }
    }
}

bool Core::ProcessEquippedSpell(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                                RE::BGSEquipSlot* a_slot) {
    if (auto staff = GetEquipedStaff(a_actor, a_slot)) {
        logger::trace("{} Attempted to replace staff staff {} equiped with spell {}", a_actor->GetName(),
                      staff->GetName(), a_spell->GetName());

        if (auto extra = GetInventoryExtraList(a_actor, staff)) {
            if (auto ench = GetEnchantment(a_spell, extra)) {
                EnchantStaff(a_actor, staff, extra);
                RefreshEquipedItem(a_actor, staff, extra, a_slot);
            } else {
                logger::trace("Enchantment not found for spell {}", a_spell->GetName());
                return false;
            }
        }

        return false;
    }
    return true;
}

RE::BGSEquipSlot* GetSlot(WornSlot slot) { 
    auto dom = RE::BGSDefaultObjectManager::GetSingleton();
    return dom->GetObject(slot == WornSlot::Left ? RE::DEFAULT_OBJECT::kLeftHandEquip :  RE::DEFAULT_OBJECT::kRightHandEquip)->As<RE::BGSEquipSlot>();
}

void Core::PostLoad() {
    leftHand->CopyEffects();
    rightHand->CopyEffects();

    auto dom = RE::BGSDefaultObjectManager::GetSingleton();
    auto player = RE::PlayerCharacter::GetSingleton();
    if (auto weapon = player->GetEquippedEntryData(true)) {
        RefreshEquipedItem(player, weapon->object, weapon->extraLists->front(), GetSlot(WornSlot::Left));
    }
    if (auto weapon = player->GetEquippedEntryData(false)) {
        RefreshEquipedItem(player, weapon->object, weapon->extraLists->front(), GetSlot(WornSlot::Right));
    }
}

void RemoveStaffEnchantment(WornSlot slot) {
    auto player = RE::PlayerCharacter::GetSingleton();
    if (auto entry = GetEquipedStaff(player, slot)) {
        if (auto extra = GetEquipedExtraList(entry)) {
            if (extra->HasType<RE::ExtraEnchantment>()) {
                extra->RemoveByType(RE::ExtraDataType::kEnchantment);
            }
        }
    }
}
void AddStaffEnchantment(WornSlot slot) {
    auto player = RE::PlayerCharacter::GetSingleton();
    auto dom = RE::BGSDefaultObjectManager::GetSingleton();
    if (auto entry = GetEquipedStaff(player, slot)) {
        if (auto weapon = entry->object->As<RE::TESObjectWEAP>()) {
            if (auto extra = GetEquipedExtraList(entry)) {
                auto wornSlot = GetWornSlot(extra);
                auto hand = GetHand(wornSlot);
                if (hand->Valid()) {
                    EnchantStaff(player, weapon, extra);
                    RefreshEquipedItem(player, entry->object, extra, GetSlot(slot));
                }
            }
        }
    }
}


void Core::EquipEvent() {
    if (!isRegularEquip.load()) {
        return;
    }

    AddStaffEnchantment(WornSlot::Left);
    AddStaffEnchantment(WornSlot::Right);
}


void Core::UnEquipEvent() {

    if (!isRegularEquip.load()) {
        return;
    }

    RemoveStaffEnchantment(WornSlot::Left);
    RemoveStaffEnchantment(WornSlot::Right);
}

