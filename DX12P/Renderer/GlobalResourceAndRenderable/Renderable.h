#pragma once

#include <../GLFW/glfw3.h>
#include <../GLFW/glfw3native.h>
#include "ResourceObjectBase.h"
#include "CamManager.h"
#include <functional>
#include <array>




struct ResourceObjectBase;

struct RtvAndDepthBlock {
	inline static std::function<void()> ClearRTVAndDepth;
	inline static std::function<void()> ReleaseAllRTVAndDepth;
	inline static std::function<void()> MakeRTVAndDepth;
	inline static std::function<int()> SizeOfRTV;
};

//TODO: set Blend to many render targets all same later
struct BlendTypeMapMadeData {


	bool AlphaToCoverageEnable = false;
	bool IndependentBlendEnable = false;
	bool BlendEnable = false;
	int BlendOp = 1;
	int BlendOpAlpha = 1;
	int DestBlend = 1;
	int DestBlendAlpha = 1;
	int SrcBlend = 1;
	int SrcBlendAlpha = 1;
	int RenderTargetWriteMask = 255;

	bool operator==(const BlendTypeMapMadeData& p) const {

		return (
			AlphaToCoverageEnable == p.AlphaToCoverageEnable &&
			IndependentBlendEnable == p.IndependentBlendEnable &&
			BlendEnable == p.BlendEnable &&
			BlendOp == p.BlendOp &&
			BlendOpAlpha == p.BlendOpAlpha &&
			DestBlend == p.DestBlend &&
			DestBlendAlpha == p.DestBlendAlpha &&
			SrcBlend == p.SrcBlend &&
			SrcBlendAlpha == p.SrcBlendAlpha &&
			RenderTargetWriteMask == p.RenderTargetWriteMask
			);

	}
};

struct BlendTypeMapMadeData_hash
{//hash for StencilTypeMapMadeData_hash
	std::size_t operator() (const BlendTypeMapMadeData& t) const {
		std::size_t h[10];
		h[0] = std::hash<bool>()(t.AlphaToCoverageEnable);
		h[1] = std::hash<bool>()(t.IndependentBlendEnable);
		h[2] = std::hash<bool>()(t.BlendEnable);
		h[3] = std::hash<int>()(t.BlendOp);
		h[4] = std::hash<int>()(t.BlendOpAlpha);
		h[5] = std::hash<int>()(t.SrcBlend);
		h[6] = std::hash<int>()(t.SrcBlendAlpha);
		h[7] = std::hash<int>()(t.RenderTargetWriteMask);
		h[8] = std::hash<int>()(t.DestBlendAlpha);
		h[9] = std::hash<int>()(t.DestBlend);
		
		for (int i = 1; i < 10; i++) {
			h[0] ^= h[i] >> i * 4;//makes unique hash pushed bit offset with intervals of 4 since it keeps all values ordered except the 255 mask which is normally 0 or 255, which works nicely still
		}
		return h[0];
	}
};

struct RasterTypeMapMadeData {
	int cull = 1;
	bool ToFill = 1;
	bool AAL = 0;
	bool operator==(const RasterTypeMapMadeData& p) const {
		return (
			cull == p.cull &&
			ToFill == p.ToFill &&
			AAL == p.AAL 
			);
	}
};

struct RasterTypeMapMadeData_hash
{//hash for StencilTypeMapMadeData_hash
	std::size_t operator() (const RasterTypeMapMadeData& t) const{
		std::size_t h[3];
		h[0] = std::hash<bool>()(t.ToFill);
		h[1] = std::hash<bool>()(t.AAL);
		h[2] = std::hash<int>()(t.cull);
		for (int i = 1; i < 14; i++) {
			h[0] ^= h[i] >> i * 4;//makes unique hash pushed bit offset with intervals of 4 since it keeps all values ordered except the 255 mask which is normally 0 or 255, which works nicely still
		}
		return h[0];
	}
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
	std::size_t operator() (const StencilTypeMapMadeData& t) const{
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

struct RATIONAL{
	uint32_t Numerator = 0;
	uint32_t Denominator = 0;
};

struct Renderable {

	virtual Renderable* GetR() = 0;

	bool BuiltPredefined = false; //TODO: built objects is seperate option than built code

	inline static std::array<float, 4> CLEAR_COLOR = std::array<float, 4> {0.1f, 0.5f, 0.1f, 1.0f};

	inline static int BLOCK_SIZE = 8;

	inline static int TargetFrameRate = 69; //TODO - use THIS

	inline static uint32_t BLOCK_X = 16;
	inline static uint32_t BLOCK_Y = 16;
	inline static uint32_t BLOCK_Z = 1;

	inline static bool UseWarpDev = false;
	
	///////////////////////
	
	bool extendedFeatures = false; //like rtx and stuff

	HWND hwnd;
	GLFWwindow* window;

	bool bFullScreen = false;

	RATIONAL refreshRateStatic {};

	bool ClearRTV = true;
	
	bool AutoFileManagerCompile = true;

	UINT REF_FOR_DEPTH_STENCIL = 1;

	bool CompiledData = false; //is file manager up to date?
	bool CompiledCode = false; //kinda redundant rn..
	bool ClearDepthEveryPass = true; //main depth buffer clear var
	bool BufferReset = true; //recompile the Compile Button of "PipelineWindow"

	bool AutoCodeCompile = true;
	float AutoCodeCompile_Wait = 1.0f;

	inline static Renderable* DXM;

	inline static ResourceObjectBase* ROB;

	inline static CameraManager* CAM;

	bool NewImGUIDat = false;

	int MainWidth;
	int MainHeight;

	int* MainWidthR;
	int* MainHeightR;

	virtual void ImGUINewFrameLogic() = 0;

	virtual void DrawLogic(bool sync = false) = 0;

	virtual void ImGUIInit() = 0;

	virtual void MakeNewWindowSwapChainAndAssociate(GLFWwindow* windowW, HWND sHwnd, int& sWidth, int& sHeight) = 0;

	virtual void RendererStartUpLogic() = 0;

	virtual void CleanRendererState() = 0;//TODO on all renderers

};

