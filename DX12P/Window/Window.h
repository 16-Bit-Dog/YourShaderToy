#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32

#include <iostream>
#include <vector>
#include <string>
#include <../GLFW/glfw3.h>
#include <../GLFW/glfw3native.h>
#include <../Renderer/DX12H.h>
#include <../imGUI/imgui.h>
#include <../imGUI/imgui_impl_glfw.h>
#include <../imGUI/imgui_impl_dx12.h>


void CreateimGUIContext() {
	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = false;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_close = false;
	static bool no_nav = false;
	static bool no_background = false;
	static bool no_bring_to_front = false;
	static bool unsaved_document = false;
	
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
	
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);


}

enum WIN_TYPE { // not used for now, but important to share all "scene" data for the most part
	W_SETTING = 1,
	W_SCENE = 2,
	W_EDITOR = 3,
	W_OBJECT = 4,
};



struct MainDX12Objects;

struct MASTER_Setting {



};
struct MASTER_Scene {



};
struct MASTER_Editor {



};
struct MASTER_Objects {



};

struct GLFW_Window_C {

	UINT WinType = W_SCENE;

	UINT id = -1;

	GLFWwindow* window;

	UINT Width = 0;
	UINT Height = 0;

	std::string title;

	bool Created = false;

	void KillWindow();

	void RemoveWindowFromAllWindowList();

	void FillDXMWithNewGLFW();

	int CreateWindowM(int Swidth, int Sheight, std::string Stitle);

	int RunWindowLogic();

	void CleanSwapChain();

}MainWin;

struct AllWindowDrawLoop{
	UINT CurrWindow = 0;
	std::vector<GLFW_Window_C*> WinList;

	void LoopRunAllContext();

}AllWin;

void GLFW_Window_C::RemoveWindowFromAllWindowList() {
	AllWin.WinList.erase(AllWin.WinList.begin()+id);
	
	
}
void GLFW_Window_C::CleanSwapChain() {
	DXM.m_renderTargets.erase(DXM.m_renderTargets.begin()+id);
	DXM.m_swapChain.erase(DXM.m_swapChain.begin() + id);
	DXM.m_commandAllocator.erase(DXM.m_commandAllocator.begin() + id);
	DXM.m_commandQueue.erase(DXM.m_commandQueue.begin() + id);
	DXM.m_rtvHeap.erase(DXM.m_rtvHeap.begin() + id);
	DXM.m_rtvDS.erase(DXM.m_rtvDS.begin() + id);
	DXM.Width.erase(DXM.Width.begin() + id);
	DXM.Height.erase(DXM.Height.begin() + id);
	DXM.hwnd.erase(DXM.hwnd.begin() + id);
	DXM.m_frameIndex.erase(DXM.m_frameIndex.begin() + id);
	//clean swap chain+related from same index of this window inrelation to WinList which is your window id
}

void GLFW_Window_C::FillDXMWithNewGLFW() {
	DXM.MakeNewWindowSwapChainAndAssociate(glfwGetWin32Window(window), Width, Height);
}

int GLFW_Window_C::CreateWindowM(int Swidth, int Sheight, std::string Stitle) {
	if (Created == false) {
		Width = Swidth;
		Height = Sheight;
		title = Stitle;

		window = glfwCreateWindow(Width, Height, Stitle.c_str(), NULL, NULL);

		if (MainWin.window == NULL)
		{
			std::cout << "Failed to create window" + Stitle << std::endl;
			glfwTerminate();
			return -1;
		}

		id = AllWin.WinList.size();
		Created = true;
		AllWin.WinList.push_back(this);

		FillDXMWithNewGLFW();

	}
}

void SettingWindowLogic() {

}
void SceneWindowLogic() {

}
void EditorWindowLogic() {

}
void ObjectWindowLogic() {

}

int GLFW_Window_C::RunWindowLogic() {
	//TODO, have vector with lambda of void which run? have premade methods based on type? dunno
	
	//basic window Startup
	


	switch(WinType) {
	case W_SETTING:
		//
		break;

	case W_SCENE:
		//
		break;

	case W_EDITOR:
		//
		break;

	case W_OBJECT:
		//
		break;

	}
		

	return -1;
}

void GLFW_Window_C::KillWindow() {
	CleanSwapChain();
	RemoveWindowFromAllWindowList();
	glfwDestroyWindow(window);
}

void KillWindowObj(GLFW_Window_C* winObj) {
	winObj->KillWindow();
}

void AllWindowDrawLoop::LoopRunAllContext() {
	CreateimGUIContext();
	
	while (!glfwWindowShouldClose(WinList[0]->window)) {
		
		IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

		for (int i = 0; i < WinList.size(); i++) {
			if (glfwWindowShouldClose(WinList[i]->window)) {
				KillWindowObj(WinList[i]);
			}
		}
		for (int i = 0; i < WinList.size(); i++) {

			WinList[i]->id = i;
		}
		for (int i = 0; i < WinList.size(); i++) {
				CurrWindow = i;

				glfwMakeContextCurrent(WinList[i]->window);
				glfwPollEvents();
				WinList[i]->RunWindowLogic();
		}
		
	}
}
