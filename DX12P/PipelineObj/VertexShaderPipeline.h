#pragma once
#include "string"
#include "PipelineBase.h"


struct VertexShaderPipeline : BasePipeline {

	//TODO: blend state associated with vertex shader

	std::vector<void**> Vdata;
	std::vector<void**> Idata;
	std::vector<int> Icount;
	UINT VertexStride = 0;

	VertexShaderPipeline() {
		ShaderTypeName = "Vertex";
		this->name = name + "PUT_FUNCTION_NAME_HERE";
	}
	
};
