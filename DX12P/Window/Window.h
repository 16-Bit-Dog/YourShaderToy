#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <cstdint>
#include "RenderableManager.h"
#include "Window_Struct.h"
#include <../Window/GUI_Logic.h>
#include <../Window/Window_Type.h>

struct MASTER_IM_GUI;
struct MainDX12Objects;
struct MASTER_Setting;
struct MASTER_Scene;
struct MASTER_Editor;
struct MASTER_Objects;
struct GroupData;


struct AllWin {
	inline static UINT CurrWindow = 0;
	inline static std::vector<GLFW_Window_C*> WinList;

	inline static void LoopRunAllContext();

};

void GLFW_Window_C::RemoveAssociatedGUIFromWindowObj() {
	for (int i = 0; i < C_GUI_Win.size(); i++) {
		delete C_GUI_Win[i];
	}

	C_GUI_Win.empty();
}

void GLFW_Window_C::RemoveWindowFromAllWindowList() {
	AllWin::WinList.erase(AllWin::WinList.begin() + id);
}
void GLFW_Window_C::CleanSwapChain() {
	//clean swap chain+related from same index of this window inrelation to WinList which is your window id
}

void GLFW_Window_C::FillDXMWithNewGLFW() {
	Renderable::DXM->MakeNewWindowSwapChainAndAssociate(window, glfwGetWin32Window(window), Width, Height);
}

int GLFW_Window_C::CreateWindowM(int Swidth, int Sheight, std::string Stitle, int WinType = 1) {
	//kept this as a back bone from what I prev' did for window creation
	if (Created == false) {

		C_GUI_Win.push_back(new GroupData(this));
		C_GUI_Win.push_back(new GroupData(this));
		C_GUI_Win.push_back(new GroupData(this));
		C_GUI_Win.push_back(new GroupData(this));

		C_GUI_Win[0]->LinkToSettings();
		C_GUI_Win[0]->ID = GLOBAL_WINDOW_ID_I();
		C_GUI_Win[1]->LinkToScene();
		C_GUI_Win[1]->ID = GLOBAL_WINDOW_ID_I();
		C_GUI_Win[2]->LinkToEditor();
		C_GUI_Win[2]->ID = GLOBAL_WINDOW_ID_I();
		C_GUI_Win[3]->LinkToPipeline();
		C_GUI_Win[3]->ID = GLOBAL_WINDOW_ID_I();

		Width = Swidth;
		Height = Sheight;
		title = Stitle;

		window = glfwCreateWindow(Width, Height, Stitle.c_str(), NULL, NULL);

		if (GLFW_Window_C::MainWin.window == NULL)
		{
			std::cout << "Failed to create window" + Stitle << std::endl;
			glfwTerminate();
			return -1;
		}

		id = AllWin::WinList.size();
		Created = true;
		AllWin::WinList.push_back(this);

		FillDXMWithNewGLFW();
	}
}

int GLFW_Window_C::RunWindowLogic() {
	//TODO, have vector with lambda of void which run? have premade methods based on type? dunno
	WindowPreamble();

	if (C_GUI_Win.size() == 0) {
		C_GUI_Win.push_back(new GroupData(this));

		C_GUI_Win[0]->LinkToSettings();
		C_GUI_Win[0]->ID = GLOBAL_WINDOW_ID_I();
	}

	for (int i = 0; i < C_GUI_Win.size(); i++) {
		if (C_GUI_Win[i]->DontKillWindowBool == false) {
			delete C_GUI_Win[i];
			C_GUI_Win.erase(C_GUI_Win.begin() + i);
			i -= 1;
		}
	} //seperate loop in case I start wanting to assign unique ID's
	for (int i = 0; i < C_GUI_Win.size(); i++) { //make new window

		for (int ii = 1; ii < WIN_TYPE::Win_Type_ID_Vector.size() + 1; ii++) {

			GroupData* TmpView = C_GUI_Win[i]->MakeNewMainWindowCheckAndDo(ii);
			if (TmpView != nullptr) {
				C_GUI_Win.push_back(TmpView);
			}

		}

	}
	for (int i = 0; i < C_GUI_Win.size(); i++) {
		C_GUI_Win[i]->ToDraw();
	}

	//ImGui::ShowDemoWindow();

	return -1;
}

void GLFW_Window_C::KillWindow() {
	CleanSwapChain();
	RemoveAssociatedGUIFromWindowObj();
	RemoveWindowFromAllWindowList();
	glfwDestroyWindow(window);
}

void KillWindowObj(GLFW_Window_C* winObj) {
	winObj->KillWindow();
}

void AllWin::LoopRunAllContext() {

	glfwSwapInterval(1); //vsync

	MASTER_IM_GUI::obj.SetAndCreateimGUIContext(WinList[0]->window);

	while (WinList.size() > 0 && !glfwWindowShouldClose(WinList[0]->window)) {


		for (int i = 0; i < WinList.size(); i++) {
			if (glfwWindowShouldClose(WinList[i]->window)) {
				KillWindowObj(WinList[i]);
				i -= 1;
			}
		}
		for (int i = 0; i < WinList.size(); i++) {
			WinList[i]->id = i;
		}

		Renderable::DXM->ImGUINewFrameLogic();

		for (int i = 0; i < WinList.size(); i++) { //I expect 1 frame in the current state of the program

			CurrWindow = i;

			glfwMakeContextCurrent(WinList[i]->window);
			glfwPollEvents();


			WinList[i]->RunWindowLogic();


		}
		MASTER_IM_GUI::obj.EndRender();
	}
}
