//////////////////////////////////////////////////////////////////////
//
// E3D_Camera.cpp: implementation of the E3D_Camera class.
//
// Copyright 2006 James Higgs
// 
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "../include/E3D.h"

// Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

E3D_Camera::E3D_Camera()
{
	// set to a "sane" view
	SetPosition(0.0f, 10.0f, -100.0f);
	SetDirection(0.0f, 0.0f, 1.0f);
	SetUpVector(0.0f, 1.0f, 0.0f);
	SetRoll(0.0f);
	SetFOV(60.0f);
}


E3D_Camera::~E3D_Camera()
{

}

/// Set the 3D position of the camera "eye" 
void E3D_Camera::SetPosition(float x1, float y1, float z1)
{
	m_pos.x = x1;
	m_pos.y = y1;
	m_pos.z = z1;
}

/// Set the 3D position of the camera "eye" 
void E3D_Camera::SetPosition(const Vector &p)
{
	m_pos = p;
}

/// Get the 3D position of the camera "eye" 
Vector E3D_Camera::GetPosition(void)
{
	return m_pos;
}

/// Set the direction that the camera "eye" is facing 
void E3D_Camera::SetDirection(const Vector &dirn)
{
	m_dirn = dirn;
	m_dirn.Normalise();
}

/// Set the direction that the camera "eye" is facing 
void E3D_Camera::SetDirection(float x1, float y1, float z1)
{
	Vector dirn(x1, y1, z1);
	SetDirection(dirn);
}

/// Get the direction that the camera "eye" is facing 
Vector E3D_Camera::GetDirection(void)
{
	return m_dirn;
}

/// Set the camera "up" vector 
void E3D_Camera::SetUpVector(const Vector &up)
{
	m_up = up;
	m_up.Normalise();
}

/// Set the camera "up" vector 
void E3D_Camera::SetUpVector(float x1, float y1, float z1)
{
	Vector up(x1, y1, z1);
	SetUpVector(up);
}

/// Get the camera "up" vector 
Vector E3D_Camera::GetUpVector(void)
{
	return m_up;
}

/// Set the camera to look at the specified 3D point
/// (Note: does not affect the position of the camera "eye") 
void E3D_Camera::LookAt(float x1, float y1, float z1)
{
	Vector dirn(x1 - m_pos.x, y1 - m_pos.y, z1 - m_pos.z);
	SetDirection(dirn);
}

/// Set the camera to look at the specified 3D point
/// (Note: does not affect the position of the camera "eye") 
void E3D_Camera::LookAt(const Vector &target)
{
	Vector dirn(target.x - m_pos.x, target.y - m_pos.y, target.z - m_pos.z);
	SetDirection(dirn);
}

/// Set the camera "roll" (ie: twist angle about the view direction vector)
/// @param r1	Roll angle in radians
void E3D_Camera::SetRoll(float r1)
{
	m_roll = r1;
}

/// Set the camera "roll" (ie: twist angle about the view direction vector)
/// @return 	Roll angle in radians
float E3D_Camera::GetRoll(void)
{
	return m_roll;
}

/// Set the camera "field of view" (in degrees)
/// @param fov	Camera's field of view in degrees (camera defaults to 60 degrees)
void E3D_Camera::SetFOV(float fov)
{
	m_FOV = fov;
}

/// @return 	Camera's field of view in degrees
float E3D_Camera::GetFOV(void)
{
	return m_FOV;
};

/*
/// Set window width
void E3D_Camera::SetViewWidth(float width)
{
	m_viewWidth = width;
}

float E3D_Camera::GetViewWidth(void)
{
	return m_viewWidth;
}

/// Set window heigth
void E3D_Camera::SetViewHeight(float height)
{
	m_viewHeight = height;
}

float E3D_Camera::GetViewHeight(void)
{
	return m_viewHeight;
}

/// Get the aspect ratio of the view window
float E3D_Camera::GetAspectRatio(void)
{
	if(m_viewHeight > 0.0f) 
		return (m_viewWidth / m_viewHeight);
	else
		return 1.0f;
}
*/
/// Camera composite set-up function
void E3D_Camera::SetCamera(const Vector &posn, const Vector &dirn, const Vector &up, float fov)
{
	SetPosition(posn);
	SetDirection(dirn);
	SetUpVector(up);
	SetFOV(fov);
}

/// Get "look at" camera matrix (for OpenGL)
/// Right-hand coord system, column-major matrix
/// @param mx					Pointer to matrix buffer (16 floats)
void E3D_Camera::GetLookAtMx(float *mx)
{
	//vec3 zaxis = normal(eye - target);    // The "forward" vector.
	Vector W = -m_dirn;    // The "forward" vector.
	W.Normalise();
	//Vector xaxis = normal(cross(up, zaxis));// The "right" vector.
	Vector U = m_up.CrossProduct(W);// The "right" vector.
	U.Normalise();
	//vec3 yaxis = cross(zaxis, xaxis);     // The "up" vector.
	Vector V = W.CrossProduct(U);     // The "up" vector.

	// Create a 4x4 view matrix from the right, up, forward and eye position vectors
/*
	mat4 viewMatrix = {
			vec4(      xaxis.x,            yaxis.x,            zaxis.x,       0 ),
			vec4(      xaxis.y,            yaxis.y,            zaxis.y,       0 ),
			vec4(      xaxis.z,            yaxis.z,            zaxis.z,       0 ),
			vec4(-dot( xaxis, eye ), -dot( yaxis, eye ), -dot( zaxis, eye ),  1 )
	};
	 
	return viewMatrix;
*/
	mx[0] = U.x;
	mx[1] = V.x;
	mx[2] = W.x;
	mx[3] = 0.0f;

	mx[4] = U.y;
	mx[5] = V.y;
	mx[6] = W.y;
	mx[7] = 0.0f;

	mx[8] = U.z;
	mx[9] = V.z;
	mx[10] = W.z;
	mx[11] = 0.0f;

	mx[12] = -U.DotProduct(m_pos);
	mx[13] = -V.DotProduct(m_pos);
	mx[14] = -W.DotProduct(m_pos);
	mx[15] = 1.0f;
}


/*  another IMPLMENTATION

#include "gluLookAt.h"

void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
          GLfloat centerx, GLfloat centery, GLfloat centerz,
          GLfloat upx, GLfloat upy, GLfloat upz)
{
    GLfloat m[16];
    GLfloat x[3], y[3], z[3];
    GLfloat mag;
    
    // Make rotation matrix 
    
    // Z vector 
    z[0] = eyex - centerx;
    z[1] = eyey - centery;
    z[2] = eyez - centerz;
    mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
    if (mag) {   
        z[0] /= mag;
        z[1] /= mag;
        z[2] /= mag;
    }
    
    // Y vector 
    y[0] = upx;
    y[1] = upy;
    y[2] = upz;
    
    // X vector = Y cross Z 
    x[0] = y[1] * z[2] - y[2] * z[1];
    x[1] = -y[0] * z[2] + y[2] * z[0];
    x[2] = y[0] * z[1] - y[1] * z[0];
    
    // Recompute Y = Z cross X
    y[0] = z[1] * x[2] - z[2] * x[1];
    y[1] = -z[0] * x[2] + z[2] * x[0];
    y[2] = z[0] * x[1] - z[1] * x[0];
    
    // mpichler, 19950515 
    // cross product gives area of parallelogram, which is < 1.0 for
    // non-perpendicular unit-length vectors; so normalize x, y here
    //
    
    mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    if (mag) {
        x[0] /= mag;
        x[1] /= mag;
        x[2] /= mag;
    }
    
    mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
    if (mag) {
        y[0] /= mag;
        y[1] /= mag;
        y[2] /= mag;
    }
    
#define M(row,col)  m[col*4+row]
    M(0, 0) = x[0];
    M(0, 1) = x[1];
    M(0, 2) = x[2];
    M(0, 3) = 0.0;
    M(1, 0) = y[0];
    M(1, 1) = y[1];
    M(1, 2) = y[2];
    M(1, 3) = 0.0;
    M(2, 0) = z[0];
    M(2, 1) = z[1];
    M(2, 2) = z[2];
    M(2, 3) = 0.0;
    M(3, 0) = 0.0;
    M(3, 1) = 0.0;
    M(3, 2) = 0.0;
    M(3, 3) = 1.0;
#undef M
    glMultMatrixf(m);
    
    // Translate Eye to Origin
    glTranslatef(-eyex, -eyey, -eyez);
    
}
*/