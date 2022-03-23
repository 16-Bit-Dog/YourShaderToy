#pragma once
#include "3DDX11Obj.h"
#include "3DCommons/StaticObjects.h"
struct StaticDX11Object{
	inline static StaticDX11Object* obj;

	bool MadeCube = false;
	DX11M3DR* CUBE;

	void MakeCube() {
		if (MadeCube == false) {
			CUBE = new DX11M3DR(StaticObjectPass[0].second());
			MadeCube = true;
		}
	}

};