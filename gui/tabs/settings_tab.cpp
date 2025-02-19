#include "pch-il2cpp.h"
#include "settings_tab.h"
#include "utility.h"
#include "gui-helpers.hpp"
#include "state.hpp"
#include "game.h"
#include "achievements.hpp"
#include "DirectX.h"
#include "imgui/imgui_impl_win32.h" // ImGui_ImplWin32_GetDpiScaleForHwnd
#include "theme.hpp" // ApplyTheme

namespace SettingsTab {
	void Render() {
		if (ImGui::BeginTabItem("Settings")) {
			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
			ImGui::Text("Show/Hide Menu Keybind:");
			ImGui::SameLine();
			if (HotKey(State.KeyBinds.Toggle_Menu)) {
				State.Save();
			}

			ImGui::Text("Show/Hide Console Keybind:");
			ImGui::SameLine();
			if (HotKey(State.KeyBinds.Toggle_Console)) {
				State.Save();
			}

			ImGui::Dummy(ImVec2(7, 7) * State.dpiScale); 
			ImGui::Separator();
			ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
			if (ImGui::Checkbox("Adjust by DPI", &State.AdjustByDPI)) {
				if (!State.AdjustByDPI) {
					State.dpiScale = 1.0f;
				}
				else {
					State.dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(DirectX::window);
				}
				State.dpiChanged = true;
				State.Save();
			}
#ifdef _DEBUG
			static const std::vector<const char*> DPI_SCALING_LEVEL = { "100%", "125%", "150%", "175%", "200%", "225%", "250%", "275%", "300%" };
			ImGui::SameLine();
			int scaleIndex =(int(std::clamp(State.dpiScale, 1.0f, 3.0f) * 100.0f) - 100) / 25;
			if (CustomListBoxInt("Scaling Level", &scaleIndex, DPI_SCALING_LEVEL, 100 * State.dpiScale)) {
				State.dpiScale = (scaleIndex * 25 + 100) / 100.0f;
				State.dpiChanged = true;
			}
#endif
			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

#ifdef _DEBUG
			if (ImGui::Checkbox("Show Debug Tab", &State.showDebugTab)) {
				State.Save();
			}
			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
#endif
			//Change this to in game only once account is overridden
			/*{
				char* nameBuffer[12]{ const_cast<char*>(State.userName.c_str()) };
				if (ImGui::InputText("PlayerName", *nameBuffer, IM_ARRAYSIZE(nameBuffer))) {
					State.userName = std::string(*nameBuffer);
				}
			}
			if (State.userName.empty()) {
				State.userName = GetPlayerName();
			}*/
			char* nameBufferHost[255]{ const_cast<char*>(State.userName.c_str()) };
			if (ImGui::InputText("Username", *nameBufferHost, IM_ARRAYSIZE(nameBufferHost))) {
				State.userName = std::string(*nameBufferHost);
			}

			if (!IsHost()) {
				if (State.userName.length() >= (size_t)13)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username is too long, gets detected by anticheat. This name will be ignored.");
				else if (!IsNameValid(State.userName))
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username contains characters blocked by anticheat. This name will be ignored.");
			}
			ImGui::SameLine();
			if (ImGui::Button("Set Name") && (!IsNameValid(State.userName) || IsHost())) {
				SetPlayerName(State.userName);
				LOG_INFO("Successfully set name to \"" + State.userName + "\"");
				if (IsInGame())
					State.rpcQueue.push(new RpcSetName(State.userName));
				else if (IsInLobby())
					State.lobbyRpcQueue.push(new RpcSetName(State.userName));
			}
			if (ImGui::Checkbox("Automatically Set Name", &State.SetName)) {
				State.Save();
			}
			
			ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);

			if (Achievements::IsSupported()
				&& ImGui::Button("Unlock all achievements"))
			{
				Achievements::UnlockAll();
			}

			ImGui::EndTabItem();
		}
	}
}