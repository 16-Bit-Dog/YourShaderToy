#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "ofbxMini.h"
#include <algorithm>
#include "DirectXMath.h"
#include <iostream>
#include <unordered_map>
#include <array>
using namespace DirectX;



//vertex per bone data
struct VertexBoneData {
	std::vector<float> IDs; //float for predictable shader pass
	std::vector<float> weights;

	VertexBoneData() {
		IDs.reserve(5);
		weights.reserve(5);
	}
};

//bone ID 
struct TopBoneIDs {
	int32_t id[4] = { 0,0,0,0 };

	void ZeroID() {
		for (int i = 0; i < sizeof(id) / sizeof(int32_t); i++) {
			id[i] = 0;
		}
	}
};

//Bone weight
struct TopBoneWeight {
	float w[4] = { 0,0,0,0 };

	void ZeroW() {
		for (int i = 0; i < sizeof(w) / sizeof(float); i++) {
			w[i] = 0;
		}
	}
};

//Vertex
struct VNT
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT3 BiNormal;
	XMFLOAT3 Tangent;
	XMFLOAT2 Tex;
	TopBoneIDs tbi; //uint 4
	TopBoneWeight tbw; //float 4
};

//bone joint manager struct
struct Joint {
	int32_t id; // index/ID of bone
	std::string name;
	const ofbx::Object* Bone;

	//transformation, rotation, scale bone node
	std::vector <const ofbx::AnimationCurveNode*> tNode;
	std::vector <const ofbx::AnimationCurveNode*> rNode;
	std::vector <const ofbx::AnimationCurveNode*> sNode;

	XMMATRIX LBT;
	XMMATRIX InvBP = XMMatrixIdentity();
	XMMATRIX BP = XMMatrixIdentity();
	XMMATRIX SkinM = XMMatrixIdentity();

	XMMATRIX BoneOffsetMatrix;

	std::vector<std::shared_ptr<Joint>> children;
	std::shared_ptr<Joint> parent;

	
	void GetInvBindT(XMMATRIX parentBindT) {
		//get inverse bind transform matrix from recursive calc
		XMMATRIX BT = parentBindT * LBT;

		BP = BT;
		InvBP = XMMatrixInverse(nullptr, BT);
		SkinM = (BP * InvBP);

		for (int i = 0; i < children.size(); i++) {
			children[i]->GetInvBindT(LBT);
		}
	}
};


//ofbx to XM matrix stuff
XMFLOAT4X4 ofbxMatToXM(ofbx::Matrix* TMPtm) {
	return XMFLOAT4X4(static_cast<float>(TMPtm->m[0]), static_cast<float>(TMPtm->m[1]), static_cast<float>(TMPtm->m[2]), static_cast<float>(TMPtm->m[3]), static_cast<float>(TMPtm->m[4]), static_cast<float>(TMPtm->m[5]), static_cast<float>(TMPtm->m[6]), static_cast<float>(TMPtm->m[7]), static_cast<float>(TMPtm->m[8]), static_cast<float>(TMPtm->m[9]), static_cast<float>(TMPtm->m[10]), static_cast<float>(TMPtm->m[11]), static_cast<float>(TMPtm->m[12]), static_cast<float>(TMPtm->m[13]), static_cast<float>(TMPtm->m[14]), static_cast<float>(TMPtm->m[15]));
}
ofbx::Matrix XMToofbxMat(XMFLOAT4X4* TMPtm) {
	return { TMPtm->m[0][0],TMPtm->m[0][1],TMPtm->m[0][2],TMPtm->m[0][3],TMPtm->m[1][0],TMPtm->m[1][1],TMPtm->m[1][2],TMPtm->m[1][3] ,TMPtm->m[2][0],TMPtm->m[2][1],TMPtm->m[2][2],TMPtm->m[2][3] ,TMPtm->m[3][0],TMPtm->m[3][1],TMPtm->m[3][2],TMPtm->m[3][3] };

}
//


//object modification of treansform manager
struct ObjTuneStatReg { //TODO: make scale and translate work in shader - make obj visible
	std::array<float, 3> Translate = { 0.0f,0.0f,0.0f }; //not using xm float, so sad... :(
	float pad1 = 0.0f;
	std::array<float, 3> Scale = { 1.0f,1.0f,1.0f };
	float pad2 = 0.0f;
	XMFLOAT4 Quat = { 0.0f,0.0f,0.0f,0.0f };
};


struct M3DR { //model Resource data only 
	/*
	abstract general 
	*/
	
	bool UpdateVDat = false;
	bool UpdateAnimDat = false;

	ObjTuneStatReg ObjTune; //object treansform manager
	std::vector < std::vector<VNT> > modelDat; //model vertex data vector
	std::vector<Joint> Bones; //bones vector
	const ofbx::Object* rootObj; //root obj link
	std::vector<XMFLOAT4X4> BoneDataTLMA; //Transform link matrix
	std::unordered_map<std::string, int> animNameS; //contains anim position in stack
	std::unordered_map<int, std::string> animNameI; //contains anim position in stack reverse
	std::vector< const ofbx::AnimationStack* > animStack;
	std::vector< float > animStackMaxTime; //anim max time vector
	std::vector< bool > AnimVCacheMade; //bone pose pre calc is made or not?*
	std::vector< std::vector< std::vector<XMFLOAT4X4> > > AnimVCacheData; //per anim, per interval - I have a set of matrix for VCache pre calc in order
	std::vector<XMMATRIX> animDat; //animation data collection
	std::vector<const ofbx::Cluster*> ClusterObject; 
	std::vector < std::vector<VertexBoneData> > VboneDat; //Vertex bone data
	std::vector < std::vector<UINT> > Indice; //indices vector
	XMMATRIX globalInverseTransform; 
	XMMATRIX globalTransform;



	const ofbx::AnimationCurveNode* GetNodeCurve(int Anim, std::string valueType, const ofbx::Object* Bone) {


		for (int ii = 0; animStack[Anim]->getLayer(ii); ++ii)
		{

			const ofbx::AnimationLayer* layer = animStack[Anim]->getLayer(ii);

			if (layer->getCurveNode(*Bone, valueType.c_str())) {
				return layer->getCurveNode(*Bone, valueType.c_str());

			}
			else {
				return nullptr;
			}
		}
	}

	void PostProcessNormals() {
		for (int i = 0; i < modelDat.size(); i++) {
			for (int x = 0; x < modelDat[i].size(); x += 3) {
				XMVECTOR A = XMLoadFloat3(&modelDat[i][x].Position);
				XMVECTOR B = XMLoadFloat3(&modelDat[i][x + 1].Position);
				XMVECTOR C = XMLoadFloat3(&modelDat[i][x + 2].Position);

				A = XMVector3Cross((B - A), (C - A));

				XMStoreFloat3(&modelDat[i][x].Normal, A);

				XMStoreFloat3(&modelDat[i][x + 1].Normal, A);

				XMStoreFloat3(&modelDat[i][x + 2].Normal, A);

			}
		}
	}

	void AutoFillIndice(int Obj) {
		if (Obj >= Indice.size()) { Indice.resize(Obj + 1); }
		Indice[Obj].resize(modelDat[Obj].size());
		for (int i = 0; i < modelDat[Obj].size(); i++) {
			Indice[Obj][i] = i;
		}
	}

	void PostProcessTangents() {
		for (int i = 0; i < modelDat.size(); i++) {

			XMVECTOR* tan1 = new XMVECTOR[modelDat[i].size() * 2];
			ZeroMemory(tan1, modelDat[i].size() * sizeof(XMFLOAT3) * 2);

			for (int x = 0; x < modelDat[i].size(); x += 3) {


				XMFLOAT3* A = (&modelDat[i][x].Position);
				XMFLOAT3* B = (&modelDat[i][x + 1].Position);
				XMFLOAT3* C = (&modelDat[i][x + 2].Position);

				XMFLOAT2* UA = (&modelDat[i][x].Tex);
				XMFLOAT2* UB = (&modelDat[i][x + 1].Tex);
				XMFLOAT2* UC = (&modelDat[i][x + 2].Tex);

				float x1 = B->x - A->x;
				float x2 = C->x - A->x;
				float y1 = B->y - A->y;
				float y2 = C->y - A->y;
				float z1 = B->z - A->z;
				float z2 = C->z - A->z;

				float s1 = UB->x - UA->x;
				float s2 = UC->x - UA->x;
				float t1 = UB->y - UA->y;
				float t2 = UC->y - UA->y;

				float r = 1.0f / (s1 * t2 - s2 * t1);

				XMFLOAT3 sdir = { (t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
					(t2 * z1 - t1 * z2) * r };

				tan1[x] += XMLoadFloat3(&sdir);
				tan1[x + 1] += XMLoadFloat3(&sdir);
				tan1[x + 2] += XMLoadFloat3(&sdir);

			}

			for (long a = 0; a < modelDat[i].size(); a++)
			{
				XMVECTOR n = XMLoadFloat3(&modelDat[i][a].Normal);
				XMVECTOR t = tan1[a];

				XMStoreFloat3(&modelDat[i][a].Tangent, XMVector3Normalize((t - n * XMVector3Dot(n, t))));
			}
			delete[] tan1;
		}

	}

	void PostProcessBinormals() {
		for (int i = 0; i < modelDat.size(); i++) {
			for (int x = 0; x < modelDat[i].size(); x++) {

				XMStoreFloat3(&modelDat[i][x].BiNormal, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&modelDat[i][x].Normal), XMLoadFloat3(&modelDat[i][x].Tangent))));

			}
		}
	}
	void VertexPostProcess() {
		PostProcessNormals();
		PostProcessTangents();
		PostProcessBinormals();
	}
	void RESIZE_VECTORS_OBJ_LOAD(int i) {
		Indice.resize(i);
		modelDat.resize(i);
		VboneDat.resize(i);
	}
	void LoadFBXFile(std::string path) {
		FILE* fp;

		fopen_s(&fp, path.c_str(), "rb");

		if (!fp) { std::cout << "no file found at fbx path"; }
		else {
			fseek(fp, 0, SEEK_END);
			long file_size = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			ofbx::u8* content = new ofbx::u8[file_size];
			fread(content, 1, file_size, fp);

			ofbx::IScene* g_scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);

			Bones.clear();
			BoneDataTLMA.clear();
			animNameI.clear();
			animNameS.clear();
			animStackMaxTime.clear();
			AnimVCacheData.clear();
			ClusterObject.clear();
			modelDat.clear();
			Indice.clear();
			VboneDat.clear();
			Bones.clear();
			BoneDataTLMA.clear();
			animDat.clear();
			animStack.clear();
			
			VNT tmpV;

			XMFLOAT3 TmpPos;
			XMFLOAT3 TmpNor;
			XMFLOAT2 TmpTex;
			UINT TmpInd;

			int obj_idx = 0;
			int indices_offset = 0;
			int normals_offset = 0;
			int mesh_count = g_scene->getMeshCount();
			
			RESIZE_VECTORS_OBJ_LOAD(mesh_count);

			rootObj = g_scene->getRoot();
			ofbx::Matrix emp = rootObj->getGlobalTransform();

			XMFLOAT4X4 tmpgl = ofbxMatToXM(&emp);

			globalTransform = XMLoadFloat4x4(&tmpgl);

			globalInverseTransform = XMMatrixInverse(nullptr, globalTransform);

			for (int i = 0; i < mesh_count; i++)
			{

				const ofbx::Mesh& mesh = *g_scene->getMesh(i);
				const ofbx::Geometry& geom = *mesh.getGeometry();
				int vertex_count = geom.getVertexCount();
				const ofbx::Vec3* vertices = geom.getVertices();
				const ofbx::Vec2* uvs = geom.getUVs();
				const int indiceC = geom.getIndexCount();
				const ofbx::Vec3* normals = geom.getNormals();
				const ofbx::Pose* cPose = mesh.getPose();
						//POS NORMAL  TEX


				for (int ii = 0; ii < vertex_count; ii++)
				{

					tmpV.Position = { static_cast<float>(vertices[ii].x), static_cast<float>(vertices[ii].y), static_cast<float>(vertices[ii].z) };

					tmpV.Normal = { 0.0f,0.0f,0.0f };
					tmpV.BiNormal = { 0.0f,0.0f,0.0f };
					tmpV.Tangent = { 0.0f,0.0f,0.0f };

					if (uvs != nullptr) {
						tmpV.Tex = { static_cast<float>(uvs[ii].x),1.0f - static_cast<float>(uvs[ii].y) };
					}
					else {
						tmpV.Tex = { 0.0f,0.0f };
					}

					modelDat[i].push_back(tmpV);
					Indice[i].push_back(Indice[i].size());
				}

				VboneDat[i].resize(modelDat[i].size());

				const ofbx::Skin* skin = geom.getSkin();
				if (skin) {
					int gctR = skin->getClusterCount();
					for (int ii = 0; ii < gctR; ii++)
					{

						const ofbx::Cluster* cluster = skin->getCluster(ii);

						if (cluster) {
							int indiceCount = cluster->getIndicesCount();
							//int counter = 0;
							if (indiceCount > 0) {
								const int* indList = cluster->getIndices();
								const double* tmpW = cluster->getWeights();

								for (int iii = 0; iii < indiceCount; iii++) {
									VboneDat[i][indList[iii]].weights.push_back(static_cast<float>(tmpW[iii]));
									VboneDat[i][indList[iii]].IDs.push_back(ii); //bone id
								}
							}
							ofbx::Matrix TMPtm = cluster->getTransformMatrix();


							ofbx::Matrix TMPtlm = cluster->getTransformLinkMatrix(); //correct matrix

							ofbx::Matrix GeoTrans = mesh.getGeometricMatrix();

							ofbx::Matrix ModelMatrix = cluster->getLink()->getGlobalTransform();

							ofbx::Matrix LocalMatrix = cluster->getLink()->getLocalTransform();

							XMFLOAT4X4 XMIB = ofbxMatToXM(&GeoTrans);
							XMMATRIX tmpMMM = XMLoadFloat4x4(&XMIB);
							//		ofbx::Matrix linkGeo = cluster->getLink()->getGlobalTransform();




							ClusterObject.push_back(cluster);
							//TMPtlm = GeoTrans* TMPtlm;
							//TMPtm = TMPtm*linkGeo;

							XMFLOAT4X4 tmpTMtmp = ofbxMatToXM(&LocalMatrix);
							XMMATRIX tmpTM = XMLoadFloat4x4(&tmpTMtmp);

							XMFLOAT4X4 tmpGtmp = ofbxMatToXM(&ModelMatrix);
							XMMATRIX tmpG = XMLoadFloat4x4(&tmpGtmp);


							XMFLOAT4X4 tmpTLMF = ofbxMatToXM(&TMPtlm);
							XMMATRIX tmpTLMM = XMLoadFloat4x4(&tmpTLMF);
							tmpTLMM = XMMatrixInverse(nullptr, tmpTLMM);

							XMFLOAT4X4 tmptmpTLMMma = ofbxMatToXM(&TMPtm);
							XMMATRIX tmpTLMMma = XMLoadFloat4x4(&tmptmpTLMMma);

							//

							Joint nJ;
							nJ.Bone = cluster->getLink();
							nJ.name = nJ.Bone->name;
							nJ.id = Bones.size();
							nJ.LBT = (tmpTM)*XMMatrixInverse(nullptr, tmpG);
							nJ.BoneOffsetMatrix = tmpTLMMma;
							Bones.push_back(nJ);
						}
					}
				}
			}

			BoneDataTLMA.resize(Bones.size());
			animDat.resize(Bones.size());

			int animCount = g_scene->getAnimationStackCount();

			for (int xx = 0; xx < animCount; xx++)
			{
				const ofbx::AnimationStack* astack = g_scene->getAnimationStack(xx);

				animStack.push_back(astack);
				animNameI[xx] = astack->name;
				animNameS[astack->name] = xx;
			}

			//load data from g_scene now
		}

		fclose(fp);
	}
	float EndTimeOfAnim(int Anim) {
		//get end of anim time if valid int
		if (Anim >= animStackMaxTime.size()) { return 0; }
		return animStackMaxTime[Anim];
	}
	void SetAnimEndTime(int Anim) {
		//set actual end of animation time
		float max = 0; // max time getter func - iterate through keys

		for (int i = 0; i < Bones.size(); i++) {

			for (int x = 0; x < 3; x++) {
				ofbx::i64 tcnV = 0.0f;
				ofbx::i64 rcnV = 0.0f;
				ofbx::i64 scnV = 0.0f;

				if (Bones[i].tNode[Anim] != NULL) {
					if (Bones[i].tNode[Anim]->getCurve(x)) {
						const ofbx::AnimationCurve* tc = Bones[i].tNode[Anim]->getCurve(x);
						int tcn = tc->getKeyCount() - 1;
						tcnV = tc->getKeyTime()[tcn];
					}
				}

				if (Bones[i].rNode[Anim] != NULL) {
					if (Bones[i].rNode[Anim]->getCurve(x)) {
						const ofbx::AnimationCurve* rc = Bones[i].rNode[Anim]->getCurve(x);
						int rcn = rc->getKeyCount() - 1;
						rcnV = rc->getKeyTime()[rcn];
					}
				}

				if (Bones[i].sNode[Anim] != NULL) {
					if (Bones[i].sNode[Anim]->getCurve(x)) {
						const ofbx::AnimationCurve* sc = Bones[i].sNode[Anim]->getCurve(x);
						int scn = sc->getKeyCount() - 1;
						scnV = sc->getKeyTime()[scn];
					}
				}
				//std::max not working...
				ofbx::i64 maxTmp;
				if (tcnV > rcnV) {
					maxTmp = tcnV;
				}
				else {
					maxTmp = rcnV;
				}
				if (maxTmp < scnV) maxTmp = scnV;

				if (max < maxTmp) max = static_cast<float>(ofbx::fbxTimeToSeconds(maxTmp));
			}

		}
		animStackMaxTime[Anim] = max;
	}

	void GetAllAnimEndTime() {
		//get all end of animation times
		for (int i = 0; i < animStack.size(); i++) {
			SetAnimEndTime(i);
		}
	}

	void AddBoneChildren(Joint* BoneP) {
		//add child to bone
		for (int i = 0; i < Bones.size(); i++) {
			if (Bones[i].Bone->getParent() == BoneP->Bone) {
				BoneP->children.push_back(std::make_shared<Joint>(Bones[i]));
			}
		}
	}
	void SetAllBonesChildren() {

		for (int i = 0; i < Bones.size(); i++) {
			AddBoneChildren(&Bones[i]);
		}
	}
	void SetAllBonesParent() {
		for (int i = 0; i < Bones.size(); i++) {
			for (int ii = 0; ii < Bones.size(); ii++) {
				if (Bones[i].Bone->getParent() == Bones[ii].Bone) {
					Bones[i].parent = std::make_shared<Joint>(Bones[ii]);
				}
			}
		}
	}
	void SetAllBonesGetInvBindT() {
		if (Bones.size() > 0) {
			Bones[0].GetInvBindT(XMMatrixIdentity());
		}
	}
	void ResizeAllBonesNodeArr() {
		for (int i = 0; i < Bones.size(); i++) {
			Bones[i].tNode.resize(animStack.size());
			Bones[i].rNode.resize(animStack.size());
			Bones[i].sNode.resize(animStack.size());
		}
	}
	void GetLTRSNode(int Bone, int Anim) {
		Bones[Bone].tNode[Anim] = GetNodeCurve(Anim, "Lcl Translation", Bones[Bone].Bone);
		Bones[Bone].rNode[Anim] = GetNodeCurve(Anim, "Lcl Rotation", Bones[Bone].Bone);
		Bones[Bone].sNode[Anim] = GetNodeCurve(Anim, "Lcl Scaling", Bones[Bone].Bone);
	}
	void GetAllLTRSNodeForEveryAnim(int Bone) {
		for (int i = 0; i < animStack.size(); i++) {
			GetLTRSNode(Bone, i);
		}
	}
	void GetAllBoneNodes() {
		for (int i = 0; i < Bones.size(); i++) {
			GetAllLTRSNodeForEveryAnim(i);
		}
	}
	void ResizeAndPrepAnimBoneStuff() {
		animStackMaxTime.resize(animStack.size());
		AnimVCacheMade.resize(animStack.size());
		AnimVCacheData.resize(animStack.size());
	}
	void BindPoseBones() {

		for (int i = 0; i < Bones.size(); i++) {
			XMStoreFloat4x4(&BoneDataTLMA[i], (Bones[i].SkinM));
		}
	}
	void FillTopBones() { 

		for (int B = 0; B < Indice.size(); B++) { //for indice size, start filling bones 

			std::unordered_map<int, int> sDubC; //--> map stops duplocate calc

			for (int i = 0; i < Indice[B].size(); i++) {
				if (sDubC.find(Indice[B][i]) == sDubC.end()) {
					sDubC[Indice[B][i]] = i;


					//fill top 4 if no contest in indice weights (only 4 or less weights
					if (VboneDat[B][Indice[B][i]].weights.size() < 4) {
						VboneDat[B][Indice[B][i]].weights.resize(4); VboneDat[B][Indice[B][i]].IDs.resize(4);
						for (int xx = 0; xx < VboneDat[B][Indice[B][i]].weights.size(); xx++) {
							if (VboneDat[B][Indice[B][i]].weights[xx] == 0) {
								VboneDat[B][Indice[B][i]].weights[xx] = 0.0f;
								VboneDat[B][Indice[B][i]].IDs[xx] = 0;
							}
						}
					}

					float tmpWH = 0.0f;
					int tmpIDH = 0;

					//add 4 strongestweights if not only 4 on vertex
					for (int ii = 0; ii < VboneDat[B][Indice[B][i]].weights.size(); ii++) {
						for (int iii = VboneDat[B][Indice[B][i]].weights.size() - 1; iii > ii; iii--) {

							if (VboneDat[B][Indice[B][i]].weights[iii] > VboneDat[B][Indice[B][i]].weights[ii]) {
								tmpWH = VboneDat[B][Indice[B][i]].weights[ii];
								VboneDat[B][Indice[B][i]].weights[ii] = VboneDat[B][Indice[B][i]].weights[iii];
								VboneDat[B][Indice[B][i]].weights[iii] = tmpWH;

								tmpIDH = VboneDat[B][Indice[B][i]].IDs[ii];
								VboneDat[B][Indice[B][i]].IDs[ii] = VboneDat[B][Indice[B][i]].IDs[iii];
								VboneDat[B][Indice[B][i]].IDs[iii] = tmpIDH;

								ii = 0;
								iii = VboneDat[B][Indice[B][i]].weights.size() - 1;
							}
						}
					}
					//set actual bone weights and id from tmps
					for (int y = 0; y < 4; y++) {
						modelDat[B][Indice[B][i]].tbw.w[y] = VboneDat[B][Indice[B][i]].weights[y];

						modelDat[B][Indice[B][i]].tbi.id[y] = VboneDat[B][Indice[B][i]].IDs[y];
					}
				}
			}

			//normalize bone weights based on man
			for (const std::pair<int, int>& kv : sDubC) {
				float max = 0.0f;

				for (int ii = 0; ii < 4; ii++) {
					max += modelDat[B][kv.first].tbw.w[ii];
				}

				if (max > 0) { //to stop error of 1
					for (int ii = 0; ii < 4; ii++) {
						modelDat[B][kv.first].tbw.w[ii] = modelDat[B][kv.first].tbw.w[ii] / max;
					}
				}
			}
		}
	}
	void LoadFBXFileWithVertex(std::string path) {
		LoadFBXFile(path);

		VertexPostProcess();

		SetAllBonesChildren();
		SetAllBonesParent();
		SetAllBonesGetInvBindT();

		ResizeAllBonesNodeArr();
		GetAllBoneNodes();

		ResizeAndPrepAnimBoneStuff();

		GetAllAnimEndTime();

		BindPoseBones();

		FillTopBones();
	}

	~M3DR() {
		//TODO: make deconstructor
	}

};