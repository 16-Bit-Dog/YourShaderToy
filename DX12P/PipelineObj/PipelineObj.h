#pragma once
#include "string"
#include "VertexShaderPipeline.h"
#include "PixelShaderPipeline.h"

//TODO: fix interact with name and create from inside pipeline state new pipeline
//TODO: make toggle work

struct PipelineObj {
	//Pipeline object that stores all pipeline data for a specific pipeline object

	
	bool On = true;//is pipeline used?
	bool ComputeOnlyToggle = false; //compute only pipeline?

	VertexShaderPipeline Vertex; //vertex shader handler

	PixelShaderPipeline Pixel; //pixel shader handler

	std::string name; //name of pipeline
	int Order = -1; //order of pipeline
	bool killP = false; //to kill pipeline toggle var -> awaiting death or not
	std::string padN = " "; //to allow imGUI padding for name
	PipelineObj(int i = -1) {
		name = std::to_string(i) + " Pipeline";
		Order = i;
	}
};
