#include "Hooks.h"
#include "Core.h"

void Hooks::Install() {
    EquipSpellHook::Install();
    RE::ScriptEventSourceHolder::GetSingleton()
        ->AddEventSink(new EquipEvent());
}



void Hooks::EquipSpellHook::thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                                  RE::BGSEquipSlot** a_slot) {

    if (!a_slot || !a_actor->IsPlayerRef() || Core::ProcessEquippedSpell(a_manager, a_actor, a_spell, *a_slot)) {
        originalFunction(a_manager, a_actor, a_spell, a_slot);
    }
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

RE::BSEventNotifyControl Hooks::EquipEvent::ProcessEvent(const RE::TESEquipEvent* a_event,
                                                         RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) {

    if (!a_event) {
        return RE::BSEventNotifyControl::kContinue;
    }

    if (a_event->actor && a_event->actor->IsPlayerRef()) {
		if (a_event->equipped) {
			Core::EquipEvent();
		}
        else {
			Core::UnEquipEvent();
		}
	}

    return RE::BSEventNotifyControl::kContinue;
}
