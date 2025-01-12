#include "UI.h"
#include "StaffEnchantment.h"

void UI::Register() {
#ifndef NDEBUG

    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection("Immersive Staves");
    SKSEMenuFramework::AddSectionItem("Add Item", Example1::Render);

#endif
}
#ifndef NDEBUG

std::map<RE::ActorValue, std::string> AVTranslation = {
        {RE::ActorValue::kAlteration,"Alteration"},
        {RE::ActorValue::kConjuration,"Conjuration"},
        {RE::ActorValue::kDestruction,"Destruction"},
        {RE::ActorValue::kIllusion,"Illusion"},
		{RE::ActorValue::kRestoration,"Restoration"},
		{RE::ActorValue::kNone,"Default"}
};
#endif

void AddObjectByKeyword(std::string  key) {
    const auto& [map, lock] = RE::TESForm::GetAllForms();
    const RE::BSReadWriteLock l{lock};
    for (auto& [id, form] : *map) {
        if (form) {
            if (auto bound = form->As<RE::TESBoundObject>()) {
                if (form->HasKeywordByEditorID(key)) {
                    auto player = RE::PlayerCharacter::GetSingleton();
                    player->AddObjectToContainer(bound, nullptr, 1, nullptr);
                }
            }
        }
    }
}
void __stdcall UI::Example1::Render() {

    if (ImGui::Button("Add All")) {
        for (auto [key, value] : Groups) {
            AddObjectByKeyword(key);
        }
    
    }

    int i = 0;
    for (auto [key, value] : Groups) {



        ImGui::Text(std::format("Num Dynamic forms: {}", dynamicForms.size()).c_str());
        ImGui::Text(std::format("Keyword: {}", key).c_str());
        
        if (ImGui::Button(std::format("Add##add_{}", i++).c_str())) {
            AddObjectByKeyword(key);
        }

        //for (auto [key2, value2] : value) {

        //    auto it = AVTranslation.find(key2);
        //    std::string text;
        //    if (it != AVTranslation.end()) {
        //        text = it->second;
        //    } else {
        //        text = "Unknown";
        //    }

        //    ImGui::Text(std::format("    ActorValue: {}", text).c_str());
        //    ImGui::Text(std::format("        areaPercentage: {}", value2.areaPercentage).c_str());
        //    ImGui::Text(std::format("        chargingTimePercentage: {}", value2.chargingTimePercentage).c_str());
        //    ImGui::Text(std::format("        costPercentage: {}", value2.costPercentage).c_str());
        //    ImGui::Text(std::format("        durationPercentage: {}", value2.durationPercentage).c_str());
        //    ImGui::Text(std::format("        magnitudePercentage: {}", value2.magnitudePercentage).c_str());
        //    ImGui::Text(std::format("        costOverride: {}", value2.costOverride).c_str());
        //}
    }
}
