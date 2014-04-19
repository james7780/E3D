//////////////////////////////////////////////////////////////////////
//
// E3D_PlatformSpecific.cpp: implementation of platform-specific stuff for Sony PSP
//
//////////////////////////////////////////////////////////////////////

#include <pspgu.h>
#include <pspgum.h>
#include <psputils.h>

extern "C" {
//#include "../sdk/samples/gu/common/callbacks.h"
//#include "../sdk/samples/gu/common/vram.h"
#include "callbacks.h"
#include "vram.h"
}

#include "../../include/E3D.h"

// Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

struct PSP_Vertex
{
	//float u, v;
	unsigned int color;
	ScePspFVector3 normal;
	ScePspFVector3 position;
};

/*
typedef struct
{
	ScePspFVector2 texture;
	u32 color;
	ScePspFVector3 normal;
	ScePspFVector3 position;
} TCNPVertex; // Texture, Color, Normal, Position
#define TCNP_VERTEX_FORMAT (GU_TEXTURE_32BITF|GU_COLOR_8888|GU_NORMAL_32BITF|GU_VERTEX_32BITF)
*/

static PSP_Vertex __attribute__((aligned(16))) vertex[10000];
static int g_lineVertCount;
static int g_polyVertCount;

E3D_PlatformSpecific::E3D_PlatformSpecific()
{

}

E3D_PlatformSpecific::~E3D_PlatformSpecific()
{

}

/// Render the E3D scene to our current rendering context
/// Note: this is a major part of the platform-specific implementation
/// It should:
/// 1. Set up lights using the E3D_Light objects
/// 2. Set up camera using the E3D_Camera object(s)
/// 3. Draw all the objects in the scene.
/// It should NOT:
/// 1. Initialise OpenGL or whatever rendering engine you're using
/// 2. Set up viewports
/// 3. Set up perspective
void E3D_PlatformSpecific::RenderScene(E3D_Scene *scene, float viewWidth, float viewHeight, bool clear)
{
	E3D_Utility util;

	// 3. Clear pixel and depth buffer if required
	if(clear) {
		Colour bgCol = scene->GetBackColour();
		//sceGuClearColor(0xff000000 || ((int)(bgCol.b * 255) << 16) || ((int)(bgCol.g * 255) << 8) || ((int)(bgCol.r * 255)));
		sceGuClearColor(GU_COLOR(bgCol.r, bgCol.g, bgCol.b, 1.0f));
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
	}

	// 1. Set up light
	// for now, we just use the first light in the scene
	E3D_Light *light = scene->GetLight(0);
	if(light) {
		ScePspFVector3 dir = { light->GetDirection().x, light->GetDirection().y, light->GetDirection().z };
		// calculate light colour
		int r = (int)(light->GetColour().r * 255);
		int g = (int)(light->GetColour().g * 255);
		int b = (int)(light->GetColour().b * 255);
		int color = 0xff000000 | (b << 16) | (g << 8) | r;
		sceGuLight(0, GU_DIRECTIONAL, GU_DIFFUSE, &dir);
		sceGuLightColor(0, GU_DIFFUSE, color);
		r = (int)(light->GetAmbient().r * 255);
		g = (int)(light->GetAmbient().g * 255);
		b = (int)(light->GetAmbient().b * 255);
		color = 0xff000000 | (b << 16) | (g << 8) | r;
		sceGuAmbient(color);
		sceGuEnable(GU_LIGHTING);
		sceGuEnable(GU_LIGHT0);
	}
	else {
		sceGuDisable(GU_LIGHTING);
	}

	// 2. set camera
	// for now, we just use the first camera in the scene
	E3D_Camera *camera = scene->GetCamera(0);
	if(camera == NULL)
		return;							// no camera, no dice!

	// make sure we're in projection matrix mode 
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective(camera->GetFOV(), viewWidth/viewHeight, 1.0f, 1000.0f);

	ScePspFVector3 target, eye, dirn, up;
	eye.x = camera->GetPosition().x;
	eye.y = camera->GetPosition().y;
	eye.z = camera->GetPosition().z;
	dirn.x = camera->GetDirection().x;
	dirn.y = camera->GetDirection().y;
	dirn.z = camera->GetDirection().z;
	up.x = camera->GetUpVector().x;
	up.y = camera->GetUpVector().y;
	up.z = camera->GetUpVector().z;
	target.x = eye.x + dirn.x;
	target.y = eye.y + dirn.y;
	target.z = eye.z + dirn.z;
	sceGumLookAt(&eye, &target, &up);

	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	// make sure we're in model matrix mode
	sceGumMatrixMode(GU_MODEL);

	g_lineVertCount = 0;
	g_polyVertCount = 0;

	sceGuDisable(GU_LIGHTING);

	// iterate thru all the object lists
	for(int i = 0; i < NUM_OBJLISTS; i++)
		{
		OBJLIST* objList = scene->GetObjectList(i);
		OBJLIST::const_iterator objIter;
		for(objIter = objList->begin() ; objIter != objList->end(); objIter++)
			{
			float dist = 0; //util.vector_length_f(pO->x - x_eye, pO->y - y_eye, pO->z - z_eye);
			if(dist < scene->GetRenderDistance())
				{
				// set up this entity's transform (from modelspace to worldspace)
				sceGumLoadIdentity();
				{
					ScePspFVector3 v;
					ScePspFVector3 rot;
					ScePspFVector3 scale;
					E3D_Object *pParentObj = objIter->parent;
					if(pParentObj)
						{
						v.x = pParentObj->position.x;
						v.y = pParentObj->position.y;
						v.z = pParentObj->position.z;
						rot.x = pParentObj->rotation.x;
						rot.y = pParentObj->rotation.y;
						rot.z = pParentObj->rotation.z;
						scale.x = pParentObj->scale.x;
						scale.y = pParentObj->scale.y;
						scale.z = pParentObj->scale.z;
						sceGumTranslate(&v);
						sceGumRotateXYZ(&rot);
						sceGumScale(&scale);
						}

					v.x = objIter->position.x;
					v.y = objIter->position.y;
					v.z = objIter->position.z;
					rot.x = objIter->rotation.x;
					rot.y = objIter->rotation.y;
					rot.z = objIter->rotation.z;
					scale.x = objIter->scale.x;
					scale.y = objIter->scale.y;
					scale.z = objIter->scale.z;
					sceGumTranslate(&v);
					sceGumRotateXYZ(&rot);
					sceGumScale(&scale);
				}

				//glCallList(displist[pO->modelNumber]);			// Draw the object
				DrawModel(objIter->model);
				}	// end if dist < whatever
			}	// next object / instance
		}


}

/// Draw a single E3D model (using the current PSP GU context)
/// Note: this is a major part of the platform-specific implementation
/// It should just draw the specific model
void E3D_PlatformSpecific::DrawModel(E3D_Model *pModel)
{
	int vi[4];

	int n = 0;
/*
	for(int i=0; i < pModel->m_numPolys; i++) {
		// calculate polygon colour
		int r = (int)(pModel->m_poly[i].colour.r * 255);
		int g = (int)(pModel->m_poly[i].colour.g * 255);
		int b = (int)(pModel->m_poly[i].colour.b * 255);
		int color = 0xff000000 | (b << 16) | (g << 8) | r;

		// handle triangles or quads
		switch(pModel->m_poly[i].numverts) {
			case 3 :
				{
				vi[0] = pModel->m_poly[i].p[0];
				vi[1] = pModel->m_poly[i].p[2];
				vi[2] = pModel->m_poly[i].p[1];

				for(int j=0; j<3; j++) {
					vertex[n].normal.x = pModel->m_norm[i].x;
					vertex[n].normal.y = pModel->m_norm[i].y;
					vertex[n].normal.z = pModel->m_norm[i].z;
					vertex[n].position.x = pModel->m_vert[vi[j]].x;
					vertex[n].position.y = pModel->m_vert[vi[j]].y;
					vertex[n].position.z = pModel->m_vert[vi[j]].z;
					vertex[n].color = color;
					n++;
				}

				// FOR REFERENCE:
				//glBegin(GL_TRIANGLES);
				//	// normal
				//	glNormal3f(pModel->norm[i].x, pModel->norm[i].y, pModel->norm[i].z);
				//	// vertices
				//	glVertex3f(pModel->vert[p1].x, pModel->vert[p1].y, pModel->vert[p1].z);
				//	glVertex3f(pModel->vert[p2].x, pModel->vert[p2].y, pModel->vert[p2].z);
				//	glVertex3f(pModel->vert[p3].x, pModel->vert[p3].y, pModel->vert[p3].z);
				//glEnd();
				}
				break;
			case 4 :
				{
				// for now, we split into 2 triangles
				vi[0] = pModel->m_poly[i].p[0];
				vi[1] = pModel->m_poly[i].p[3];
				vi[2] = pModel->m_poly[i].p[2];

				for(int j=0; j<3; j++) {
					vertex[n].normal.x = pModel->m_norm[i].x;
					vertex[n].normal.y = pModel->m_norm[i].y;
					vertex[n].normal.z = pModel->m_norm[i].z;
					vertex[n].position.x = pModel->m_vert[vi[j]].x;
					vertex[n].position.y = pModel->m_vert[vi[j]].y;
					vertex[n].position.z = pModel->m_vert[vi[j]].z;
					vertex[n].color = color;
					n++;
				}

				vi[0] = pModel->m_poly[i].p[2];
				vi[1] = pModel->m_poly[i].p[1];
				vi[2] = pModel->m_poly[i].p[0];

				for(int j=0; j<3; j++) {
					vertex[n].normal.x = pModel->m_norm[i].x;
					vertex[n].normal.y = pModel->m_norm[i].y;
					vertex[n].normal.z = pModel->m_norm[i].z;
					vertex[n].position.x = pModel->m_vert[vi[j]].x;
					vertex[n].position.y = pModel->m_vert[vi[j]].y;
					vertex[n].position.z = pModel->m_vert[vi[j]].z;
					vertex[n].color = color;
					n++;
				}

				}
				break;
		} // end switch
	}

	sceKernelDcacheWritebackAll();

	sceGumDrawArray(GU_TRIANGLES, GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, n ,0 ,&vertex[0]);
	//sceGumDrawArray(GU_TRIANGLES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, n ,0 ,&vertex[0]);
	//sceGumDrawArray(GU_TRIANGLES, TCNP_VERTEX_FORMAT | GU_TRANSFORM_3D, n ,0 ,&vertex[0]);
*/

	// Now draw lines
	if(pModel->m_numLines > 0) {
/*		glBegin(GL_LINES);
		for(int i=0; i < pModel->m_numLines; i++) {
			// calculate line colour
			glColor3f(pModel->m_line[i].colour.r, pModel->m_line[i].colour.g, pModel->m_line[i].colour.b);

			// add line start and end vertices
			vi[0] = pModel->m_line[i].vert1;
			vi[1] = pModel->m_line[i].vert2;
			glVertex3f(pModel->m_vert[vi[0]].x, pModel->m_vert[vi[0]].y, pModel->m_vert[vi[0]].z);
			glVertex3f(pModel->m_vert[vi[1]].x, pModel->m_vert[vi[1]].y, pModel->m_vert[vi[1]].z);
		}
		glEnd();*/

		int m = g_lineVertCount;
		for(int i=0; i < pModel->m_numLines; i++) {
			// calculate line colour
			int r = (int)(pModel->m_line[i].colour.r * 255);
			int g = (int)(pModel->m_line[i].colour.g * 255);
			int b = (int)(pModel->m_line[i].colour.b * 255);
			int color = 0xff000000 | (b << 16) | (g << 8) | r;

			vi[0] = pModel->m_line[i].vert1;
			vi[1] = pModel->m_line[i].vert2;

			vertex[m].position.x = pModel->m_vert[vi[0]].x;
			vertex[m].position.y = pModel->m_vert[vi[0]].y;
			vertex[m].position.z = pModel->m_vert[vi[0]].z;
			vertex[m].color = color;
			m++;
			vertex[m].position.x = pModel->m_vert[vi[1]].x;
			vertex[m].position.y = pModel->m_vert[vi[1]].y;
			vertex[m].position.z = pModel->m_vert[vi[1]].z;
			vertex[m].color = color;
			m++;
		}


		// neccessary?
		sceKernelDcacheWritebackAll();

		//sceGuFinish();
		//sceGuSync(0,0);

		// draw the lines, starting from vertex n
		sceGumDrawArray(GU_LINES, GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, m-g_lineVertCount ,0 ,&vertex[g_lineVertCount]);
		//sceGuEnable(GU_LIGHTING);

		g_lineVertCount = m;

	}

}


