#pragma once

struct ValueModifier {
    float magnitudeMult = 1;
    float areaMult = 1;
    float duartionMult = 1;
    float costMult = 1;
    float chargingTimeMult = 1;
    int32_t costOverride = -1;
};

struct ValueModifierGroup {
    ValueModifier* skillValueModifier;
    ValueModifier* otherValueModifier;
};
enum StaffType {
    None,
    Destruction = 1,
    Alteration = 2,
    Conjuration = 3,
    Illusion = 4,
    Restoration = 5,
    Falmer = 6,
    Forsworn = 7
};



inline std::map<StaffType, ValueModifierGroup> modifierGroups = {
    {StaffType::Alteration, ValueModifierGroup{new ValueModifier{1,1,1,0.8,1,-1}, new ValueModifier{}}},
    {StaffType::Conjuration, ValueModifierGroup{new ValueModifier{1, 1, 1.2, 1, 1, -1}, new ValueModifier{}}},
    {StaffType::Destruction, ValueModifierGroup{new ValueModifier{1.2, 1, 1, 1, 1, -1}, new ValueModifier{}}},
    {StaffType::Restoration, ValueModifierGroup{new ValueModifier{1.2, 1, 1, 1, 1, -1}, new ValueModifier{}}},
    {StaffType::Illusion, ValueModifierGroup{new ValueModifier{1, 1, 1, 0.8, 1, -1}, new ValueModifier{}}},
    {StaffType::Forsworn, ValueModifierGroup{new ValueModifier{0.1, 1, 1, 0.0, 1, -1}, nullptr}},
    {StaffType::Falmer, ValueModifierGroup{new ValueModifier{0.9, 1, 1, 1.0, 0.5, -1}, nullptr}}
};

class StaffEnchantment {
    RE::ActorValue GetActorValue(StaffType staffType);

public:

    RE::EnchantmentItem* enchantment = nullptr;
    RE::SpellItem* spell = nullptr;
    StaffType staffType = StaffType::None;
    ~StaffEnchantment() {}
    StaffEnchantment(RE::EnchantmentItem* enchantment, RE::SpellItem* spell):enchantment(enchantment), spell(spell) {}

    bool Valid();

    void CopyEffects();
};

inline std::map<RE::FormID, StaffEnchantment*> dynamicForms;
