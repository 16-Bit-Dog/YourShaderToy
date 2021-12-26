#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <cstdint>
#include <../GLFW/glfw3.h>
#include <../GLFW/glfw3native.h>
#include <../imGUI/imgui.h>
#include <../imGUI/imgui_impl_glfw.h>
#include <../imGUI/imgui_impl_dx12.h>
#include <../Renderer/DX12H.h>


struct GroupData;
struct MainDX12Objects;

UINT GLOBAL_WINDOW_ID = 0;

UINT GLOBAL_WINDOW_ID_I() {
	GLOBAL_WINDOW_ID += 1;
	return GLOBAL_WINDOW_ID-1;
}

struct GroupData {
	//the idea of this struct:
	/*
	when ever it moves - the pointer is passed
	if associated tab is closed, this is deleted :TODO:

	has map of uint to int which holds values of button unique id's from MASTER_*GUI_type* -- this value has the goal to set per window values to allow seperate views of things:
	ex: expand a menu in 1 MASTER_SETTING tab, but in another have that menu not expanded. This allows Unity style syncronous work to be faster when looking at shader to make
	//if ID does not exist it defaults it to 0 int value, and sets the UINT so it now exists
	*/

	UINT ID = 0;

	int WindowType = 0;

	std::function<void()> ToDraw;

	std::map < UINT, int> BidI; //button id associated with for each i'm gui 

	bool DontKillWindowBool = true;

	void LinkToSettings();
	void LinkToScene();
	void LinkToEditor();
	void LinkToObjects();
};
enum WIN_TYPE { // not used for now, but important to share all "scene" data for the most part
	W_SETTING = 1,
	W_SCENE = 2,
	W_EDITOR = 3,
	W_OBJECTS = 4,
};

struct MASTER_IM_GUI {
	ImGuiContext* GUIContext; // global for global use
	bool RendererMade = false;
	GLFWwindow* window;

	void SetGUIWindow(GLFWwindow* w) {
		window = w;
	}

	ImGuiWindowFlags WindowDrawFlagBuilder(bool no_titlebar_t, bool no_scrollbar_t,
		bool no_menu_t, bool no_move_t, bool no_resize_t, bool no_collapse_t,
		bool no_close_t, bool no_nav_t, bool no_background_t, bool no_bring_to_front_t,
		bool unsaved_document_t) {

		bool no_titlebar = no_titlebar_t;
		bool no_scrollbar = no_scrollbar_t;
		bool no_menu = no_menu_t;
		bool no_move = no_move_t;
		bool no_resize = no_resize_t;
		bool no_collapse = no_collapse_t;
		bool no_close = no_close_t;
		bool no_nav = no_nav_t;
		bool no_background = no_background_t;
		bool no_bring_to_front = no_bring_to_front_t;
		bool unsaved_document = unsaved_document_t;

		ImGuiWindowFlags window_flags = 0;
		if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
		if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
		if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
		if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
		if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
		if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
		if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
		if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
		if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;

		return window_flags;
	}

	void SetAndCreateimGUIContext(GLFWwindow* w) {
		SetGUIWindow(w);
		if (RendererMade == false) {
			GUIContext = ImGui::CreateContext();
			ImGuiIO& GUIio = ImGui::GetIO();
			GUIio.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			GUIio.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

			ImGuiStyle& style = ImGui::GetStyle();

			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;



			ImGui_ImplDX12_Init(DXM.m_device.Get(), DXM.FrameCount, DXGI_FORMAT_R8G8B8A8_UNORM, DXM.ImGUIHeap.Get(), DXM.ImGUIHeap->GetCPUDescriptorHandleForHeapStart(), DXM.ImGUIHeap->GetGPUDescriptorHandleForHeapStart());

			ImGui_ImplGlfw_InitForOther(window, true);
			RendererMade = true;

			//	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
			//	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
			//	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		}

	}

	void EndRender() {
		ImGui::Render();

		DXM.SetupAndSendimGUIData();
	}

}MASTER_IM_GUI_obj;


//TODO: make ID for object values to read from GroupData* GD, and add to map accordingly, default 0, else read from (And use consistant BUTTON ID) for new buttons

struct MASTER_Function_Inherit {
	ImGuiWindowFlags SettingWindowFlag;

	void DrawBasicDemoText(MASTER_Function_Inherit* WindowFlags, GroupData* GD) {
		if(GD->DontKillWindowBool){
			if (!ImGui::Begin("This is a demo window" + GD->ID, &GD->DontKillWindowBool, WindowFlags->SettingWindowFlag)) {
				ImGui::End();
			}
			else {
				ImGui::Text("This is some useful text.");

				ImGui::SameLine();
				ImGui::Text("counter = empty");

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}
		}
	}

};

struct MASTER_Setting : MASTER_Function_Inherit {

	void settingWindowSettingsMaker() {
		SettingWindowFlag = MASTER_IM_GUI_obj.WindowDrawFlagBuilder(
			false, false,
			false, false, false, false,
			false, false, false, false,
			false
		);
	}

	MASTER_Setting() {
		settingWindowSettingsMaker();
	}

	void BasicViewDraw(GroupData* GD) {

		DrawBasicDemoText(this, GD);

	}

}MASTER_Setting_m;
struct MASTER_Scene : MASTER_Function_Inherit {

	void settingWindowSettingsMaker() {
		SettingWindowFlag = MASTER_IM_GUI_obj.WindowDrawFlagBuilder(
			false, false,
			false, false, false, false,
			false, false, false, false,
			false
		);
	}

	MASTER_Scene() {
		settingWindowSettingsMaker();
	}

	void BasicViewDraw(GroupData* GD) {

		DrawBasicDemoText(this, GD);

	}

}MASTER_Scene_m;
struct MASTER_Editor : MASTER_Function_Inherit {

	void settingWindowSettingsMaker() {
		SettingWindowFlag = MASTER_IM_GUI_obj.WindowDrawFlagBuilder(
			false, false,
			false, false, false, false,
			false, false, false, false,
			false
		);
	}

	MASTER_Editor() {
		settingWindowSettingsMaker();
	}

	void BasicViewDraw(GroupData* GD) {

		DrawBasicDemoText(this, GD);

	}

}MASTER_Editor_m;
struct MASTER_Objects : MASTER_Function_Inherit {

	void settingWindowSettingsMaker() {
		SettingWindowFlag = MASTER_IM_GUI_obj.WindowDrawFlagBuilder(
			false, false,
			false, false, false, false,
			false, false, false, false,
			false
		);
	}

	MASTER_Objects() {
		settingWindowSettingsMaker();
	}

	void BasicViewDraw(GroupData* GD) {

		DrawBasicDemoText(this, GD);

	}

}MASTER_Objects_m;





	void GroupData::LinkToSettings() {
		WindowType = W_SETTING;

		ToDraw = [&]() { MASTER_Setting_m.BasicViewDraw(this); };
	}
	void GroupData::LinkToScene() {
		WindowType = W_SCENE;

		ToDraw = [&]() { MASTER_Scene_m.BasicViewDraw(this); };
	}
	void GroupData::LinkToEditor() {
		WindowType = W_EDITOR;

		ToDraw = [&]() { MASTER_Editor_m.BasicViewDraw(this); };
	}
	void GroupData::LinkToObjects() {
		WindowType = W_OBJECTS;

		ToDraw = [&]() { MASTER_Objects_m.BasicViewDraw(this); };
	}

