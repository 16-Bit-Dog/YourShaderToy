#pragma once

#include "3DCommons/3DCommon.h"
#include "LuaObj.h"
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <cstdint>
#include <../GLFW/glfw3.h>
#include <../GLFW/glfw3native.h>
#include <../imGUI/imgui.h>
#include <../imGUI/imgui_impl_glfw.h>
#include "RenderableManager.h"
#include "D3D11ResourceObjects.h"
#include <../Window/Window_Type.h>
#include <../imGUI/imgui_stdlib.h>
#include <../imGUI/ImGUIFileDialog/ImGuiFileDialog.h>
#include "PipelineMain.h"

struct GLFW_Window_C;

namespace ImGui {

	void InputTextMultilineQuick(std::string Tag, std::string* S, ImGuiInputTextFlags* flags) {
		ImGui::InputTextMultiline(Tag.c_str(), S, ImVec2(ImGui::GetWindowWidth() * 0.8f, 200), *flags, NULL, nullptr);
	}
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

}

//TODO: also allow switching what tab is what - option called - "Switch Tab" where it switches tab
struct GroupData {
	//the idea of this struct:
	/*
	when ever it moves - the pointer is passed
	if associated tab is closed, this is deleted :TODO:

	has map of uint to int which holds values of button unique id's from MASTER_*GUI_type* -- this value has the goal to set per window values to allow seperate views of things:
	ex: expand a menu in 1 MASTER_SETTING tab, but in another have that menu not expanded. This allows Unity style syncronous work to be faster when looking at shader to make
	//if ID does not exist it defaults it to 0 int value, and sets the UINT so it now exists
	*/
	GroupData(GLFW_Window_C* WTH) {
		WindowTopHandle = WTH;
	}

	GLFW_Window_C* WindowTopHandle;

	std::map<int, bool> NewWindowCreationHandle;

	UINT ID = 0;

	int WindowType = 0;

	std::function<void()> ToDraw;

	std::map < UINT, int> BidI; //button id associated with for each i'm gui 

	bool DontKillWindowBool = true;

	void LinkToSettings();
	void LinkToScene();
	void LinkToEditor();
	void LinkToPipeline();
	void LinkToFileManager();
	int LinkBasedOnInt(int Input);
	GroupData* MakeNewMainWindowCheckAndDo(int WindowType);
};

struct MASTER_IM_GUI {
	ImGuiContext* GUIContext; // global for global use
	bool RendererMade = false;
	GLFWwindow* window;

	ImGuiStyle* style;

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

			style = &ImGui::GetStyle();

			//style->WindowRounding = 0.0f;
			//


			DXM->ImGUIInit();

			ImGui_ImplGlfw_InitForOther(window, true);
			RendererMade = true;

			//	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
			//	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
			//	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		}

	}

	void EndRender() {
		ImGui::Render();
		DXM->NewImGUIDat = true;
		DXM->DrawLogic();
	}


	//Settings that adjust with settings tab TODO:

	//Global Settings:
	/*[All have Toggle option for USE_GLOBAL[Check box]*/
	//Text: 
	//TextSize
	//TextColor
	//TextFont - TTF file loader
	//
	//
	//Window BackGround Color
	//

	//Performance Settings:
	//
	//
	//
	//

}MASTER_IM_GUI_obj;
std::string sPad(int num) {
	std::string n = " ";
	for (int i = 0; i < num; i++) n += " ";
	return n;
}

//TODO: make ID for object values to read from GroupData* GD, and add to map accordingly, default 0, else read from (And use consistant BUTTON ID) for new buttons

/*
STEPS TO MAKE NEW IMGUI WINDOW HANDLE FOR PROGRAM:
1. make struct to inherit from MASTER_Function_Inherit with simmilar procedure to "MASTER_Setting" and associated
2. call before using your new window struct Add_New_Win_Type([](GroupData* GDV) { MASTER_NEW-WINDOW-NAME_m.BasicViewDraw(GDV); }, "NEW-WINDOW-NAME"); // same format as in GUI_Logic.h
3. store the returned int as the window type ID
and everything now works :thumbsup: -- TODO: Still need to make this easier by allowing easy acsess to the StartUpFillVecs() and adding custom extension
*/
