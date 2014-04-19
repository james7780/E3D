/*
 *		E3D example 6: Tube Wars
 *		James Higgs 2006/2007
 *
 *		This Code based on Jeff Molofee's OpenGL template
 */

// TODO:
// 1. Sequencer
// 2. Scoring
// 3. End of level animation

#include <windows.h>				// Header File For Windows
#include <stdio.h>					// Header File For Standard Input/Output
#include <math.h>
#include <gl\gl.h>					// Header File For The OpenGL32 Library
#include <gl\glu.h>					// Header File For The GLu32 Library
#include <gl\glaux.h>				// Header File For The Glaux Library
#include "../common/glfw.h"			// Header File for the GL Framwork Lib

// NB: Include our E3D header file
#include "../../../include/E3D.h"
// NB: Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

#include "tempest.h"
#include "vecglyph.h"

#define VIEW_WIDTH		600
#define VIEW_HEIGHT		400

// Windows/GL version variables
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default
bool	light;				// Lighting ON/OFF ( NEW )
bool	lp;					// L Pressed? ( NEW )

GLuint	texture[1];			// Storage For 1 Texture
GLuint	texid;
GLuint	model1;				// Storage For The Box Display List
GLuint	top;				// Storage For The Top Display List
GLuint	displist[20];
GLuint	xloop;				// Loop For X Axis
GLuint	yloop;				// Loop For Y Axis

GLfloat	xrot;				// Rotates Cube On The X Axis
GLfloat	yrot;				// Rotates Cube On The Y Axis
GLfloat zrot = 0.0f;
GLfloat zoom = 50;
GLfloat prop_rot;
GLfloat xpos[50], ypos[50], zpos[50];
GLfloat x_tree[200], y_tree[200], z_tree[200];
GLfloat z_gnd;
GLfloat y_eye = 50.0f;
GLfloat z_eye = -80.0f;
GLfloat x_eye = 0.0f;
GLfloat treescale = 1.0f;
GLfloat LightAmbient[]= { 0.2f, 0.2f, 0.2f, 1.0f }; 			// Ambient Light Values
GLfloat LightDiffuse[]= { 0.8f, 0.8f, 0.6f, 1.0f };				// Diffuse Light Values 
GLfloat LightPosition[]= { 0.0f, 0.7f, 0.7f, 0.0f };			// Light Position ( NEW )

// game variables
unsigned int score = 0;
unsigned int nextBonusScore = 0;

#define PI 3.141592653589793

#define OBJ_MAX			400
#define NUM_LEVELS		100
#define TUBE_LENGTH		200.0f
#define STEP_SIZE		6.0f
#define HIT_RADIUS		2.5f		// must be slightly less than half STEP_SIZE!
#define RELOAD_COUNT	8

class E3D_Scene Scene;
class E3D_Model gpModel[OBJ_MAX];
class E3D_Object gpObject[OBJ_MAX];

// model map
// NB: Models must be added to the scene in this order:
#define TUBE_MODEL		0
#define PLAYER1_MODEL	1
#define PLAYER2_MODEL	2
#define BULLET_MODEL	3
#define FRAG_MODEL		4
#define FLIPPER_MODEL	5
#define TANKER_MODEL	6
#define EBULLET_MODEL	7
#define SPIKE_MODEL		8
#define SPIKER_MODEL	9
#define BARRIER_MODEL	10

// object instance "partition" map
// NB: Objects must be added to the scene in this order
#define TUBE_ID			0
#define PLAYER1_ID		1
#define PLAYER2_ID		2
#define BULLET_ID		3			// to 29
#define BULLET_ENDID	29
#define FRAG_ID			30			// to 99
#define FRAG_ENDID		99
#define ENEMY_ID		100
#define ENEMY_ENDID		199

// object behaviour map
#define BEHAVE_NONE		0				// object inactive/off
#define BEHAVE_CIRCLE	1				// object circling/wating for launch onto web
#define BEHAVE_NORMAL	2				// object's normal behaviour
#define BEHAVE_ALT		3				// object's alternative behaviour

#include "enemy_seq.h"

void GLFWCALL ResizeWindow( int width, int height ) {
	//glfwSetWindowSize( width, height );

	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	// JH - changed far z here to 500
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,1.0f,500.0f);

	// set camera position and direction
	//gluLookAt(0, 20, 0, 0, 0, 50, 0, 1, 0); 

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

}

// Setup OpenGL global parameters
int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	printf("InitGL()\n");

	glShadeModel(GL_FLAT);								// FLAT / SMOOTH shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Background colour
	glClearDepth(1.0f);									// Depth Buffer Setup

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LESS);								// The Type Of Depth Testing To Do
	//glDepthRange(0.5, 200.0);
	glEnable(GL_COLOR_MATERIAL);						// Enable Material Coloring
	//glEnable(GL_POLYGON_SMOOTH);

	//glPolygonMode(GL_FRONT, GL_LINE);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	// lighting
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// set ambient light level
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// set light colour
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// set light position or direction
	glEnable(GL_LIGHT1);								// Enable Light One
	glEnable(GL_LIGHTING);								// Enable Lighting
	glEnable(GL_NORMALIZE);								// fix normals after scaling

	return TRUE;										// Initialization Went OK
}


// Draw a scaled character to the current GL context
void DrawVectorGlyph(char c, GLint x, GLint y, float scale)
{
	int i;

	// convert ascii value to our glyph data index 
	c -= 48;

	if(c > 63)
		return;

	// Setup translation and scaling of the glyph
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef((float)x, (float)y, 0.0f);
	glScalef(scale, scale, 1.0f);

	// output lines to GL
	VECTORGLYPH *glyph = &vecGlyph[c];
	int ls, le;
	glBegin(GL_LINES);
		for(i=0; i < glyph->numlines; i++) {
			ls = glyph->ls[i];
			le = glyph->le[i];
			glVertex3i(glyphPoints[ls].x, glyphPoints[ls].y, 0.0f); 
			glVertex3i(glyphPoints[le].x, glyphPoints[le].y, 0.0f); 
		} // next line
	glEnd();

}

// Draw a string of scaled vector characters to the current GL context
void DrawVectorText(char *string, GLint x, GLint y, float scale, int spacing)
{
	while ((x < VIEW_WIDTH) && *string) {
		if(*string > 47) DrawVectorGlyph(*string, x, y, scale);
		string++;
		x += ((scale * GLYPH_WIDTH) + spacing);
	}
}

// Draw the 3D scene
int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	// Tell E3D to render it's scene to the current GL context
	// TODO: blur effects, camera jiggle effects, strobe effects, etc
	Scene.Render(VIEW_WIDTH, VIEW_HEIGHT, true);


	return TRUE;						// Jump Back
}

// Setup the display for drawing 2D (eg: for 2D vector text)
void Setup2DOverlay(void)
{
    glViewport( 0, 0, VIEW_WIDTH, VIEW_HEIGHT );
    glScissor( 0, 0, VIEW_WIDTH, VIEW_HEIGHT );
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	gluOrtho2D(0, VIEW_WIDTH, VIEW_HEIGHT, 0);			// left, right, bottom, top


	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

// update the players score
void AddScore(unsigned int value)
{
	score += value;
	// TODO: if score > nextBonusScore, then award bonus / extra life etc
}

// add some fragments to the scene
void addFrags(Vector p, Vector v, int count) {
	int i, j;
	// add 10 fragments
	for(i=0; i<count; i++) {
		for(j = FRAG_ID; j <= FRAG_ENDID; j++) {
			E3D_Object *pObject = Scene.GetObject(j);
			if(0 == pObject->behaviour) {
				pObject->behaviour = BEHAVE_NORMAL;
				pObject->counter = 40 + (rand() % 20);
				pObject->SetVelocity((rand() % 20 - 10) / 100.0f, (rand() % 20 - 10) / 100.0f, 0.0); //(rand() % 6 - 3) / 100.0f;
				pObject->position = p + (pObject->velocity * 3.0f); //x = p.x + pObject->vx * 3.0f;
				//pObject->y = p.y + pObject->vy * 3.0f;
				//pObject->z = p.z + pObject->vz * 3.0f;
				pObject->SetRotation((rand() % 60) / 10.0f, (rand() % 60) / 10.0f, 0.0f);
				break; //j = FRAG_ENDID + 1;
			}
		}
	}
}

/// Make a 3D model of the specified tube
void makeTubeModel2(E3D_Model *model, TUBEDATA *tube, Colour colour)
{
	int i;
	model->Reset();

	// create vertices
	for(i=0; i<tube->numVertices; i++) {
		Vertex v;
		v.x = tube->vertex[i].x * STEP_SIZE;
		v.y = tube->vertex[i].y * STEP_SIZE;
		v.z = 0.0;
		model->AddVertex(v);
	}
	for(i=0; i<tube->numVertices; i++) {
		Vertex v;
		v.x = tube->vertex[i].x * STEP_SIZE;
		v.y = tube->vertex[i].y * STEP_SIZE;
		v.z = -TUBE_LENGTH;
		model->AddVertex(v);
	}
	// create lines
	for(i=0; i<tube->numVertices; i++) {
		int startVert = i;
		int endVert = i + tube->numVertices;
		Line *line = model->AddLine(startVert, endVert);
		line->colour = colour;
	}
	for(i=0; i<tube->numVertices-1; i++) {
		int startVert = i;
		int endVert = i+1;
		Line *line = model->AddLine(startVert, endVert);
		line->colour = colour;
		startVert = i + tube->numVertices;
		endVert = i + tube->numVertices + 1;
		line = model->AddLine(startVert, endVert);
		line->colour = colour;
	}

}

/// Get the centre and angle of a tube segment
/// @param[out] centre			Calculated centre point of the segment
/// @return						Angle of the segment
float GetTubeSegmentCentreAngle(TUBEDATA *tube, int segment, Vector &centre) { 
	Vector v1 = tube->vertex[segment];
	Vector v2 = tube->vertex[segment+1];
	centre.x = (v1.x+v2.x)*STEP_SIZE*0.5f;
	centre.y = (v1.y+v2.y)*STEP_SIZE*0.5f;
	centre.z = 0.0f;
	Vector baseLine(v2.x - v1.x, v2.y - v1.y, 0.0);
	float angle = (float)atan2(baseLine.y, baseLine.x);
	return angle;
}

void FireBullet(Vector &pos, Vector &velocity) {
	int i;
	E3D_Object *pObject = NULL;

	// find first free bullet and launch it
	for(i = BULLET_ID; i <= BULLET_ENDID; i++) {
		pObject = Scene.GetObject(i);
		if(0 == pObject->behaviour) {
			pObject->behaviour = BEHAVE_NORMAL;
			pObject->counter = 80;
			pObject->position = pos;
			pObject->velocity = velocity;
			break; //i = BULLET_ENDID + 1;
		}
	}
}

void LaunchEnemyBullet(Vector &pos, Vector &velocity) {
	int i;
	E3D_Object *pObject = NULL;

	// find first free enemy slot, create a bullet and launch it
	for(i = ENEMY_ID; i <= ENEMY_ENDID; i++) {
		pObject = Scene.GetObject(i);
		if(!pObject)
			continue;

		if(0 == pObject->behaviour) {
			pObject->modelNumber = EBULLET_MODEL;
			pObject->model = Scene.GetModel(EBULLET_MODEL);
			pObject->behaviour = BEHAVE_NORMAL;
			pObject->counter = 100;
			pObject->position = pos + Vector(0.0f, 0.0f, 2.0f); //SetPosition(pos.x, pos.y, pos.z + 2.0f);
			pObject->velocity = velocity;
			break; //i = ENEMY_ENDID + 1;
		}
	}
}

/// Activate the first "free" enemy object in the scene
E3D_Object *AddEnemy(int modelId) {
	int i;
	E3D_Object *pObject = NULL;

	// find first free enemy slot, create an enemy
	for(i = ENEMY_ID; i <= ENEMY_ENDID; i++) {
		pObject = Scene.GetObject(i);
		if(!pObject)
			continue;

		if(BEHAVE_NONE == pObject->behaviour) {
			pObject->modelNumber = modelId;
			pObject->model = Scene.GetModel(modelId);
			pObject->behaviour = BEHAVE_NORMAL;
			pObject->counter = 100;
			break; //i = ENEMY_ENDID + 1;
		}
	}

	return pObject;
}

/// Populate the 3D scene, given the level (stage)
int CreateScene(int level, TUBEDATA *tube) {
	// NB: we pre-fill all the "object slots" we are
	//     going to use so that we don't have to generate
	//     objects on the fly
	// NB: Order is objects is as per "object map" at the top of this file
	int i;
	char webFilename[2000];
	E3D_Object *pObject = NULL;

	Scene.RemoveAllObjects();

	// load the web shape
	GetWebFilename(level, webFilename);
	LoadWeb(webFilename, tube);

	// create the tube model
	E3D_Model *tubeModel = Scene.GetModel(TUBE_MODEL);
	Colour colour;
	colour.r = 0.0;
	colour.g = 0.0;
	colour.b = 1.0;
	makeTubeModel2(tubeModel, tube, colour);


	// The tube/web
	pObject = Scene.AddObject("tube");
	pObject->SetPosition(0.0f, 0.0f, 0.0f);
	pObject->behaviour = BEHAVE_NONE;

	// player 1 and 2
	pObject = Scene.AddObject("player1");				// player 1
	pObject->SetPosition(0.0f, 0.0f, 0.0f);
	pObject->behaviour = BEHAVE_NONE;
	pObject = Scene.AddObject("player2");				// player 2
	pObject->SetPosition(0.0f, 1000.0f, 0.0f);			// offscreen
	pObject->behaviour = BEHAVE_NONE;

	// create the bullet objects
	for(i = BULLET_ID; i <= BULLET_ENDID; i++) {
		pObject = Scene.AddObject("bullet1");
		pObject->SetPosition(0.0f, 1000.0f, 0.0f);				// offscreen
	}

	// create the frag objects
	for(i = FRAG_ID; i <= FRAG_ENDID; i++) {
		pObject = Scene.AddObject("frag");
		pObject->SetPosition(0.0f, 1000.0f, 0.0f);				// offscreen
	}


	// manufacture the enemies for this level
	int objIndex = ENEMY_ID;
	ENEMYSPAWNDATA *spawnData = &enemySpawnData[level];
	// Flippers
	for(i = 0; i < spawnData->numFlippers; i++) {
		// calculate enemy position and rotation
		int seg = rand() % tube->numVertices;
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -TUBE_LENGTH - (rand() % 400);

		pObject = Scene.AddObject("flipper");
		pObject->position = p; 
		pObject->SetRotation(0.0, 0.0, angle);
		pObject->var1 = (short)seg;		// current segment
		pObject->energy = 0;
		pObject->energymax = 50;		// time till flip
		pObject->counter = 200 + (rand() % 200);
		pObject->behaviour = BEHAVE_NORMAL;
	}

	// Tankers
	for(i = 0; i < spawnData->numTankers; i++) {
		// calculate enemy position and rotation
		int seg = rand() % tube->numVertices;
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -TUBE_LENGTH - (rand() % 400);

		pObject = Scene.AddObject("tanker");
		pObject->position = p; 
		pObject->SetRotation(0.0, 0.0, angle);
		pObject->var1 = (short)seg;
		pObject->counter = 0;
		pObject->behaviour = BEHAVE_NORMAL;
	}
	
	// Spikers
	for(i = 0; i < spawnData->numSpikers; i++) {
		// calculate enemy position and rotation
		int seg = rand() % tube->numVertices;
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -TUBE_LENGTH - (rand() % 400);

		pObject = Scene.AddObject("spike");
		p.z = -TUBE_LENGTH;
		pObject->position = p; 
		//pObject->SetRotation(0.0, 0.0, angle);
		pObject->counter = 0;
		pObject->behaviour = BEHAVE_NORMAL;
	}	

	// Fuseballs
	for(i = 0; i < spawnData->numFuseballs; i++) {
		// calculate enemy position and rotation
		int seg = rand() % tube->numVertices;
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -TUBE_LENGTH - (rand() % 400);

		pObject = Scene.AddObject("fuseball");
		p.z = -TUBE_LENGTH;
		pObject->position = p; 
		//pObject->SetRotation(0.0, 0.0, angle);
		pObject->counter = 0;
		pObject->behaviour = BEHAVE_NORMAL;
	}	
/*
	// Zappers
	for(i = 0; i < spawnData->numZappers; i++) {
		// calculate enemy position and rotation
		int seg = rand() % tube->numVertices;
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -TUBE_LENGTH - (rand() % 400);

		pObject = Scene.AddObject("fuseball");
		p.z = -TUBE_LENGTH;
		pObject->position = p; 
		//pObject->SetRotation(0.0, 0.0, angle);
		pObject->counter = 0;
		pObject->behaviour = BEHAVE_NORMAL;
	}	
*/
	// Barriers
	for(i = 0; i < spawnData->numBarriers; i++) {
		// calculate enemy position and rotation
		int seg = rand() % tube->numVertices;
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -TUBE_LENGTH - (rand() % 400);

		pObject = Scene.AddObject("barrier");
		pObject->position = p; 
		pObject->SetRotation(0.0, 0.0, angle);
		pObject->counter = 0;
		pObject->behaviour = BEHAVE_NORMAL;
	}	


	// make placeholder enemys
	for(i = 0; i < 20; i++) {
		pObject = Scene.AddObject("flipper");
		pObject->SetPosition(0.0f, 1000.0f, 0.0f); 
		pObject->counter = 0;
		pObject->behaviour = BEHAVE_NONE;
	}

	return i;
}

// SPlit tanker into 2 object (usually flippers)
void SplitTanker(E3D_Object *pObject, TUBEDATA *tube) {
	// split into 2 flippers
	Vector p;
	float angle = GetTubeSegmentCentreAngle(tube, pObject->var1, p);
	// 1. convert this tanker into an flipper
	//pObject->position.z = -200.0f + (rand() % 50);
	pObject->modelNumber = FLIPPER_MODEL;
	pObject->model = Scene.GetModel(FLIPPER_MODEL);
	pObject->rotation.z = angle;
	pObject->energy = 0;
	pObject->energymax = 50;
	pObject->counter = 200 + (rand()%200);
	// 2. add a new enemy object
	E3D_Object *newEnemy = AddEnemy(FLIPPER_MODEL);
	if(newEnemy) {
		newEnemy->rotation.z = angle;
		newEnemy->position = pObject->position;
		newEnemy->position.z -= 0.5;
		newEnemy->var1 = pObject->var1;				// segment
		newEnemy->energy = 0;
		newEnemy->energymax = 50;
		newEnemy->behaviour = BEHAVE_NORMAL;
		newEnemy->counter = 200 + (rand()%200);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Object behaviour engine
// This function updates all objects for 1 frame of action
/////////////////////////////////////////////////////////////////////////////////
void DoBehaviours(TUBEDATA *tube) {
	// Notes:
	// Behaviour 0 is object inactive (off)
	// Behaviour 1 is circle/wait for all enemies
	// Behaviour 2 is object's normal behaviour

	// TODO: spikers amd zappers and fuseball

	int i, j;
	E3D_Object *pObject = NULL;

	for(i = 1; i < Scene.GetNumObjects(); i++) {
		pObject = Scene.GetObject(i);
		if(!pObject)
			continue;

		switch(pObject->GetModelNumber()) {
/*
			case 3 : // spiders
				if(1 == pObject->behaviour) {
					pObject->ry += 0.1;
					pObject->z -= 0.2f;
					if(pObject->z < -250.0f) {
						pObject->z = 200.0f + rand() % 50;
					}
				}
				break;
*/
			case BULLET_MODEL : // player bullet
				if(BEHAVE_NORMAL == pObject->behaviour)	{	// active
					pObject->position += pObject->velocity;
					pObject->rotation.z += 0.2f;
					// check if this bullet has hit any flippers
					for(j = ENEMY_ID; j<Scene.GetNumObjects(); j++) {
						E3D_Object *pTarget = Scene.GetObject(j);
						if(fabs(pTarget->position.z - pObject->position.z) < HIT_RADIUS) {
							if(fabs(pTarget->position.x - pObject->position.x) < HIT_RADIUS) {
								if(fabs(pTarget->position.y - pObject->position.y) < HIT_RADIUS) {
									// hit!
									switch(pTarget->modelNumber) {
										case FLIPPER_MODEL :
											{
											Vector v(0.0f, 1.0f,0.0f);
											addFrags(pTarget->position, v, 8);
											pTarget->SetPosition(0.0f, 1000.0f, 0.0f);
											pTarget->behaviour = BEHAVE_NONE;		// switch off enemy
											pObject->counter = 1;		// expire bullet
											AddScore(FLIPPER_VALUE);
											}
											break;
										case TANKER_MODEL :
											{
											// split into 2 flippers
											// 1. create frags
											Vector v(0.0f, 1.0f,0.0f);
											addFrags(pTarget->position, v, 8);
											// 2. convert this tanker into 2 flippers
											SplitTanker(pTarget, tube);
											// 3. kill the bullet
											pObject->counter = 1;		// expire bullet
											AddScore(TANKER_VALUE);
											}
											break;
										case EBULLET_MODEL :
											{
											Vector v(0.0f, 1.0f,0.0f);
											addFrags(pTarget->position, v, 8);
											pTarget->SetPosition(0.0f, 1000.0f, 0.0f);
											pTarget->behaviour = BEHAVE_NONE;		// switch off enemy
											pObject->counter = 1;		// expire bullet
											}
											break;
										case SPIKE_MODEL :
											{
											Vector v(0.0f, 1.0f,0.0f);
											addFrags(pTarget->position, v, 5);
											// temporarily put spike into shrinking mode
											pTarget->behaviour = BEHAVE_ALT;
											pTarget->counter = 20;
											pObject->counter = 1;		// expire bullet
											AddScore(SPIKER_VALUE);
											}
											break;
										case BARRIER_MODEL :
											pObject->counter = 1;		// expire bullet
											break;
										//case UFO_MODEL :
										//	pObject->counter = 1;		// expire bullet
										//	break;
										//case ASTEROID_MODEL :
										//	pObject->counter = 1;		// expire bullet
										//	break;
									} // end switch
								}
							}
							
						}
					}
					if(pObject->counter) pObject->counter--;	// decrement counter
					if(!pObject->counter) {
						pObject->behaviour = BEHAVE_NONE;	// switch off if finished count
						pObject->SetPosition(0.0f, 1000.0f, 0.0f);
					}
				}
				break;
			case FRAG_MODEL : // fragment
				if(BEHAVE_NORMAL == pObject->behaviour) {
					pObject->position += pObject->velocity;
					pObject->rotation += (pObject->velocity * 0.5f);
					if(pObject->counter) pObject->counter--;	// decrement counter
					if(!pObject->counter) {
						pObject->behaviour = BEHAVE_NONE;	// switch off if finished count
						pObject->SetPosition(0.0f, 1000.0f, 0.0f);
					}
				}
				break;
			case FLIPPER_MODEL : // (flipper)
				// Level 0: flipper moves straight up
				// Level 1: flippers all flip clockwise
				// Level 2: flippers flip back and forth
				// Flipper needs aggression to increase as level increases:
				// 1. Faster
				// 2. Changes lane more often
				// 3. Less pause at top of web
				// 4. Fires more often
				if(BEHAVE_NORMAL == pObject->behaviour) {
					//pObject->rotation.z += 0.1f;
					pObject->position.z += 0.2f;
					if(pObject->position.z > 0.0f) {
						pObject->position.z = -200.0f + (rand() % 50);
					}
					// fire if counter is zero and enemy is not too far away nor too close
					if(!pObject->counter) {
						if(pObject->position.z > -(TUBE_LENGTH * 0.66)) {
							if(pObject->position.z < -(TUBE_LENGTH * 0.1)) {
								Vector pos = pObject->position;
								pos.z += 2.0f;
								Vector vel(0.0f, 0.0f, 1.0f);
								LaunchEnemyBullet(pos, vel);
							}
						}
						pObject->counter = 200 + (rand()%200);
					}
					pObject->counter--;	// decrement counter

					// do we need to flip?
					if(pObject->position.z > -TUBE_LENGTH)
						pObject->energy++;

					if(pObject->energy == pObject->energymax) {
						pObject->energy = 0;
						int direction = ((rand() % 10) > 4) ? 1 : -1;
						int newseg = pObject->var1 + direction;
						if(newseg < 0 || newseg > (tube->numVertices-2)) {
							// would flip off web, so forget about flip
							pObject->behaviour = BEHAVE_NORMAL;
						}
						else {
							pObject->var1 = newseg;		// var1 becomes "target segment"
							//Vector p;
							//float targetAngle = GetTubeSegmentCentreAngle(tube, newseg, p);
							pObject->turn = -1 * (float)direction * PI / 20.0; //2 * PI + pObject->rotation.z - targetAngle;
							pObject->behaviour = BEHAVE_ALT;		// put into flip mode
						}
					}
				}
				else if(BEHAVE_ALT == pObject->behaviour) {
					// flipping towards target segment
					pObject->position.z += 0.1f;
					pObject->rotation.z += pObject->turn;
					Vector p;
					float targetAngle = GetTubeSegmentCentreAngle(tube, pObject->var1, p);
					// if rotating clockwise, then moving "right" on web
					if(pObject->turn < 0.0) {
						// rotating towards the right
						// use left edge of target segment as pivot point
						Vector edgePt(p.x - (STEP_SIZE/2) * cos(targetAngle), p.y - (STEP_SIZE/2) * sin(targetAngle), 0.0);
						pObject->SetPosition(edgePt.x - (STEP_SIZE/2) * cos(pObject->rotation.z/2),
											edgePt.y - (STEP_SIZE/2) * sin(pObject->rotation.z/2),
											pObject->position.z);
						targetAngle -= PI;
					}
					// if rotating anticlockwise, then moving "left" on web
					else if(pObject->turn > 0.0) {
						// rotating towards the left
						// use right edge of target segment as pivot point
						Vector edgePt(p.x + (STEP_SIZE/2) * cos(targetAngle), p.y + (STEP_SIZE/2) * sin(targetAngle), 0.0);
						pObject->SetPosition(edgePt.x + (STEP_SIZE/2) * cos(pObject->rotation.z/2),
											edgePt.y + (STEP_SIZE/2) * sin(pObject->rotation.z/2),
											pObject->position.z);
						targetAngle += PI;
					}
					// check if finished
					
					if(fabs(pObject->rotation.z/2 - targetAngle) < (PI / 20.0)) {
						if(pObject->turn < 0.0)
							pObject->rotation.z = targetAngle + PI;
						else
							pObject->rotation.z = targetAngle - PI;
						pObject->SetPosition(p.x, p.y, pObject->position.z);
						pObject->behaviour = BEHAVE_NORMAL;
					}

				}
				break;
			case TANKER_MODEL : // (tanker)
				// Tanker moves straight up, then splits close to top of web
				// (Should make it pause for a sec or two before splitting)
				// Tanker needs aggression to increase as level increases:
				// 1. Faster
				// 2. Fires more often
				// 3. Enemies spawned are more advanced
				if(BEHAVE_NORMAL == pObject->behaviour) {
					pObject->rotation.z += 0.02f;
					pObject->position.z += 0.2f;
					// fire if counter is zero
					if(!pObject->counter) {
					//	Vector pos = pObject->position;
					//	pos.z += 2.0f;
					//	Vector vel(0.0f, 0.0f, 1.0f);
					//	LaunchEnemyBullet(pos, vel);
						pObject->counter = 200 + (rand()%200);
					}
					pObject->counter--;	// decrement counter

					// split into 2 flippers if close to this end of web
					if(pObject->position.z > -30.0f) {
						SplitTanker(pObject, tube);
					}
				}
				break;
			case EBULLET_MODEL : // (enemy bullet)
				if(BEHAVE_NORMAL == pObject->behaviour) {
					pObject->rotation.z += 0.1f;
					pObject->position += pObject->velocity;
					if(pObject->position.z > 5.0f) {
						// terminate
						pObject->SetPosition(0.0f, 1000.0f, 0.0f);
						pObject->behaviour = BEHAVE_NONE;
						pObject->counter = 0;
					}
				}
				break;
			case SPIKE_MODEL : // (spike)
				if(BEHAVE_NORMAL == pObject->behaviour) {
					// growing
					pObject->rotation.z += 0.1f;
					pObject->position.z += 0.2f;
					pObject->scale.z = pObject->position.z + TUBE_LENGTH;
					if(pObject->position.z > 5.0f) {
						// terminate
						pObject->SetPosition(0.0f, 1000.0f, 0.0f);
						pObject->behaviour = BEHAVE_NONE;
						pObject->counter = 0;
					}
				}
				else if(BEHAVE_ALT == pObject->behaviour) {
					// shrinking after a hit
					pObject->position.z -= 1.0f;
					pObject->scale.z = pObject->position.z + TUBE_LENGTH;
					// kill spike if shrunk right back
					if(pObject->scale.z <= 0.0f) {
						pObject->SetPosition(0.0f, 1000.0f, 0.0f);
						pObject->behaviour = BEHAVE_NONE;		// switch off enemy
					}
					else {
						pObject->counter--;
						// are we finished shrinking?
						if(!pObject->counter)
							pObject->behaviour = BEHAVE_NORMAL;	
					}
				}
				break;
			case BARRIER_MODEL : // (barrier)
				if(BEHAVE_NORMAL == pObject->behaviour) {
					pObject->position.z += 0.05f;
					if(pObject->position.z > 5.0f) {
						// terminate
						pObject->SetPosition(0.0f, 1000.0f, 0.0f);
						pObject->behaviour = BEHAVE_NONE;
						pObject->counter = 0;
					}
				}
				break;
			//case UFO_MODEL : // (ufo)
			//	if(BEHAVE_NORMAL == pObject->behaviour) {
			//		pObject->position.z += 0.05f;
			//		if(pObject->position.z > 5.0f) {
			//			// terminate
			//			pObject->SetPosition(0.0f, 1000.0f, 0.0f);
			//			pObject->behaviour = BEHAVE_NONE;
			//			pObject->counter = 0;
			//		}
			//	}
			//	break;
			//case ASTEROID_MODEL : // (asteroid)
			//	if(BEHAVE_NORMAL == pObject->behaviour) {
			//		pObject->position.z += 0.05f;
			//		if(pObject->position.z > 5.0f) {
			//			// terminate
			//			pObject->SetPosition(0.0f, 1000.0f, 0.0f);
			//			pObject->behaviour = BEHAVE_NONE;
			//			pObject->counter = 0;
			//		}
			//	}
			//	break;

		}

	}

}

// Level run state
enum RUNSTATE {
	RUNSTATE_RUNNING		= 0,
	RUNSTATE_QUIT			= 1,	// player quit level
	RUNSTATE_LEVELCOMPLETE	= 2,	// player has completed level
	RUNSTATE_PLAYERKILLED	= 3		// played was killed
};

/////////////////////////////////////////////////////////////////////////////////
// Main loop
/////////////////////////////////////////////////////////////////////////////////
int main(void) {
	//char stemp[256];
	unsigned int shotsTotal = 0;
	unsigned int shotsHit = 0;
	unsigned int reloadcount = 0;
	unsigned int debounce = 0;
	bool gameOver = false;
	unsigned int level = 0;

	int	frames;
	int x, y;
    double  t, t0, fps;
    char titlestr[500];
	int	scr_mode = GLFW_WINDOW;
	E3D_Object *pObject;
	TUBEDATA tube;


    // Initialise GLFW
    if(glfwInit() != GL_TRUE) return(1);

    // Open OpenGL window
    if( !glfwOpenWindow( VIEW_WIDTH, VIEW_HEIGHT, 0,0,0,0, 0,0, scr_mode ) )
    {
        glfwTerminate();
        return 0;
    }
    glfwSetWindowTitle( "E3DOO Win32OpenGL Example5" );

	glfwSetWindowSizeCallback( ResizeWindow );

	//printf("Refresh rate: %d \n", glfwGetWindowParam(GLFW_REFRESH_RATE));
	//printf("ext1: %d \n", glfwExtensionSupported("GL_ARB_multitexture"));

    // Enable sticky keys
    //glfwEnable( GLFW_STICKY_KEYS );

    // Disable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );

	// Set up our OpenGL env and build our display lists
	printf( "InitGL() returned %d\n", InitGL() );

    // Swap buffers
    glfwSwapBuffers();

	// set keyboard callback
	//glfwSetKeyCallback( KeyboardHandler );


	// Set up E3D
	Scene.Start();
	Scene.SetRenderDistance(400.0f);

	// Set up the scene's default light (light0)
	E3D_Light *light0 = Scene.GetLight(0);
	light0->SetColour( 0.8f, 0.8f, 0.6f );
	light0->SetAmbient( 0.4f, 0.4f, 0.4f );
	light0->SetDirection( 0.0, 0.0, -1.0); //0.707f, 0.707f, 0.0f );

	// Set up the scene's default camera
	E3D_Camera *camera = Scene.GetCamera(0);
	camera->SetPosition(0.0f, 1.5f, 0.0f);
	camera->SetDirection(0.0f, 0.0f, 1.0f);
	camera->SetUpVector(0.0f, 1.0f, 0.0f);
	camera->SetFOV(60.0f);
	Vector cameraDest;				// running camera destination

	// NB: Models must be added to the scene in the same order as the model map
	Scene.AddModel("tube");
	Scene.AddModel("player1", "models/player.obj");
	Scene.AddModel("player2", "models/player.obj");
	Scene.AddModel("bullet1", "models/bullet1.obj");
	Scene.AddModel("frag", "models/frag2.obj");
	Scene.AddModel("flipper", "models/flipper.obj");
	Scene.AddModel("tanker", "models/tanker.obj");
	Scene.AddModel("ebullet", "models/enemy_bullet.obj");
	Scene.AddModel("spike", "models/spike.obj");
	Scene.AddModel("spiker", "models/spider.obj");
	Scene.AddModel("barrier", "models/barrier.obj");

	// ********************* OUTER CONTROL LOOP ************************
	while(1)
		{
		// 1. Show title screen / hiscore until player start

		// 2. Game Play Loop
		while(!gameOver)
			{
			// Create the relevant level in E3D
			// NB: we pre-fill all the "object slots" we are
			//     going to use so that we don't have to generate
			//     objects on the fly
			CreateScene(level, &tube);

			float dx = 0.5f;
			float dy = 0.2f;
			float dz = 0.0f;

			// Main loop
			bool levelStart = true;
			float heading = 0.0;
			int currentSegment = tube.startingSegment;
			cameraDest.Set(0.0, 0.0, 20.0);
			RUNSTATE runState = RUNSTATE_RUNNING;
			frames = 0;
			t0 = glfwGetTime();
			double startTime = t0;
			//glfwDisable(GLFW_MOUSE_CURSOR);
			glfwPollEvents();
			while( RUNSTATE_RUNNING == runState )
				{
				// fps limiter
				while((glfwGetTime() - startTime) < 0.016) ;
				startTime = glfwGetTime();

				// Get time and mouse position
				t = glfwGetTime();
				glfwGetMousePos( &x, &y );

				// Calculate and display FPS (frames per second)
				if( (t-t0) > 1.0 || frames == 0 )
				{
					fps = (double)frames / (t-t0);
					sprintf( titlestr, 
						"E3DOO Win32OpenGL Example1 (%.1f FPS) %d objects", 
						fps, Scene.GetNumObjects());
					glfwSetWindowTitle( titlestr );
					t0 = t;
					frames = 0;
				}
				frames ++;

				// animate
				//dz += 0.02f;

				// Set player position and rotation
				pObject = Scene.GetObject(1);
				Vector p;
				float playerAngle = GetTubeSegmentCentreAngle(&tube, currentSegment, p); 
				pObject->position = p; 
				pObject->SetRotation(0.0f, 0.0f, playerAngle);
				
				//pObject->rz = dz;
				//pObject->x = (float)((VIEW_WIDTH/2) - x) / 40.0f;
				//pObject->y = (float)((VIEW_HEIGHT/2) - y) / 40.0f;

				DoBehaviours(&tube);

				// Draw our E3D scene
				DrawGLScene();

				// Overlay 2D text on our display
				Setup2DOverlay();
				char s[20];
				if(levelStart) {
					glColor3f(1.0f,1.0f,0.0f);
					sprintf(s, "LEVEL %02d", level);
					DrawVectorText(s, VIEW_WIDTH/2, VIEW_HEIGHT/2, 1.0f, 0.0f);
				}
				else {
					glColor3f(1.0f,1.0f,1.0f);
					DrawVectorText("HISCORE 000000", 20, 10, 1.0f, 0.0f);
					sprintf(s, "SCORE %06d", score);
					DrawVectorText(s, VIEW_WIDTH/2, 10, 1.0f, 0.0f);
				}

				// Swap buffers
				glfwSwapBuffers();

				// Pause at first frame of level to allow user to see level number
				// (And to prevent level skip key overpress)
				if(levelStart) {
					Sleep(2000);
					levelStart = false;
				}

				if(debounce)
					debounce--;

				if(GLFW_PRESS == glfwGetKey(GLFW_KEY_LEFT)) {
					//heading += 0.03;
					if(0 == debounce) {
						if(0 == currentSegment) {
							if(tube.closed)
								currentSegment = tube.numVertices-2;
						}
						else {
							currentSegment--;
						}
						debounce = 8;
					}
				}
				else if(GLFW_PRESS == glfwGetKey(GLFW_KEY_RIGHT)) {
					//heading -= 0.03;
					if(0 == debounce) {
						if(currentSegment == tube.numVertices-2) {
							if(tube.closed)
								currentSegment = 0;
						}
						else {
							currentSegment++;
						}
						debounce = 8;
					}
				}

				// camera "up" is perpendicular to current segment base line
				//Vector baseLine(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
				Vector depthLine(0.0, 0.0, 1.0);
				Vector normal((float)-sin(playerAngle), (float)cos(playerAngle), 0.0f);
				//E3D_Utility::cross_product_f(depthLine, baseLine, &normal);
				//normal.Normalise();
				// rotate camera up vector towards normal (or slerp it)
				Vector camUpVec = camera->GetUpVector();
				Vector upDiff = normal - camUpVec;
				camUpVec += (upDiff * 0.1f);
				camUpVec.Normalise();
				camera->SetUpVector(camUpVec);

				Vector dirn;
				dirn.x = 0.0; //sin(heading);
				dirn.z = -1.0; //cos(heading);
				dirn.y = 0.0;
				camera->SetDirection(dirn);

				// set camera final destination point
				pObject = Scene.GetObject(1);
				Vector playerPos = pObject->position;
				cameraDest.Set(playerPos.x*0.5f, playerPos.y * 0.5f, 20.0f);
				// float camera towards final destination point
				Vector camPos = camera->GetPosition();
				Vector diff = cameraDest - camPos;
				camPos += (diff * 0.1f);
				camera->SetPosition(camPos);

				// check fire button
				if(GLFW_PRESS == glfwGetKey(GLFW_KEY_SPACE)) {
					if(0 == reloadcount) {
						// fire bullet
						Vector pos = Scene.GetObject(PLAYER1_ID)->position;
						pos.z -= 4.0f; //(Scene.GetObject(PLAYER1_ID)->x, Scene.GetObject(PLAYER1_ID)->y, Scene.GetObject(PLAYER1_ID)->z - 4.0f);
						Vector vel(0.0, 0.0, -3.0);
						FireBullet(pos, vel);
						reloadcount = RELOAD_COUNT;
					}
				}

				// check for level skip
				if(GLFW_PRESS == glfwGetKey(GLFW_KEY_PAGEUP))
					runState = RUNSTATE_LEVELCOMPLETE;

				// Check if the ESC key was pressed
				if(GLFW_PRESS == glfwGetKey( GLFW_KEY_ESC ))
					runState = RUNSTATE_QUIT;

				// Check if the window was closed
				if(!glfwGetWindowParam( GLFW_OPENED ))
					{
					// Close OpenGL window and terminate GLFW
					glfwEnable(GLFW_MOUSE_CURSOR);
					glfwTerminate();
					return 0;					// Exit The Program
					}

				if(reloadcount) reloadcount--;
				} // wend running
			
			// check for reason play loop is finished
			switch(runState)
				{
				case RUNSTATE_QUIT :
					// Close OpenGL window and terminate GLFW
					glfwEnable(GLFW_MOUSE_CURSOR);
					glfwTerminate();
					return 0;									// Exit The Program
					break;
				case RUNSTATE_LEVELCOMPLETE :
					level++;
					printf("Level up: %d\n", level);
					break;
				case RUNSTATE_PLAYERKILLED :
					break;
				}
			} // wend !gameOver

		} // wend (1)

    // Close OpenGL window and terminate GLFW
	glfwEnable(GLFW_MOUSE_CURSOR);
    glfwTerminate();
	return 0;									// Exit The Program
}

