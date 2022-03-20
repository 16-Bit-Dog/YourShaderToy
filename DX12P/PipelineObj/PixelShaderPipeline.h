#pragma once
#include "string"
#include "PipelineBase.h"

struct PixelShaderPipeline : BasePipeline {

	PixelShaderPipeline() {
		ShaderTypeName = "Pixel";
		this->name = name + "PUT_FUNCTION_NAME_HERE";
	}

};
