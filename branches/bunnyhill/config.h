// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#ifndef CONFIG_H
#define CONFIG_H

#include "bh.h"

using namespace std;

typedef struct {
	// folders
	string progdir;
	string config_dir;

	string datadir;
	string terraindir;
	string texturedir;
	string common_coursedir;
	string common_campaigndir;
	string objectdir;
	string character_dir;
	string keyframe_dir;
	string screenshot_dir;
	string font_dir;
	string env_dir;
	string sound_dir;
	string music_dir;
	string develop_dir;

	// screen params
	int scrwidth;				// width of the viewport
	int scrheight;				// height of viewport
	bool fullscreen;			// fullscreen if desired
	int fov;					// field of view ("focal length")
	int bpp;					// color depth: 0=default, 1=16, 2=32

	// audio
	bool play_music;
	int music_volume;
	int sound_volume;

	// visual range and density of details
	float nearclip;				// clipping distance, near
	float forwardclip;			// clipping distance forward
	float backwardclip;			// clipping distance backward
	int sphere_divisions;		// precision of drawing Tux
	int detail_level;

	// camera and viewing mode
	int  viewmode;				// kind (position) of camera
	bool use_cva;				
	bool cva_hack;
	bool terr_blend;

	// fps, performance
	float interval;       		// set to 25 ms, don't change

	// particles and trackmarks
	bool draw_particles;
	bool draw_trackmarks;
	bool draw_previous_track;	// trackmarks of the previous race
	bool optimize_trackmarks;	// if true, less marks, but less precision
	bool full_skybox;			// use 6 pics instead of 3

	// toggle
	bool show_hud;
	bool show_fps;     
	bool show_energy;
	bool show_herring;

	// items
	bool save_itemslist;		// true: no itemlist is generated and the trees.png
								// is read further on
} TConfig;


// most setting params are placed in the particular modules. It's intended
// do collect them in this structure. So you can adjust the behaviour of Tux
// in case of collision or with respect to steering, braking etc. The adjust
// structure can be stored in an adjust file similar to the config file. But
// that's only for experts
typedef struct {
	float max_paddling_speed;
	float radius;
} TAdjust;

// --------------------------------------------------------------------

void InitConfig (char *arg0);
void SaveConfig ();
bool LoadConfig ();
void InitAdjust ();

extern TConfig cfg;			// user configurations
extern TAdjust adj;			// variable for settings

#endif
