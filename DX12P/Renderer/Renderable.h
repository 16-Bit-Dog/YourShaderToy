#pragma once

#include <../GLFW/glfw3.h>
#include <../GLFW/glfw3native.h>
#include "ResourceObjectBase.h"

struct ResourceObjectBase;

struct Renderable {
	static Renderable* DXM;

	inline static ResourceObjectBase* ROB;

	bool NewImGUIDat = false;

	virtual void ImGUINewFrameLogic() = 0;

	virtual void DrawLogic(bool sync = false) = 0;

	virtual void ImGUIInit() = 0;

	virtual void MakeNewWindowSwapChainAndAssociate(GLFWwindow* windowW, HWND sHwnd, UINT sWidth, UINT sHeight) = 0;

	virtual void RendererStartUpLogic() = 0;

	virtual void CleanRendererState() = 0;//TODO on all renderers
};