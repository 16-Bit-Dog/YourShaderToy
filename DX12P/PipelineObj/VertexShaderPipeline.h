#pragma once
#include "string"
#include "PipelineBase.h"
#include "Renderable.h"

struct VertexShaderPipeline : BasePipeline {
	//Abstract pipeline to repisent Vertex pipeline object and settings

	//TODO: blend state associated with vertex shader

	std::vector<void**> Vdata; //vertex data generic pointer
	std::vector<void**> Idata; //indice data generic pointer
	std::vector<int> Icount; // indice count to draw
	UINT VertexStride = 0; // stride until next vertex --> set by model loader dynamically
	//
	//for choosing raster by map
	UINT FaceToRender = 4; // which raster state should be used is determined by this var
	bool Wireframe = false; // is wire frame used?
	//
	//stencil buffer settings for choosing stencil by map
	StencilTypeMapMadeData StencilToMake; 

	VertexShaderPipeline() {
		ShaderTypeName = "Vertex";
		this->name = name + "SimpleVS";
	}
	
};
