#pragma once
#include "string"
#include "PipelineBase.h"
#include "Renderable.h"

struct BuiltModel_c;

struct VertexShaderPipeline : BasePipeline {
	//Abstract pipeline to repisent Vertex pipeline object and settings

	//TODO: blend state associated with vertex shader
	int ModelType = 0;

	BuiltModel_c* modelData;

	std::vector<void*> MatInfo; //vertex data generic pointer
	std::vector<void*> Vdata; //vertex data generic pointer
	std::string LoadedModelName = "";

	std::vector<void*> Idata; //indice data generic pointer
	std::vector<int> Icount; // indice count to draw
	UINT VertexStride = 0; // stride until next vertex --> set by model loader dynamically

	void* RawModel = nullptr;
	//
	//for choosing raster by map
	RasterTypeMapMadeData RasterToMake;
	//
	//stencil buffer settings for choosing stencil by map
	StencilTypeMapMadeData StencilToMake; 

	//Blend
	BlendTypeMapMadeData BlendToMake;
	std::array<float, 4> BlendFactor = { 1.0f,1.0f,1.0f,1.0f };
	
	VertexShaderPipeline() {
		T_P = VERTEX;
		ShaderTypeName = "Vertex";
		this->name = name + "SimpleVS";
	}
	
};
