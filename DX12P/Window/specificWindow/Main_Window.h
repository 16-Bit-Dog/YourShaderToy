#pragma once

#include "Master_ImGUI_Settings.h"

struct MASTER_Function_Inherit {
	ImGuiWindowFlags WinBlockF = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoDocking;
		

	ImGuiInputTextFlags TextType = ImGuiInputTextFlags_AllowTabInput;

	ImGuiWindowFlags SettingWindowFlag; //DO NOT USE VARS FROM THIS STRUCT, ONLY FROM ALIAS OF THIS STRUCT

	static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
	void DrawTopMenuBar(MASTER_Function_Inherit* WF, GroupData* GD) {
		if (ImGui::BeginMenuBar()) {

			if (ImGui::BeginMenu("New Window"))
			{
				for (const auto& i : WIN_TYPE::Win_Type_Name_Vector) {
					if (GD->NewWindowCreationHandle.count(i.first) == 0) { //new window bool set to false if not in map of new windows to be made bool map
						GD->NewWindowCreationHandle[i.first] = false;
					}
					ImGui::MenuItem(i.second.c_str(), NULL, &GD->NewWindowCreationHandle[i.first]);
				}

				ImGui::EndMenu();
			}


			ImGui::EndMenuBar();
		}
	}

	bool DrawBasicWindow(MASTER_Function_Inherit* WF, GroupData* GD, std::string WindowName) {
		if (GD->DontKillWindowBool) {
			if (!ImGui::Begin((WindowName + std::to_string(GD->ID)).c_str() /*This adds garbage to the end, but it is unique garbage which I use as an invisible 'id' to seperate closing tabs and such*/
				, &GD->DontKillWindowBool, WF->SettingWindowFlag)) {
				ImGui::End();
				return false;
			}
			else {
				DrawTopMenuBar(WF, GD);

				//ImGui::Text("This is some useful text.");
				//ImGui::SameLine();
				//ImGui::Text("counter = empty");
				//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				//ImGui::End();
				return true;
			}
		}
	}

	virtual void BasicViewDraw(GroupData* GD) = 0;

	virtual void settingWindowSettingsMaker() = 0;

};
