#include "Persistence.h"
#include "Serializer.h"
#include "StaffEnchantment.h"
#include "Core.h"

#define VERSION 4

static void SaveCallback(SKSE::SerializationInterface* a_intfc) {
    if (a_intfc->OpenRecord('SPEL', VERSION)) {

        auto serializer = Serializer(a_intfc);

        serializer.Write<uint32_t>(dynamicForms.size());

        for (auto& [key, value] : dynamicForms) {
            serializer.WriteForm(value->enchantment);
            serializer.WriteForm(value->spell);
            serializer.Write<float>(value->magnitudeMult);
            serializer.Write<float>(value->duartionMult);
            serializer.Write<float>(value->areaMult);
            serializer.Write<float>(value->costMult);
            serializer.Write<float>(value->chargingTimeMult);
            serializer.Write<int32_t>(value->costOverride);
        }

    }
}


static void LoadCallback(SKSE::SerializationInterface* a_intfc) {
    uint32_t type;
    uint32_t version;
    uint32_t length;
    bool refreshGame = false;

    while (a_intfc->GetNextRecordInfo(type, version, length)) {
        if (type == 'SPEL' && version == VERSION) {

            auto serializer = Serializer(a_intfc);

            auto size = serializer.Read<uint32_t>();

            for (auto [key, value] : dynamicForms) {
                delete value;
            }

            dynamicForms.clear();

            for (auto i = 0; i < size; ++i) {
                auto enchantment = serializer.ReadForm<RE::EnchantmentItem>();
                auto spell = serializer.ReadForm<RE::SpellItem>();
                auto se = new StaffEnchantment(enchantment, spell);

                se->magnitudeMult = serializer.Read<float>();
                se->duartionMult = serializer.Read<float>();
                se->areaMult = serializer.Read<float>();
                se->costMult = serializer.Read<float>();
                se->chargingTimeMult = serializer.Read<float>();
                se->costOverride = serializer.Read<int32_t>();

                se->CopyEffects();
                dynamicForms[enchantment->GetFormID()] = se;
            }

            Core::PostLoad();
        }
    }
}

void Persistence::Install() {
    auto serialization = SKSE::GetSerializationInterface();
    serialization->SetUniqueID('DSTV');
    serialization->SetSaveCallback(SaveCallback);
    serialization->SetLoadCallback(LoadCallback);
}
