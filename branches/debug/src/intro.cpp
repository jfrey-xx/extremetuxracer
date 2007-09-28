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

#include "intro.h"

#include "ppgltk/audio/audio.h"
#include "ppgltk/alg/defs.h"

#include "keyframe.h"
#include "course_render.h"
#include "gl_util.h"
#include "fps.h"
#include "loop.h"
#include "render_util.h"
#include "view.h"
#include "player.h"
#include "tux_shadow.h"
#include "fog.h"
#include "hud.h"
#include "viewfrustum.h"
#include "phys_sim.h"
#include "part_sys.h"
#include "course_load.h"
#include "joystick.h"
#include "game_config.h"
#include "winsys.h"
#include "course_mgr.h"

#include "game_mgr.h"

#include "bench.h"

Intro::Intro()
{
	int i, num_items;
    Item *item_locs;

    pp::Vec2d start_pt = get_start_pt();

    init_key_frame();

    gameMgr->time = 0.0;
    gameMgr->airbornetime = 0.0;
   
    
    for(i=0;i<gameMgr->numPlayers;i++) {
	    players[i].orientation_initialized = false;

	    players[i].view.initialized = false;

	    players[i].herring = 0;
	    players[i].score = 0;

		if(Benchmark::getMode()==Benchmark::PAUSED){
			set_start_pt(Benchmark::getPosition());
		}else{
			players[i].pos.x = start_pt.x;
	    		players[i].pos.z = start_pt.y;
		}
		 players[i].vel = pp::Vec3d(0,0,0);
	}
	
    init_physical_simulation();

   

    clear_particles();

	for(i=0;i<gameMgr->numPlayers;i++) {
	    set_view_mode( players[i], ABOVE );
	    update_view( players[i], EPS ); 
    }

    // reset all items as collectable 
    num_items = get_num_items();
    item_locs = get_item_locs();
    for (i = 0; i < num_items; i++ ) {
		item_locs[i].setCollected(false);
		/*		
		if(item_locs[i].getTypeCollectable()==Item::COLLECTED){
	    	item_locs[i].setCollectable(Item::YES);
		}
		*/
    }

    play_music( "intro" );
}

Intro::~Intro()
{
	
	
}

void
Intro::loop(float timeStep)
{
	int width, height;

    if ( getparam_do_intro_animation() == false ) {
		set_game_mode( RACING );
		return;
    }

    width = getparam_x_resolution();
    height = getparam_y_resolution();

    // Check joystick
    if ( is_joystick_active() ) {
		update_joystick();

		if ( is_joystick_continue_button_down() ) {
	    	abort( players[0] );
	    	return;
		}
    }
    
    fpsCounter.update();

    update_audio();

	int i;
	
	for(i=0;i<gameMgr->numPlayers;i++) {
    		update_key_frame( players[i], timeStep );
	}
	
    clear_rendering_context();

    fogPlane.setup();
    
	
    update_view( players[0], timeStep ); //only local player
	
    setup_view_frustum( players[0], NEAR_CLIP_DIST,  //--||--
			getparam_forward_clip_distance() );

    draw_sky( players[0].view.pos ); //--||--
	

    draw_fog_plane();

    set_course_clipping( true );
    set_course_eye_point( players[0].view.pos ); //--||--
    setup_course_lighting();
    render_course( );
    draw_trees();

    for(i=0;i<gameMgr->numPlayers;i++) {
    	players[i].Model->draw_tux();
    }
    draw_tux_shadow();

    HUD1.draw(players[0]); //Draw only local players hud

    reshape( width, height );
    winsys_swap_buffers();
	
	if( Benchmark::getMode() != Benchmark::NONE){
		abort(players[0]); //Abort only for local player
	}
}

void
Intro::abort( Player& plyr )
{
    pp::Vec2d start_pt = get_start_pt();

    set_game_mode( RACING );

	plyr.orientation_initialized = false;
    plyr.view.initialized = true;

	plyr.pos.x = start_pt.x;
    plyr.pos.z = start_pt.y;
    winsys_post_redisplay();
}

bool
Intro::keyPressEvent(SDLKey key)
{ 	
	abort( players[0] );//Abort only for local player
	
	return true;
}
