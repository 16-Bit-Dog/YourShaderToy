#pragma once
#include "3DDX11Obj.h"
#include "3DCommons/StaticObjects.h"
#include <tuple>

struct StaticDX11Object{
	inline static StaticDX11Object* obj;
	
	std::vector<bool> bools;
	std::vector<DX11M3DR*> items;

	void MakeOBJ(int OBJ_I) {
		if (bools[OBJ_I] == false) {
			if (std::get<2>(StaticObjectPass[OBJ_I]) == 'd') {
				items[OBJ_I] = new DX11M3DR(std::get<1>(StaticObjectPass[OBJ_I])(), std::get<3>(StaticObjectPass[OBJ_I]));
			}
			else if (std::get<2>(StaticObjectPass[OBJ_I]) == 's') {
				items[OBJ_I] = new DX11M3DR( *((std::string*)(std::get<1>(StaticObjectPass[OBJ_I])())), std::get<3>(StaticObjectPass[OBJ_I]));
			}

			bools[OBJ_I] = true;
		}
	}

	StaticDX11Object() {
		bools.resize(StaticObjectPass.size());
		items.resize(StaticObjectPass.size());
	
	}



};