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

	inline static uint32_t SetX1 = 400;
	inline static uint32_t SetY1 = 400;

	void HandleWindowSetSize() {
		ImGui::GreenText("Window Size:");
		ImGui::Indent();
		ImGui::InputScalar("X Size ##window size X to change to", ImGuiDataType_U32, &MASTER_Setting::SetX1);
		ImGui::SameLine();
		ImGui::InputScalar("Y Size ##window size Y to change to", ImGuiDataType_U32, &MASTER_Setting::SetY1);
		if (ImGui::Button("|Set Window Size|")) {
			glfwSetWindowSize(GLFW_Window_C::MainWin->window, MASTER_Setting::SetX1, MASTER_Setting::SetY1);
		}
		ImGui::Unindent();

	}

	virtual void BasicViewDraw(GroupData* GD) {

		if (DrawBasicWindow(this, GD, "Settings:")) {

			HandleWindowSetSize();

			ImGuiStyle style_T;
			
			if (ImGui::Button("SAVE WINDOWING SETTINGS##save ini file for ImGui")) {
				ImGui::SaveIniSettingsToDisk("");
			}
			ImGui::Checkbox("Save ImGUI Settings on program shutdown##save ini after shut off", &GLFW_Window_C::SaveImGuiAfterShutdown);

			//
			ImGui::Text("Choose the renderer (support for DXR and Mesh shaders are only D3D12");
			ImGui::InputInt("##Choose the renderer (support for DXR and Mesh shaders are only D3D12)", &GLFW_Window_C::RendererNumber);
			ImGui::SameLine();
			ImGui::HelpMarker("0 is D3D11, 1 is D3D12");
			if (GLFW_Window_C::RendererNumber < 0) GLFW_Window_C::RendererNumber = 0;	
			if (GLFW_Window_C::RendererNumber > 1) GLFW_Window_C::RendererNumber = 1;
			//

			ImGui::Checkbox("Auto Code Compile", &Renderable::DXM->AutoCodeCompile);
			if(Renderable::DXM->AutoCodeCompile) ImGui::InputFloat("Time until Auto Code Compile", &Renderable::DXM->AutoCodeCompile_Wait);

			ImGui::Checkbox("Auto FileManager Compile With Code Compile", &Renderable::DXM->AutoFileManagerCompile);

			ImGui::Checkbox("ClearDepthEveryPass ##Check", &Renderable::DXM->ClearDepthEveryPass);

			ImGui::InputFloat("Cam Forward Shift##Cam Forward Shift Amount Adjustment", &Renderable::DXM->CAM->ShiftForwardAmount, 0.005f, 0.01f);
			ImGui::InputFloat("Cam Side Shift##Cam Side Shift Amount Adjustment", &Renderable::DXM->CAM->ShiftSideAmount, 0.005f, 0.01f);
				
			if (ImGui::ShowStyleSelector("Colors##Selector"))
				ImGuiMainStyle = &style_T;

			if (ImGui::Button("Save Style Theme"))

				//TODO Save Style settings as new theme

				ImGui::SameLine();

			if (ImGui::Button("Revert Style"))
				ImGui::StyleColorsDark();
			ImGuiMainStyle = &ImGui::GetStyle();
			ImGuiMainStyle->Colors[ImGuiCol_WindowBg].w = 1.0f;

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
					ImGui::HelpMarker(
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
						ImGui::ColorEdit4("##color", (float*)&ImGuiMainStyle->Colors[i], ImGuiColorEditFlags_AlphaBar);

						ImGui::SameLine(0.0f, ImGuiMainStyle->ItemInnerSpacing.x);
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
