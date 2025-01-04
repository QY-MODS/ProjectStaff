#include "Hooks.h"


void Hooks::Install() {
    EquipSpellHook::Install();
}

RE::TESObjectWEAP* GetEquipedStaff(RE::Actor* a_actor, RE::BGSEquipSlot** a_slot) {
    if (a_actor && a_slot && *a_slot) {
        if (auto item = a_actor->GetEquippedObjectInSlot(*a_slot)) {
            if (auto weapon = item->As<RE::TESObjectWEAP>()) {
                if (weapon->GetWeaponType() == RE::WEAPON_TYPE::kStaff) {
                    return weapon;
                }
            }
        }
    }
    return nullptr;
}

void RefreshEquipedItem(RE::Actor* a_actor, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraData, RE::BGSEquipSlot* a_slot) {

    RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, a_object, nullptr, 1, a_slot, false, false, false, true,nullptr);
    RE::ActorEquipManager::GetSingleton()->EquipObject(a_actor, a_object, a_extraData, 1, a_slot, false, false, false, true);
}

void Hooks::EquipSpellHook::thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                                  RE::BGSEquipSlot** a_slot) {

    if (auto staff = GetEquipedStaff(a_actor, a_slot)) {
        logger::trace("{} Attempted to replace staff staff {} equiped with spell {}", a_actor->GetName(), staff->GetName(), a_spell->GetName());

        auto inv = a_actor->GetInventory();

        auto it = inv.find(staff);

        if (it != inv.end()) {
            if (auto& data = it->second.second) {
                if (!data->extraLists->empty()) {
                    if (auto extra = data->extraLists->front()) {


                        auto form = RE::TESForm::LookupByID<RE::EnchantmentItem>(0x29b5c);


                        logger::trace("enchantment {}", form->GetFormType());


                        if (extra->HasType<RE::ExtraEnchantment>()) {
                            auto e = extra->GetByType<RE::ExtraEnchantment>();
                            e->enchantment = form;
                            e->charge = 10000;
                        } else {
                            auto enchantment_fake = RE::BSExtraData::Create<RE::ExtraEnchantment>();
                            enchantment_fake->charge = 10000;
                            enchantment_fake->enchantment = form;   
                            extra->Add(enchantment_fake);

                            if (auto actor = a_actor->AsActorValueOwner()) {
                                bool wornLeft = extra->HasType(RE::ExtraDataType::kWornLeft);
                                bool wornRight = !wornLeft && extra->HasType(RE::ExtraDataType::kWorn);
                                if (wornLeft || wornRight) {
                                    RE::ActorValue actorValue =
                                        wornLeft ? RE::ActorValue::kLeftItemCharge : RE::ActorValue::kRightItemCharge;

                                    float itemCharge = actor->GetActorValue(actorValue);

                                    actor->ModActorValue(actorValue, 10000);

                                    RefreshEquipedItem(a_actor, staff, extra, *a_slot);
  
                                }
                            }
                        }
                    } else {
                        logger::trace("no fonrt");
                    }
                } else {
                    logger::trace("not in inventory");
                }
            } else {
                logger::trace("no data");
            }
        } else {
            logger::trace("not in inventory");
        }

        return;
    }

    originalFunction(a_manager, a_actor, a_spell, a_slot);
}

void Hooks::EquipSpellHook::Install() {
    //SE ID: 37952 SE Offset: 0xd7 (Heuristic)
    //AE ID: 38908 AE Offset: 0xd7
    // 
    //SE ID: 37950 SE Offset: 0xc5 (Heuristic)
    //AE ID: 38906 AE Offset: 0xca
    // 
    //SE ID: 37939 SE Offset: 0x47 (Heuristic)
    //AE ID: 38895 AE Offset: 0x47
    SKSE::AllocTrampoline(14 * 3);
    auto& trampoline = SKSE::GetTrampoline();

    trampoline.write_call<5>(REL::RelocationID(37952, 38908).address() + REL::Relocate(0xd7, 0xd7), thunk); // Clicking
    trampoline.write_call<5>(REL::RelocationID(37950, 38906).address() + REL::Relocate(0xc5, 0xca), thunk); // Hotkey
    originalFunction = trampoline.write_call<5>(REL::RelocationID(37939, 38895).address() + REL::Relocate(0x47, 0x47), thunk); // Commonlib
                                             
}
