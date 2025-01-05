#include "Plugin.h"
#include "Hooks.h"
#include "Core.h"
#include "Persistence.h"
#include "Config.h"
void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        Config::Load();
    }
    if (message->type == SKSE::MessagingInterface::kPostLoadGame) {
        Core::PostLoad();
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    SetupLog();
    logger::info("Plugin loaded");
    UI::Register();
    Hooks::Install();
    Persistence::Install();
    return true;
}