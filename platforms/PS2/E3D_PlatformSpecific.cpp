//////////////////////////////////////////////////////////////////////
//
// E3D_PlatformSpecific.cpp: implementation of platform-specific stuff
//                           for Sony PS2
//
//////////////////////////////////////////////////////////////////////

// #include neccessary PS2 includes here

// These 2 lines very important!
#include "../../include/E3D.h"
using namespace E3D;


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
/* reference code only!
	E3D_Utility util;
	E3D_Object *pO; 

	// make sure we're in model matrix mode
	sceGumMatrixMode(GU_MODEL);

	for(int i=0; i<scene->m_numObjects; i++) {
		pO = scene->m_pObject[i];

		float dist = 0; //util.vector_length_f(pO->x - x_eye, pO->y - y_eye, pO->z - z_eye);

		if(dist < scene->m_renderDistance) {
			// set up this entity's transform (from modelspace to worldspace)
			sceGumLoadIdentity();
			{
				ScePspFVector3 v;
				ScePspFVector3 rot;
				ScePspFVector3 scale;
				E3D_Object *pParentObj = pO->GetParent();
				if(pParentObj) {
					v.x = pParentObj->x;
					v.y = pParentObj->y;
					v.z = pParentObj->z;
					rot.x = pParentObj->rx;
					rot.y = pParentObj->ry;
					rot.z = pParentObj->rz;
					scale.x = pParentObj->sx;
					scale.y = pParentObj->sy;
					scale.z = pParentObj->sz;
					sceGumTranslate(&v);
					sceGumRotateXYZ(&rot);
					sceGumScale(&scale);
				}

				v.x = pO->x;
				v.y = pO->y;
				v.z = pO->z;
				rot.x = pO->rx;
				rot.y = pO->ry;
				rot.z = pO->rz;
				scale.x = pO->sx;
				scale.y = pO->sy;
				scale.z = pO->sz;
				sceGumTranslate(&v);
				sceGumRotateXYZ(&rot);
				sceGumScale(&scale);
			}

			//glCallList(displist[pO->modelNumber]);			// Draw the object
			DrawModel(pO->GetModel());

		}	// end if dist < whatever
	}	// next object / instance
*/
}

/*
struct PSP_Vertex
{
	//float u, v;
	unsigned int color;
	float x,y,z;
};
*/

// Draw a single 3D object
void E3D_PlatformSpecific::DrawModel(E3D_Model *pModel)
{
/* reference code only!
	int vi[4];

	PSP_Vertex vertex[6000];

	int n = 0;			
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
					vertex[n].x = pModel->m_vert[vi[j]].x;
					vertex[n].y = pModel->m_vert[vi[j]].y;
					vertex[n].z = pModel->m_vert[vi[j]].z;
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
				//p1 = pModel->poly[i].p[0];
				//p2 = pModel->poly[i].p[3];
				//p3 = pModel->poly[i].p[2];
				//p4 = pModel->poly[i].p[1];

				////glBindTexture( GL_TEXTURE_2D, texid );

				//glBegin(GL_QUADS);
				//	// normal
				//	glNormal3f(pModel->norm[i].x, pModel->norm[i].y, pModel->norm[i].z);
				//	// vertices
				//	//glTexCoord2f( 0.0f,  0.0f );
				//	glVertex3f(pModel->vert[p1].x, pModel->vert[p1].y, pModel->vert[p1].z);
				//	//glTexCoord2f(  1.0f,  0.0f );
				//	glVertex3f(pModel->vert[p2].x, pModel->vert[p2].y, pModel->vert[p2].z);
				//	//glTexCoord2f(  1.0f, 1.0f );
				//	glVertex3f(pModel->vert[p3].x, pModel->vert[p3].y, pModel->vert[p3].z);
				//	//glTexCoord2f( 0.0f, 1.0f );
				//	glVertex3f(pModel->vert[p4].x, pModel->vert[p4].y, pModel->vert[p4].z);
				//glEnd();
				break;
		} // end switch
	}

	sceGumDrawArray(GU_TRIANGLES,GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D,n,0,&vertex[0]);
*/
}


