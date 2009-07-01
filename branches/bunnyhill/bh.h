// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#ifndef BH_H
#define BH_H

// first some settings to define the OS

#define IS_UNIX 1
// --------------------------------------------------------------------
// Includes
//
// This header is loaded by all moduls, so it's not necessary to 
// include the common headers in the other modules
// --------------------------------------------------------------------

// the following includes seem to be used by Unix and Windows
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <float.h>
#include <sys/stat.h>

#	include <pwd.h>
#   include <unistd.h>
#   include <sys/types.h>
#   include <pwd.h>
#   include <dirent.h>
#   include <sys/time.h>
#   include <sys/types.h>
#   include <dirent.h>
#	include <GL/glx.h>

// Linux or mingw
#define SEP "/"

// native Windows compiler:
//#define SEP "\\"

#include <png.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"

#include <string>
#include <iostream>

// not used yet
#define PROGDIR "/usr/local/games/bh"
#define CONFIG_DIR ".bunnyhill_new"

// --------------------------------------------------------------------
//	type declarations
// --------------------------------------------------------------------

#define float GLfloat

// common types
typedef struct {int i, j;}				TIndex2;
typedef struct {int i, j, k;}			TIndex3;
typedef struct {int i, j, k, l;}		TIndex4;
typedef struct {float x, y;}			TVector2;
typedef struct {float x, y, z;}			TVector3;
typedef struct {float x, y, z, w;} 		TVector4;
typedef struct {float x, y, z, w;}		TQuaternion;
typedef struct {GLbyte r, g, b, a;}		TIntColorGL;
typedef struct {float r, g, b, a;}		TColor;
typedef struct {TVector3 nml; float d;}	TPlane;

typedef struct {TVector3 vert[3]; TVector3 nml; float pconst;} TTriangle;
typedef struct {TVector3 vert[4]; TVector3 nml; float pconst;} TQuad;
typedef struct {float left, right, bottom, top;}               TRegion;

typedef float	TMatrix[4][4]; 
typedef float	TMatrixGL[16];
typedef int		TVIndex3[3];
typedef int		TVIndex4[4];

#define True 1
#define False 0

typedef struct {
	TVector3 vert[3];
	TVector2 coeff[3];
	TVector3 nml;
	float pconst;
	float frictcoeff;
	float depth;
	short int idx;
} TTriangleC;

typedef struct {
	TVector3 vert[4];
	TVector2 coeff[4];
	TVector3 nml;
	float pconst;
	float frictcoeff;
	float depth;
	short int idx;
} TQuadC;

// program modes (see loop.cpp)
typedef enum {
    NO_MODE,
    SPLASH,
	MAINMENU,
	COURSEMENU,
	CAMPAIGNMENU,
	CAMPAIGNINTRO,
	PLAYERMENU,
    LOADING,
	START,
    RACING,
    FINISH,
    PAUSED,
	TOOLSMENU,
	TOOLS,
    NUM_GAME_MODES
} TGameMode;

// kind of camera
typedef enum {
    BEHIND,					// follows exactly the character
    FOLLOW,					// with interpolation
	ABOVE,					// always in -z direction
    NUM_VIEW_MODES
} TViewMode;

typedef enum {
	NONE,
	TUXSHAPE,
	KEYFRAME,
	TREEGEN,
	LEARN,
} TToolMode;

typedef enum {
	TRAINING,				// no challenge
	TIME,					// time has priority
	HERRING,				// herring priority
	TIME_HERRING,			// standard challenge
	SCORE,					// points
	TIME_TIME,				// race vs. opponent
} TRaceMode;

// important: TControl is now in player.h since it will be linked
// with a character. This change was necessary with regard do multiplaying

// TGameData controls the global program sequences
typedef struct {
	int argument;			// start parameter
    TGameMode mode;			// currend program mode              
    TGameMode prevmode; 	// necessary for PAUSED  
	float timestep;			// wanted time span between two frames, for adjusting the timer
	float frametime;		// for hud
	float normaltime;		// for hud   
	int  framecount; 		// for fps
	bool keylock;			// locks the keyboard when changing a mode
	int  toolmode;
	char test_frame[32];	// for progmode KEYFRAME

	int  racemode;			// kind of challenge
	int  level;				// later controlled by player
	int  course_id;			// the id refers to info
	int  campaign_id;
	int  race_id;
	int  location_id;		// environment location
	int  light_id;			// access about EnvInfo.icons[8]
	int  snow_id;
	int  wind_id;
	int  bonus;
	bool is_campaign;

	bool new_coursemenu;
	bool new_campaignmenu;
	bool new_campaignintro;
} TGameData;

// function types
typedef void (*TInitFunc)   (void);
typedef void (*TLoopFunc)   (float time_step);
typedef void (*TTermFunc)   (void);
typedef void (*TMouseFunc)  (int button, int state, int x, int y);
typedef void (*TMotionFunc) (bool active, int x, int y);
typedef void (*TAtexitFunc) (void);
typedef void (*TKeybFunc) (unsigned int key, bool special, bool release, int x, int y);
typedef struct {TInitFunc init; TLoopFunc loop; TTermFunc term;} TModeFunc;

// --------------------------------------------------------------------
//	often used headers are included here
// --------------------------------------------------------------------

#include "mathlib.h"
#include "common.h"
#include "config.h"
#include "gllib.h"
#include "player.h"
#include "sp.h"

// --------------------------------------------------------------------
//	declaration of main functions (defined in main.cpp)
// --------------------------------------------------------------------

void SetModeFuncs (TGameMode mode, TInitFunc init, TLoopFunc loop, TTermFunc term);
void SetGameMode (TGameMode mode);
bool ModePending ();
void SwapBuffers ();
void ShowCursor (bool visible);
void WinExit (int code);
void SetKeyboardFunc (TKeybFunc func);
void SetMouseFunc (TMouseFunc func);
void SetMotionFunc (TMotionFunc func);
void SetPassMotionFunc (TMotionFunc func);

// --------------------------------------------------------------------
//	global 
// --------------------------------------------------------------------

using namespace std;
extern TGameData game;		// variable for common program control

#endif
