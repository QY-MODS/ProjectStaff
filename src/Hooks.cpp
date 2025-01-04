#include "Hooks.h"

void Hooks::Install() {
    //uint8_t data3[] = {0xe, 0x84, 0xc5, 0x0, 0x0, 0x0};  // allow disenchant anything // SkyrimSE.exe+9091ac 0x90
    //REL::safe_write(REL::RelocationID(37945, 38901).address() + REL::Relocate(0x0, 0x3c), data3, sizeof(data3));
    EquipHook::Install();
}

void Hooks::EquipHook::thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                             const RE::BGSEquipSlot* a_slot) {
    logger::trace("Called: {}", a_actor->GetName(), a_spell->GetName(), a_slot->GetName());
    originalFunction(a_manager, a_actor, a_spell, a_slot);
}

void Hooks::EquipHook::Install() {

        //SE ID: 37939 SE Offset: 0x47 (Heuristic)
        //AE ID: 38895 AE Offset: 0x47
        SKSE::AllocTrampoline(14);
        auto& trampoline = SKSE::GetTrampoline();
        auto address = REL::RelocationID(37939, 38895).address();
        auto offset = REL::Relocate(0x47, 0x47);
        originalFunction = trampoline.write_call<5>(address+offset, thunk);
}
