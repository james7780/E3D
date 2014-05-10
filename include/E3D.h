//////////////////////////////////////////////////////////////////////
//
// e3d.h - header file for E3D 3D Scene library
//
// changes 30/7/2006:
//   #include other E3D headers at the bottom of this file
//
// changes 10/6/1999:
//   updated poly defn to handle include a single colour for the polygon,
//   and colour values for poly verts (more compatible).
//   also, poly type defn (refers to Allegro poly types :)
//
//////////////////////////////////////////////////////////////////////

#include <math.h>

#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

#ifdef _LINUX
#define stricmp	strcasecmp
#endif

//#include <list>
#include <vector>

// Global debug output flag (defined in E3D_Utility.cpp, initially set to 0 (off))
extern int E3D_debug;

// E3D namespace
namespace E3D {

#define E3D_VERSION		19
#define E3D_MODEL_VERSION	3


//#define LIGHT_AMBIENT	1
//#define LIGHT_DISTANT	2
//#define LIGHT_OMNI		3

#define MAX_VERTS_PER_MODEL	2000
#define MAX_LINES_PER_MODEL	2000
#define MAX_POLYS_PER_MODEL	2000
#define MAX_MODELS_PER_SCENE	2000
#define MAX_OBJECTS_PER_SCENE	2000
#define MAX_LIGHTS_PER_SCENE	10
#define MAX_CAMERAS_PER_SCENE	10

// for defining poly types for rendering compatibility later :)
// (stolen from allegro.h)
#define POLYTYPE_FLAT               0
#define POLYTYPE_GCOL               1
#define POLYTYPE_GRGB               2
#define POLYTYPE_ATEX               3
#define POLYTYPE_PTEX               4
#define POLYTYPE_ATEX_MASK          5
#define POLYTYPE_PTEX_MASK          6
#define POLYTYPE_ATEX_LIT           7
#define POLYTYPE_PTEX_LIT           8
#define POLYTYPE_ATEX_MASK_LIT      9
#define POLYTYPE_PTEX_MASK_LIT      10

// flags
#define FLAGS_SURFPOLY				256

// degrees to radians conversion
#define DEG2RAD(a) (a*0.0174532925f)
#define RAD2DEG(a) (a*57.295779513f)

// 3D vector or coordinate
class Vector
{
public:
	float x, y, z;

	Vector() { x = 0.0f; y = 0.0f; z = 0.0f; };
	Vector(float x0, float y0, float z0) { x = x0; y = y0; z = z0; };

	void Set(float x0, float y0, float z0) { x = x0; y = y0; z = z0; };
	float GetLength(void) { return (float)sqrt(x*x + y*y + z*z); };
	void Normalise(void) {
		float scale = 1.0f / GetLength();
		x *= scale;
		y *= scale;
		z *= scale;
	}
	void SetLength(float d) {
		float ratio = d / GetLength();
		x *= ratio;
		y *= ratio;
		z *= ratio;
	};

	Vector operator-() const { return Vector(-x, -y, -z);   }

	Vector& operator=(const Vector& other)	{ x = other.x; y = other.y; z = other.z; return *this; }

	Vector operator+(const Vector& other) const { return Vector(x + other.x, y + other.y, z + other.z);	}
	Vector& operator+=(const Vector& other)	{ x+=other.x; y+=other.y; z+=other.z; return *this; }

	Vector operator-(const Vector& other) const { return Vector(x - other.x, y - other.y, z - other.z);	}
	Vector& operator-=(const Vector& other)	{ x-=other.x; y-=other.y; z-=other.z; return *this; }

	Vector operator*(const float v) const { return Vector(x * v, y * v, z * v);	}
	Vector& operator*=(const float v) { x*=v; y*=v; z*=v; return *this; }

};

typedef struct {
	float x, y;
} Point;

/*
class E3D_RGB {
	float r, g, b;
	E3D_RGB() { r = 1.0; g = 1.0; b = 1.0; );
	E3D_RGB(float r1, float g1, float b1) { r = r1; g = g1; b = b1; };
};
*/

//typedef struct {
//	float r, g, b;
//} Colour;

class Colour
{
public:
	float r, g, b;

	Colour() { r = 0.5f; g = 0.5f; b = 0.5f; };
	Colour(float r0, float g0, float b0) { r = r0; g = g0; b = b0; };

	void Set(float r0, float g0, float b0) { r = r0; g = g0; b = b0; };
};

//typedef struct {
//	float x, y, z;
//	float r, g, b;
//    int group[5];
//} Vertex;

class Vertex
{
public:
	float x, y, z;
	float r, g, b;
    int group[5];

	Vertex() { x = 0.0f; y = 0.0f; z = 0.0f; r = 0.5f; g = 0.5f; b = 0.5f; };
	Vertex(float x0, float y0, float z0) { x = x0; y = y0; z = z0; r = 0.5f; g = 0.5f; b = 0.5f; };

	void SetPosition(float x0, float y0, float z0) { x = x0; y = y0; z = z0; };
	void SetColour(float r0, float g0, float b0) { r = r0; g = g0; b = b0; };
};

typedef struct {
    Colour colour;						// Line colour
	unsigned int vert1;
	unsigned int vert2;
    int group;
} Line;

// Face on a 3D model
typedef struct {
	short type, numverts;
	short p[6];							// vertex indices
	short n[6];							// normal indices
    Colour colour;						// polygon colour
    Colour vcol[6];						// vertex colours
    short shade;		// 0 = no shade, 1 = shade
    short gouraud;		// 0 = flat, 1 = gouraud
    short filling;		// 0 = solid colour, 1 = colour grad, 2 = texture
	short texmap;		// 1+ = texmap index
    Point texp[6];
	float dist;
    int group[5];
} Face;

typedef struct {
    char name[20];
} Group;

/*
typedef struct {
	short numverts;
	Vertex_f vert[12];
    Pt_f texp[12];
} ClippedPgon;
 
typedef struct {
	Vertex_f pnt[12];
	short numverts;
    Pt_f texp[12];
	float dist;
} MappedPolygon;

typedef struct {
	Vertex_f pnt[12];
	Pt_f texp[12];
} ClippedMappedPolygon;

typedef struct {
	float x, y, z;
	float r, g, b;
	short type;
} ThreeDeeLight;

typedef struct {
	Vector posn;
	Vector dirn;
	Vector up;
	float viewangle;
} ThreeDeeCamera;

typedef struct {
	char name[100];
	short numverts, numnorms, numlines, numpolys;
	Vertex_f vert[MAX_VERTS_PER_MODEL];
	Vector norm[MAX_VERTS_PER_MODEL];
	Pgon poly[MAX_POLYS_PER_MODEL];
	short shade;							// flag for shading
	float maxdim;							// max dimension for inst clip
    GroupType group[40];
    int numgroups;
} ThreeDeeModel;

typedef struct {
	short model, attach;
	short shade, active, visible, clipped;
	short preshade;
	E3D_RGB pscolour[30];						// inst preshade flag and preshaded poly colours
	Vector posn;
	Vector rotn;
	Vector scal;
	Vector dirn;							// dirn of motion
	float dist, maxdist;
	float speed, maxspeed, accel;			// speed of motion
	float turn, maxturn;					// manouvreability
	short energy;
	short behaviour, counter;				// behaviour stuff
    short nextbehaviour, nextcounter;		// behaviour stuff
	float tx, ty, tz;						// behaviour stuff
	short effect;							// special effect for instance
} ThreeDeeInstance;

typedef struct {
	int numlights;
	int nummodels;
	int numinsts;
	int numtexs;
	char texname[50][15];
} ThreeDeeScene;

typedef struct {			// need "ShowZones" routine
	int x1;					// and "GetZone" routine
	int y1;
	int x2;
	int y2;
	int active;				// may be used later
} MouseZone;

typedef struct {			// button definition
	int x, y;				// screen x and y posn
	int w, h;				// width and height of button
	int state;				// TRUE for down, FALSE for up
	int active;				// TRUE or FALSE
	int spr_on;				// sprite no index in datafile for "on" state
	int spr_off;			// ditto for "off" condition
	char text[40];
} GUIButton;

typedef struct {			// edit window
	int view;				// current view type - top, front or side
	float zoom;				// zoom factor
	float pan_x, pan_y;	// offset of window centre
	int grid;				// grid on/off
} EditWindow;

typedef struct {
	int r, g, b;
} EditRGB;

// *** for TIM's
typedef struct {
	int r;
    int g;
    int b;
    unsigned char alpha;
} RGB_Entry;

typedef struct {
	char fname[80];					// TIM file name
    int x;							// texture origin x (mult. of 64)
    int y;							// tex origin y (0 or 256)
    int w;							// tex width
    int h;							// tex height
    int cx;							// clut origin x (mult. of 16)
    int cy;							// clut origin y
    int mode;						// clut mode (4/8/16bit)
    int ABR;						// semitransparency mode
} PSX_Texture;
*/

enum E3D_Result {
	E3D_SUCCESS = 0,
	E3D_FAIL = 1
};

#include "E3D_Utility.h"
#include "E3D_Model.h"
#include "E3D_Object.h"
#include "E3D_Light.h"
#include "E3D_Camera.h"
#include "E3D_Scene.h"
#include "E3D_PlatformSpecific.h"

} // end namespace

