// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "config.h"

TConfig cfg;
TAdjust adj;

void SetDirectories (char *arg0) {
	// First set the main folders:

	// progdir is the base of all subdirs, so we must know this dir.
	// It's easy if the prog is installed in the predefined PROGDIR (bh.h).
	// There are no problems, too, if you start the program by typing ./bh.
	// Then you are in the progdir, and you can get this dir with getcwd.
	//
	// But what to do if you start the prog from any place? In this case
	// you must type the complete path beginning with the root folder. 
	// Now you can separate the folder from argv[0]. I don't know, is there
	// a better solution? 

	char buff[256];
	if (strcmp (arg0, "./bh") == 0) { 	// start from work directory
		getcwd (buff, 256);
	} else {								// start with full path
		strcpy (buff, arg0);		
		if (strlen (buff) > 5) buff[strlen(buff)-3] = 0;
	} 
	// strcpy (buff, PROGDIR); 				// if PROGDIR is predefined
	cfg.progdir = buff;

	// Not the dir for the config file
	// Normally it should be placed in the home dir,
	// but first let use the progdir/config

	cfg.config_dir = cfg.progdir + SEP + "config";

	// to get the home dir on Linux:
	// struct passwd *pwent;
    // pwent = getpwuid (getuid ());
	// cfg.config_dir = pwent->pw_dir;

	// Finally set the Subdirs:
	cfg.datadir = MakePathStr (cfg.progdir, "data");
	cfg.common_coursedir = MakePathStr (cfg.datadir, "courses");
	cfg.common_campaigndir = MakePathStr (cfg.datadir, "campaigns");
	cfg.terraindir = MakePathStr (cfg.datadir, "terrains");
	cfg.texturedir = MakePathStr (cfg.datadir, "textures");
	cfg.objectdir = MakePathStr (cfg.datadir, "objects");
	cfg.character_dir = MakePathStr (cfg.datadir, "characters");
	cfg.keyframe_dir = MakePathStr (cfg.character_dir, "keyframes");
	cfg.screenshot_dir = MakePathStr (cfg.datadir, "screenshots");
	cfg.font_dir = MakePathStr (cfg.datadir, "fonts");
	cfg.env_dir = MakePathStr (cfg.datadir, "environment");
	cfg.sound_dir = MakePathStr (cfg.datadir, "sounds");
	cfg.music_dir = MakePathStr (cfg.datadir, "music");
	cfg.develop_dir = MakePathStr (cfg.datadir, "develop");
}

void AddComment (CSPList &list, string comment)  {
	string line;
	line = "# " + comment;
	list.Add (line);
}

void SaveConfig () {
	CSPList list (1000);
	string line;

	AddComment (list, "fullscreen");
	AddComment (list, "If true then the game will run in full-screen mode.");
	AddComment (list, "");
	line = "[fullscreen] " + Bool_StrN (cfg.fullscreen);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "x_resolution");
	AddComment (list, "The horizontal size of the Tux Racer window");
	AddComment (list, "");
	line = "[xres] " + Int_StrN (cfg.scrwidth);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "y_resolution");
	AddComment (list, "The vertical size of the Tux Racer window");
	AddComment (list, "");
	line = "[yres] " + Int_StrN (cfg.scrheight);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "forward_clip_distance");
	AddComment (list, "Controls how far ahead of the camera the course");
	AddComment (list, "is rendered.  Larger values mean that more of the course is");
	AddComment (list, "rendered, resulting in slower performance. Decreasing this ");
	AddComment (list, "value is an effective way to improve framerates.");
	AddComment (list, "");
	line = "[forwardclip] " + Float_StrN (cfg.forwardclip, 0);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "backward_clip_distance");
	AddComment (list, "[EXPERT] Some objects aren't yet clipped to the view frustum, ");
	AddComment (list, "so this value is used to control how far up the course these ");
	AddComment (list, "objects are drawn.");
	AddComment (list, "");
	line = "[backwardclip] " + Float_StrN (cfg.backwardclip, 0);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "near_clip_distance");
	AddComment (list, "[EXPERT] This value is used for setting the viewport");
	AddComment (list, "");
	line = "[nearclip] " + Float_StrN (cfg.nearclip, 1);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "fov");
	AddComment (list, "[EXPERT] Sets the camera field of view");
	AddComment (list, "");
	line = "[fov] " + Float_StrN (cfg.fov, 0);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "bpp_mode");
	AddComment (list, "Controls how many bits per pixel are used in the game.");
	AddComment (list, "Valid values are:");
	AddComment (list, "0: Use current bpp setting of operating system");
	AddComment (list, "1: 16 bpp");
	AddComment (list, "2: 32 bpp");
	AddComment (list, "");
	line = "[bpp] " + Int_StrN (cfg.bpp);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "show_hud");
	AddComment (list, "If false then the hud displays will be hidden.");
	AddComment (list, "That may be advantageous for screenshots.");
	AddComment (list, "You can toggle this option during the race by typing 'h'");
	AddComment (list, "");
	line = "[show_hud] " + Bool_StrN (cfg.show_hud);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "show_fps");
	AddComment (list, "If your computer is fast enough (> 40 fps) you needn't show the fps.");
	AddComment (list, "");
	line = "[show_fps] " + Bool_StrN (cfg.show_fps);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "show_herring");
	AddComment (list, "If false the herrings are unvisible. This option seems to be");
	AddComment (list, "nonsense but it can be helpful for time measuring regardless");
	AddComment (list, "of the herrings.");
	AddComment (list, "");
	line = "[show_herring] " + Bool_StrN (cfg.show_herring);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "draw_particles");
	AddComment (list, "Controls the drawing of snow particles that are kicked up as Tux");
	AddComment (list, "turns and brakes. Setting this to false should help improve ");
	AddComment (list, "performance.");
	AddComment (list, "");
	line = "[draw_particles] " + Bool_StrN (cfg.draw_particles);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "draw_trackmarks");
	AddComment (list, "If true then the players will leave track marks in the snow.");
	AddComment (list, "");
	line = "[draw_trackmarks] " + Bool_StrN (cfg.draw_trackmarks);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "draw_previous_track");
	AddComment (list, "If true then the track of the previous race is shown.");
	AddComment (list, "");
	line = "[draw_previous_track] " + Bool_StrN (cfg.draw_previous_track);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "optimize_trackmarks");
	AddComment (list, "If true then the trackmarks are not calculated in each frame.");
	AddComment (list, "That improves performance but the track might appear less beautifully.");
	AddComment (list, "perfectly.");
	AddComment (list, "");
	line = "[optimize_trackmarks] " + Bool_StrN (cfg.optimize_trackmarks);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	AddComment (list, "full_skybox");
	AddComment (list, "On a normal course you never see the top, back and bottom view");
	AddComment (list, "of the skybox. In this case it's not required to draw them.");
	AddComment (list, "Set this option to true if you want to display the complete set");
	AddComment (list, "of 6 pics - provided that the pics exist.");
	AddComment (list, "");
	line = "[full_skybox] " + Bool_StrN (cfg.full_skybox);
	list.Add (line);
	list.Add ("");
	list.Add ("");

	list.Save (cfg.config_dir, "options.lst");	
}

bool LoadConfig () {
	CSPList list (1000);
	string line;
	if (!list.Load (cfg.config_dir, "options.lst")) {
		Message ("couldn't load options.lst");
		return false;
	}	
	for (int i=0; i<list.Count(); i++) {
		line = list.Line(i);
		cfg.fullscreen = SPBoolNX (line, "fullscreen", false);
		cfg.scrwidth = SPIntN (line, "xres", 1024);
		cfg.scrheight = SPIntN (line, "yres", 768);
		cfg.forwardclip = SPFloatN (line, "forwardclip", 75);
		cfg.backwardclip = SPFloatN (line, "backwardclip", 20);
		cfg.nearclip = SPFloatN (line, "nearclip", 0.1);
		cfg.fov = SPIntN (line, "fov", 60);
		cfg.bpp = SPIntN (line, "bpp", 2);
		cfg.show_hud = SPBoolNX (line, "show_hud", true);
		cfg.show_fps = SPBoolNX (line, "show_fps", false);
		cfg.show_herring = SPBoolNX (line, "show_herring", true);
		cfg.draw_particles = SPBoolNX (line, "draw_particles", true);
		cfg.draw_trackmarks = SPBoolNX (line, "draw_trackmarks", true);
		cfg.draw_previous_track = SPBoolNX (line, "draw_previous_track", true);
		cfg.optimize_trackmarks = SPBoolNX (line, "optimize_trackmarks", true);
		cfg.full_skybox = SPBoolNX (line, "full_skybox", false);
	}
	return true;
}

void InitAdjust () {
	adj.max_paddling_speed = 60;
	adj.radius = 0.35;
}

void InitConfig (char *arg0) {
	SetDirectories (arg0);

	// first read the configfile. It the file can't be read
	// the params are set do defaults
	if (LoadConfig () == false) {
		cfg.fullscreen = false;
		cfg.scrwidth = 1024;
		cfg.scrheight = 768;
		cfg.forwardclip = 125;
		cfg.backwardclip = 20;
		cfg.nearclip = 0.1;
		cfg.fov = 60;
		cfg.bpp = 2; 

		cfg.show_fps = true;
		cfg.show_hud = true;
		cfg.show_herring = true;

	 	cfg.draw_particles = true;
		cfg.draw_trackmarks = true;
		cfg.draw_previous_track = true;
		cfg.optimize_trackmarks = true;
		cfg.full_skybox = false;

		SaveConfig ();
	} 

	// set the other params to default:
	cfg.music_volume = 40;
	cfg.sound_volume = 100;
	cfg.play_music = true;
	cfg.sphere_divisions = 12;
	cfg.detail_level = 75; // quadtree
	cfg.viewmode = FOLLOW;
	cfg.use_cva = true;
	cfg.cva_hack = false;
	cfg.terr_blend = true;
	cfg.show_energy = true;
	cfg.interval = 25;
	cfg.save_itemslist = true;

	// some corrections
	if (!cfg.draw_trackmarks) cfg.draw_previous_track = false;
}









