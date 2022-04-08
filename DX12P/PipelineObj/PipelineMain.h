#pragma once
#include "string"
#include "PipelineObj.h"
//TODO: fix interact with name and create from inside pipeline state new pipeline
//TODO: make toggle work


struct PipelineMain {
	//pipeline obj holder class

	inline static PipelineMain* obj;
	
	std::map<int, PipelineObj*> P;//P for Pipeline objects, linked to map

	void RemovePipeline(const int& i) {
		MapTools::Remove(i, P);
	} 

	void CheckToKillPipe() {
		//check if toggle kill var wants pipeline to be removed, loop through all and then kill since else ref breaks
		std::vector<int> t;
		for (const auto& i : P) {
			if (i.second->killP) t.push_back(i.first);
		}
		for (auto i : t) RemovePipeline(i);
	}

	void AddNewPipelineToPosition(int StartPos) { // add new pipeline to earliest allowed position
		MapTools::AddNew(StartPos, P);
	}

	void AddNewPipelineToEnd() { //add pipeline to end of list of pipelines
		MapTools::AddNewToEnd(P);
	}
	

};