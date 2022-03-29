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

struct StencilTypeMapMadeData { //sets new stencil based on user toggles if not made yet
	bool EnableDepth = 1;
	bool DepthWriteMask = 1;
	int DepthComp = 2;

	bool EnableStencil = 0;
	
	int BackTriComp = 2;
	int FrontTriComp = 2;

	int WhereToReadFromStencil = 255;
	int WhereToWriteToStencil = 255;

	int FrontFaceStencilFailOp = 1;
	int FrontFaceStencilDepthFailOp = 1;
	int FrontFaceStencilPassOp = 1;

	int BackFaceStencilFailOp = 1;
	int BackFaceStencilDepthFailOp = 1;
	int BackFaceStencilPassOp = 1;

	bool operator==(const StencilTypeMapMadeData& p) const {
		return (
			EnableDepth == p.EnableDepth &&
			DepthWriteMask == p.DepthWriteMask &&
			DepthComp == p.DepthComp &&

			EnableStencil == p.EnableStencil &&

			BackTriComp == p.BackTriComp &&
			FrontTriComp == p.FrontTriComp &&

			WhereToReadFromStencil == p.WhereToReadFromStencil &&
			WhereToWriteToStencil == p.WhereToWriteToStencil &&

			FrontFaceStencilFailOp == p.FrontFaceStencilFailOp &&
			FrontFaceStencilDepthFailOp == p.FrontFaceStencilDepthFailOp &&
			FrontFaceStencilPassOp == p.FrontFaceStencilPassOp &&

			BackFaceStencilFailOp == p.BackFaceStencilFailOp &&
			BackFaceStencilDepthFailOp == p.BackFaceStencilDepthFailOp &&
			BackFaceStencilPassOp == p.BackFaceStencilPassOp
			);
	}
};
struct StencilTypeMapMadeData_hash
{//hash for StencilTypeMapMadeData_hash
	std::size_t operator() (const StencilTypeMapMadeData& t) const
	{
		std::size_t h[14];
		h[0] = std::hash<bool>()(t.EnableDepth);
		h[1] = std::hash<bool>()(t.DepthWriteMask);
		h[2] = std::hash<int>()(t.DepthComp);

		h[3] = std::hash<bool>()(t.EnableStencil);
		
		h[4] = std::hash<int>()(t.BackTriComp);
		h[5] = std::hash<int>()(t.FrontTriComp);
		
		h[6] = std::hash<int>()(t.WhereToReadFromStencil);
		h[7] = std::hash<int>()(t.WhereToWriteToStencil);

		h[8] = std::hash<int>()(t.FrontFaceStencilFailOp);
		h[9] = std::hash<int>()(t.FrontFaceStencilDepthFailOp);
		h[10] = std::hash<int>()(t.FrontFaceStencilPassOp);


		h[11] = std::hash<int>()(t.BackFaceStencilFailOp);
		h[12] = std::hash<int>()(t.BackFaceStencilDepthFailOp);
		h[13] = std::hash<int>()(t.BackFaceStencilPassOp);
		for (int i = 1; i < 14; i++) {
			h[0] ^= h[i]>>i*4;//makes unique hash pushed bit offset with intervals of 4 since it keeps all values ordered except the 255 mask which is normally 0 or 255, which works nicely still
		}
		return h[0];
	}
};


struct Renderable {
	bool AutoFileManagerCompile = true;

	UINT REF_FOR_DEPTH_STENCIL = 1;

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