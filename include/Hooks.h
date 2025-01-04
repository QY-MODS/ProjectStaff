#pragma once


namespace Hooks
{
	void Install();

	struct EquipHook {
        static void thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell,
                          const RE::BGSEquipSlot* a_slot);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
		static void Install();
	};
}