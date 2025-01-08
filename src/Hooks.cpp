#include "Hooks.h"
#include "Core.h"
#include "MessageBox.h"
void Hooks::Install() {
    EquipSpellHook::Install();
    //UpdateHooks::Install();
    //EquipEvent::Install();
}



void Hooks::EquipSpellHook::thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                                  RE::BGSEquipSlot** a_slot_ptr) {

    if (a_slot_ptr && a_actor->IsPlayerRef()) {
        if (Core::IsAttemptingToEquipStaff(a_actor, *a_slot_ptr, a_spell)) {
            auto slot = *a_slot_ptr;
            SkyrimScripting::ShowMessageBox("What do you want to do?",
                {"Equip spell", "Apply Enchantment", "Cancel"}, [a_manager,a_actor,a_spell, slot](int i) {
                    if (i == 0) {
                        auto sl = slot;
                        originalFunction(a_manager, a_actor, a_spell, &sl);
                    } else if (i == 1) {
                        Core::ProcessEquippedSpell(a_manager, a_actor, a_spell, slot);
                    } else {
                    }    
                
            });

            return;
        }
        
    }
    originalFunction(a_manager, a_actor, a_spell, a_slot_ptr);
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
    originalFunction = trampoline.write_call<5>(REL::RelocationID(37952, 38908).address() + REL::Relocate(0xd7, 0xd7),
                                             thunk);  // Clicking
    //trampoline.write_call<5>(REL::RelocationID(37950, 38906).address() + REL::Relocate(0xc5, 0xca), thunk); // Hotkey
   // originalFunction = trampoline.write_call<5>(REL::RelocationID(37939, 38895).address() + REL::Relocate(0x47, 0x47), thunk); // Commonlib
                                             
}

RE::BSEventNotifyControl Hooks::EquipEvent::ProcessEvent(const RE::TESEquipEvent* a_event,
                                                         RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) {

    if (!a_event) {
        return RE::BSEventNotifyControl::kContinue;
    }

    if (a_event->actor && a_event->actor->IsPlayerRef()) {
		if (a_event->equipped) {
		}
        else {
		}
	}

    return RE::BSEventNotifyControl::kContinue;
}

void Hooks::EquipEvent::Install() {
    RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink(new EquipEvent());
}

void Hooks::UpdateHooks::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction =
        trampoline.write_call<5>(REL::RelocationID(35565, 36564).address() + REL::Relocate(0x748, 0xc2b), thunk);
}

void Hooks::UpdateHooks::thunk() { 
    originalFunction(); 
    float g_deltaTime = *(float*)REL::RelocationID(523660, 410199).address();
}
