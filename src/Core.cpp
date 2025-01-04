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
    RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, a_object, nullptr, 1, a_slot, false, false, false,
                                                         true, nullptr);
    RE::ActorEquipManager::GetSingleton()->EquipObject(a_actor, a_object, a_extraData, 1, a_slot, false, false, false,
                                                       true);
}

RE::EnchantmentItem* GetEnchantment(RE::SpellItem* spell) {

    auto factory = RE::IFormFactory::GetFormFactoryByType(RE::FormType::Enchantment);
    if (auto createdForm = factory->Create()) {
        if (auto ench = createdForm->As<RE::EnchantmentItem>()) {
            ench->data.spellType = RE::MagicSystem::SpellType::kStaffEnchantment;
            for (auto effect : spell->effects) {
				ench->effects.push_back(effect);
			}
            ench->data.castingType = spell->GetCastingType();
            ench->data.delivery = spell->GetDelivery();
            return ench;
        }
    }
    return nullptr;
}

bool Core::ProcessEquippedSpell(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                                RE::BGSEquipSlot* a_slot) {
    if (auto staff = GetEquipedStaff(a_actor, a_slot)) 
    {
        logger::trace("{} Attempted to replace staff staff {} equiped with spell {}", a_actor->GetName(),
                      staff->GetName(), a_spell->GetName());

        if (auto extra = GetInventoryExtraList(a_actor, staff)) {
                if (auto ench = GetEnchantment(a_spell)) {
                    logger::trace("enchantment {}", ench->GetFormType());

                    if (extra->HasType<RE::ExtraEnchantment>()) {
                        auto e = extra->GetByType<RE::ExtraEnchantment>();
                        e->enchantment = ench;
                        e->charge = 10000;
                    } else {
                        auto enchantment_fake = RE::BSExtraData::Create<RE::ExtraEnchantment>();
                        enchantment_fake->charge = 10000;
                        enchantment_fake->enchantment = ench;
                        extra->Add(enchantment_fake);

                        if (auto actor = a_actor->AsActorValueOwner()) {
                            bool wornLeft = extra->HasType(RE::ExtraDataType::kWornLeft);
                            bool wornRight = !wornLeft && extra->HasType(RE::ExtraDataType::kWorn);
                            if (wornLeft || wornRight) {
                                RE::ActorValue actorValue =
                                    wornLeft ? RE::ActorValue::kLeftItemCharge : RE::ActorValue::kRightItemCharge;

                                float itemCharge = actor->GetActorValue(actorValue);

                                actor->ModActorValue(actorValue, 10000);

                            }
                        }
                    }
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
