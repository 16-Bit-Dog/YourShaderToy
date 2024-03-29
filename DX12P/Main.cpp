#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#define START_WIDTH 800
#define START_HEIGHT 800

#include <iostream>
#include <../GLFW/glfw3.h>
#include <../GLFW/glfw3native.h>
#include "RenderableManager.h"
#include <../Window/Window.h>

struct MainDX12Objects;
struct GLFW_Window_C;
struct AllWindowDrawLoop;

void FillAllObjects() {

#ifndef D3D12_OFF
	MainDX12Objects::obj = new MainDX12Objects();
#endif

	MainDX11Objects::obj = new MainDX11Objects();
	GLFW_Window_C::MainWin = new GLFW_Window_C();
	MASTER_IM_GUI::obj = new MASTER_IM_GUI();
	MASTER_Pipeline::obj = new MASTER_Pipeline();
	MASTER_Scene::obj = new MASTER_Scene();
	MASTER_Setting::obj = new MASTER_Setting();
	MASTER_Editor::obj = new MASTER_Editor();
	MASTER_FileManager::obj = new MASTER_FileManager();
	MASTER_CodeError::obj = new MASTER_CodeError();
	PipelineMain::obj = new PipelineMain();
	StaticObject::obj = new StaticObject();
}


int runMain() {
	while (!glfwWindowShouldClose(GLFW_Window_C::MainWin->window)) {

	} 
	return 0;
}

int GLFWPreLogic() {

	glfwInit();

	SetDX11Renderer(); //default DX11	
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	StartUpFillVecs(); // before first context do basic Setup

	GLFW_Window_C::MainWin->CreateWindowM(START_WIDTH, START_HEIGHT, "MAIN_CONTEXT");

	AllWin::LoopRunAllContext();

	return 0;
}

int main() {
	FillAllObjects(); //runs first
	GLFWPreLogic();
}