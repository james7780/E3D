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
void E3D_Camera::SetPosition(Vector p)
{
	m_pos = p;
}

/// Get the 3D position of the camera "eye" 
Vector E3D_Camera::GetPosition(void)
{
	return m_pos;
}

/// Set the direction that the camera "eye" is facing 
void E3D_Camera::SetDirection(Vector dirn)
{
	dirn.Normalise();
	m_dirn = dirn;
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
void E3D_Camera::SetUpVector(Vector up)
{
	up.Normalise();
	m_up = up;
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
void E3D_Camera::LookAt(Vector target)
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
void E3D_Camera::SetCamera(Vector posn, Vector dirn, Vector up, float fov)
{
	SetPosition(posn);
	SetDirection(dirn);
	SetUpVector(up);
	SetFOV(fov);
}


