// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

/*
The new physics algorithms differ to the algorithms in Tuxracer / PPRacer / ETRacer.
The latter programs are based on an ODE solver to make the challenges comparable
on computers with different performance. That works fine but has two disadvantages:
- The ODE solver needs a lot of performance
- You run into a lot of problems when you try to implement real 3D collision
- It's less effort to generate a chain of way-time-marks for the race vs. opponent

For me, that were the reasons to avoid the ODE solver. To make the races comparable 
this code runs on timer. The timer is set to 25 ms (40 fps). Most computers should
reach this fps. If a computer is too slow, Tux will move more slowly - as well as the 
clock. If the fps is between ~32 and 40 the user won't probably realize ist. And what 
about lower fps than 32? I'm afraid, in this case an ODE solver couldn't help, too.

The entrence to the physical simulation is the function UpdateSimulation (). First
the forces are calculated and the velocity vector is updated. After that the collision
procedure is called. It moves the character (as far as possible) and returns all
params that are needed for complete collision response. Normally, the collision
is called several times within a frame - until the way is free.
*/

#ifndef PHYSICS_H
#define PHYSICS_H

#include "bh.h"

void	InitSimulation (TControl *ctrl);
void	UpdateSimulation (float timestep, TControl *ctrl);

#endif
