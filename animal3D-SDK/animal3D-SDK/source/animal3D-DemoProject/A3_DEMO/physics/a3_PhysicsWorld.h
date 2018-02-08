/*
	Copyright 2011-2018 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_PhysicsWorld.h
	Interface for physics world.

	This file was modified by Tyler Chermely with permission from Daniel Buckstein
	Tyler Chermely 0967813
	EGP-425-01
	Lab 2
	2/08/2018

	I certify that this work is
	entirely my own. The assessor of this project may reproduce this project
	and provide copies to other academic staff, and/or communicate a copy of
	this project to a plagiarism-checking service, which may retain a copy of the
	project on its database.
*/

#ifndef __ANIMAL3D_PHYSICSWORLD_H
#define __ANIMAL3D_PHYSICSWORLD_H


//-----------------------------------------------------------------------------
// physics includes

#include "a3_RigidBody.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
	typedef struct a3_PhysicsWorld					a3_PhysicsWorld;
	typedef struct a3_PhysicsWorldState					a3_PhysicsWorldState;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

	// counters
	enum a3_PhysicsWorldMaxCount
	{
		physicsMaxCount_rigidbody = 16,
	};


//-----------------------------------------------------------------------------

	struct a3_PhysicsWorldState
	{
		// e.g. set of rigid bodies
		int verticalAxis;

		struct planetData
		{
			double distance, orbitalPeriod, size;
			double orbit, earthDay;
			a3_RigidBody rigidBody[1];
		} planetData[physicsMaxCount_rigidbody];

		double planetSpeed, planetTime;
		unsigned int planetCount;
	};

	// persistent demo state data structure
	struct a3_PhysicsWorld
	{
		//---------------------------------------------------------------------
		// SIMPLE MUTEX LOCK
		int lock;

		//---------------------------------------------------------------------
		// general variables pertinent to the state
		// timer rate
		double rate;

		a3_PhysicsWorldState state[1];
		//---------------------------------------------------------------------
	};

	
//-----------------------------------------------------------------------------

	// threaded simulation
	long a3physicsThread(a3_PhysicsWorld *world);
	void a3physicsUpdate(a3_PhysicsWorld *world, double dt);

	// mutex handling
	inline int a3physicsLockWorld(a3_PhysicsWorld *world);
	inline int a3physicsUnlockWorld(a3_PhysicsWorld *world);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_PHYSICSWORLD_H