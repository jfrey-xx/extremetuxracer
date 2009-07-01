// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "physics.h"
#include "course.h"
#include "collision.h"
#include "models.h"
#include "tux.h"
#include "keyframe.h"
#include "particles.h"
#include "race.h"

// --------------------------------------------------------------------
//					settings
// --------------------------------------------------------------------

// *ctr is set in UpdatePlayer and uses by all function in this unit
static TControl *ctr;

// common:
const float C_MIN_SPEED = 10.01 / 3.6; 		// avoid to stall the motion
const float C_TURN_ANGLE = 30.0;			// if not vector control 		
const float C_BRAKING_TURN_ANGLE = 60.0;	// turning AND braking (not vector conntrol)
const float C_NET = 1.0;					// factor for over-all netforce	

// basic force factors: (don't change these constants)
const float C_GRAV = 0.35;					// gravitational force factor
const float C_AIR_GRAV = 0.3;				// special gravitaion for airborne state
const float C_FRICT = 0.32;					// basic friction factor
const float C_AIR_RESIST = 0.015;			// important for the airborne state
											// and to get a natural speed limitation
const float C_SPEED_FACTOR = 0.003;			// to speed up or slow down the race
											// see speed_factor of the course
// additional force factors
const float C_PADDLING = 0.08; 				// basic paddling factor
const float C_BRAKE = 0.08;					// basic braking factor
const float C_JUMP = 0.4;					// basic jumping force
const float C_SPEED_JUMP = 2.0;				// speed-dependent part of jumping_force
const float C_BIG_JUMP = 2.0;				// jump vector is multiplied with this value
const float C_AIR_PADDLING = 0.05;			// paddling force in air
const float C_EXT_PADDLING = 0.1;			// hidden extended paddling
const float C_EXT_BRAKING = 0.2;			// hidden extended braking

// steering and deviation if not vector-controlled:
const float C_FAST_DEV = 0.4;				// deviation at high speed
const float C_SLOW_DEV = 5.0;				// deviation at low speed
const float C_BRAKING_MULT = 2.0;			// more deviation when braking

const float C_WINDDRIFT = 0.0015;
const float C_AIR_WINDDRIFT = 0.0012;

// --------------------------------------------------------------------
//					introductions
/* --------------------------------------------------------------------
	Before starting a course (best in RacingInit) you have to call
	InitSimulation (). In RacingLoop you must call UpdatePlayerPos
	with the current timestep. This must be done at first, 
	before updating the light and view position.
*/

	/*
	// ------------------------- steering by angle --------------------
	// only used if not steering with vectors (see CalcNetforce above)
	if (!C_VECTOR_ROLL) {
		float rollangle;
		if (!ctr->airborne) {
			if (ctr->turn != 0) {
				ctr->turnanimation += ctr->turn * 3.0 * timestep;
				ctr->turnanimation = MIN (1.0, MAX (-1.0, ctr->turnanimation));
			} else {
		    	ctr->turnanimation *= 1.0 - timestep / 0.2;
			}

			if (ctr->brake) rollangle = C_BRAKING_TURN_ANGLE * ctr->turn * timestep;
				else rollangle = C_TURN_ANGLE * ctr->turn * timestep;
			ctr->vel = RotateVectorY (ctr->vel, -rollangle);
		} else {
			ctr->turn = 0;
			ctr->turnanimation = 0;
		}
	}
	// ------------------------- end of steering ----------------------
	*/


// --------------------------------------------------------------------
//				check boundaries before collision
// --------------------------------------------------------------------

void CheckBoundaries () {
    float boundary = (Course.width - Course.play_width) / 2; 
	TVector3 destpos = AddVec (ctr->pos, ctr->vel);
    if  (destpos.x < boundary) {
        ctr->vel.x = -ctr->vel.x / 10;
    } else if  (destpos.x > Course.width - boundary) {
        ctr->vel.x = -ctr->vel.x / 10;
    } 	
}

static float brake_steps = 60;
static int brake_cnt;
static float brake_start_speed;

// --------------------------------------------------------------------
//				adjust velocity and position after collision
// --------------------------------------------------------------------

void AdjustPosVel (float timestep) {
	float speed;
	TVector3 vel;

	// race state
	if (!ctr->final_state) {
		// test for minimal speed
		speed = NormVec (ctr->vel);
	    speed = MAX (C_MIN_SPEED * timestep, speed);
		ctr->vel = ScaleVec (speed, ctr->vel);

		// test for start position
		if  (ctr->pos.z > Course.startpoint.y) ctr->pos.z = Course.startpoint.y;

		// test for final position
		if (-ctr->pos.z >= Course.play_length) {
			brake_start_speed = VecLen (ctr->vel);
			Race.final_speed = brake_start_speed / timestep * 3.6; 

			// switching to the braking phase 
			if (!TuxFailure.loaded && !TuxSuccess.loaded) {
				SetGameMode (FINISH);
			} else {
				ctr->final_state = true;
				SetPlyrBraking (true);
				ctr->turn_animation = 0;
				brake_cnt = 0;
			}
		} 

	// final braking state
	} else {
		brake_cnt ++;
		vel = ctr->vel;
		speed = NormVec (vel);
		float reduce = 1 - (brake_cnt / brake_steps);

		TVector3 zz = ctr->vel;
		zz.x = zz.x * 0.9;
		NormVec (zz);
		vel = ScaleVec (0.5, AddVec (vel, zz));
		NormVec (vel);
		ctr->vel = ScaleVec (brake_start_speed * reduce, vel);
		if (reduce < 0.02) SetGameMode (FINISH);
	} 
	SetCharacterPosition (ctr->pos, timestep, ctr);
}

// --------------------------------------------------------------------
//					calculate netforce
// --------------------------------------------------------------------

TVector3 CalcNetforce (float timestep) {
	float factor;
	TVector3 netforce;
	TVector3 paddlingforce;
	TVector3 gravitationforce;
	TVector3 frictionforce;
	TVector3 jumpforce;
	TVector3 airforce;
	TVector3 deviationforce;
	TVector3 speedforce;
	TVector3 paddlingforceX;
	TVector3 brakingforceX;

//	TVector3 grav_nml = MakeVec (0.0, -1.0, 0.0);
	float squarespeed = ctr->speed * ctr->speed;
	netforce = NullVec;

	// ---------------- not airborne ----------------------------------
	if (ctr->steer_enabled) {

		// set turn_animation:
		if (ctr->turn != 0) {
			ctr->turn_animation += ctr->turn * 3.0 * timestep;
			ctr->turn_animation = MIN (1.5, MAX (-1.5, ctr->turn_animation));
		} else ctr->turn_animation *= (1.0 - timestep / 0.2);

		float frict = ctr->friction;

		// gravitation force
		gravitationforce =  ScaleVec (C_GRAV, GravVec); 
		netforce = AddVec (netforce, gravitationforce);

		// friction force
 		factor = 0.489 * frict + 0.1788;
		frictionforce = ScaleVec (C_FRICT * -factor, ctr->dir); 
		netforce = AddVec (netforce, frictionforce);

		// speed force
		factor = C_SPEED_FACTOR * MIN (Course.speed, 100);
		speedforce = ScaleVec (factor, MakeVec (0, 0, -1));
		netforce = AddVec (netforce, speedforce);

		// air resistance and wind
		airforce = ScaleVec (-C_AIR_RESIST * squarespeed, ctr->dir);
		airforce = AddVec (airforce, ScaleVec (C_WINDDRIFT, Wind.WindDrift ()));
		netforce = AddVec (netforce, airforce);

		// braking and paddling force
		if (ctr->brake ^ ctr->paddle) {
			if (ctr->paddle) {
				factor = C_PADDLING * (0.5 * frict + 0.825);
				factor += (1 - ctr->deviation) / 5;
				float limitfact = (adj.max_paddling_speed - ctr->realspeed) / 10;
				limitfact = MAX (-1, MIN (1, limitfact));
				if (limitfact < 0) limitfact *= 3;
				paddlingforce = ScaleVec (factor * limitfact, ctr->dir);
			} else { 
				factor = C_BRAKE * (1.0 * frict + 0.65) ;
				paddlingforce = ScaleVec (-factor, ctr->dir);
			}
			netforce = AddVec (netforce, paddlingforce);
		} 

		// hidden option, extended paddling:
		if (ctr->paddleX) {
			paddlingforceX = ScaleVec (C_EXT_PADDLING, ctr->dir);			
			netforce = AddVec (netforce, paddlingforceX);
		}

		// hidden option, extended braking:
		if (ctr->brakeX) {
			brakingforceX = ScaleVec (-C_EXT_BRAKING, ctr->dir);			
			netforce = AddVec (netforce, brakingforceX);
		}

		// jumping force
		if (ctr->jump) {
			factor = (C_SPEED_JUMP * ctr->speed + C_JUMP) * ctr->jump_level;
			if (ctr->brake) factor *= C_BIG_JUMP;
			jumpforce = ScaleVec (factor, MakeVec (0.0, 1.0, 0.0));
			netforce = AddVec (netforce, jumpforce);
		}

		// deviation force for steering
		deviationforce = CrossVec (ctr->surf_nml, ctr->dir);

		factor = C_FAST_DEV * ctr->speed + C_SLOW_DEV * timestep;
		if (ctr->brake) factor *= C_BRAKING_MULT;

		float dirfactor = ctr->deviation + 0.5;
		dirfactor = MAX (0.6, MIN (1.0, dirfactor));
		float frictfactor = MIN (1.0, (3.0 * frict) - 0.05);
		factor *= dirfactor * frictfactor;

		deviationforce = ScaleVec (-factor * ctr->turn, deviationforce);
		netforce = AddVec (netforce, deviationforce);

	// ----------------- airborne -------------------------------------
	} else { 	
		ctr->turn = 0;
		ctr->turn_animation = 0;

		// gravitation
		gravitationforce = ScaleVec (C_AIR_GRAV, GravVec);
		netforce = AddVec (netforce, gravitationforce);

		// air resistance and wind
		airforce = ScaleVec (-C_AIR_RESIST * squarespeed, ctr->dir);
		airforce = AddVec (airforce, ScaleVec (C_AIR_WINDDRIFT, Wind.WindDrift ()));
		netforce = AddVec (netforce, airforce);
		
		// paddling in air
		if (ctr->paddle) {
			TVector3 airpaddling = ctr->dir;
			airpaddling.y = 0;
			paddlingforce  = ScaleVec (C_AIR_PADDLING, airpaddling);
			netforce = AddVec (netforce, paddlingforce);
		} 
	}
	return ScaleVec (timestep * C_NET, netforce);
}

TVector3 CalcFinishNetforce (float timestep) {
	TVector3 netforce;
	TVector3 gravitationforce;
	TVector3 frictionforce;
	TVector3 airforce;

	TVector3 grav_nml = MakeVec (0, -1, 0);
	float squarespeed = ctr->speed * ctr->speed;

	// ---------------- not airborne ----------------------------------
	if (!ctr->airborne) {
		float friction = ctr->friction;
		gravitationforce =  ScaleVec (C_GRAV, grav_nml); 
		frictionforce = ScaleVec (-friction * C_FRICT , ctr->dir);
		airforce =   ScaleVec (-C_AIR_RESIST * squarespeed, ctr->dir);

	// ----------------------- airborne -------------------------------
	} else { 
		gravitationforce = ScaleVec (C_AIR_GRAV * 10, grav_nml);
		airforce =  ScaleVec (-C_AIR_RESIST * squarespeed, airforce);
		frictionforce = NullVec;
		airforce = ctr->dir;
	}

	netforce = NullVec;
	netforce = AddVec (netforce, gravitationforce);
	netforce = AddVec (netforce, frictionforce);
	netforce = AddVec (netforce, airforce);
	return ScaleVec (timestep * C_NET, netforce);
}

// --------------------------------------------------------------------
//					update simulation
// --------------------------------------------------------------------

void UpdateSimulation (float timestep, TControl *ctrl) {
	if (timestep < 0.0001) return; 
	ctr = ctrl;

	TCollDetection coll;

	ctr->vel = ScaleVec (timestep, ctr->vel);
	Collision.Init (ctr->pos, AddVec (ctr->vel, MakeVec (0, -0.1, 0)), adj.radius);
	Collision.CheckSurface (ctr);
	ctr->steer_enabled = !ctr->airborne;
	if (ctr->airborne) {
		TPlane surf_plane = Course.GetCoursePlane (ctr->pos);
		float dist_from_surface = DistToPlane (surf_plane, ctr->pos) - adj.radius;
		ctr->steer_enabled = (dist_from_surface < 0.2);
	}

	CheckBoundaries ();

	// here is the place for steering by angle
	// see function at the top of this module
	
	// now add the netforce
	if (!ctr->final_state) ctr->netforce = CalcNetforce (timestep);
	else ctr->netforce = CalcFinishNetforce (timestep);
	ctr->vel = AddVec (ctr->vel, ctr->netforce);

	// ------------------- collision ----------------------------------
	Course.AddPolygons (ctr);
	Models.AddModels (ctr);
	Collision.Init (ctr->pos, ctr->vel, adj.radius);

	bool crun = true;
	while (crun) {
		coll = Collision.CheckModelCollision ();
		if (coll.ready) {
			ctr->pos = coll.pos; 
			ctr->vel = coll.vel;
			crun = false; 
		}
	}
	// ------------------- end of collision ---------------------------

	AdjustPosVel (timestep);
	SetCharacterOrientation (timestep, ctr);
	SetCharacterAnimation (ctr);
 	if (!ctr->final_state) Race.Update (ctr);

	// for further usage in other modules
	ctr->dir = ctr->vel;
	ctr->speed = NormVec (ctr->dir);

	float aa = ctr->dir.x / ctr->dir.z;
	ctr->dir_angle = atan (aa) * 180 / 3.14;

	if (ctr->speed <= 0) ctr->speed = 0.0001;
	ctr->nmlspeed = ctr->speed / timestep;
	ctr->realspeed = ctr->nmlspeed * 3.6;
	ctr->deviation = DotVec (ctr->dir, MakeVec (0, 0, -1));

	// return with rescaled velocity
	ctr->vel = ScaleVec (1/timestep, ctr->vel);
}

void InitSimulation (TControl *ctrl) {
	ctrl->pos.x = Course.startpoint.x;
	ctrl->pos.z = Course.startpoint.y;
 	ctrl->pos.y = Course.GetYCoord (ctrl->pos) + adj.radius;
	ctrl->lastpos = ctrl->pos;
	ctrl->vel = MakeVec (0.0, -8.0, -3);
	ctrl->final_state = false;
}

