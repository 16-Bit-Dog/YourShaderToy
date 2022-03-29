#pragma once
#include "string"
#include "PipelineObj.h"
//TODO: fix interact with name and create from inside pipeline state new pipeline
//TODO: make toggle work


struct PipelineMain {
	//pipeline obj holder class

	inline static PipelineMain* obj;
	
	std::map<int, PipelineObj*> P;//P for Pipeline objects, linked to map

	void RemovePipeline(int i) {
		//removes pipeline if exists
		if (P.find(i) != P.end()) P.erase(i);
	} 

	void CheckToKillPipe() {
		//check if toggle kill var wants pipeline to be removed, loop through all and then kill since else ref breaks
		std::vector<int> t;
		for (const auto& i : P) {
			if (i.second->killP) t.push_back(i.first);
		}
		for (auto i : t) RemovePipeline(i);
	}

	bool Swap(int newP, int oldP, int RecursivePush = 0, int originalStop = 0 /* paired with recursive push to signify an end if swaping negative*/) {
		if (newP != oldP) {
			PipelineObj* tmp = nullptr;

			if (P.count(newP) != 0) {
				tmp = P[newP];
			}

			P[oldP]->Order = newP;

			P[newP] = P[oldP];

			if (tmp != nullptr) {
				tmp->Order = oldP;
				P[oldP] = tmp;

				if (RecursivePush != 0 && originalStop != oldP) Swap(newP + RecursivePush, oldP + RecursivePush, RecursivePush, originalStop); //push until none to swap
				
				return true;
			}
			else {
				P.erase(oldP); 
				
				if (RecursivePush != 0 && originalStop != oldP) Swap(newP + RecursivePush, oldP + RecursivePush, RecursivePush, originalStop); //push until none to swap

				return false;
			}
		}
	}

	void AddNewPipelineToPosition(int StartPos) { // add new pipeline to earliest allowed position
		if (P.count(StartPos) != 0) {
			int end = StartPos+1;
			for (int i = StartPos+1; i < P.size()+ StartPos+1; i++) {
				if (P.count(i) == 0) {
					end = i; i = P.size() + StartPos; //nearest open spot from position i forward
				}
			}

			Swap(end, end-1, -1, StartPos);

		}

		P[StartPos] = new PipelineObj(StartPos);
	}

	void AddNewPipelineToEnd() { //add pipeline to end of list of pipelines
		int size = P.size() + 1;
		for (int i = 1; i < size + 1; i++) {
			if (P.count(i) == 0) {
				P[i] = new PipelineObj(i);
				i = size + 1;
			}
		}
	}
	

};