#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32

#include <iostream>
#include <../GLFW/glfw3.h>
#include <../GLFW/glfw3native.h>
#include <../Renderer/DX12H.h>

struct MainDX12Objects;

struct GLFW_Window_C{
	GLFWwindow* window;

	void FillDXMWithGLFW(MainDX12Objects* DXM) {
		DXM->hwnd = glfwGetWin32Window(window);
	}

}Win;

