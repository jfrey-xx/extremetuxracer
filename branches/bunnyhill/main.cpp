// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "bh.h"
#include "course.h"
#include "textures.h"
#include "physics.h"
#include "ogl.h"
#include "course.h"
#include "common.h"
#include "view.h"
#include "loop.h"
#include "models.h"
#include "treegen.h"
#include "tux.h"
#include "keyframe.h"
#include "tools.h"
#include "env.h"
#include "font.h"
#include "c_test.h"
#include "sound.h"
#include "info.h"

TGameData game;

const char *Title = "Bunny Hill";
const char *Icon = "Bunny Hill";

static SDL_Surface *screen = NULL;
static TGameMode new_mode = NO_MODE;
static TKeybFunc keyboard_func = NULL;
static TMouseFunc mouse_func = NULL;
static TMotionFunc motion_func = NULL;
static TModeFunc modefuncs [NUM_GAME_MODES];

bool ModePending () {return game.mode != new_mode; }
void SetGameMode (TGameMode mode) {new_mode = mode; }
void SwapBuffers () {SDL_GL_SwapBuffers (); }
void ShowCursor (bool visible) {SDL_ShowCursor (visible); }

void SetKeyboardFunc (TKeybFunc func) {keyboard_func = func; }
void SetMouseFunc (TMouseFunc func) {mouse_func = func; }
void SetMotionFunc (TMotionFunc func) {motion_func = func; }
void SetModeFuncs (TGameMode mode, TInitFunc init, TLoopFunc loop, TTermFunc term) {
    modefuncs[mode].init = init;
   	modefuncs[mode].loop = loop;
    modefuncs[mode].term = term;
}

// ---------------------- game params ---------------------------------
void SetGameParams (int argc, char **argv) {
	game.toolmode = NONE;
	if (argv[1] != NULL) game.argument = Str_IntN (argv[1], 0);
	else game.argument = 0;

	strcpy (game.test_frame, "tux_sad.lst");

	game.course_id = 0;
	game.campaign_id = 0;
	game.race_id = 0;
	game.location_id = 0;
	game.light_id = 0;
	game.racemode = 0; 
	game.level = 0;

	game.new_coursemenu = true;
	game.new_campaignmenu = true;
	game.new_campaignintro = true;
	game.keylock = true;
}
// -----------------------------------------------------------------


static void setup_sdl_video_mode () {
    Uint32 video_flags = SDL_OPENGL; 
    int bpp = 0;
    int width, height;

    if (cfg.fullscreen) video_flags |= SDL_FULLSCREEN;

    switch (cfg.bpp) {
    case 0:	bpp = 0; break;
    case 1:	bpp = 16; break;
    case 2:	bpp = 32; break;
    default:
		cfg.bpp = 0;
		bpp = cfg.bpp;
    }

    width = cfg.scrwidth;
    height = cfg.scrheight;

    if ((screen = SDL_SetVideoMode (width, height, 32, video_flags)) == 0) {
		printf ("Couldn't initialize video: %s",  SDL_GetError()); 	
    }
}

void InitWindow (int *argc, char **argv) {
    Uint32 sdl_flags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER;

    if (SDL_Init (sdl_flags) < 0) printf ("Couldn't initialize SDL");
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

	#if defined (USE_STENCIL_BUFFER )
    SDL_GL_SetAttribute (SDL_GL_STENCIL_SIZE, 8);
	#endif
    
	setup_sdl_video_mode ();
    SDL_WM_SetCaption (Title, Icon);
}

// this is the place for calling destructors (if not called 
// automatically) and for freeing resources
void WinExit (int code) {
	Audio.Shutdown ();
	SDL_Quit();
    exit (code);
}

void InitLoop () {
	for (int i=0; i<NUM_GAME_MODES; i++) {
		modefuncs[i].init = 0;
		modefuncs[i].loop = 0;
		modefuncs[i].term = 0;
	}
	keyboard_func = 0;
	mouse_func = 0;
	motion_func = 0;
	RegisterLoopFuncs ();
	RegisterToolFuncs ();
}

// --------------------------------------------------------------------
//					time
// --------------------------------------------------------------------
static float fpstime = 0.0;
static int fpsframes = 0;
static float clock_time;

void CalcTimeParams () {
	float cur_time = SDL_GetTicks() * 1.e-3;
	game.timestep = cur_time - clock_time;
	if (game.timestep < 0.0001) game.timestep = 0.0001;
	clock_time = cur_time;

	if (cfg.show_fps) { 
		fpstime += game.timestep;
		fpsframes++;
		if (fpstime >= 1.0) {
			game.framecount = fpsframes;
			fpstime = 0;
			fpsframes = 0;
		}
	}
} 

// ====================================================================
//						event loop
// ====================================================================

static float lasttick = 0;
static float elapsed_time;

void ProcessEvents () {
    SDL_Event event; 
    unsigned int key;
    int x, y;

    while (true) {
		while (SDL_PollEvent (&event)) {
	    	switch (event.type) {
		    case SDL_KEYDOWN:
			if (keyboard_func) {
			    SDL_GetMouseState (&x, &y);
			    key = event.key.keysym.sym; 
			    (*keyboard_func) (key, key >= 256, false, x, y);
			}
			break;

		    case SDL_KEYUP:
			if (keyboard_func) {
		    	SDL_GetMouseState (&x, &y);
			    key = event.key.keysym.sym; 
			    (*keyboard_func) (key, key >= 256, true, x, y);
			}
			break;

		    case SDL_MOUSEBUTTONDOWN:
	    	case SDL_MOUSEBUTTONUP:
			if (mouse_func) {
			    (*mouse_func) (event.button.button, event.button.state,
					   event.button.x, event.button.y );
			}
			break;

		    case SDL_MOUSEMOTION:
			if (event.motion.state) {
			    if (motion_func) (*motion_func) (true, event.motion.x, event.motion.y);
			} else {
			    if (motion_func) (*motion_func) (false, event.motion.x, event.motion.y);
			}
			break;

		    case SDL_VIDEORESIZE:
			setup_sdl_video_mode();
			SetViewport (event.resize.w, event.resize.h);
			View.Init ();
			break;
		
			case SDL_QUIT: 
			WinExit (0);
			break;
	    	}
		}

		// ---------------------------------------------------------------
		//	control of program modes
		// ---------------------------------------------------------------
	    if (game.mode != new_mode) {  // during changing the mode
			game.keylock = true;
			// 1. terminate function of previous mode
			if (game.mode >= 0 &&  modefuncs[game.mode].term != 0) 
			    (modefuncs[game.mode].term) ();
			game.prevmode = game.mode;
			game.mode = new_mode;
		
			// 2. init function of new mode
			if (modefuncs[game.mode].init != 0) {
				(modefuncs[game.mode].init) ();
 				clock_time = SDL_GetTicks() * 1.e-3;
			}
			game.keylock = false;
    	}

		// 3. the new mode is now the current mode
//    	CalcTimeParams (); // only if it runs on idle
		game.timestep = cfg.interval / 1000;
		if (modefuncs[game.mode].loop != 0) 
			(modefuncs[game.mode].loop) (game.timestep);



		if (game.mode > LOADING) {
			elapsed_time = SDL_GetTicks() - lasttick;
			game.frametime = elapsed_time / 1000;
			if (elapsed_time < cfg.interval ) {
				while (SDL_GetTicks() - lasttick < cfg.interval) {}
			} 
		}
		lasttick = SDL_GetTicks ();

		/* Experimental: -----------------------------------
		elapsed_time = SDL_GetTicks() - lasttick;
		remain_ticks = (int)(interval - elapsed_time);			
		if (remain_ticks >= 0) {
			SDL_Delay (remain_ticks);
		} else {
			PrintStr ("too slow");
			SDL_Delay(1);
		}
		lasttick = SDL_GetTicks ();
		--------------------------------------------------*/

		SDL_Delay (1);
    }
}

// ====================================================================
// 					main
// ====================================================================

int main (int argc, char **argv) {
	// first prepare the program
	SetGameParams (argc, argv);

	// with "./bh 99 " you start the test mode (without OpenGL)
	if (game.argument == 99) {C_Test (); return 0;}

	// some initialization:
	srand (time (NULL));
	InitConfig (argv[0]);
	InitAdjust ();
	InitLoop ();
    InitWindow (&argc, argv);
    InitGL ();
	Audio.Init ();

	//PrintGLInfo ();	// to find out the OpenGL version and properties

	// load only the resources needed for the splash screen
	LoadCommonTextures ();
	FT.LoadFontlist ();

	switch (game.argument) {
		case 0:
			// the other resources are loaded in splah mode
			game.mode = NO_MODE;
			SetGameMode (SPLASH);
	 		ProcessEvents();
	    	return 0;
			break;
		case 1:
			// in this case load the resources instantly, this
			// argument starts the first course in courses.lst without menus
			Course.Init ();	
			View.Init ();
			Models.Init ();
			Env.Init ();	
			Audio.Load ();
			LoadCharacter ();
			Course.LoadTerrainTypes ();
			Models.LoadObjectTypes ();
			CourseInfo.Load ();
			EnvInfo.Load ();
			cfg.show_fps = true;

			game.mode = NO_MODE;
			SetGameMode (LOADING);
	 		ProcessEvents();
	    	return 0;
			break;
	}	

} 

