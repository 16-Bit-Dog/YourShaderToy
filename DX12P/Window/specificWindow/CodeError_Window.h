#pragma once

#include "Main_Window.h"

struct MASTER_CodeError : MASTER_Function_Inherit {
	inline static MASTER_CodeError* obj;

	virtual void settingWindowSettingsMaker() {
		SettingWindowFlag = MASTER_IM_GUI::obj->WindowDrawFlagBuilder(
			false, false,
			false, false, false, false,
			false, false, false, false,
			false
		);
	}

	MASTER_CodeError() {
		settingWindowSettingsMaker();
	}
	void PrintError(PipelineObj* P) {
		P->Vertex.PrintError();
		P->Pixel.PrintError();
	}

	virtual void BasicViewDraw(GroupData* GD) {

		if (DrawBasicWindow(this, GD, "Code Error:")) {

			ImGui::Text("Errors:");
			
			for (const auto& i : PipelineMain::obj->P) {

				if (ImGui::CollapsingHeader(i.second->GetName().c_str(), NULL)) {

					PrintError(i.second);

				}

			}

			ImGui::End();
		}

	}

};
