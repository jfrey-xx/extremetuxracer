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

#include "racer.h"
#include "phys_sim.h"


#include "model_hndl.h"
#include "game_mgr.h"

#include "view.h"

#include "course_render.h"
#include "fog.h"
#include "tux_shadow.h"
#include "viewfrustum.h"

#include "render_util.h" //only for NEAR_CLIP_DIST
#include "game_config.h" //for getparam_far_clip_dist...

#include <iostream>

Racer::Racer(void)
{

  //TODO: get the starting point from the course file
  mPosition.x = 35;
  mPosition.z = -3;

  //Find the height at which Tux should start based on his position in the course
	mPosition.y = find_y_coord(mPosition.x, mPosition.z);

	plane_nml = find_course_normal(mPosition.x, mPosition.z );

  pp::Matrix rotMat;
	rotMat.makeRotation( -90., 'x' );

  pp::Vec3d initialVelocity = rotMat.transformVector( plane_nml );
	initialVelocity = 3 * initialVelocity; //was INIT_TUX_SPEED
	mVelocity = initialVelocity;
//	direction = initialVelocity; //this clearly isn't right...

	net_force = pp::Vec3d( 0., 0., 0. );
	normal_force = pp::Vec3d(0,0,0);

	orientation_initialized = false;
	airborne = false;
}

void Racer::updatePosition(float timeStep)
{
  // This code drives Tux down the course in a straight line, hovering above the ground.
  mPosition.x = 50;
  mPosition.z -= .1; //forward is negative!

  mPosition.y = find_y_coord( mPosition.x, mPosition.z ) + .5;

  mPosition = limitPosition( mPosition ); // Enforce playing area limits
}

void Racer::updateView(float timeStep)
{
  update_view( *this, timeStep );
}

void Racer::drawView(void)
{
  fogPlane.setup();

  setup_view_frustum( view, NEAR_CLIP_DIST, getparam_forward_clip_distance() );

  draw_sky(view.pos);

  draw_fog_plane();

  set_course_clipping( true );
  set_course_eye_point( view.pos );
  setup_course_lighting();
  render_course();

//	draw_snow(players[0].view.pos);
	
  draw_trees();
/*
  if ( getparam_draw_particles() ) {
	  draw_particles( players[0] );
  }
*/
  ModelHndl->draw_tux();
  draw_tux_shadow();

//  HUD1.draw(players[0]);

}


void Racer::keyboardEvent(SDLKey key, bool release)
{

//Don't do anything by default

/*
Is there a more effecient way to get the information about which keys
control which players?  On the other hand, this is only being called for
each key press and release, so the events are relatively few.

	if(key==getparam_turn_left_key()){
		m_leftTurn = (bool) !release;
		return true;
	}else if(key==getparam_turn_right_key()){
		m_rightTurn = (bool) !release;
		return true;
	}else if(key==getparam_paddle_key()){
		m_paddling = (bool) !release;
		return true;
	}else if(key==getparam_brake_key()){
		m_braking = (bool) !release;
		return true;
	}else if(key==getparam_trick_modifier_key()){
		m_trickModifier = (bool) !release;
		return true;
	}else if(key==getparam_jump_key()){
		m_charging = (bool) !release;
		return true;
	}else{
		return false;
	}
*/

}

float getCoursePercentage()
{
  return(50);
}


