//////////////////////////////////////////////////////////////////////
//
// E3D_Scene1.cpp: class representing/managing a 3D scene
//
// Copyright 2006 James Higgs
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include "../include/E3D.h"

// Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

E3D_Scene::E3D_Scene()
{
	m_renderDistance = 1000.0f;
	m_fogDistance = 0.0f;
	SetBackColour(0.0f, 0.0f, 0.0f);

	// Clear/reset scene from scratch (removes all objects, models, lights, cameras)
	this->Start();
}

E3D_Scene::~E3D_Scene()
{
	int i;
/*
	// remove all objects
	for(i=0; i<MAX_OBJECTS_PER_SCENE; i++) {
		if(m_pObject[i]) {
			delete m_pObject[i];
			m_pObject[i] = NULL;
		}
	}
*/
	// remove all models
	for(i=0; i<MAX_MODELS_PER_SCENE; i++) {
		if(m_pModel[i]) {
			delete m_pModel[i];
			m_pModel[i] = NULL;
		}
	}

	// remove all cameras
	for(i=0; i<MAX_CAMERAS_PER_SCENE; i++) {
		if(m_pCamera[i]) {
			delete m_pCamera[i];
			m_pCamera[i] = NULL;
		}
	}

	// remove all lights
	for(i=0; i<MAX_LIGHTS_PER_SCENE; i++) {
		if(m_pLight[i]) {
			delete m_pLight[i];
			m_pLight[i] = NULL;
		}
	}
}

/// Clear/reset scene from scratch
/// (Removes all objects, models, lights, cameras)
void E3D_Scene::Start()
{
	int i;

	m_numLights = 0;
	m_numCameras = 0;
	m_numModels = 0;
	m_numObjects = 0;

/*
	// remove all objects
	for(i=0; i<MAX_OBJECTS_PER_SCENE; i++) {
		if(m_pObject[i]) {
			delete m_pObject[i];
			m_pObject[i] = NULL;
		}
	}
*/
	// remove all models
	for(i=0; i<MAX_MODELS_PER_SCENE; i++) {
		if(m_pModel[i]) {
			delete m_pModel[i];
			m_pModel[i] = NULL;
		}
	}

	// remove all cameras
	for(i=0; i<MAX_CAMERAS_PER_SCENE; i++) {
		if(m_pCamera[i]) {
			delete m_pCamera[i];
			m_pCamera[i] = NULL;
		}
	}

	// remove all lights
	for(i=0; i<MAX_LIGHTS_PER_SCENE; i++) {
		if(m_pLight[i]) {
			delete m_pLight[i];
			m_pLight[i] = NULL;
		}
	}

	// Add a default light to the scene
	E3D_Light *light0 = this->AddLight();
	light0->SetColour( 1.0f, 1.0f, 1.0f );
	light0->SetDirection( 0.0f, 0.7f, 0.7f );

	// Add a default camera to the scene
	E3D_Camera *camera0 = this->AddCamera();
	camera0->SetPosition(0.0f, 0.0f, -20.0f);
	camera0->LookAt(0.0f, 0.0f, 0.0f);

}

/// Clear all objects (instances) from scene
void E3D_Scene::RemoveAllObjects()
{
	int i;
	m_numObjects = 0;

	for(i = 0; i < NUM_OBJLISTS; i++)
		m_objList[i].clear();
}

/// Add a 3D model to this scene
/// @return		Pointer to the model added (or NULL on failure)
E3D_Model * E3D_Scene::AddModel(char *name)
{
	if(m_numModels >= MAX_MODELS_PER_SCENE) {
		printf("Could not create Model!\n");
		return NULL;
	}

	E3D_Model *model = new E3D_Model;

	model->SetName(name);
	m_pModel[m_numModels++] = model;
	return model;
}

/// Add a 3D model to this scene
/// @return		Pointer to the model added (or NULL on failure)
E3D_Model * E3D_Scene::AddModel(char *name, char *filename)
{
	if(m_numModels >= MAX_MODELS_PER_SCENE) {
		printf("Could not create Model!\n");
		return NULL;
	}

	E3D_Model *model = new E3D_Model;

	if(E3D_SUCCESS != model->Load(filename)) {
		delete model;
		return NULL;
	}

	model->SetName(name);
	m_pModel[m_numModels++] = model;
	return model;
}

/// Find a model in this scene
/// @note		case sensitive
/// @return		Model instance id, or -1 on failure
int E3D_Scene::FindModel(const char *modelName)
{
	int i;

	for(i=0; i<m_numModels; i++) {
		if(strcmp(m_pModel[i]->GetName(), modelName) == 0) {
			return i;
		}
	}

	return -1;
}



/// Add an object instance to this scene
/// @param whichList			Which object list in this scene to add this object to
/// @param modelName			The name of the model (shape) of this object
/// @return		Pointer to object instance (or NULL on failure)
E3D_Object * E3D_Scene::AddObject(unsigned short whichList, char *modelName)
{
	if (whichList >= NUM_OBJLISTS)
		return NULL;

	int n;
	E3D_Object newObject; // = new E3D_Object;
	n = FindModel(modelName);
	if(-1 == n) {
		printf("Could not add Object!\n");
		return NULL;
	}
	newObject.modelNumber = n;
	newObject.model = m_pModel[n];

	m_objList[whichList].push_back(newObject);
	return &m_objList[whichList].back(); //newObject;
}

/// Attach this object (instance) to another
/// @param whichList		Which object list in this scene the objects are in
/// @param objectNumber
bool E3D_Scene::AttachObject(unsigned short whichList, unsigned short objectNumber, unsigned short parentObjectNumber)
{
	if (whichList >= NUM_OBJLISTS)
		return false;

	if (objectNumber >= m_objList[whichList].size())
		return false;

	if (parentObjectNumber >= m_objList[whichList].size())
		return false;

	// prevent recusion
	if (objectNumber == parentObjectNumber)
		return false;

	// TODO: parent is a pointer, may become invalid if object list changes???
	//m_objList[whichList].at(objectNumber).parent = parentObjectNumber;
	m_objList[whichList].at(objectNumber).parent = &m_objList[whichList].at(parentObjectNumber);
	return true;
}

/// Add a light to this scene
/// @return		Pointer to the light added (or NULL on failure)
E3D_Light * E3D_Scene::AddLight()
{
	if(m_numLights < MAX_LIGHTS_PER_SCENE) {
		m_pLight[m_numLights] = new E3D_Light;
		return m_pLight[m_numLights++];
	}

	return NULL;
}

/// Add a camera to this scene
/// @return		Pointer to the camera added (or NULL on failure)
E3D_Camera * E3D_Scene::AddCamera()
{
	if(m_numCameras < MAX_CAMERAS_PER_SCENE) {
		m_pCamera[m_numCameras] = new E3D_Camera;
		return m_pCamera[m_numCameras++];
	}

	return NULL;
}

// Get the nth camera in this scene
E3D_Camera *E3D_Scene::GetCamera(int index)
{
	if(index > -1 && index < MAX_CAMERAS_PER_SCENE)
		return m_pCamera[index];
	else
		return NULL;
}

// Get the nth light in this scene
E3D_Light *E3D_Scene::GetLight(int index)
{
	if(index > -1 && index < MAX_LIGHTS_PER_SCENE)
		return m_pLight[index];
	else
		return NULL;
}

// Get the nth model in this scene
E3D_Model *E3D_Scene::GetModel(int index)
{
	if(index > -1 && index < MAX_MODELS_PER_SCENE)
		return m_pModel[index];
	else
		return NULL;
}

// Get the nth object in one of the object lists in this scene
E3D_Object *E3D_Scene::GetObject(unsigned short whichList, unsigned short index)
{
	if(whichList >= NUM_OBJLISTS)
		return NULL;

	if(index >= m_objList[whichList].size())
		return NULL;

	return &m_objList[whichList].at(index);
}

int E3D_Scene::GetNumCameras(void)
{
	return m_numCameras;
}

int E3D_Scene::GetNumLights(void)
{
	return m_numLights;
}

int E3D_Scene::GetNumModels(void)
{
	return m_numModels;
}

int E3D_Scene::GetNumObjects(void)
{
	return m_numObjects;
}




/// Set clipping near and far values for this scene
void E3D_Scene::SetClipping()
{

}

/// Render this scene to the screen
int E3D_Scene::Render(float viewWidth, float viewHeight, bool clear)
{
	// This function is platform specific and must be implemented
	// in E3D_PlatformSpecific.cpp 
	E3D_PlatformSpecific::RenderScene(this, viewWidth, viewHeight, clear);

	return E3D_SUCCESS;
}


/// Load a scene file (.scn)
/// @return		E3D_SUCCESS, or an E3D_SceneError error code
int E3D_Scene::Load(char *filename)
{
	int i, j;
	int validcmd, validparam, end;
	//class E3D_Model * pM;
	class E3D_Object * pO;
	class E3D_Light * pL;
	class E3D_Camera * pCam;

	float x, y, z, x1, y1, z1, x2, y2, z2, r, g, b, a;
	char scmd[80], stemp[80];
	char *ext;
	FILE *pfscene;
	FILE *plog;

	plog = fopen("loadscene.log", "w");

	fprintf(plog, "Opening scene file %s\n", filename);

	pfscene = fopen(filename, "r");
	// check if file open failed
	if(pfscene == NULL) {
		fprintf(plog, "Scene file open failed.\n");
        return FILE_OPEN_FAIL;
	}

	// empty out scene
	this->Start();

	bool defaultLightReplaced = false;

	end = FALSE;
	while(!end) {
		// reset valid flags to trap errors
		validcmd = FALSE; validparam = FALSE;

		fscanf(pfscene, "%s", &scmd[0]);
		fprintf(plog, "Command is %s\n", scmd);

		// check for comment line
		if(scmd[0] == '#') {
			validcmd = TRUE;
			fprintf(plog, "Comment encountered. Scanning till eol.\n");
			// read till end of line (character 10)
			fscanf(pfscene, "%c", &stemp[0]);
			while(stemp[0] != 10) fscanf(pfscene, "%c", &stemp[0]);
		}

		// LIGHT
		if(strcmp(scmd, "light") == 0) {
			validcmd = TRUE;
			validparam = FALSE;
			i = m_numLights;
			// if this is the first light we are loading, then replace the default light with it
			if(defaultLightReplaced) {
				pL = AddLight();
			}
			else {
				pL = this->GetLight(0);
				defaultLightReplaced = true;
			}
			// get type of light
			fscanf(pfscene, "%s", &stemp[0]);
			if(strcmp(stemp, "distant") == 0) {
				fprintf(plog, "distant light:  ");
				fscanf(pfscene, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
				fprintf(plog, "xyz %3.3f %f3.3 %3.3f   rgb %3.3f %3.3f %3.3f\n", x, y, z, r, g, b);
				pL->SetType(E3D_Light::DISTANT);
				pL->SetDirection(x, y, z);		// x, y, z is direction vector (unit)
				pL->SetColour(r, g, b);
				validparam = TRUE;
			}
			else if(strcmp(stemp, "omni") == 0) {
				fprintf(plog, "omni light:  ");
				fscanf(pfscene, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
				fprintf(plog, "xyz %3.3f %3.3f %3.3f   rgb %3.3f %3.3f %3.3f\n", x, y, z, r, g, b);
				pL->SetType(E3D_Light::OMNI);
				pL->SetDirection(x, y, z);		// x, y, z is position vector (unit)
				pL->SetColour(r, g, b);
				validparam = TRUE;
			}
/*
			else if(strcmp(stemp, "ambient") == 0) {
				fprintf(plog, "ambient light:  ");
				fscanf(pfscene, "%f %f %f", &r, &g, &b);
				fprintf(plog, "rgb %3.3f %3.3f %3.3f\n", r, g, b);
				pL->type = LIGHT_AMBIENT;
				pL->r = r;
				pL->g = g;
				pL->b = b;
				validparam = TRUE;
			}
*/
			else {
				fprintf(plog, "Error - Light type not recognised! Exiting...\n");
				fclose(pfscene);
				return BAD_LIGHT_TYPE;		// error
			}
		}

		// CAMERA
		if(strcmp(scmd, "camera") == 0) {
			validcmd = TRUE;
			//fprintf(plog, "Camera detected.\n");
			i = m_numCameras;
			pCam = AddCamera();
			fscanf(pfscene, "%f %f %f %f %f %f %f %f %f %f",
				&x, &y, &z, &x1, &y1, &z1, &x2, &y2, &z2, &a);
			fprintf(plog, "xyz %3.3f %3.3f %3.3f   dirn %3.3f %3.3f %3.3f   orient %3.3f %3.3f %3.3f  va %3.3f\n",
				x, y, z, x1, y1, z1, x2, y2, z2, a);
			pCam->SetPosition(x, y, z);
			pCam->SetDirection(x1, y1, z1);
			pCam->SetUpVector(x2, y2, z2);
			pCam->SetFOV(a);
		}

/*		// load in background bitmap
		if(strcmp(scmd, "background") == 0) {
			validcmd = TRUE;
			fprintf(stderr, "Background bitmap detected - ");
			fscanf(pfscene, "%s", &stemp[0]);
			fprintf(stderr, "%s\n",	stemp);
			ext = strupr(strrchr(stemp, '.'));
			fprintf(stderr, "bg file ext is %s\n", ext);
			// check if right filetype (bmp or pcx)
			if((strcmp(ext, ".BMP") == 0) || (strcmp(ext, ".PCX") == 0)) {
				scene->background = load_bitmap(stemp, paltemp);
				if(scene->background == NULL) {
					fprintf(stderr, "Error: background bitmap load failed!\n");
				}
			}
			else fprintf(stderr, "Error: background bitmap filetype incorrect!\n");
		}
*/
		// load in background colour
		if(strcmp(scmd, "backcolour") == 0) {
			validcmd = TRUE;
			//fprintf(plog, "Background colour detected.\n");
			fscanf(pfscene, "%f %f %f", &r, &g, &b);
			fprintf(plog, "bg col %3.3f %3.3f %3.3f\n", r, g, b);
			SetBackColour(r, g, b);
		}

		// load in fog params
		if(strcmp(scmd, "fog") == 0) {
			validcmd = TRUE;
			//fprintf(plog, "Fog command detected.\n");
			fscanf(pfscene, "%f %f %f %f", &r, &g, &b, &x);
			fprintf(plog, "fog %3.3f %3.3f %3.3f %4.3f\n", r, g, b, x);
			SetFogColour(r, g, b);
			SetFogDistance(x);
		}

		// load in renderdistance param
		if(strcmp(scmd, "renderdistance") == 0) {
			validcmd = TRUE;
			//fprintf(plog, "Renderdistance command detected.\n");
			fscanf(pfscene, "%f", &x);
			fprintf(plog, "renderdistance %5.3f\n", x);
			if(x > 1.0)	m_renderDistance = x;
			else fprintf(plog, "Error: invalid renderdist!\n");
		}


		// load in texture bitmap
		if(strcmp(scmd, "texture") == 0) {
			validcmd = TRUE;
			//fprintf(plog, "Texture bitmap detected - ");
			fscanf(pfscene, "%s", &stemp[0]);
			fprintf(plog, "%s\n",	stemp);
			ext = strrchr(stemp, '.');
			fprintf(plog, "texture file ext is %s\n", ext);
			fprintf(plog, "*** Warning: textures not supported yet! ***\n");
			// check if right filetype (bmp or pcx)
			if((0 == stricmp(ext, ".BMP")) || (0 == stricmp(ext, ".PCX"))) {
				//scene->ptex[scene->numtex] = load_bitmap(stemp, paltemp);
//blit(scene->ptex[0], screen, 0, 0, 0, 0, 256, 256);		// copy upper left
				//if(scene->ptex[scene->numtex] == NULL) {
				//	fprintf(plog, "Error: texture bitmap load failed!\n");
				//}
			}
			else fprintf(plog, "Error: texture bitmap filetype incorrect!\n");
			// increment texture count anyway
			// (textures must be loaded into the right slot!)
			m_numTextures++;
		}

		// MODEL
		if(strcmp(scmd, "model") == 0) {
			validcmd = TRUE;
			//fprintf(plog, "Model detected.\n");
			fscanf(pfscene, "%s", &stemp[0]);
			char sfile[1000];
			fscanf(pfscene, "%s", sfile);
			fprintf(plog, "Model %d, %s, file %s\n", m_numModels, stemp, sfile);

			// Don't allow 2 models with the same name
			if(-1 == FindModel(stemp)) {
				if(!AddModel(stemp, sfile)) {
					fprintf(plog, "Error - Model load failed! Exiting...\n");
					fclose(pfscene);
					return MODEL_LOAD_FAIL;
				}
				else {
					fprintf(plog, "Loaded model %s\n", stemp);
				}
			}
			else {
				fprintf(plog, "Model %s already loaded!\n", stemp);
			}
		}

		// OBJECT
		if(strcmp(scmd, "object") == 0) {
			validcmd = TRUE;
			//fprintf(plog, "Object detected.\n");
			fscanf(pfscene, "%s %f %f %f %f %f %f %f %f %f %d",
				&stemp[0], &x, &y, &z, &x1, &y1, &z1, &x2, &y2, &z2, &j);
			fprintf(plog, "Object %s\n   posn %3.3f %3.3f %3.3f\n   rotn %3.3f %3.3f %3.3f\n   scale %3.3f %3.3f %3.3f\n   flags %d\n",
				stemp, x, y, z, x1, y1, z1, x2, y2, z2, j);
			pO = AddObject(0, stemp);
			if(pO) {
				// set instance parameters
				//pO->flags = j;
				pO->SetPosition(x, y, z);
				pO->SetRotation(x1, y1, z1);
				pO->SetScale(x2, y2, z2);
				pO->visible = 1;				// init visible
				//pO->attach = NULL;
			}
			else {
				fprintf(plog, "Error - Object add failed! Exiting...\n");
				fclose(pfscene);
				return BAD_OBJECT_SPEC;
				// possible reasons:
				// 1. model not loaded
			}
		}

/*
		// load in landscape model
		if(strcmp(scmd, "landscape") == 0) {
			validcmd = TRUE;
			fprintf(stderr, "Landscape detected.\n");
			fscanf(pfscene, "%s %f %f %f %f %f %f %f %f %f",
				&stemp[0], &x, &y, &z, &x1, &y1, &z1, &x2, &y2, &z2);
			fprintf(stderr, "land %s   posn %3.3f %3.3f %3.3f   rotn %3.3f %3.3f %3.3f   scale %3.3f %3.3f %3.3f\n",
				stemp, x, y, z, x1, y1, z1, x2, y2, z2);
			scene->pland = (ThreeDeeInstance *)malloc(sizeof(ThreeDeeInstance));
			// set instance parameters
			scene->pland->posn.x = x;
			scene->pland->posn.y = y;
			scene->pland->posn.z = z;
			scene->pland->rotn.x = x1;
			scene->pland->rotn.y = y1;
			scene->pland->rotn.z = z1;
			scene->pland->scal.x = x2;
			scene->pland->scal.y = y2;
			scene->pland->scal.z = z2;

			// load landscape model and set object model index
			// allocate mem for model
			scene->plandmodel = (ThreeDeeLandscape *)malloc(sizeof(ThreeDeeLandscape));
			// reset model
			//if(Load3DO(stemp, scene->pmodel[scene->nummodels])) {
			i = E3D_LoadLandscape(stemp, scene->plandmodel);
			fprintf(stderr, "Object load returned %d\n", i);
			if(i == 0) {
				fprintf(stderr, "Object loaded succesfully.\n");
				//scene->pland->model = scene->plandmodel;
				// calculate poly normals and centroids for landscape
				E3D_CalculateCentroids(scene->plandmodel);
			}
			else {
				fprintf(stderr, "Error - Landscape load failed! Exiting...\n");
				fclose(pfscene);
				return(3);
			}
		}
*/
		if(strcmp(scmd, "end") == 0) {
			validcmd = TRUE;
			fprintf(plog, "End of scene file.\n");
			fprintf(plog, "Loaded %d models, %d instances, %d textures, %d lights and %d cameras.\n",
				 m_numModels, m_numObjects, m_numTextures, m_numLights, m_numCameras);
			end = TRUE;
		}

		// check if no valid command found (ie: parse error)
		if(!validcmd) {
			fprintf(plog, "Error - Scene Command not recognised! Exiting...\n");
			fclose(pfscene);
			return BAD_COMMAND;		// error
		}
	}

	fclose(pfscene);

	fclose(plog);

	return E3D_SUCCESS;		// success!


}


/// Set the scene's background colour
void E3D_Scene::SetBackColour(float r, float g, float b)
{
	m_backColour.r = r;
	m_backColour.g = g;
	m_backColour.b = b;
}

Colour E3D_Scene::GetBackColour(void)
{
	return m_backColour;
}

/// Set the scene's fog colour
void E3D_Scene::SetFogColour(float r, float g, float b)
{
	m_fogColour.r = r;
	m_fogColour.g = g;
	m_fogColour.b = b;
}

Colour E3D_Scene::GetFogColour(void)
{
	return m_fogColour;
}

/// Set the scene's fog distance (distance of full fog obscurance)
/// Note: fog distance = 0 means no fog
void E3D_Scene::SetFogDistance(float z)
{
	m_fogDistance = z;
}

float E3D_Scene::GetFogDistance(void)
{
	return m_fogDistance;
}

void E3D_Scene::SetRenderDistance(float z)
{
	m_renderDistance = z;
}

float E3D_Scene::GetRenderDistance(void)
{
	return m_renderDistance;
}



#ifdef E3D_TRACK
// Uses track sections make in Tracker to create track section models
void E3D_Scene::LoadTrack(char *filename) {

	int i, j;
	char sModelName[100];
	char sLoModelName[100];
	E3D_Model *pModel;
	E3D_Model *pLoModel;
	E3D_Object *pObject;
	Pgon *pPoly;
	Vertex_f v;

#define TRACK_SCALE	5.0

	float seclen;
	float x, y, z;
	float tx, ty, tz;
	float startpitch, startyaw, startroll;
	float endpitch, endyaw, endroll;
	//float dpitch, dyaw, droll;
	float pitch, dirn, roll;
	float dforward;

	// load the track
	CTrack *pTrack = new CTrack();
	pTrack->Load(filename);

	CTrackSection *ts; 

	pitch = dirn = roll = 0.0f;
	x = y = z = 0;

	for(i=0; i<pTrack->GetSectionCount(); i++) {
		pModel = AddModel();
		pModel->Reset();

		sprintf(sModelName, "TS%03d", i);
		strcpy(pModel->name, sModelName);

		ts = pTrack->GetSection(i);

		float dturn = DEG2RAD(ts->turn / ts->numSegments);
		float dpitch = DEG2RAD(ts->pitch / ts->numSegments);
		float droll = DEG2RAD(ts->roll / ts->numSegments);

		tx = 0;
		ty = y;
		tz = 0;
		// generate vertices and polys of our new model
		pModel->numverts = (ts->numSegments + 1) * 4;

		for(j=0; j<=ts->numSegments; j++) {
			// write LHS top vertex
			pModel->vert[j*4].x = tx - cos(dirn) * TRACK_SCALE;
			pModel->vert[j*4].y = ty + sin(pitch) * TRACK_SCALE;
			pModel->vert[j*4].z = tz + sin(dirn) * TRACK_SCALE;
			// write RHS top vertex
			pModel->vert[j*4+1].x = tx + cos(dirn) * TRACK_SCALE;
			pModel->vert[j*4+1].y = ty + sin(pitch) * TRACK_SCALE;
			pModel->vert[j*4+1].z = tz - sin(dirn) * TRACK_SCALE;
			// write LHS bottom vertex
			pModel->vert[j*4+2].x = tx - cos(dirn) * TRACK_SCALE;
			pModel->vert[j*4+2].y = 0.0f;
			pModel->vert[j*4+2].z = tz + sin(dirn) * TRACK_SCALE;
			// write RHS bottom vertex
			pModel->vert[j*4+3].x = tx + cos(dirn) * TRACK_SCALE;
			pModel->vert[j*4+3].y = 0.0f;
			pModel->vert[j*4+3].z = tz - sin(dirn) * TRACK_SCALE;

			// update (but not on the last vertex pair)
			if(j<ts->numSegments) {
				tx = tx - sin(dirn) * TRACK_SCALE;
				tz = tz - cos(dirn) * TRACK_SCALE;
				ty = ty + sin(pitch) * TRACK_SCALE;
				dirn += dturn;
				pitch += dpitch;
				roll += droll;
			}
		}

		for(j=0; j<ts->numSegments; j++) {
			// top
			pPoly = pModel->AddPoly4(j*4, j*4+4, j*4+5, j*4+1);
			pPoly->filling = 0;
			if(j & 1) {
				pPoly->colour.r = 1.0f;
				pPoly->colour.g = 0.5f;
				pPoly->colour.b = 0.2f;
			}
			else {
				pPoly->colour.r = 0.2f;
				pPoly->colour.g = 0.5f;
				pPoly->colour.b = 1.0f;
			}
			// left wall
			pPoly = pModel->AddPoly4(j*4, j*4+2, j*4+6, j*4+4);
			pPoly->filling = 0;
			pPoly->colour.r = 0.9f;
			pPoly->colour.g = 0.9f;
			pPoly->colour.b = 0.9f;
			// right wall
			pPoly = pModel->AddPoly4(j*4+5, j*4+7, j*4+3, j*4+1);
			pPoly->filling = 0;
			pPoly->colour.r = 0.9f;
			pPoly->colour.g = 0.9f;
			pPoly->colour.b = 0.9f;
		}

		pModel->CalculateNormals();			// generate normals for this model

		// add an instance of this model
		pObject = AddObject(sModelName);
		pObject->MoveTo(x, 0.0, z);
		pObject->Rotate(0.0, 0.0, 0.0);
		pObject->Scale(1.0, 1.0, 1.0);
		pObject->flags |= FLAGS_SURFPOLY;		// this object has surface polys

/*
		// create LOW-DETAIL VERSION of this track section
		pLoModel = AddModel();
		pLoModel->Reset();

		sprintf(sLoModelName, "TS%03d_LO", i);
		strcpy(pLoModel->name, sLoModelName);

		v = pModel->vert[0];
		pLoModel->AddVertex(v);
		v = pModel->vert[1];
		pLoModel->AddVertex(v);
		v = pModel->vert[2];
		pLoModel->AddVertex(v);
		v = pModel->vert[3];
		pLoModel->AddVertex(v);
		v = pModel->vert[ts.numDivs*4];
		pLoModel->AddVertex(v);
		v = pModel->vert[ts.numDivs*4+1];
		pLoModel->AddVertex(v);
		v = pModel->vert[ts.numDivs*4+2];
		pLoModel->AddVertex(v);
		v = pModel->vert[ts.numDivs*4+3];
		pLoModel->AddVertex(v);
		// top
		pPoly = pLoModel->AddPoly4(1, 5, 6, 2);
		pPoly->filling = 0;
		pPoly->colour.r = 1.0f;
		pPoly->colour.g = 0.5f;
		pPoly->colour.b = 0.2f;
		// left wall
		pPoly = pLoModel->AddPoly4(0, 4, 5, 1);
		pPoly->filling = 0;
		pPoly->colour.r = 0.9f;
		pPoly->colour.g = 0.9f;
		pPoly->colour.b = 0.9f;
		// right wall
		pPoly = pLoModel->AddPoly4(2, 6, 7, 3);
		pPoly->filling = 0;
		pPoly->colour.r = 0.9f;
		pPoly->colour.g = 0.9f;
		pPoly->colour.b = 0.9f;

		pLoModel->CalculateNormals();			// generate normals for this model

		pObject = AddObject(sLoModelName);
		pObject->MoveTo(ts.vStart.x, 0.0, ts.vStart.z + 200.0);
		pObject->Rotate(0, 0, 0);
		pObject->Scale(1, 1, 1);
*/
		//update track section position
		x += tx;
		y = ty;
		z += tz;
	}

}
#endif

