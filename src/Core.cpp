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



bool GetSlot(RE::BGSEquipSlot* slot) {
    auto dom = RE::BGSDefaultObjectManager::GetSingleton();
    auto left = dom->GetObject(RE::DEFAULT_OBJECT::kLeftHandEquip);
    return slot == left;
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
        auto mana = actor->GetBaseActorValue(RE::ActorValue::kMagicka);

        auto ench = se->enchantment;

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
                actor->SetActorValue(actorValue, mana);
                actor->SetBaseActorValue(actorValue, mana);
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

bool Core::IsAttemptingToEquipStaff(RE::Actor* a_actor, RE::BGSEquipSlot* a_slot, RE::SpellItem* a_spell) {

    //if (a_spell->IsTwoHanded()) {
    //    return false;
    //}

    if (auto obj = a_actor->GetEquippedEntryData(GetSlot(a_slot))) {
        if (obj->object) {
            if (auto weapon = obj->object->As<RE::TESObjectWEAP>()) {
                if (weapon->GetWeaponType() == RE::WeaponTypes::kStaff) {
                    if (StaffEnchantment::HasKeyword(weapon)) {
                        if (auto list = GetEquipedExtraList(obj)) {
                            if (list->HasType<RE::ExtraEnchantment>()) {
                                if (auto ench = list->GetByType<RE::ExtraEnchantment>()) {
                                    if (auto item = ench->enchantment) {
                                        return dynamicForms.find(item->GetFormID()) != dynamicForms.end();
                                    }
                                }
                                return false;
                            } else {
                                return !weapon->formEnchanting;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Core::ProcessEquippedSpell(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                                RE::BGSEquipSlot* a_slot) {
    if (auto obj = a_actor->GetEquippedEntryData(GetSlot(a_slot))) {
        if (auto weapon = obj->object->As<RE::TESObjectWEAP>()) {
            if (weapon->GetWeaponType() == RE::WeaponTypes::kStaff) {
                if (auto extra = GetEquipedExtraList(obj)) {
                    if (auto ench = GetEnchantment(a_spell, extra, weapon)) {
                        EnchantStaff(a_actor, weapon, extra, ench);
                        RefreshEquipedItem(a_actor, weapon, extra, a_slot);
                    } else {
                        return false;
                    }
                }
            }

            return false;
        }
    }
    return true;
}

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
