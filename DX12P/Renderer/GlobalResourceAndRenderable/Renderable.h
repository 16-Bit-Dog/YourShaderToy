#pragma once

#include <../GLFW/glfw3.h>
#include <../GLFW/glfw3native.h>
#include "ResourceObjectBase.h"
#include "CamManager.h"

struct ResourceObjectBase;

enum RASTER_TYPE {
	ALL_SOLID = 0,
	ALL_WIRE = 1,
	BACK_SOLID = 2,
	BACK_WIRE = 3,
	FRONT_SOLID = 4,
	FRONT_WIRE = 5
};
enum DEPTH_STENCIL_TYPE {

};

struct Renderable {
	bool CompiledData = false;
	bool CompiledCode = false;
	bool ClearDepthEveryPass = true;

	inline static Renderable* DXM;

	inline static ResourceObjectBase* ROB;

	inline static CameraManager* CAM;

	bool NewImGUIDat = false;


	virtual void ImGUINewFrameLogic() = 0;

	virtual void DrawLogic(bool sync = false) = 0;

	virtual void ImGUIInit() = 0;

	virtual void MakeNewWindowSwapChainAndAssociate(GLFWwindow* windowW, HWND sHwnd, UINT sWidth, UINT sHeight) = 0;

	virtual void RendererStartUpLogic() = 0;

	virtual void CleanRendererState() = 0;//TODO on all renderers
};