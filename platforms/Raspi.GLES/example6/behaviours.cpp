// Object behaviours for Tempest
// JH 2007

// NB: Include our E3D header file
#include "../../../include/E3D.h"
#include <stdlib.h>
// NB: Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

#include "webfest.h"

// in tempest.cpp
extern float GetTubeSegmentCentreAngle(TUBEDATA *tube, int segment, Vector &centre);
extern unsigned int g_shotsHit;

// add some fragments to the scene
void AddFrags(Vector p, Vector v, int count) {
	// add 10 fragments
	OBJLIST* objList = Scene.GetObjectList(FRAG_LIST);
	for(int i=0; i<count; i++)
		{
		OBJLIST::iterator iter;
		for(iter = objList->begin(); iter != objList->end(); iter++)
			{
			if(0 == iter->behaviour)
				{
				iter->behaviour = BEHAVE_NORMAL;
				iter->counter = 40 + (rand() % 20);
				iter->SetVelocity((rand() % 20 - 10) / 60.0f, (rand() % 20 - 10) / 60.0f, 0.0); //(rand() % 6 - 3) / 100.0f;
				iter->position = p + (iter->velocity * 3.0f); //x = p.x + pObject->vx * 3.0f;
				iter->SetRotation((rand() % 60) / 10.0f, (rand() % 60) / 10.0f, 0.0f);
				break; //j = FRAG_ENDID + 1;
				}
			}
		}
}

/// Activate the first "free" enemy object in the scene
E3D_Object *AddEnemy(int modelId)
{
	// find first free enemy slot, create an enemy
	int n = 0;
	OBJLIST* objList = Scene.GetObjectList(ENEMY_LIST);
	OBJLIST::iterator iter;
	for(iter = objList->begin(); iter != objList->end(); iter++)
		{
		if(BEHAVE_NONE == iter->behaviour)
			{
			iter->modelNumber = modelId;
			iter->model = Scene.GetModel(modelId);
			iter->behaviour = BEHAVE_NORMAL;
			iter->counter = 100;
			//break; //i = ENEMY_ENDID + 1;
			return Scene.GetObject(ENEMY_LIST, n);
			}
		n++;
		}

	return NULL;
}

// Launch and enemy bullet
void LaunchEnemyBullet(Vector &pos, Vector &velocity) {
	// find first free enemy slot, create a bullet and launch it
	OBJLIST* objList = Scene.GetObjectList(ENEMY_LIST);
	OBJLIST::iterator iter;
	for(iter = objList->begin(); iter != objList->end(); iter++)
		{
		if(0 == iter->behaviour)
			{
			iter->modelNumber = EBULLET_MODEL;
			iter->model = Scene.GetModel(EBULLET_MODEL);
			iter->behaviour = BEHAVE_NORMAL;
			iter->counter = 100;
			iter->position = pos + Vector(0.0f, 0.0f, 2.0f); //SetPosition(pos.x, pos.y, pos.z + 2.0f);
			iter->velocity = velocity;
			break; //i = ENEMY_ENDID + 1;
			}
		}
}

// Split a tanker into 2 flippers or other objects
void SplitTanker(OBJLIST::iterator tanker, TUBEDATA *tube) {
	// split into 2 flippers
	WFPlaySound(2, SAMPLE_TANKERSPAWN);

	Vector p;
	float angle = GetTubeSegmentCentreAngle(tube, tanker->var1, p);
	// 1. convert this tanker into an flipper
	//pObject->position.z = -200.0f + (rand() % 50);
	tanker->modelNumber = FLIPPER_MODEL;
	tanker->model = Scene.GetModel(FLIPPER_MODEL);
	tanker->rotation.z = angle;
	tanker->energy = 0;
	tanker->energymax = 50;
	tanker->counter = 200 + (rand()%200);
	// 2. add a new enemy object
	E3D_Object *newEnemy = AddEnemy(FLIPPER_MODEL);
	if(newEnemy) {
		newEnemy->rotation.z = angle;
		newEnemy->position = tanker->position;
		newEnemy->velocity = tanker->velocity;
		newEnemy->var1 = tanker->var1;				// segment
		newEnemy->energy = 0;
		newEnemy->energymax = 50;
		newEnemy->behaviour = BEHAVE_NORMAL;
		newEnemy->counter = 200 + (rand()%200);
	}
	// 3. add some fragments
	AddFrags(tanker->position, tanker->velocity, 8);

}

// Update flipper for one tick
void StepFlipper(OBJLIST::iterator flipper, TUBEDATA *tube, unsigned int level)
{
	// Level 0: flipper moves straight up
	// Level 1: flippers all flip clockwise
	// Level 2: flippers flip back and forth
	// Flipper needs aggression to increase as level increases:
	// 1. Faster
	// 2. Changes lane more often
	// 3. Less pause at top of web
	// 4. Fires more often
	if(BEHAVE_NORMAL == flipper->behaviour)
		{
		if(flipper->position.z < -TUBE_LENGTH)					// dropping into tube
			flipper->position.z += 3.0f;
		else
			flipper->position.z += flipper->velocity.z;			// in tube
		if(flipper->position.z > 0.0f)
			{
			WFPlaySound(2, SAMPLE_ENEMYONRIM);
			// flipper should now continue flipping around rim for a while, then retreat
			flipper->position.z = 0.0f;
			flipper->velocity.z = 0.0f;
			flipper->counter = 150 + (rand()%50);
			}
		// if counter expires, we need to fire, or if on the rim, retreat
		if(!flipper->counter)
			{
			// on the rim?
			if(flipper->position.z == 0.0f)
				{
				flipper->behaviour = BEHAVE_RETREAT;
				flipper->velocity.z = -1.3f;
				}
			else
				{
				// fire if enemy is not too far away nor too close
				if(flipper->position.z > -(TUBE_LENGTH * 0.66))
					{
					if(flipper->position.z < -(TUBE_LENGTH * 0.1))
						{
						WFPlaySound(1, SAMPLE_ENEMYFIRE);
						Vector pos = flipper->position;
						pos.z += 2.0f;
						Vector vel(0.0f, 0.0f, 1.0f);
						LaunchEnemyBullet(pos, vel);
						}
					}
				flipper->counter = 200 + (rand()%200) - min(level*10, 150);
				}
			}
		flipper->counter--;	// decrement counter

		// do we need to flip?
		if(flipper->position.z > -TUBE_LENGTH)
			flipper->energy++;

		if(flipper->energy == flipper->energymax)
			{
			flipper->energy = 0;
			int direction = ((rand() % 10) > 4) ? 1 : -1;
			int newseg = flipper->var1 + direction;
			if(newseg < 0 || newseg > (tube->numVertices-2))
				{
				// would flip off web, so forget about flip
				flipper->behaviour = BEHAVE_NORMAL;
				}
			else
				{
				flipper->var1 = newseg;		// var1 becomes "target segment"
				//Vector p;
				//float targetAngle = GetTubeSegmentCentreAngle(tube, newseg, p);
				flipper->turn = -1 * (float)direction * PI / 20.0f; //2 * PI + pObject->rotation.z - targetAngle;
				flipper->behaviour = BEHAVE_ALT;		// put into flip mode
				}
			}
		}
	else if(BEHAVE_ALT == flipper->behaviour)
		{
		// flipping towards target segment
		flipper->position.z += 0.1f;
		if(flipper->position.z > 0.0f)
			flipper->position.z = 0.0f;
		flipper->rotation.z += flipper->turn;
		Vector p;
		float targetAngle = GetTubeSegmentCentreAngle(tube, flipper->var1, p);
		// if rotating clockwise, then moving "right" on web
		if(flipper->turn < 0.0)
			{
			// rotating towards the right
			// use left edge of target segment as pivot point
			Vector edgePt(p.x - (STEP_SIZE/2) * cos(targetAngle), p.y - (STEP_SIZE/2) * sin(targetAngle), 0.0);
			flipper->SetPosition(edgePt.x - (STEP_SIZE/2) * cos(flipper->rotation.z/2),
								edgePt.y - (STEP_SIZE/2) * sin(flipper->rotation.z/2),
								flipper->position.z);
			targetAngle -= PI;
			}
		// if rotating anticlockwise, then moving "left" on web
		else if(flipper->turn > 0.0)
			{
			// rotating towards the left
			// use right edge of target segment as pivot point
			Vector edgePt(p.x + (STEP_SIZE/2) * cos(targetAngle), p.y + (STEP_SIZE/2) * sin(targetAngle), 0.0);
			flipper->SetPosition(edgePt.x + (STEP_SIZE/2) * cos(flipper->rotation.z/2),
								edgePt.y + (STEP_SIZE/2) * sin(flipper->rotation.z/2),
								flipper->position.z);
			targetAngle += PI;
			}
		// check if finished
		
		if(fabs(flipper->rotation.z/2 - targetAngle) < (PI / 20.0))
			{
			if(flipper->turn < 0.0)
				flipper->rotation.z = targetAngle + PI;
			else
				flipper->rotation.z = targetAngle - PI;
			flipper->SetPosition(p.x, p.y, flipper->position.z);
			flipper->behaviour = BEHAVE_NORMAL;
			}
		}
	else if(BEHAVE_RETREAT == flipper->behaviour)
		{
		// retreating
		if(flipper->position.z > (-TUBE_LENGTH*2.0f))
			{
			flipper->position.z += flipper->velocity.z;			// in tube
			}
		else
			{
			flipper->SetPosition(0.0f, 5000.0f, 0.0f);
			flipper->behaviour = BEHAVE_NONE;
			}
		}

}

// Update tanker for one tick
void StepTanker(OBJLIST::iterator tanker, TUBEDATA *tube, unsigned int level)
{
	// Tanker moves straight up, then splits close to top of web
	// (Should make it pause for a sec or two before splitting)
	// Tanker needs aggression to increase as level increases:
	// 1. Faster
	// 2. Fires more often
	// 3. Enemies spawned are more advanced
	if(BEHAVE_NORMAL == tanker->behaviour)
		{
		if(tanker->position.z < -TUBE_LENGTH)
			tanker->position.z += 3.0f;
		else
			tanker->position.z += tanker->velocity.z;
		tanker->rotation.z += 0.02f;
		// fire if counter is zero
		if(!tanker->counter)
			{
			//	Vector pos = pObject->position;
			//	pos.z += 2.0f;
			//	Vector vel(0.0f, 0.0f, 1.0f);
			//	LaunchEnemyBullet(pos, vel);
			tanker->counter = 200 + (rand()%200) - min(level*10, 150);
			}
		tanker->counter--;	// decrement counter

		// split into 2 flippers if close to this end of web
		if(tanker->position.z > -50.0f)
			{
			SplitTanker(tanker, tube);
			}
		}
}

// Update enemy bullet for one tick
void StepEnemyBullet(OBJLIST::iterator ebullet, unsigned int level)
{
	if(BEHAVE_NORMAL == ebullet->behaviour)
		{
		ebullet->rotation.z += 0.1f;
		ebullet->position += ebullet->velocity;
		if(ebullet->position.z > 5.0f)
			{
			// terminate
			ebullet->SetPosition(0.0f, 1000.0f, 0.0f);
			ebullet->behaviour = BEHAVE_NONE;
			ebullet->counter = 0;
			}
		}

}

// Update spike for one tick
void StepSpike(OBJLIST::iterator spike, unsigned int level)
{
	if(BEHAVE_NORMAL == spike->behaviour)
		{
		// growing
		if(spike->position.z < 0.0f)
			spike->position += spike->velocity;
		spike->rotation.z += 0.1f;
		if(spike->position.z > -TUBE_LENGTH)
			spike->scale.z = spike->position.z + TUBE_LENGTH;
		else
			spike->scale.z = 0.001f;
		if(spike->position.z > 0.0f)
			{
			// Steal player energy and terminate
			AddCharge(-20);
			spike->SetPosition(0.0f, 1000.0f, 0.0f);
			spike->behaviour = BEHAVE_NONE;
			spike->counter = 0;
			}
		}
	else if(BEHAVE_ALT == spike->behaviour)
		{
		// shrinking after a hit
		spike->position.z -= 1.0f;
		spike->scale.z = spike->position.z + TUBE_LENGTH;
		// kill spike if shrunk right back
		if(spike->scale.z <= 0.0f)
			{
			spike->SetPosition(0.0f, 1000.0f, 0.0f);
			spike->behaviour = BEHAVE_NONE;		// switch off enemy
			}
		else
			{
			spike->counter--;
			// are we finished shrinking?
			if(!spike->counter)
				spike->behaviour = BEHAVE_NORMAL;	
			}
		}

}

// Update barrier for one tick
void StepBarrier(OBJLIST::iterator barrier, unsigned int level)
{
	if(BEHAVE_NORMAL == barrier->behaviour)
		{
		barrier->position += barrier->velocity;
		if(barrier->position.z > 5.0f)
			{
			// terminate
			barrier->SetPosition(0.0f, 1000.0f, 0.0f);
			barrier->behaviour = BEHAVE_NONE;
			barrier->counter = 0;
			}
		}

}

// Update player bullet for one tick
void StepBullet(OBJLIST::iterator bullet, TUBEDATA *tube)
{
	if(BEHAVE_NORMAL == bullet->behaviour)
		{	// active
		bullet->position += bullet->velocity;
		bullet->rotation.z += 0.2f;
		// check if this bullet has hit any enemies
		OBJLIST* enemyList = Scene.GetObjectList(ENEMY_LIST);
		OBJLIST::iterator enemyIter;
		for(enemyIter = enemyList->begin(); enemyIter != enemyList->end(); enemyIter++)
			{
			if(fabs(enemyIter->position.z - bullet->position.z) < HIT_RADIUS)
				{
				if(fabs(enemyIter->position.x - bullet->position.x) < HIT_RADIUS)
					{
					if(fabs(enemyIter->position.y - bullet->position.y) < HIT_RADIUS)
						{
						// hit!
						switch(enemyIter->modelNumber)
							{
							case FLIPPER_MODEL :
								{
								WFPlaySound(1, SAMPLE_ENEMYEXPLODE);
								Vector v(0.0f, 0.0f, 1.0f);
								AddFrags(enemyIter->position, v, 10);
								enemyIter->SetPosition(0.0f, 1000.0f, 0.0f);
								enemyIter->behaviour = BEHAVE_NONE;		// switch off enemy
								bullet->counter = 1;		// expire bullet
								if (enemyIter->velocity.z < 0)
									{
									AddScore(FLIPPER_VALUE * 4);	// retreating flipper
									AddCharge(FLIPPER_VALUE / 5);
									}
								else
									{
									AddScore(FLIPPER_VALUE);
									AddCharge(FLIPPER_VALUE / 20);
									}
								g_shotsHit++;
								}
								break;
							case TANKER_MODEL :
								{
								WFPlaySound(1, SAMPLE_ENEMYEXPLODE);
								// split into 2 flippers
								// 1. create frags
								Vector v(0.0f, 0.0f, 1.0f);
								AddFrags(enemyIter->position, v, 16);
								// 2. convert this tanker into 2 flippers
								SplitTanker(enemyIter, tube);
								// 3. kill the bullet
								bullet->counter = 1;		// expire bullet
								AddScore(TANKER_VALUE);
								AddCharge(TANKER_VALUE / 20);
								g_shotsHit++;
								}
								break;
							case EBULLET_MODEL :
								{
								Vector v(0.0f, 0.0f, 1.0f);
								AddFrags(enemyIter->position, v, 8);
								enemyIter->SetPosition(0.0f, 1000.0f, 0.0f);
								enemyIter->behaviour = BEHAVE_NONE;		// switch off enemy
								bullet->counter = 1;		// expire bullet
								AddCharge(FLIPPER_VALUE / 20);
								g_shotsHit++;
								}
								break;
							case SPIKE1_MODEL :
								{
								WFPlaySound(1, SAMPLE_ENEMYEXPLODE);
								Vector v(0.0f, 0.0f, 1.0f);
								AddFrags(enemyIter->position, v, 10);
								// Change spike model alt spike model (ie: no spiral)
								enemyIter->modelNumber = SPIKE2_MODEL;
								enemyIter->model = Scene.GetModel(SPIKE2_MODEL);
								// Stop spike from moving forward
								enemyIter->velocity = Vector(0.0f, 0.0f, 0.0f);
								// temporarily put spike into shrinking mode
								enemyIter->behaviour = BEHAVE_ALT;
								enemyIter->counter = 20;
								bullet->counter = 1;		// expire bullet
								AddScore(SPIKER_VALUE);
								AddCharge(SPIKER_VALUE / 20);
								g_shotsHit++;
								}
								break;
							case SPIKE2_MODEL :
								{
								Vector v(0.0f, 0.0f, 1.0f);
								AddFrags(enemyIter->position, v, 3);
								// temporarily put spike into shrinking mode
								enemyIter->behaviour = BEHAVE_ALT;
								enemyIter->counter = 20;
								bullet->counter = 1;		// expire bullet
								//AddScore(SPIKER_VALUE);
								AddCharge(SPIKER_VALUE / 20);
								g_shotsHit++;
								}
								break;
							case BARRIER_MODEL :
								{
								WFPlaySound(1, SAMPLE_BARRIERHIT);
								bullet->counter = 1;		// expire bullet
								// Add some fragments
								Vector v(0.0f, 0.0f, 3.0f);
								AddFrags(enemyIter->position, v, 1);
								}
								break;
							case UFO_MODEL :
								{
								Vector v(0.0f, 0.0f, 0.0f);
								AddFrags(enemyIter->position, v, 20);
								enemyIter->SetPosition(0.0f, 1000.0f, 0.0f);
								enemyIter->behaviour = BEHAVE_NONE;		// switch off enemy
								bullet->counter = 1;		// expire bullet
								AddScore(UFO_VALUE);
								AddCharge(UFO_VALUE / 20);
								g_shotsHit++;
								}
								break;
							//case ASTEROID_MODEL :
							//	pObject->counter = 1;		// expire bullet
							//	break;
							} // end switch
						}
					}
				
				}
			}
		if(bullet->counter) bullet->counter--;	// decrement counter
		if(!bullet->counter || bullet->position.z < -TUBE_LENGTH)
			{
			bullet->behaviour = BEHAVE_NONE;	// switch off if finished count
			bullet->SetPosition(0.0f, 1000.0f, 0.0f);
			}
		}

}

// Update frag for one tick
void StepFragment(OBJLIST::iterator frag)
{
	if(BEHAVE_NORMAL == frag->behaviour)
		{
		frag->position += frag->velocity;
		frag->rotation += (frag->velocity * 0.5f);
		if(frag->counter) frag->counter--;	// decrement counter
		if(!frag->counter)
			{
			frag->behaviour = BEHAVE_NONE;	// switch off if finished count
			frag->SetPosition(0.0f, 1000.0f, 0.0f);
			}
		}

}

// Update UFO for one tick
void StepUFO(OBJLIST::iterator ufo, TUBEDATA *tube, unsigned int level)
{
	// UFO drops onto far end of web (BEHAVE_NORMAL)
	// Then moves across far end of web, firing at player occasionally
	// Aggression and speed to increase as level increases:
	// 1. Faster
	// 2. Fires more often
	if (BEHAVE_NORMAL == ufo->behaviour)
		{
		//ufo->rotation.y += 0.02f;
		if(ufo->position.z < -TUBE_LENGTH)
			ufo->position.z += 3.0f;
		else
			{
			ufo->position.z = -TUBE_LENGTH;
			ufo->behaviour = BEHAVE_ALT;
			ufo->var2 = 1;		// "move right"
			}
		} // end NORMAL behaviour
	else if (BEHAVE_ALT == ufo->behaviour)
		{
		//ufo->rotation.y += 0.02f;
		// fire if counter is zero
		if(!ufo->counter)
			{
			Vector pos = ufo->position;
			pos.z += 2.0f;
			Vector vel(0.0f, 0.0f, 1.0f);
			LaunchEnemyBullet(pos, vel);
			ufo->counter = 100 + (rand()%100) - min(level*5, 50);
			}
		ufo->counter--;	// decrement counter

		// Move UFO toward target segment
		Vector p;
		float targetAngle = GetTubeSegmentCentreAngle(tube, ufo->var1, p);
		p.z = -TUBE_LENGTH;
		Vector d = p - ufo->position;
		// Have we reached our target segment?
		if (d.GetLength() < ufo->speed)
			{
			// Find next target segment
			if (ufo->var2)
				{
				int nextSeg = ufo->var1 + 1;
				if (nextSeg < (tube->numVertices - 1))
					{
					ufo->var1 = nextSeg;
					}
				else
					{
					ufo->var1--;
					ufo->var2 = 0; // move left
					}
				}
			else
				{
				if (ufo->var1 > 0)
					{
					ufo->var1--;
					}
				else
					{
					ufo->var1++;
					ufo->var2 = 1;	// move right
					}
				}
			}
		// actaully move the UFO
		d.SetLength(ufo->speed);
		ufo->position += d;
		ufo->rotation.z = targetAngle;
		} // end ALT behaviour

}
// Update asteroid for one tick
void StepAsteroid(OBJLIST::iterator asteroid)
{
}



/////////////////////////////////////////////////////////////////////////////////
// Object behaviour engine
// This function updates all objects for 1 frame of action
/////////////////////////////////////////////////////////////////////////////////
void DoBehaviours(TUBEDATA *tube, unsigned int level)
	{
	// Notes:
	// Behaviour 0 is object inactive (off)
	// Behaviour 1 is circle/wait for all enemies
	// Behaviour 2 is object's normal behaviour

	// TODO: spikers amd zappers and fuseball

	// BEGIN BEHAVIOURS
	// Behaviours 1: Enemy objects (flippers, tankers, etc)
	OBJLIST* objList = Scene.GetObjectList(ENEMY_LIST);
	OBJLIST::iterator objIter;
	for(objIter = objList->begin(); objIter != objList->end(); objIter++)
		{
		switch(objIter->modelNumber)
			{
			case FLIPPER_MODEL :
				StepFlipper(objIter, tube, level);
				break;
			case TANKER_MODEL :
				StepTanker(objIter, tube, level);
				break;
			case EBULLET_MODEL :
				StepEnemyBullet(objIter, level);
				break;
			case SPIKE1_MODEL :
			case SPIKE2_MODEL :
				StepSpike(objIter, level);
				break;
			case BARRIER_MODEL :
				StepBarrier(objIter, level);
				break;
			case UFO_MODEL :
				StepUFO(objIter, tube, level);
				break;
			//case ASTEROID_MODEL :
			//	ASSERT(FALSE);
			//	StepAsteroid(objIter);
			//	break;
			}	// end switch
		} // next

	// Behaviours 2: Player bullet list
	objList = Scene.GetObjectList(BULLET_LIST);
	for(objIter = objList->begin(); objIter != objList->end(); objIter++)
		{
		switch(objIter->modelNumber)
			{
			case BULLET_MODEL :
				StepBullet(objIter, tube);
				break;
			}
		}

	// Behaviours 3: Fragment list
	objList = Scene.GetObjectList(FRAG_LIST);
	for(objIter = objList->begin(); objIter != objList->end(); objIter++)
		{
		switch(objIter->modelNumber)
			{
			case FRAG_MODEL :
				StepFragment(objIter);
				break;
			}
		}

}
