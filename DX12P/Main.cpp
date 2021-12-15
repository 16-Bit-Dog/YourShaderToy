#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32

#include <iostream>
#include <../GLFW/glfw3.h>
#include <../GLFW/glfw3native.h>
#include <../Renderer/DX12H.h>
#include <../Window/Window.h>

struct MainDX12Objects;
struct GLFW_Window_C;

int Start() {
	Win.FillDXMWithGLFW(&DXM);
	DXM.RendererStartUpLogic();
	return 0;
}

int Close() {

	return 0;
}

int runMain() {
	while (!glfwWindowShouldClose(Win.window)) {

	}
	return 0;
}

void MainLogic() {
	Start();
	runMain();
	Close();
}

int GLFWPreLogic() {
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
	return 0;
}

int main() {
	if (GLFWPreLogic() == 0) {
		MainLogic();
	}
}