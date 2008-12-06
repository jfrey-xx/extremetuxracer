/* 
 * PPRacer 
 * Copyright (C) 2004-2005 Volker Stroebel <volker@planetpenguin.de>
 * 
 * Copyright (C) 1999-2001 Jasmin F. Patry
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

#include "racing.h"

#include "ppgltk/audio/audio.h"
#include "ppgltk/alg/defs.h"

#include "keyframe.h"
#include "course_render.h"
#include "gl_util.h"
#include "fps.h"
#include "loop.h"
#include "render_util.h"
#include "view.h"
#include "model_hndl.h"
#include "tux_shadow.h"
#include "phys_sim.h"
#include "part_sys.h"
#include "screenshot.h"
#include "fog.h"
#include "viewfrustum.h"
#include "track_marks.h"
#include "hud.h"
#include "joystick.h"
#include "snow.h"
#include "game_config.h"
#include "winsys.h"

#include "game_mgr.h"
#include "bench.h"

#include "ppgltk/ppgltk.h"

#include "humanracer.h"

#include <iostream>

Racing::Racing()
{
  HumanRacer* r = new HumanRacer();
  mRacers.push_back(r);

  // Initialize view; default to "above" view.
  if ( getparam_view_mode() < 0 || getparam_view_mode() >= NUM_VIEW_MODES ){
    setparam_view_mode( ABOVE );
  }
  mRacers[0]->setViewMode( (view_mode_t)getparam_view_mode() );

  if ( GameMode::prevmode != PAUSED ) {
    init_physical_simulation();
  }

  gameMgr->abortRace(false);

  //not using snow at the moment
  //	init_snow(players[0].view.pos);

  play_music( "racing" );	
}


Racing::~Racing()
{
  //TODO: stop all sound which are specified by the used theme
  halt_sound( "flying_sound" );
  halt_sound( "rock_sound" );
  halt_sound( "ice_sound" );
  halt_sound( "snow_sound" );
  break_track_marks();
}

void Racing::loop(float timeStep)
{
  fpsCounter.update();

//  update_audio();

  updatePhysics(timeStep);
  renderView();

  gameMgr->time += timeStep;
}

void Racing::updatePhysics(float timeStep)
{
  mRacers[0]->updatePosition(timeStep);
  mRacers[0]->updateView(timeStep);


//	update_snow( timeStep, false, players[0].view.pos );
/*
  if ( getparam_draw_particles() ) {
	  update_particles( timeStep );
  }*/

 //Track Marks
//  add_track_mark( players[0] );
}

void Racing::renderView(void)
{
  clear_rendering_context();

/* This code will let us draw multiple viewports. :-)
  glViewport( 0, 0, (GLint)getparam_x_resolution(), (GLint) getparam_y_resolution()/2);
  mRacers[0]->drawView();
  glViewport( 0, (GLint) getparam_y_resolution()/2, (GLint)getparam_x_resolution(), (GLint) getparam_y_resolution()/2);
  mRacers[0]->drawView();
*/

  mRacers[0]->drawView();

  //this can be simplified.  reshape() sets the x and y parameters, which means we do a 
  //get and set each loop through the function.  We still need to be able to set the x
  //and y, but reshape() isn't really the right place to do it.
	reshape(getparam_x_resolution(), getparam_y_resolution());

  winsys_swap_buffers();
}


bool Racing::keyboardEvent(SDLKey key, bool release)
{
  mRacers[0]->keyboardEvent(key, release); //Pass the event on to the racers

  return false; //assume that the event has not been handled so that keyPressEvent gets to run.
}

bool Racing::keyPressEvent(SDLKey key)
{
	switch(key){
		case 'q':
			gameMgr->abortRace();
  		set_game_mode( GAME_OVER );
  		return true;
		case SDLK_ESCAPE: 
			set_game_mode( PAUSED );
			return true;	
		case '1':
  		mRacers[0]->setViewMode( ABOVE );
  		setparam_view_mode( ABOVE );
			return true;
		case '2':
			mRacers[0]->setViewMode( FOLLOW );
			setparam_view_mode( FOLLOW );
			return true;
		case '3':
			mRacers[0]->setViewMode( BEHIND );
			setparam_view_mode( BEHIND );
			return true;	
		case 's':
  		screenshot();
			return true;
		case 'p':
			set_game_mode( PAUSED );
			return true;
		default:
			if(key==getparam_reset_key()){
				set_game_mode( RESET );
				return true;
			}
	}
		
	return false;
}
