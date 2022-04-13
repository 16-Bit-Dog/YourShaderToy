#pragma once
#include "string"
#include "VertexShaderPipeline.h"
#include "PixelShaderPipeline.h"
#include "ComputeShaderPipeline.h"
#include "map"
#include "FileManagerResourceStruct.h"

//TODO: fix interact with name and create from inside pipeline state new pipeline
//TODO: make toggle work

namespace MapTools {
	template<typename T>
	void CheckIfSelectedExists(std::unordered_map<uint64_t, T*>& RTV, uint64_t* item) {
		if (RTV.find(*item) == RTV.end()) {
			for (const auto& i : RTV) {
				(*item) = i.first;
				return;
			}
		}
	}

	template<class T>
	static bool Swap(int newP, int oldP, std::map<int, T*>& P, const int& RecursivePush = 0, const int& originalStop = 0 /* paired with recursive push to signify an end if swaping negative*/) {
		if (newP != oldP) {
			T* tmp = nullptr;

			if (P.count(newP) != 0) {
				tmp = P[newP];
			}

			P[oldP]->Order = newP;

			P[newP] = P[oldP];

			if (tmp != nullptr) {
				tmp->Order = oldP;
				P[oldP] = tmp;

				if (RecursivePush != 0 && originalStop != oldP) Swap(newP + RecursivePush, oldP + RecursivePush, P, RecursivePush, originalStop); //push until none to swap

				return true;
			}
			else {
				P.erase(oldP);

				if (RecursivePush != 0 && originalStop != oldP) Swap(newP + RecursivePush, oldP + RecursivePush, P, RecursivePush, originalStop); //push until none to swap

				return false;
			}
		}
	}

	template<class T>
	void Remove(const int& i, std::map<int, T*>& P) {
		//removes pipeline if exists
		if (P.find(i) != P.end()) P.erase(i);
	}

	template<class T>
	void AddNew(const int& StartPos, std::map<int, T*>& P) { // add new pipeline to earliest allowed position
		if (P.count(StartPos) != 0) {
			int end = StartPos + 1;
			for (int i = StartPos + 1; i < P.size() + StartPos + 1; i++) {
				if (P.count(i) == 0) {
					end = i; i = P.size() + StartPos; //nearest open spot from position i forward
				}
			}

			Swap(end, end - 1, P, -1, StartPos);

		}

		P[StartPos] = new T(StartPos);
	}

	template<class T>
	void AddNewToEnd(std::map<int, T*>& P) { //add pipeline to end of list of pipelines
		int size = P.size() + 1;
		for (int i = 1; i < size + 1; i++) {
			if (P.count(i) == 0) {
				P[i] = new T(i);
				i = size + 1;
			}
		}
	}

}

struct PipelineObj {
	//Pipeline object that stores all pipeline data for a specific pipeline object
	
	bool On = true;//is pipeline used?
	bool ComputeOnlyToggle = false; //compute only pipeline?
	bool TurnComputeOffToggle = false;

	VertexShaderPipeline Vertex; //vertex shader handler

	PixelShaderPipeline Pixel; //pixel shader handler

	std::map<int, ComputeShaderPipeline*> Compute;
	
	std::string name; //name of pipeline
	int Order = -1; //order of pipeline
	bool killP = false; //to kill pipeline toggle var -> awaiting death or not
	
	uint64_t RTV_Selected = 0;
	uint64_t DEPTH_Selected = 0;
	inline static uint64_t SelectedFinalRTV;

	//TODO: maybe just template these last 3 up

	std::string Spacing() {
		return std::move(std::string("PO", Order));
	}

	std::string GetName() {
		return std::move(name + Spacing());
	}

	PipelineObj(int i = -1) {
		name = std::to_string(i) + " Pipeline";
		Order = i;
	}
	~PipelineObj() {
		for (const auto& i : Compute) {
			delete i.second;
		}
	}

	void DrawCompute() {
		if(ImGui::CollapsingHeader(("Compute [runs after]:##" + Spacing()).c_str(), NULL)) {

			if (ImGui::Button(("+##Compute Add Start" + Spacing()).c_str())) {
				MapTools::AddNew(0, Compute);
			}

			for (const auto& i : Compute) {
				ImGui::Text(i.second->NameOfCompute.c_str());
				ImGui::SameLine();
				if(ImGui::CollapsingHeader((i.second->Spacing() + Spacing()).c_str())) {
					i.second->DrawNameSwitch();
					i.second->Input();

					i.second->DrawBlockInputs();
				}
			}

			if (ImGui::Button(("+##Compute Add End" + std::to_string(Order)).c_str())) {
				MapTools::AddNewToEnd(Compute);
			}

		}
	}
};
