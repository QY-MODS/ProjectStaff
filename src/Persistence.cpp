#include "Persistence.h"
#include "Serializer.h"
#include "Core.h"

static void SaveCallback(SKSE::SerializationInterface* a_intfc) {
    if (a_intfc->OpenRecord('SPEL', 1)) {

        auto serializer = Serializer(a_intfc);

        serializer.WriteForm(Core::leftHand->enchantment);
        serializer.WriteForm(Core::leftHand->spell);
        serializer.WriteForm(Core::rightHand->enchantment);
        serializer.WriteForm(Core::rightHand->spell);
    }
}


static void LoadCallback(SKSE::SerializationInterface* a_intfc) {
    uint32_t type;
    uint32_t version;
    uint32_t length;
    bool refreshGame = false;

    while (a_intfc->GetNextRecordInfo(type, version, length)) {
        if (type == 'SPEL' && version == 1) {

            auto serializer = Serializer(a_intfc);

            Core::leftHand->enchantment = serializer.ReadForm<RE::EnchantmentItem>();
            Core::leftHand->spell = serializer.ReadForm<RE::SpellItem>();
            Core::rightHand->enchantment = serializer.ReadForm<RE::EnchantmentItem>();
            Core::rightHand->spell = serializer.ReadForm<RE::SpellItem>();

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
