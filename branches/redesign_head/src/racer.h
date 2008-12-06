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
#ifndef _RACER_H_
#define _RACER_H_

#include "SDL.h" //for SDL key definitions
#include "pp_types.h"

class Racer
{
public:
  Racer();

  //! Main update function
  //! A crude motion is implemented by default, but can be overriden
  virtual void updatePosition(float timeStep);
  void updateView(float timeStep);
  void drawView(void);

  //! Key event handler.  Implemented in child classes
  virtual void keyboardEvent(SDLKey key, bool release);

  inline pp::Vec3d velocity(void) { return mVelocity; };
  inline void setVelocity(pp::Vec3d newVelocity) { mVelocity = newVelocity; }; 

  inline pp::Vec3d position(void) { return mPosition; };
  inline void setPosition(pp::Vec3d newPosition) { mPosition = newPosition; }; 


  //! Course percentage, a floating-point value from 0 to 100.
	float getCoursePercentage();

  inline bool paddling() { return mPaddling; };
  inline bool braking() { return mBraking; };
  inline bool turningRight() { return mRightTurn; };
  inline bool turningLeft() { return mLeftTurn; };
  inline double turnFactor() { return mTurnFactor; };

protected:
  //! Turn factor for the player.  Ranges from -1 to +1; 0 is straight.
  // Note that this will be exactly -1, 0, or 1 if the player is using the keyboard.
  float mTurnFactor;

  bool mPaddling;
  bool mBraking;
  bool mRightTurn;
  bool mLeftTurn;

  //!Time when the player was last paddling.  See the update_paddling() function
  //in phys_sim.cpp.
  float mPaddleTime;

  bool beginJump;
  bool jumping;
  bool jumpCharging;
  float jumpAmount;
  float jumpStartTime;

private:
  //keep track of herring or score?  both?
  int mHerring;

  //time to complete course?


  //! Maximum speed (what units?)
	int mMaximumSpeed;

  //stuff to handle the model
  //model()
  //setModel(Model)


protected:
  //! Current position
	pp::Vec3d mPosition;
	
  //! Current velocity (what units?)
	pp::Vec3d mVelocity;

//ugly hack...
//We'll fix this member-by-member.
public:	
	//! Current orientation.  Note that this is a 4-dimensional quantity.
	pp::Quat orientation; 
	
  //! Is orientation initialized?
	bool orientation_initialized; //do we eliminate this?
	
  //! Vector sticking out of bellybutton.  Is this what is normal to the surface?
	pp::Vec3d plane_nml;
	
	//! Vector sticking out of feet.
  //! Used partly to keep track of the direction of the frictional force.
	pp::Vec3d direction; 
	
  //! Force exerted on the player by the ground (normal force)
	pp::Vec3d normal_force;  

  //! Net force on player 
	pp::Vec3d net_force;
	
  //! Is the player in the air?
	bool airborne;
	
  //! Has the player collided with obstacle?
	bool collision;



  //! player's view point
	view_t view;

  //move these functions into a PlayerView class?
  inline void setViewMode(view_mode_t mode){ view.mode = mode; };
  inline view_mode_t setViewMode(void){ return(view.mode); };
  


};

#endif
