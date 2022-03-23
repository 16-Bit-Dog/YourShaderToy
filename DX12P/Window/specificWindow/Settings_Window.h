#pragma once

#include "Main_Window.h"

struct MASTER_Setting : MASTER_Function_Inherit {
	inline static MASTER_Setting* obj;

	virtual void settingWindowSettingsMaker() {
		SettingWindowFlag = MASTER_IM_GUI::obj->WindowDrawFlagBuilder(
			false, false,
			false, false, false, false,
			false, false, false, false,
			false
		);
	}

	MASTER_Setting() {
		settingWindowSettingsMaker();
	}

	virtual void BasicViewDraw(GroupData* GD) {

		if (DrawBasicWindow(this, GD, "Settings:")) {

			ImGuiStyle style_T;

			ImGui::Checkbox("##ClearDepthEveryPass Check", &Renderable::DXM->ClearDepthEveryPass);

			if (ImGui::ShowStyleSelector("Colors##Selector"))
				MASTER_IM_GUI::obj->style = &style_T;

			if (ImGui::Button("Save Style Theme"))

				//TODO Save Style settings as new theme

				ImGui::SameLine();

			if (ImGui::Button("Revert Style"))
				ImGui::StyleColorsDark();
			MASTER_IM_GUI::obj->style = &ImGui::GetStyle();
			MASTER_IM_GUI::obj->style->Colors[ImGuiCol_WindowBg].w = 1.0f;

			ImGui::Separator();


			if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) //Litterally borrowed directly from ImGui Demo, since it works *so very well*
			{
				if (ImGui::BeginTabItem("Color"))
				{
					static ImGuiTextFilter filter;
					filter.Draw("Filter color options", ImGui::GetFontSize() * 16);

					static ImGuiColorEditFlags alpha_flags = 0;
					if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None)) { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
					if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
					if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
					HelpMarker(
						"In the color list:\n"
						"Left-click on color square to open color picker,\n"
						"Right-click to open edit options menu.");

					ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
					ImGui::PushItemWidth(-160);

					for (int i = 0; i < ImGuiCol_COUNT; i++)
					{
						const char* name = ImGui::GetStyleColorName(i);
						if (!filter.PassFilter(name))
							continue;
						ImGui::PushID(i);
						ImGui::ColorEdit4("##color", (float*)&MASTER_IM_GUI::obj->style->Colors[i], ImGuiColorEditFlags_AlphaBar);

						ImGui::SameLine(0.0f, MASTER_IM_GUI::obj->style->ItemInnerSpacing.x);
						ImGui::TextUnformatted(name);
						ImGui::PopID();
					}
					ImGui::PopItemWidth();
					ImGui::EndChild();

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
			//MASTER_IM_GUI_obj.style.Colors

			ImGui::End();
		}

	}

};
