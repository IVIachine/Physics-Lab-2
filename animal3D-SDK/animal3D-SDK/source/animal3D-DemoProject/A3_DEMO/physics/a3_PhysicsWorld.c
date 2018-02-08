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

a3_PhysicsWorld.c/.cpp
Physics world function implementations.
*/


#include "a3_PhysicsWorld.h"

// include utilities on an as-needed basis
#include "animal3D/a3utility/a3_Timer.h"


// external
#include <stdio.h>


//-----------------------------------------------------------------------------
// internal utility for initializing and terminating physics world
void a3physicsInitialize_internal(a3_PhysicsWorld *world)
{
	// any init tasks here

}

// physics simulation
long a3physicsThread(a3_PhysicsWorld *world)
{
	// physics simulation timer
	a3_Timer physicsTimer[1] = { 0 };

	// second counter for physics (debugging)
	unsigned int currSecond = 0, prevSecond = 0;

	a3physicsInitialize_internal(world);

	// start timer
	// rate should be set before beginning thread
	a3timerSet(physicsTimer, world->rate);
	a3timerStart(physicsTimer);

	// if lock is negative, terminate
	while (world->lock >= 0)
	{
		if (a3timerUpdate(physicsTimer))
		{
			// update timer ticked, do the thing
			a3physicsUpdate(world, physicsTimer->previousTick);

			// debug display time in seconds
			currSecond = (unsigned int)(physicsTimer->totalTime);
			if (currSecond > prevSecond)
			{
				prevSecond = currSecond;
				printf("\n physics time: %.4lf;  ticks: %llu \n     ups avg: %.4lf;  dt avg: %.4lf",
					physicsTimer->totalTime,
					physicsTimer->ticks,
					(double)physicsTimer->ticks / physicsTimer->totalTime,
					physicsTimer->totalTime / (double)physicsTimer->ticks
				);
			}
		}
	}

	// terminate world
	return 0;
}

void a3physicsUpdate(a3_PhysicsWorld *world, double dt)
{
	const double daysPerHour = 1.0 / 24.0;

	//READ
	a3_PhysicsWorldState state[1] = { 0 };
	a3physicsLockWorld(world);
	*state = *world->state;
	a3physicsUnlockWorld(world);

	size_t i;
	//Update everything
	state->planetTime += state->planetSpeed * dt;
	for (i = 0; i < state->planetCount; ++i)
	{
		// calculate day of year based on accumulated time
		state->planetData[i].earthDay = state->planetTime * daysPerHour;

		// calculate orbit based on day of year
		state->planetData[i].orbit = state->planetData[i].earthDay / state->planetData[i].orbitalPeriod;
		state->planetData[i].orbit = (a3f64)a3trigValid_sind((a3real)state->planetData[i].orbit * a3realThreeSixty);
	}

	// rotate and position objects
	if (state->verticalAxis)
	{
		// y-up
		for (i = 0; i < state->planetCount; ++i)
		{
			// calculate position based on orbit
			state->planetData[i].rigidBody->position.x = +a3cosd((a3real)state->planetData[i].orbit) * (a3real)state->planetData[i].distance;
			state->planetData[i].rigidBody->position.z = -a3sind((a3real)state->planetData[i].orbit) * (a3real)state->planetData[i].distance;
		}
	}
	else
	{
		// z-up
		for (i = 0; i < state->planetCount; ++i)
		{
			// calculate position based on orbit
			state->planetData[i].rigidBody->position.x = +a3cosd((a3real)state->planetData[i].orbit) * (a3real)state->planetData[i].distance;
			state->planetData[i].rigidBody->position.y = +a3sind((a3real)state->planetData[i].orbit) * (a3real)state->planetData[i].distance;
		}
	}
	//Write to world-owned state
	a3physicsLockWorld(world);
	*world->state = *state;
	a3physicsUnlockWorld(world);
}


// get thread ID
#ifdef _WIN32
#include <Windows.h>
int threadID()
{
	return GetCurrentThreadId();
}
#else
#include <sys/types.h>
int threadID()
{
	return gettid();
}
#endif	// _WIN32

// mutex
extern inline int a3physicsLockWorld(a3_PhysicsWorld *world)
{
	// wait for lock to be released, then set it
	while (world->lock > 0);
	world->lock = threadID();
	return world->lock;
}

extern inline int a3physicsUnlockWorld(a3_PhysicsWorld *world)
{
	const int ret = world->lock;
	world->lock = 0;
	return ret;
}


//-----------------------------------------------------------------------------
