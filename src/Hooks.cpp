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
void Hooks::EquipSpellHook::thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                                  RE::BGSEquipSlot** a_slot) {

    if (auto staff = GetEquipedStaff(a_actor, a_slot)) {
        logger::trace("{} Attempted to replace staff staff {} equiped with spell {}", a_actor->GetName(), staff->GetName(), a_spell->GetName());

        auto inv = a_actor->GetInventory();

        auto it = inv.find(staff);

        if (it != inv.end()) {
            if (auto& data = it->second.second) {
                if (!data->extraLists->empty()) {
                    if (auto front = data->extraLists->front()) {
                        auto form = RE::TESForm::LookupByID<RE::EnchantmentItem>(0xb62e1);
                        auto enchantment_fake = RE::BSExtraData::Create<RE::ExtraEnchantment>();
                        auto charge_fake = RE::BSExtraData::Create<RE::ExtraCharge>();
                        charge_fake->charge = 100.f;
                        enchantment_fake->charge = 100;
                        enchantment_fake->enchantment = form;   
                        front->Add(enchantment_fake);
                        front->Add(charge_fake);

                        auto added_charge = front->GetByType<RE::ExtraCharge>();

                        logger::trace("charge {}", added_charge->charge);

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
