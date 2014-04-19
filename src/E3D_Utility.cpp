//////////////////////////////////////////////////////////////////////
//
// E3D_Utility.cpp: implementation of the E3D_Utility class.
//
// James Higgs 2002
//
//////////////////////////////////////////////////////////////////////

// Note: This is a "static" class!!!

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../include/E3D.h"

// Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

E3D_Utility::E3D_Utility()
{

}

E3D_Utility::~E3D_Utility()
{

}


/// Calculate the length of a 3D vector
float E3D_Utility::vector_length_f(float x, float y, float z)
{
   return (float)sqrt(x*x + y*y + z*z);
}


/// Normalise a 3D vector (ie: make it length = 1.0)
void E3D_Utility::normalize_Vector(float *x, float *y, float *z)
{
   float length = 1.0f / vector_length_f(*x, *y, *z);

   *x *= length;
   *y *= length;
   *z *= length;
}

/// Calculate cross product
void E3D_Utility::cross_product_f(float x1, float y1, float z1, float x2, float y2, float z2, float *xout, float *yout, float *zout)
{
    *xout = (y1 * z2) - (z1 * y2);
    *yout = (z1 * x2) - (x1 * z2);
    *zout = (x1 * y2) - (y1 * x2);
}

/// Calculate cross product
void E3D_Utility::cross_product_f(Vector v1, Vector v2, Vector *vout)
{
    vout->x = (v1.y * v2.z) - (v1.z * v2.y);
    vout->y = (v1.z * v2.x) - (v1.x * v2.z);
    vout->z = (v1.x * v2.y) - (v1.y * v2.x);
}

/// Calculate Z component of a polygon's normal vector
float E3D_Utility::polygon_z_normal_f(Vector *v1, Vector *v2, Vector *v3)
{
   return ((v2->x-v1->x) * (v3->y-v2->y)) - ((v3->x-v2->x) * (v2->y-v1->y));
}

/// Replace the file name in a file path
/// NB: path must be dilimited by '/' (not '\')
/// NB: Does not do any safety checks
void E3D_Utility::ReplaceFilename(char *path, const char *newFilename)
{
	// 1. Find last '/' in path, and replace staring after it
	char *p = path + (strlen(path) - 1);
	while(p > path) {
		if(*p == '/') {
			strcpy(++p, newFilename);
			return;
		}
		p--;
	}

	// if no '/' found, then replace the whole string
	strcpy(path, newFilename);
}

