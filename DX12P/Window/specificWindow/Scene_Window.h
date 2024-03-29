#include "Main_Window.h"

struct MASTER_Scene : MASTER_Function_Inherit {
	inline static MASTER_Scene* obj;

	virtual void settingWindowSettingsMaker() {
		SettingWindowFlag = MASTER_IM_GUI::obj->WindowDrawFlagBuilder(
			false, false,
			false, false, false, false,
			false, false, false, false,
			false
		);
	}

	MASTER_Scene() {
		settingWindowSettingsMaker();
	}

	virtual void BasicViewDraw(GroupData* GD) {
		if (DrawBasicWindow(this, GD, "Scene:")) {
			//TODO: takes in image from view port



			ImGui::End();
		}
	}

};