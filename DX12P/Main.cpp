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

int Start() {

	Renderable::DXM->RendererStartUpLogic();
	return 0;
}

int runMain() {
	while (!glfwWindowShouldClose(GLFW_Window_C::MainWin.window)) {

	} 
	return 0;
}

int GLFWPreLogic() {
	SetDX11Renderer(); //default DX11
	
	glfwInit();
	
	Renderable::DXM->RendererStartUpLogic();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	StartUpFillVecs(); // before first context do basic Setup

	GLFW_Window_C::MainWin.CreateWindowM(START_WIDTH, START_HEIGHT, "MAIN_CONTEXT");

	AllWin::LoopRunAllContext();

	return 0;
}

int main() {
	GLFWPreLogic();
}