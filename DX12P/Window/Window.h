#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <cstdint>
#include <../GLFW/glfw3.h>
#include <../GLFW/glfw3native.h>
#include <../Renderer/DX12H.h>

#include <../imGUI/imgui.h>
#include <../imGUI/imgui_impl_glfw.h>
#include <../imGUI/imgui_impl_dx12.h>


//TODO: use glfwGetFramebufferSize and get window size and set DX + more

//main window in focus is window 0 -- C_GUI Win[0] -- You click to swap the main - each window must have min of 1 of these. Make Way to delete these contexts

//make it so each window has a + icon to add sub context -- vector holds pointer to current context 
//(which you switch around as you switch tabs). new context can be closed. has specific state info. 
//window is imGUI size if not GAME or SCENE type 

struct GroupData;



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

	void SetAndCreateimGUIContext(GLFWwindow* w) {
		SetGUIWindow(w);
		if (RendererMade == false) {
			GUIContext = ImGui::CreateContext();
			ImGuiIO& GUIio = ImGui::GetIO();
			GUIio.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			GUIio.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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


struct MainDX12Objects;

//TODO: make ID for object values to read from GroupData* GD, and add to map accordingly, default 0, else read from (And use consistant BUTTON ID) for new buttons

struct MASTER_Setting {

	void BasicViewDraw(GroupData* GD) {

	}

}MASTER_Setting_m;
struct MASTER_Scene {

	void BasicViewDraw(GroupData* GD) {

	}

}MASTER_Scene_m;
struct MASTER_Editor {

	void BasicViewDraw(GroupData* GD) {

	}

}MASTER_Editor_m;
struct MASTER_Objects {

	void BasicViewDraw(GroupData* GD) {

	}

}MASTER_Objects_m;


struct GroupData {
	//the idea of this struct:
	/*
	when ever it moves - the pointer is passed
	if associated tab is closed, this is deleted :TODO:

	has map of uint to int which holds values of button unique id's from MASTER_*GUI_type* -- this value has the goal to set per window values to allow seperate views of things:
	ex: expand a menu in 1 MASTER_SETTING tab, but in another have that menu not expanded. This allows Unity style syncronous work to be faster when looking at shader to make
	//if ID does not exist it defaults it to 0 int value, and sets the UINT so it now exists
	*/


	int WindowType = 0;

	std::function<void()> ToDraw;

	std::map < UINT, int> BidI; //button id associated with for each i'm gui 

	

	void LinkToSettings() {
		WindowType = W_SETTING;
		
		ToDraw = [&]() { MASTER_Setting_m.BasicViewDraw(this); };
	}
	void LinkToScene() {
		WindowType = W_SCENE;

		ToDraw = [&]() { MASTER_Scene_m.BasicViewDraw(this); };
	}
	void LinkToEditor() {
		WindowType = W_EDITOR;
	
		ToDraw = [&]() { MASTER_Editor_m.BasicViewDraw(this); };
	}
	void LinkToObjects() {
		WindowType = W_OBJECTS;

		ToDraw = [&]() { MASTER_Objects_m.BasicViewDraw(this); };
	}
};

struct GLFW_Window_C {

	std::vector<GroupData*> C_GUI_Win;//GroupData* - window group associated with GLFW Object windows to allow tab stacking -[0] is your current tab

	UINT id = -1;

	GLFWwindow* window;

	UINT Width = 0;
	UINT Height = 0;

	std::string title;


	bool Created = false;

	void KillWindow();

	void RemoveWindowFromAllWindowList();

	void RemoveAssociatedGUIFromWindowObj();

	void FillDXMWithNewGLFW();

	int CreateWindowM(int Swidth, int Sheight, std::string Stitle, int WinType);

	int RunWindowLogic();

	void CleanSwapChain();

}MainWin;

struct AllWindowDrawLoop{
	UINT CurrWindow = 0;
	std::vector<GLFW_Window_C*> WinList;

	void LoopRunAllContext();

}AllWin;

void GLFW_Window_C::RemoveAssociatedGUIFromWindowObj() {
	for (int i = 0; i < C_GUI_Win.size(); i++) {
		delete C_GUI_Win[i];
	}

	C_GUI_Win.empty();
}

void GLFW_Window_C::RemoveWindowFromAllWindowList() {
	AllWin.WinList.erase(AllWin.WinList.begin()+id);	
}
void GLFW_Window_C::CleanSwapChain() {
	//clean swap chain+related from same index of this window inrelation to WinList which is your window id
}

void GLFW_Window_C::FillDXMWithNewGLFW() {
	DXM.MakeNewWindowSwapChainAndAssociate(glfwGetWin32Window(window), Width, Height);
}

int GLFW_Window_C::CreateWindowM(int Swidth, int Sheight, std::string Stitle, int WinType = 1) {
	//kept this as a back bone from what I prev' did for window creation
	if (Created == false) {
		C_GUI_Win.push_back(new GroupData());

		C_GUI_Win[0]->WindowType = WinType; // loop through all and make based on GroupData?

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
	
	{ //ex app
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
	ImGui::ShowDemoWindow();

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

void AllWindowDrawLoop::LoopRunAllContext() {
	
	glfwSwapInterval(1); //vsync

	MASTER_IM_GUI_obj.SetAndCreateimGUIContext(WinList[0]->window);

	while (!glfwWindowShouldClose(WinList[0]->window)) {

		
		for (int i = 0; i < WinList.size(); i++) {
			if (glfwWindowShouldClose(WinList[i]->window) || WinList[i]->C_GUI_Win.size()==0) {
				KillWindowObj(WinList[i]);
				i -= 1;
			}
		}
		for (int i = 0; i < WinList.size(); i++) {
			WinList[i]->id = i;
		}
		for (int i = 0; i < WinList.size(); i++) {

				CurrWindow = i;

				glfwMakeContextCurrent(WinList[i]->window);
				glfwPollEvents();
				ImGui_ImplDX12_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				WinList[i]->RunWindowLogic();

				MASTER_IM_GUI_obj.EndRender();
		}
		
	}
}
