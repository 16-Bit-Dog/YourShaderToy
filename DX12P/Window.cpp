#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32

#include <iostream>
#include <../GLFW/glfw3.h>
#include <../GLFW/glfw3native.h>
#include "DX12H.h"

struct GLFW_Window_C{
	GLFWwindow* window;
}Win;

int runMain() {
	return -1;
}

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	Win.window = glfwCreateWindow(800, 800, "DX12_App", NULL, NULL);
	
	if (Win.window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(Win.window);
	 
	DXM.hwnd = glfwGetWin32Window(Win.window);
	
	DXM.StartUpLogic();

	while (!glfwWindowShouldClose(Win.window)) {
		runMain();
	}
}