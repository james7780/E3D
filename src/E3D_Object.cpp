//////////////////////////////////////////////////////////////////////
//
// E3D_Object.cpp: a class that represents a 3D object
//                 (ie: an instance of a model)
//
// Copyright 2006 James Higgs
// 
//////////////////////////////////////////////////////////////////////
#include "stdlib.h"					// for #define NULL
#include "../include/E3D.h"

// Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

E3D_Object::E3D_Object()
{
	SetPosition(0.0f, 0.0f, 0.0f);
	SetScale(1.0f, 1.0f, 1.0f);
	SetRotation(0.0f, 0.0f, 0.0f);
	SetVelocity(0.0f, 0.0f, 0.0f);
	modelNumber = 0;
	model = NULL;
	parent = NULL;

	shade = 0;
	active = 0;
	visible = 1;
	clipped = 0;
	preshade = 0;
	flags = 0;

	behaviour = 0;
	counter = 0;

	dist = 0.0f;
	maxdist = 0.0f;
	speed = 0.0f;
	maxspeed = 0.0f;
	accel = 0.0f;

	turn = 0.0f;
	maxturn = 0.0f;

	energy = 0;
	energymax = 0;
	nextbehaviour = 0;
	nextcounter = 0;

	var1 = 0;
	var2 = 0;
	var3 = 0;					// behaviour stuff

	tx = ty = tz = 0.0f;
	effect = 0;							// special effect for instance

}

E3D_Object::E3D_Object(const E3D_Object& lhs)
{
	position = lhs.position;
	scale = lhs.scale;
	rotation = lhs.rotation;
	velocity = lhs.velocity;
	modelNumber = lhs.modelNumber;
	model = lhs.model;
	parent = lhs.parent;

	shade = lhs.shade;
	active = lhs.active;
	visible = lhs.visible;
	clipped = lhs.clipped;
	preshade = lhs.preshade;
	flags = lhs.flags;

	behaviour = lhs.behaviour;
	counter = lhs.counter;

	dist = lhs.dist;
	maxdist = lhs.maxdist;
	speed = lhs.speed;
	maxspeed = lhs.maxspeed;
	accel = lhs.accel;

	turn = lhs.turn;
	maxturn = lhs.maxturn;

	energy = lhs.energy;
	energymax = lhs.energymax;
	nextbehaviour = lhs.nextbehaviour;
	nextcounter = lhs.nextcounter;

	var1 = lhs.var1;
	var2 = lhs.var2;
	var3 = lhs.var3;					// behaviour stuff

	tx = lhs.tx;
	ty = lhs.ty;
	tz = lhs.tz;
	effect = lhs.effect;							// special effect for instance
}

E3D_Object &E3D_Object::operator=(const E3D_Object &rhs)
{
	if (&rhs != this)
		{
		position = rhs.position;
		scale = rhs.scale;
		rotation = rhs.rotation;
		velocity = rhs.velocity;
		modelNumber = rhs.modelNumber;
		model = rhs.model;
		parent = rhs.parent;

		shade = rhs.shade;
		active = rhs.active;
		visible = rhs.visible;
		clipped = rhs.clipped;
		preshade = rhs.preshade;
		flags = rhs.flags;

		behaviour = rhs.behaviour;
		counter = rhs.counter;

		dist = rhs.dist;
		maxdist = rhs.maxdist;
		speed = rhs.speed;
		maxspeed = rhs.maxspeed;
		accel = rhs.accel;

		turn = rhs.turn;
		maxturn = rhs.maxturn;

		energy = rhs.energy;
		energymax = rhs.energymax;
		nextbehaviour = rhs.nextbehaviour;
		nextcounter = rhs.nextcounter;

		var1 = rhs.var1;
		var2 = rhs.var2;
		var3 = rhs.var3;					// behaviour stuff

		tx = rhs.tx;
		ty = rhs.ty;
		tz = rhs.tz;
		effect = rhs.effect;							// special effect for instance
		}

	return *this;
}

E3D_Object::~E3D_Object()
{

}

/// Set this object's 3D position
void E3D_Object::SetPosition(float x1, float y1, float z1)
{
	position.x = x1;
	position.y = y1;
	position.z = z1;
}

/// Set this object's velocity
void E3D_Object::SetVelocity(float dx, float dy, float dz)
{
	velocity.x = dx;
	velocity.y = dy;
	velocity.z = dz;
}

/// Scale this object relative to it's model centre
void E3D_Object::SetScale(float sx1, float sy1, float sz1)
{
	scale.x = sx1;
	scale.y = sy1;
	scale.z = sz1;
}

/// Rotate this object about it's model centre
void E3D_Object::SetRotation(float rx1, float ry1, float rz1)
{
	rotation.x = rx1;
	rotation.y = ry1;
	rotation.z = rz1;
}

/// Get XYZ measurement of this object
Vector E3D_Object::GetExtents()
{
	// TODO: calculate size of this object
	return Vector(0.0f, 0.0f, 0.0f);
}

