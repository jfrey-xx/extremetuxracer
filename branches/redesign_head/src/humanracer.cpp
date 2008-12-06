/* 
 * Extreme Tux Racer
 * Copyright (C) 2008 Steven Bell <botsnlinux@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "humanracer.h"

#include "phys_sim.h"
#include "ppgltk/alg/defs.h" //for eps
#include "game_mgr.h"

/* Time constant for automatic steering centering (s) */
#define TURN_DECAY_TIME_CONSTANT 0.5

/* Time constant for automatic rolling centering (s) */
#define ROLL_DECAY_TIME_CONSTANT 0.2

/* Maximum time over which jump can be charged */
#define MAX_JUMP_AMT 1.0

#define JUMP_CHARGE_DECAY_TIME_CONSTANT 0.1

/* If too high off the ground, tux flaps instead of jumping */
#define JUMP_MAX_START_HEIGHT 0.30

HumanRacer::HumanRacer(void)
{
  std::cout << "HumanRacer::HumanRacer()" << std::endl;

  mPaddling = false;
  mBraking = false;
  mRightTurn = false;
  mLeftTurn = false;

  mTurnFactor = 0;

}

void HumanRacer::updatePosition(float timeStep)
{
  std::cout << "HumanRacer::updatePosition()" << std::endl;

  updateTurnAnimation(timeStep);

  // If timeStep is above zero, run the ODE solver.  We don't use == or != because
  // timeStep is a float and can have rounding error.
  if ( timeStep > 2. * EPS ) {
    solve_ode_system( *this, timeStep );
  }

  // Get the plane that represents the surface where the player currently is
  pp::Plane surfacePlane = get_local_course_plane( position() );

  forceMinimumVelocity( &mVelocity, surfacePlane );
  mPosition = enforceMaxPenetration( mPosition, surfacePlane );

  mPosition = limitPosition( mPosition ); // Enforce playing area limits

//  std::cout << "position - x: " << mPosition.x << " y: " << mPosition.y << " z: " << mPosition.z << std::endl;

  pp::Vec3d surfaceNormal = surfacePlane.nml;
  float distanceFromSurface = surfacePlane.distance( position() );

  adjust_orientation( *this, timeStep, mPosition, mVelocity, distanceFromSurface, surfaceNormal );


  //Calculate information necessary to draw Tux
  pp::Vec3d tempVelocity = velocity();
  float speed = tempVelocity.normalize();

  // calculate force in Tux's local coordinate system
  pp::Vec3d local_force = orientation.conjugate().rotate(net_force);

  float paddling_factor;
  float flap_factor = 0;

  if ( mPaddling ) {
    //calculate the time that the player has been paddling
	  double factor = (gameMgr->time - mPaddleTime) / 0.40; //was / PADDLING_DURATION
	  if ( airborne ) {
	      paddling_factor = 0;
	      flap_factor = factor;
	  } else {
	      paddling_factor = factor;
	      flap_factor = 0;
	  }
  } else {
	  paddling_factor = 0.0;
    //flap factor = 0?
  }

/*
  if (jumping) {
    flap_factor = (gameMgr->time - jump_start_time) / 
      JUMP_FORCE_DURATION;
  }
*/
//  ModelHndl->adjust_tux_joints( mTurnAnimation, mBraking,
//	       paddling_factor, speed, local_force, flap_factor );

}

//! Helper function to calculate the turn animation factor
void HumanRacer::updateTurnAnimation(float timeStep)
{
	mTurnAnimation += mTurnFactor * timeStep * 3.;

  //Limit mTurnAnimation to -1:1
  if(mTurnAnimation > 1){
    mTurnAnimation = 1;
  }
  else if(mTurnAnimation < -1){
    mTurnAnimation = -1;
  }

	// Automatically center the turn animation
	if ( timeStep < ROLL_DECAY_TIME_CONSTANT ){
    mTurnAnimation *= 1.0 - timeStep/ROLL_DECAY_TIME_CONSTANT;
	}
  else { // By ROLL_DECAY_TIME_CONSTANT, Tux will be completely centered.
    mTurnAnimation = 0.0;
	}

}

void HumanRacer::keyboardEvent(SDLKey key, bool release)
{
  if(key == SDLK_UP){
		mPaddling = !release;
	}
  else if(key == SDLK_DOWN){
    mBraking = !release;
  }
  else if(key == SDLK_RIGHT){
		mRightTurn = !release;
	}
  else if(key == SDLK_LEFT){
		mLeftTurn = !release;
	}

  mTurnFactor = mRightTurn - mLeftTurn; //this will be -1 if left is pressed, 0 if neither or both are pressed

  //Jumping

}

