#pragma once
#include "string"
#include "PipelineBase.h"
#include "Renderable.h"

struct VertexShaderPipeline : BasePipeline {

	//TODO: blend state associated with vertex shader

	std::vector<void**> Vdata;
	std::vector<void**> Idata;
	std::vector<int> Icount;
	UINT VertexStride = 0;
	UINT FaceToRender = 4;
	bool Wireframe = false;
	StencilTypeMapMadeData StencilToMake;

	VertexShaderPipeline() {
		ShaderTypeName = "Vertex";
		this->name = name + "SimpleVS";
	}
	
};
