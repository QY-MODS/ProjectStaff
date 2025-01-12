#include "Core.h"

#include "StaffEnchantment.h"


enum class WornSlot { None, Left, Right };

RE::BGSEquipSlot* GetSlot(WornSlot slot) {
    auto dom = RE::BGSDefaultObjectManager::GetSingleton();

    if (slot == WornSlot::Left) {
        return dom->GetObject(RE::DEFAULT_OBJECT::kLeftHandEquip)->As<RE::BGSEquipSlot>();
    }

    if (slot == WornSlot::Right) {
        return dom->GetObject(RE::DEFAULT_OBJECT::kRightHandEquip)->As<RE::BGSEquipSlot>();
    }
    return nullptr;
}

WornSlot GetSlot(RE::BGSEquipSlot* slot) {
    auto dom = RE::BGSDefaultObjectManager::GetSingleton();
    auto left = dom->GetObject(RE::DEFAULT_OBJECT::kLeftHandEquip);
    return slot == left ? WornSlot::Left : WornSlot::Right;
}


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

RE::TESForm* GetEquippedObjectInSlot(const RE::Actor* actor, const RE::BGSEquipSlot* slot) {
    auto _currentProcess = actor->GetActorRuntimeData().currentProcess;
    if (_currentProcess) {
        for (const auto& equippedObject : _currentProcess->equippedForms) {
            if (equippedObject.slot == slot) {
                return equippedObject.object;
            }
        }
    }

    return nullptr;
}

void RefreshEquipedItem(RE::Actor* a_actor, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraData,
                        RE::BGSEquipSlot* a_slot) {
    RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, a_object, nullptr, 1, a_slot, true, false, false,
                                                         true, nullptr);
    RE::ActorEquipManager::GetSingleton()->EquipObject(a_actor, a_object, a_extraData, 1, a_slot, true, false, false,
                                                       true);
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

StaffEnchantment* GetEnchantment(RE::SpellItem* spell, RE::ExtraDataList* extra, RE::TESObjectWEAP* weap) {

    std::string keyword;
    if (!StaffEnchantment::GetKeyword(weap, keyword)) {
        return nullptr;
    }

    auto wornSlot = GetWornSlot(extra);

    if (extra->HasType<RE::ExtraEnchantment>()) {
        auto e = extra->GetByType<RE::ExtraEnchantment>();

        if (e->enchantment) {
            auto it = dynamicForms.find(e->enchantment->GetFormID());
            if (it != dynamicForms.end()) {
                auto df = it->second;
                df->keyword = keyword;
                df->spell = spell;
                df->CopyEffects();
                return df;
            }
        }
    } else {
        auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::EnchantmentItem>();
        if (auto ench = factory->Create()) {
            ench->AddChange(1);
            auto df = new StaffEnchantment(ench, spell);
            df->keyword = keyword;
            df->CopyEffects();
            dynamicForms[ench->GetFormID()] = df;
            return df;
        }
    }
    return nullptr;
}

class PrefixRemover {
    std::string prefix;

public:
    PrefixRemover(std::string prefix) : prefix(prefix) {}
    std::string Remove(const std::string& input) { return input.substr(prefix.length()); }
    bool Has(const std::string& input) {
        if (input.rfind(prefix, 0) == 0) {
            return true;
        }
        return false;
    }
};

void EnchantStaff(RE::Actor* a_actor, RE::TESObjectWEAP* staff, RE::ExtraDataList* extra, StaffEnchantment* se) {
    auto wornSlot = GetWornSlot(extra);

    if (!se->Valid()) {
        return;
    }

    if (auto actor = a_actor->AsActorValueOwner()) {
        auto magicka = actor->GetBaseActorValue(RE::ActorValue::kMagicka);
        auto health = actor->GetBaseActorValue(RE::ActorValue::kHealth);
        auto stamina= actor->GetBaseActorValue(RE::ActorValue::kStamina);

        auto charge = std::max(std::max(magicka, health), stamina);

        auto ench = se->enchantment;

        if (extra->HasType<RE::ExtraEnchantment>()) {
            auto e = extra->GetByType<RE::ExtraEnchantment>();
            e->enchantment = ench;
            if (se->costActorValue == RE::ActorValue::kNone) {
                e->charge = charge;
            } else {
                e->charge = 0;
            }
        } else {
            auto enchantment_fake = RE::BSExtraData::Create<RE::ExtraEnchantment>();

            if (se->costActorValue == RE::ActorValue::kNone) {
                enchantment_fake->charge = charge;
            } else {
                enchantment_fake->charge = 0;
            }

            enchantment_fake->enchantment = ench;


            extra->Add(enchantment_fake);

            if (wornSlot != WornSlot::None) {
                auto actorValue = GetChargeValue(wornSlot);
                actor->SetActorValue(actorValue, charge);
                actor->SetBaseActorValue(actorValue, charge);
            }

            if (!extra->HasType<RE::ExtraTextDisplayData>()) {
                auto remover = PrefixRemover("Unenchanted ");
                if (remover.Has(staff->GetName())) {
                    auto xText = RE::BSExtraData::Create<RE::ExtraTextDisplayData>();
                    xText->SetName(remover.Remove(staff->GetName()).c_str());
                    extra->Add(xText);
                }
            }
        }
    }
}

RE::ActorValue Core::ProcessActorValueCost(RE::MagicItem* a1) { 
    if (a1) {
        auto it = dynamicForms.find(a1->GetFormID());
        if (it != dynamicForms.end()) {
            return it->second->costActorValue;
        }
    }
    return RE::ActorValue::kNone;
}

StaffEnchantment* GetStaffEnchantment(RE::ExtraDataList* list) {
    if (list->HasType<RE::ExtraEnchantment>()) {
        if (auto ench = list->GetByType<RE::ExtraEnchantment>()) {
            if (auto item = ench->enchantment) {
                auto it = dynamicForms.find(item->GetFormID());

                if (it != dynamicForms.end()) {
                    return it->second;
                }
            }
        }
    }
    return nullptr;
}

RE::TESObjectWEAP* GetStaff(RE::InventoryEntryData* obj) {
    if (!obj || !obj->object) {
        return nullptr;
    }
    if (auto weapon = obj->object->As<RE::TESObjectWEAP>()) {
        if (weapon->GetWeaponType() == RE::WeaponTypes::kStaff) {
            return weapon;
        }
    }
    return nullptr;
}

bool IsAnyStaffEquiped(RE::InventoryEntryData* obj) {
    if (auto weapon = GetStaff(obj)) {
        if (StaffEnchantment::HasKeyword(weapon)) {
            return true;
        }
    }
    return false;
}

bool Core::IsAttemptingToEquipStaff(RE::Actor* a_actor, RE::BGSEquipSlot* a_slot, RE::SpellItem* a_spell) {

    if (!a_spell) {
        return false;
    }

    if (
        a_spell->GetSpellType() != RE::MagicSystem::SpellType::kLeveledSpell &&
        a_spell->GetSpellType() != RE::MagicSystem::SpellType::kSpell) {
        return false;
    }

    auto hand = GetSlot(a_slot);

    if (auto obj = a_actor->GetEquippedEntryData(hand == WornSlot::Left)) {
        if (IsAnyStaffEquiped(obj)) {
            return true;
        }
    }

    return false;
}

bool ProcessHandEquipedSpell(WornSlot hand, RE::Actor* a_actor, RE::SpellItem* a_spell) {
    if (auto obj = a_actor->GetEquippedEntryData(hand == WornSlot::Left)) {
        if (auto weapon = GetStaff(obj)) {
            if (auto extra = GetEquipedExtraList(obj)) {
                if (auto ench = GetEnchantment(a_spell, extra, weapon)) {
                    EnchantStaff(a_actor, weapon, extra, ench);
                    RefreshEquipedItem(a_actor, weapon, extra, GetSlot(hand));
                    return true;
                }
            }
        }
    }
    return false;
}
bool Core::ProcessEquippedSpell(RE::Actor* a_actor, RE::SpellItem* a_spell,
                                RE::BGSEquipSlot* a_slot) {
    auto hand = GetSlot(a_slot);

    if (ProcessHandEquipedSpell(hand, a_actor, a_spell)) {
        return false;
    }

    return true;
}


void Core::StopCastIfAvIsEmpty(RE::MagicItem* magicItem, RE::ActorValue actorValue) {
    auto player = RE::PlayerCharacter::GetSingleton();

    if (player->IsCasting(magicItem)) {
         if (auto actor = player->AsActorValueOwner()) {
            if (actor->GetActorValue(actorValue) <= 0) {
                player->InterruptCast(true);
            }
        }
    }
}


void Core::PostLoad() {
    auto dom = RE::BGSDefaultObjectManager::GetSingleton();
    auto player = RE::PlayerCharacter::GetSingleton();
    if (auto weapon = player->GetEquippedEntryData(true)) {
        RefreshEquipedItem(player, weapon->object, weapon->extraLists->front(), GetSlot(WornSlot::Left));
    }
    if (auto weapon = player->GetEquippedEntryData(false)) {
        RefreshEquipedItem(player, weapon->object, weapon->extraLists->front(), GetSlot(WornSlot::Right));
    }
}
