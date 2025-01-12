#include "Hooks.h"
#include "Core.h"
#include "MessageBox.h"
void Hooks::Install() {
    EquipSpellHook::Install();
    GetActorValueForCost::Install();
    //UpdateHooks::Install();
    //EquipEvent::Install();
    //uint8_t data[] = {0x90, 0x90};
    //uint8_t data[] = {0xeb, 0x2b};  
    //REL::safe_write(REL::RelocationID(51020, 51898).address() + REL::Relocate(0x0, 0x577), data, sizeof(data));


}

void ShowEquipMessageBox(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                         RE::BGSEquipSlot* a_slot) {
    SkyrimScripting::ShowMessageBox("Where do you want to equip the spell?", {"Equip on Hand", "Equip on staff", "Cancel"},
    [a_manager, a_actor, a_spell, a_slot](int i) {
        if (i == 0) {
            auto sl = a_slot;
            Hooks::EquipSpellHook::originalFunction(a_manager, a_actor, a_spell, &sl);
        } else if (i == 1) {
            Core::ProcessEquippedSpell(a_actor, a_spell, a_slot);
        }
    });
}


void Hooks::EquipSpellHook::thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                                         RE::BGSEquipSlot** a_slot_ptr) {
    if (a_slot_ptr && a_actor->IsPlayerRef()) {
        if (Core::IsAttemptingToEquipStaff(a_actor, *a_slot_ptr, a_spell)) {
            ShowEquipMessageBox(a_manager, a_actor, a_spell, *a_slot_ptr);
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
    originalFunction = trampoline.write_call<5>(REL::RelocationID(37952, 38908).address() + REL::Relocate(0xd7, 0xd7), // Click
                                 thunk);                                                                     // Clicking
    //trampoline.write_call<5>(REL::RelocationID(37950, 38906).address() + REL::Relocate(0xc5, 0xca), thunk); // Hotkey
    //trampoline.write_call<5>(REL::RelocationID(37939, 38895).address() + REL::Relocate(0x47, 0x47), thunkPresise); // Commonlib
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


void Hooks::GetActorValueForCost::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(REL::RelocationID(33362, 34143).address() + REL::Relocate(0x151, 0x151), thunk); // Drain
    trampoline.write_call<5>(REL::RelocationID(33359, 34140).address() + REL::Relocate(0x4c, 0x4d), thunk); // Cancel  cast
    trampoline.write_call<5>(REL::RelocationID(33364, 34145).address() + REL::Relocate(0xc1, 0xbe), thunk); // Can cast
}

RE::ActorValue Hooks::GetActorValueForCost::thunk(RE::MagicItem* a1, bool rightHand) {
    auto av = Core::ProcessActorValueCost(a1);
    if (av != RE::ActorValue::kNone) {
        Core::StopCastIfAvIsEmpty(av);
        return av;
    }
    return originalFunction(a1, rightHand);
}
