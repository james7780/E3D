//////////////////////////////////////////////////////////////////////
//
// E3D_PlatformSpecific.cpp: implementation of platform-specific stuff
//                           for Linux
//
// Copyright 2006/2007 James Higgs
// 
//////////////////////////////////////////////////////////////////////

#include "SDL.h"   /* All SDL App's need this */
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../../include/E3D.h"

// Set our C++ namespace prefix to E3D so we can more easily use E3D objects
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
	E3D_Utility util;
	//E3D_Object *pO; 

	// 1. Set up lights
	// for now, we only use first light in the scene
	E3D_Light *light = scene->GetLight(0);
	// trtanslate to GL
	GLfloat LightAmbient[]= { light->GetAmbient().r, light->GetAmbient().g, light->GetAmbient().b, 1.0f }; 			// Ambient Light Values
	GLfloat LightDiffuse[]= { light->GetColour().r, light->GetColour().g, light->GetColour().b, 1.0f };				// Diffuse Light Values 
	GLfloat LightDirection[]= { light->GetDirection().x, light->GetDirection().y, light->GetDirection().z, 0.0f };			// Light Position/Direction

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// set ambient light level
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// set light colour
	glLightfv(GL_LIGHT1, GL_POSITION,LightDirection);	// set light position or direction
	glEnable(GL_LIGHT1);								// Enable Light One
	glEnable(GL_LIGHTING);								// Enable Lighting

	// 2. Set up camera (eye)
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	// for now, we only use the first camera in the scene
	E3D_Camera *camera = scene->GetCamera(0);
	if(camera == NULL)
		return;							// no camera, no dice!

	// TODO: what effect foes far/near z have?
	//gluPerspective(45.0f,(GLfloat)VIEW_WIDTH/VIEW_HEIGHT,1.0f,1000.0f);
	gluPerspective(camera->GetFOV(), viewWidth/viewHeight, 1.0f, scene->GetRenderDistance());

	Vector target, eye, dirn, up;
	eye = camera->GetPosition();
	dirn = camera->GetDirection();
	up = camera->GetUpVector();

	target.x = eye.x + dirn.x;
	target.y = eye.y + dirn.y;
	target.z = eye.z + dirn.z;
	gluLookAt(eye.x, eye.y, eye.z, target.x, target.y, target.z, up.x, up.y, up.z); 

	// 3. Clear pixel and depth buffer if required
	if(clear) {
		Colour bgCol = scene->GetBackColour();
		glClearColor(bgCol.r, bgCol.g, bgCol.b, 0.5f);				// Background colour
		glClearDepth(1.0f);									// Depth Buffer Setup
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	}

	glMatrixMode(GL_MODELVIEW);					// Make sure we're using the Modelview Matrix

	// TODO: use iterator!
	for(int i = 0; i < NUM_OBJLISTS; i++)
	{
	OBJLIST* objList = scene->GetObjectList(i);

	//for(int i=0; i<scene->GetNumObjects(); i++) {
	//	pO = scene->GetObject(0, i);
	//for (int i = 0; i < objList.size(); i++)
	OBJLIST::const_iterator objIter;
	for(objIter = objList->begin() ; objIter != objList->end(); objIter++)
		{
		//pO = &objList.at(i);

		// TODO: add scene->GetCamera()->GetPosition() for getting eye pos
		// TODO: calc distance to this object
		float dist = 0; //util.vector_length_f(pO->x - x_eye, pO->y - y_eye, pO->z - z_eye);

		if(dist < scene->GetRenderDistance()) {
			// set up this entity's transform (from modelspace to worldspace)
			glLoadIdentity();

			if(objIter->parent) {
				Vector p = objIter->parent->position;
				glTranslatef(p.x, p.y, p.z);
				Vector r = objIter->parent->rotation;
				glRotatef(RAD2DEG(r.y), 0.0f, 1.0f, 0.0f);			// Spin Left And Right
				glRotatef(RAD2DEG(r.z), 0.0f, 0.0f, 1.0f);			// roll
				glRotatef(RAD2DEG(r.x), 1.0f, 0.0f, 0.0f);			// pitch
				Vector s = objIter->parent->scale;
				glScalef(s.x, s.y, s.z);
			}

			glTranslatef(objIter->position.x, objIter->position.y, objIter->position.z);
			glRotatef(RAD2DEG(objIter->rotation.y) , 0.0f, 1.0f, 0.0f);			// Spin Left And Right
			glRotatef(RAD2DEG(objIter->rotation.z), 0.0f, 0.0f, 1.0f);			// roll
			glRotatef(RAD2DEG(objIter->rotation.x), 1.0f, 0.0f, 0.0f);			// pitch
			glScalef(objIter->scale.x, objIter->scale.y, objIter->scale.z);

			//glCallList(displist[pO->modelNumber]);			// Draw the object
			DrawModel(objIter->model);

		}	// end if dist < whatever
	}	// next object / instance

	}	// next objlist
}

/// Draw a single E3D model (using the current GL context)
/// Note: this is a major part of the platform-specific implementation
/// It should just draw the specific model
void E3D_PlatformSpecific::DrawModel(E3D_Model *pModel)
{
	int vi[4];

	// draw polygons
// TEST!
	glDisable(GL_LIGHTING);								// Enable Lighting
// END TEST
	for(int i=0; i < pModel->m_numPolys; i++) {
		// calculate polygon colour
		glColor3f(pModel->m_poly[i].colour.r, pModel->m_poly[i].colour.g, pModel->m_poly[i].colour.b);

		// handle triangles or quads
		switch(pModel->m_poly[i].numverts) {
			case 3 :
				{
				vi[0] = pModel->m_poly[i].p[0];
				vi[1] = pModel->m_poly[i].p[2];
				vi[2] = pModel->m_poly[i].p[1];

				glBegin(GL_TRIANGLES);
					// normal
					glNormal3f(pModel->m_norm[i].x, pModel->m_norm[i].y, pModel->m_norm[i].z);
					// vertices
					glVertex3f(pModel->m_vert[vi[0]].x, pModel->m_vert[vi[0]].y, pModel->m_vert[vi[0]].z);
					glVertex3f(pModel->m_vert[vi[1]].x, pModel->m_vert[vi[1]].y, pModel->m_vert[vi[1]].z);
					glVertex3f(pModel->m_vert[vi[2]].x, pModel->m_vert[vi[2]].y, pModel->m_vert[vi[2]].z);
				glEnd();
				}
				break;
			case 4 :
				vi[0] = pModel->m_poly[i].p[0];
				vi[1] = pModel->m_poly[i].p[3];
				vi[2] = pModel->m_poly[i].p[2];
				vi[3] = pModel->m_poly[i].p[1];

				//glBindTexture( GL_TEXTURE_2D, texid );

				glBegin(GL_QUADS);
					// normal
					glNormal3f(pModel->m_norm[i].x, pModel->m_norm[i].y, pModel->m_norm[i].z);
					// vertices
					//glTexCoord2f( 0.0f,  0.0f );
					glVertex3f(pModel->m_vert[vi[0]].x, pModel->m_vert[vi[0]].y, pModel->m_vert[vi[0]].z);
					//glTexCoord2f(  1.0f,  0.0f );
					glVertex3f(pModel->m_vert[vi[1]].x, pModel->m_vert[vi[1]].y, pModel->m_vert[vi[1]].z);
					//glTexCoord2f(  1.0f, 1.0f );
					glVertex3f(pModel->m_vert[vi[2]].x, pModel->m_vert[vi[2]].y, pModel->m_vert[vi[2]].z);
					//glTexCoord2f( 0.0f, 1.0f );
					glVertex3f(pModel->m_vert[vi[3]].x, pModel->m_vert[vi[3]].y, pModel->m_vert[vi[3]].z);
				glEnd();
				break;
		} // end switch
	}

	// draw lines
	if(pModel->m_numLines > 0) {
		glDisable(GL_LIGHTING);								// Enable Lighting
		glBegin(GL_LINES);
		for(int i=0; i < pModel->m_numLines; i++) {
			// calculate line colour
			//glColor3f(pModel->m_line[i].colour.r, pModel->m_line[i].colour.g, pModel->m_line[i].colour.b);
			Line* line = &pModel->m_line[i];
			glColor3f(line->colour.r,line->colour.g, line->colour.b);

			// add line start and end vertices
			Vertex* vert1 = &pModel->m_vert[line->vert1];
			Vertex* vert2 = &pModel->m_vert[line->vert2];
			//glColor3f(vert1->r,vert1->g, vert1->b);
			glVertex3f(vert1->x, vert1->y, vert1->z);
			//glColor3f(vert2->r,vert2->g, vert2->b);
			glVertex3f(vert2->x, vert2->y, vert2->z);
		}
		glEnd();
		glEnable(GL_LIGHTING);								// Enable Lighting
	}

}


