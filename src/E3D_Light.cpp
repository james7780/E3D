//////////////////////////////////////////////////////////////////////
//
// E3D_Light.cpp: implementation of the E3D_Light class.
//
// Copyright 2006 James Higgs
// 
//////////////////////////////////////////////////////////////////////

#include "../include/E3D.h"

// Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

E3D_Light::E3D_Light()
{
	m_type = E3D_Light::DISTANT;
	SetColour(1.0f, 1.0f, 1.0f);
	SetAmbient(0.2f, 0.2f, 0.2f);
}

E3D_Light::~E3D_Light()
{

}

void E3D_Light::SetType(int type)
{
	m_type = type;
}

int E3D_Light::GetType(void)
{
	return m_type;
}

/// Set the XYZ coordinate of this light
void E3D_Light::MoveTo(float x1, float y1, float z1)
{
	x = x1;
	y = y1;
	z = z1;
}

void E3D_Light::MoveTo(Vector posn)
{
	x = posn.x;
	y = posn.y;
	z = posn.z;
}

/*
/// Set the XYZ coordinate to point this light at
void E3D_Light::PointAt()
{

}
*/

/// Set the direction of this light
void E3D_Light::SetDirection(float dx, float dy, float dz)
{
	x = dx;
	y = dy;
	z = dz;
}

/// Set the direction of this light
void E3D_Light::SetDirection(Vector dirn)
{
	x = dirn.x;
	y = dirn.y;
	z = dirn.z;
}

/// Get the direction fo the light
Vector E3D_Light::GetDirection(void)
{
	Vector dirn(x, y, z);
	return dirn;
}

/// Set the colour of this light
void E3D_Light::SetColour(float r, float g, float b)
{
	m_colour.r = r;
	m_colour.g = g;
	m_colour.b = b;
}

void E3D_Light::SetColour(Colour col)
{
	m_colour = col;
}

/// Set the colour of this light (alt)
void E3D_Light::SetColor(float r, float g, float b)
{
	SetColour(r, g, b);
}

void E3D_Light::SetColor(Colour col)
{
	SetColour(col);
}


Colour E3D_Light::GetColour(void)
{
	return m_colour;
}

Colour E3D_Light::GetColor(void)
{
	return m_colour;
}


/// Set the ambient colour component of this light
void E3D_Light::SetAmbient(float r, float g, float b)
{
	m_ambient.r = r;
	m_ambient.g = g;
	m_ambient.b = b;
}

void E3D_Light::SetAmbient(Colour col)
{
	SetAmbient(col.r, col.g, col.b);
}

/// Get the ambient colour component
Colour E3D_Light::GetAmbient(void)
{
	return m_ambient;
}
