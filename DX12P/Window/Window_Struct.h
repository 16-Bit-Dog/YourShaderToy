#pragma once

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
#include <../imGUI/imgui.h>
#include <../imGUI/imgui_impl_glfw.h>

struct MASTER_IM_GUI;
struct MainDX12Objects;
struct MASTER_Setting;
struct MASTER_Scene;
struct MASTER_Editor;
struct MASTER_Objects;
struct GroupData;

inline static ImGuiStyle* ImGuiMainStyle;
inline static ImFont* CustomFont;
inline static ImFont* CustomFontSmall;

struct GLFW_Window_C {
	//generic glfw class 

	inline static GLFW_Window_C* MainWin;

	std::vector<GroupData*> C_GUI_Win;//GroupData* - window group associated with GLFW Object windows to allow tab stacking -[0] is your current tab

	UINT id = -1;

	GLFWwindow* window;

	GLFWscrollfun GLFWscrollfunObj;
	GLFWmousebuttonfun GLFWmousebuttonfunObj;

	inline static int LastFrameOfRendererNumber = 0;
	inline static int RendererNumber = 0;

	inline static void CheckToRemakeAndLaunchRenderer();

	inline static void RunD3D12RendererLaunchLogic();
	inline static void RunD3D11RendererLaunchLogic();

	inline static int Width = 0;
	inline static int Height = 0;

	inline static double MousePosX;
	inline static double MousePosY;

	inline static int MouseLeftState;
	inline static int MouseRightState;
	inline static int MouseMiddleState;

	inline static double time;

	inline static double TimeOfLastPress;
	
	inline static double DeltaOfLastPress;
	inline static double DeltaOfLastPress_CompileReset;

	inline static bool SaveImGuiAfterShutdown = true;

	static void StartPipelineCompileTimer() { DeltaOfLastPress_CompileReset = 0.0f; }


	std::string title;


	bool Created = false;

	void KillWindow();

	void RemoveWindowFromAllWindowList();

	void RemoveAssociatedGUIFromWindowObj();

	void FillDXMWithNewGLFW();

	int CreateWindowM(int Swidth, int Sheight, const std::string& Stitle, int WinType);

	int RunWindowLogic();

	void CleanSwapChain();

	
	void WindowPreamble() {
		glfwGetCursorPos(window, &MousePosX, &MousePosY);
		glfwGetFramebufferSize(window, &Width, &Height);
		MouseLeftState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		MouseRightState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
		MouseMiddleState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
		time = glfwGetTime();
		DeltaOfLastPress = time - TimeOfLastPress;
		if(DeltaOfLastPress_CompileReset > -1.0f)
			DeltaOfLastPress_CompileReset = DeltaOfLastPress;
	}
	void ResetTime() {
		TimeOfLastPress = glfwGetTime();
		DeltaOfLastPress_CompileReset = 0.0f;
	}
	
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		MainWin->ResetTime();
	}


	GLFW_Window_C() {
		TimeOfLastPress = glfwGetTime();
		BasePipeline::StartPipelineCompileTimer = StartPipelineCompileTimer;
	}
};
