#include "UI.h"

void UI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection("SKSE Menu Framework Compiled Example 4");
    SKSEMenuFramework::AddSectionItem("Add Item", Example1::Render);
}


void __stdcall UI::Example1::Render() {
    if (ImGui::Button("JUST DO IT")) {
    
        auto player = RE::PlayerCharacter::GetSingleton();



    }
}
