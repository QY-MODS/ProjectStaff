#pragma once


namespace Hooks
{
	void Install();

    struct EquipSpellHook {
        static void thunk(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::SpellItem* a_spell, RE::BGSEquipSlot** a_slot);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static void Install();
    };

    class UpdateHooks {
    public:
            static void Install();
    private:
        static void thunk();
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    class GetActorValueForCost {
    public:
        static void Install();

    private:
        static RE::ActorValue thunk(RE::MagicItem* a1, bool rightHand);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };


    class EquipEvent final : public RE::BSTEventSink<RE::TESEquipEvent> {
    public:
        // This method will be called when the event is triggered
        RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* a_event,
                                              RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) override;

        static void Install();
    };
}