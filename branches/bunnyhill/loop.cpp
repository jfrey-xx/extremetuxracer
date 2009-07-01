// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "loop.h"
#include "ogl.h"
#include "textures.h"
#include "course.h"
#include "view.h"
#include "physics.h"
#include "env.h"
#include "quadtree.h"
#include "hud.h"
#include "models.h"
#include "treegen.h"
#include "tools.h"
#include "keyframe.h"
#include "tux.h"
#include "particles.h"
#include "trackmarks.h"
#include "image.h"
#include "race.h"
#include "font.h"
#include "gui_menu.h"
#include "info.h"
#include "sound.h"

// This module controls the different program modes
// Each mode contains 3 important functions:
// - Init () for setting the keyboard and mouse function pointers etc.
// - Loop () for the actual procedures in the frames
// - Term () for doing the remaining work before entering the next mode
//
// Hint: Finally it will be better do divide this unit in several smaller
// units. At the moment the overview is more important for me

// --------------------------------------------------------------------
//	Splash screen
// --------------------------------------------------------------------

void SplashInit (void) { 
	ShowCursor (false);
	SetMouseFunc (0);
    SetMotionFunc (0);
	SetKeyboardFunc (0);
}

void SplashLoop (float time_step) {
	string mess = "loading resources, please wait ...";
	TColor fontcol = MakeColor (1, 1, 1, 1);

	CheckGLError ();
    ClearRenderContext ();
	SetGLOptions (GUI);
    SetupGuiDisplay ();

//	DrawCommonTexture (LOGO, (cfg.scrwidth-512)/2, 150, 1);
	DrawCommonTexture (ETR_LOGO, (cfg.scrwidth-512)/2, 150, 1);
	FT.SetProps ("normal", 25, fontcol);
	FT.DrawStringNY ((cfg.scrwidth - FT.GetWidth (mess.c_str())) / 2, 500, mess);

	SetViewport ();
    SwapBuffers ();

	// at this point most resources are loaded and most initializsation
	// is done.
	
	Course.Init ();	
	View.Init ();
	Models.Init ();
	Env.Init ();	
	Audio.Load ();
	LoadCharacter ();
	Course.LoadTerrainTypes ();
	Models.LoadObjectTypes ();
	CourseInfo.Load ();
	EnvInfo.Load (); // this must be done BEFORE CampaignInfo.Load
	CampaignInfo.Load ();
	if (CourseInfo.Count() < 1) {
		Message ("no courses found");
		WinExit (0);
	}

	SDL_Delay (100);
	SetGameMode (MAINMENU);
} 

void SplashTerm () {
}

// --------------------------------------------------------------------
//	main menu
// --------------------------------------------------------------------

int mmplay, mmquit, mmconfig, mmcredits, mmtools, mmcup, mmplayer;
static CMenu main_menu;

void MainmenuCallback (int idx, int select, float val) {
	TVector3 vv = MakeVec (idx, select, val);

	if (idx == mmplay) {
		game.course_id = 0;
		game.light_id = 0;
		game.snow_id = 0;
		game.wind_id = 0;
		SetGameMode (COURSEMENU);
	}
	if (idx == mmquit) WinExit (0);
	if (idx == mmcup) SetGameMode (CAMPAIGNMENU);
	if (idx == mmplayer) SetGameMode (PLAYERMENU);
	if (idx == mmtools) SetGameMode (TOOLSMENU);
}

void MainmenuKeys (unsigned int key, bool special, bool release, int x, int y){
	if (game.keylock) return;
	if (release) return;
	switch (key) {
		case 27: WinExit(0); break;
		case SDLK_q: WinExit(0); break;
		case SDLK_t: SetGameMode (TOOLSMENU); break;
		case SDLK_s: SetGameMode (COURSEMENU); break;
		case SDLK_c: SetGameMode (CAMPAIGNMENU); break;
		case SDLK_p: SetGameMode (PLAYERMENU); break;
 		default: main_menu.KeyAction (key, x, y);
	}
}

void MainmenuMouse (int button, int state, int x, int y) {
	if (game.keylock) return;
	main_menu.MouseAction (button, state, x, y);
}

void MainmenuMotion (bool active, int x, int y) {
	main_menu.MouseMotion (active, x, y);
}

void MainmenuInit (void) { 
	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	ShowCursor (true);

	SetMouseFunc (MainmenuMouse);
	SetMotionFunc (MainmenuMotion);
	SetKeyboardFunc (MainmenuKeys);
	main_menu.SetCallback (MainmenuCallback);

	float lb = 20;
	mmplay = main_menu.AddButton (lb + 0, cfg.scrheight-40, 110, 28, "Single race");
	mmcup = main_menu.AddButton (lb + 120, cfg.scrheight-40, 100, 28, "Campaign");
	mmplayer = main_menu.AddButton (lb + 230, cfg.scrheight-40, 100, 28, "Player");
	mmconfig = main_menu.AddButton (cfg.scrwidth-340, cfg.scrheight-40, 80, 28, "Config");
	mmcredits = main_menu.AddButton (cfg.scrwidth-250, cfg.scrheight-40, 80, 28, "Credits");
	mmtools = main_menu.AddButton (cfg.scrwidth-160, cfg.scrheight-40, 70, 28, "Tools");
	mmquit = main_menu.AddButton (cfg.scrwidth-80, cfg.scrheight-40, 60, 28, "Quit");
	main_menu.SetFocus (mmplay);
	Music.Play ("start_screen");
}

void MainmenuLoop (float time_step) {
	CheckGLError ();
	Audio.Update ();
	SetGLOptions (GUI);

	SetViewport ();
    ClearRenderContext ();
    SetupGuiDisplay ();
	DrawCommonTexture (TOPLEFT, 0, 0, 1.0);
	DrawCommonTexture (TOPRIGHT, cfg.scrwidth-256, 0, 1);
	DrawCommonTexture (BOTTOMLEFT, 0, cfg.scrheight-311, 1);
	DrawCommonTexture (BOTTOMRIGHT, cfg.scrwidth-256, cfg.scrheight-311, 1);
	DrawHLine (0, cfg.scrheight-55, cfg.scrwidth, cfg.scrheight, colWhite);
	DrawCommonTexture (ETR_LOGO, (cfg.scrwidth-(int)(512*0.8))/2, 100, 0.8);
	DrawCommonTexture (LOGO, (cfg.scrwidth-(int)(512*0.6))/2, 30, 0.6);

	FT.SetProps ("normal", 18, colWhite);
	FT.DrawTextNY (320, cfg.scrheight -120, "Player: First Racer");

	main_menu.Draw ();
	glFlush ();
    SwapBuffers ();
} 

void MainmenuTerm () { 
	main_menu.Reset (); 
	game.new_coursemenu = true;
 	game.new_campaignmenu = true;
}

// --------------------------------------------------------------------
//	course menu
// --------------------------------------------------------------------

static CMenu course_menu;
int cmplay, cmquit, cmtextblock, cmback, cmlist, cmwind, cmlight, cmsnow, cmlocation;
static float lb, tb;
static CSPList courselist (128);
int *lighticons;
int numLightIcons;

void CoursemenuCallback (int idx, int select, float val) {
	if (select < 0) SetGameMode (LOADING);
	else {
		if (idx == cmplay) SetGameMode (LOADING);
		if (idx == cmquit) WinExit (0);
		if (idx == cmback) SetGameMode (MAINMENU);
		if (idx == cmlist) {
			if (select < CourseInfo.Count()) {
				game.course_id = courselist.Flag (select);
				course_menu.SetText (cmtextblock, CourseInfo.GetDescC (game.course_id));
			} else SetGameMode (LOADING);
		}
		if (idx == cmlight) game.light_id = select;
		if (idx == cmsnow) game.snow_id = select;
		if (idx == cmwind) game.wind_id = select;
		if (idx == cmlocation) {
			game.location_id = select;
			course_menu.SetTexid (cmlocation, EnvInfo.GetTexid (game.location_id));
			lighticons = EnvInfo.GetIcons (game.location_id);
			numLightIcons = EnvInfo.GetNumLights (game.location_id);
			course_menu.SetIcons (cmlight, numLightIcons, lighticons);
			game.light_id = 0;
		}
	}
}

void CoursemenuKeys (unsigned int key, bool special, bool release, int x, int y){
	if (game.keylock) return;
	if (release) return;
	switch (key) {
		case 27: SetGameMode (MAINMENU); break;
		case SDLK_q: WinExit (0); break;
		case SDLK_c: course_menu.SetFocus (cmlist); break;
		case SDLK_e: course_menu.SetFocus (cmlocation); break;
		case SDLK_l: course_menu.SetFocus (cmlight); break;
		case SDLK_s: course_menu.SetFocus (cmsnow); break;
		case SDLK_w: course_menu.SetFocus (cmwind); break;
 		default: course_menu.KeyAction (key, x, y);
	}
}

void CoursemenuMouse (int button, int state, int x, int y) {
	if (game.keylock) return;
	course_menu.MouseAction (button, state, x, y);
}

void CoursemenuMotion (bool active, int x, int y) {
	course_menu.MouseMotion (active, x, y);
}

void CoursemenuInit (void) { 
	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
//	SDL_EnableKeyRepeat (0, 0);
	ShowCursor (true);

	SetMouseFunc (CoursemenuMouse);
	SetMotionFunc (CoursemenuMotion);
	SetKeyboardFunc (CoursemenuKeys);

	game.racemode = 0; // training
	game.is_campaign = false;

	// the flag new_coursemenu is set to true when entering the
	// coursemenu mode from main menu
	if (game.new_coursemenu) {
		course_menu.Reset ();

		courselist.Clear ();
		for (int i=0; i<CourseInfo.Count (); i++) {
			if (CourseInfo.IsPublic(i)) 
				courselist.Add (CourseInfo.GetName (i), i);			
		}

		course_menu.SetCallback (CoursemenuCallback);
		game.course_id = courselist.Flag (0);

		cmplay = course_menu.AddButton (20, cfg.scrheight-40, 70, 28, "Play");
		cmback = course_menu.AddButton (100, cfg.scrheight-40, 60, 28, "Back");
	
		lb = (cfg.scrwidth - 450) / 2;
		tb = (cfg.scrheight - 55 - 255) / 2;
	
		cmtextblock = course_menu.AddTextblock (lb, tb + 155, 192, 100, 12, 12);
		course_menu.SetText (cmtextblock, CourseInfo.GetDescC (game.course_id));
	
		cmlist = course_menu.AddList (lb + 210, tb, 220, 255, &courselist, 15);
		float lbb = (cfg.scrwidth - 268) / 2;
	
		cmlocation = course_menu.AddImageButton 
			(lbb, cfg.scrheight-137, 128, 64, EnvInfo.GetNumInfo ()); 		
		course_menu.SetTexid (cmlocation, EnvInfo.GetTexid (game.location_id));

		cmlight = course_menu.AddSelButton (lbb+150, cfg.scrheight-105, 1);
		lighticons = EnvInfo.GetIcons (game.location_id);
		numLightIcons = EnvInfo.GetNumLights (game.location_id);
		course_menu.SetIcons (cmlight, numLightIcons, lighticons);
	
		cmsnow = course_menu.AddSelButton (lbb + 200, cfg.scrheight-105, 2);
		course_menu.SetIcons (cmsnow, 4, 0);
	
		cmwind = course_menu.AddSelButton (lbb + 250, cfg.scrheight-105, 0);
		course_menu.SetIcons (cmwind, 4, 0);

		cmquit = course_menu.AddButton (cfg.scrwidth-80, cfg.scrheight-40, 60, 28, "Quit");
		game.new_coursemenu = false;	
	}
	course_menu.SetFocus (cmlist);
	Music.Play ("start_screen");
}

void CoursemenuLoop (float time_step) {
	CheckGLError ();
	Audio.Update ();
	SetGLOptions (GUI);

		SetViewport ();
		ClearRenderContext ();
		SetupGuiDisplay ();
		DrawCommonTexture (TOPLEFT, 0, 0, 1.0);
		DrawCommonTexture (TOPRIGHT, cfg.scrwidth-256, 0, 1);
		DrawCommonTexture (BOTTOMLEFT, 0, cfg.scrheight-311, 1);
		DrawCommonTexture (BOTTOMRIGHT, cfg.scrwidth-256, cfg.scrheight-311, 1);
		DrawHLine (0, cfg.scrheight-55, cfg.scrwidth, cfg.scrheight, colWhite);
	
		DrawFrame (lb, tb, 192, 144, cfg.scrheight, colBlack, colWhite, 2);
		DrawTexture (CourseInfo.GetTexid (game.course_id), (int)lb, (int)tb, 192, 144);
		
		course_menu.Draw ();
		glFlush ();
		SwapBuffers ();

} 

void CoursemenuTerm () { 
// no reset since the menu is used multiple times
}

// --------------------------------------------------------------------
//	campaign menu (select a campaign)
// --------------------------------------------------------------------

static CMenu campaign_menu;
static int emplay, emback, emquit, emtextblock, emlist;
static CSPList campaignlist (16);
static float eleft, etop;

void CampaignmenuCallback (int idx, int select, float val) {
	if (select < 0) SetGameMode (CAMPAIGNINTRO);
	else {
		if (idx == emplay) SetGameMode (CAMPAIGNINTRO);
		if (idx == emquit) WinExit (0);
		if (idx == emback) SetGameMode (MAINMENU);
		if (idx == emlist) {
			if (select < CampaignInfo.Count()) {
				game.campaign_id = select;
				campaign_menu.SetText (emtextblock, CampaignInfo.GetDescC (game.campaign_id));
				game.bonus = CampaignInfo.GetBonus (game.campaign_id);				
				game.racemode = CampaignInfo.GetRaceMode (game.campaign_id);
			} else SetGameMode (CAMPAIGNINTRO);
		}
	}
}

void CampaignmenuKeys (unsigned int key, bool special, bool release, int x, int y){
	if (game.keylock) return;
	if (release) return;
	switch (key) {
		case 27: SetGameMode (MAINMENU); break;
		case SDLK_RETURN: SetGameMode (CAMPAIGNINTRO); break;
		case SDLK_q: WinExit (0); break;
 		default: campaign_menu.KeyAction (key, x, y);
	}
}

void CampaignmenuMouse (int button, int state, int x, int y) {
	if (game.keylock) return;
	campaign_menu.MouseAction (button, state, x, y);
}

void CampaignmenuMotion (bool active, int x, int y) {
}

void CampaignmenuInit (void) { 
	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	ShowCursor (true);

	game.is_campaign = true;
	SetMouseFunc (CampaignmenuMouse);
	SetMotionFunc (CampaignmenuMotion);
	SetKeyboardFunc (CampaignmenuKeys);

	if (game.new_campaignmenu) {
		campaign_menu.Reset ();

		campaignlist.Clear ();
		for (int i=0; i<CampaignInfo.Count (); i++)
			campaignlist.Add (CampaignInfo.GetName (i), i);			

		campaign_menu.SetCallback (CampaignmenuCallback);
		game.campaign_id = 0;
		game.bonus = CampaignInfo.GetBonus (game.campaign_id);
		game.racemode = CampaignInfo.GetRaceMode (game.campaign_id);

		emplay = campaign_menu.AddButton (20, cfg.scrheight-40, 70, 28, "Play");
		emback = campaign_menu.AddButton (100, cfg.scrheight-40, 60, 28, "Back");
	
		eleft = (cfg.scrwidth - 450) / 2;
		etop = (cfg.scrheight - 55 - 255) / 2;
	
		emtextblock = campaign_menu.AddTextblock (eleft, etop+170, 192, 144, 12, 12);
		campaign_menu.SetText (emtextblock, CampaignInfo.GetDescC (game.campaign_id));
		emlist = campaign_menu.AddList (eleft + 210, etop+170, 220, 144, &campaignlist, 15);

		emquit = campaign_menu.AddButton (cfg.scrwidth-80, cfg.scrheight-40, 60, 28, "Quit");
		game.new_campaignmenu = false;	
	}

	campaign_menu.SetFocus (emlist);
	Music.Play ("start_screen");
}

void CampaignmenuLoop (float time_step) {
	CheckGLError ();
	Audio.Update ();
	SetGLOptions (GUI);

	SetViewport ();
    ClearRenderContext ();
    SetupGuiDisplay ();
	DrawCommonTexture (TOPLEFT, 0, 0, 1.0);
	DrawCommonTexture (TOPRIGHT, cfg.scrwidth-256, 0, 1);
	DrawCommonTexture (BOTTOMLEFT, 0, cfg.scrheight-311, 1);
	DrawCommonTexture (BOTTOMRIGHT, cfg.scrwidth-256, cfg.scrheight-311, 1);
	DrawHLine (0, cfg.scrheight-55, cfg.scrwidth, cfg.scrheight, colWhite);

	eleft = (cfg.scrwidth - 192) / 2;

	DrawFrame (eleft, etop, 192, 144, cfg.scrheight, colBlack, colWhite, 2);
	DrawTexture (CampaignInfo.GetTexid (game.campaign_id), (int)eleft, (int)(etop), 192, 144);

	campaign_menu.Draw ();
	glFlush ();
    SwapBuffers ();
} 

void CampaignmenuTerm () {
	game.new_campaignintro = true;
}

// --------------------------------------------------------------------
//	campaign intro screen 
// --------------------------------------------------------------------

static CMenu campaign_intro;
static int imquit, imback, implay;

void CampaignIntroCallback (int idx, int select, float val) {
	if (select < 0) SetGameMode (LOADING);
	if (idx == implay) SetGameMode (LOADING);
	if (idx == imquit) WinExit (0);
	if (idx == imback) SetGameMode (CAMPAIGNMENU);
}

void CampaignIntroKeys (unsigned int key, bool special, bool release, int x, int y){
	if (game.keylock) return;
	if (release) return;
	switch (key) {
		case 27: SetGameMode (CAMPAIGNMENU); break;
		case SDLK_q: WinExit (0); break;
		case SDLK_PLUS: if (game.level<2) game.level++; break;
		case SDLK_MINUS: if (game.level>0) game.level--; break;
 		default: campaign_intro.KeyAction (key, x, y);
	}
}

void CampaignIntroMouse (int button, int state, int x, int y) {
	if (game.keylock) return;
	campaign_intro.MouseAction (button, state, x, y);
}

void CampaignIntroMotion (bool active, int x, int y) {
}

void CampaignIntroInit (void) { 
	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	ShowCursor (true);

	SetMouseFunc (CampaignIntroMouse);
	SetMotionFunc (CampaignIntroMotion);
	SetKeyboardFunc (CampaignIntroKeys);

	if (game.new_campaignintro) {
		campaign_intro.Reset ();
		campaign_intro.SetCallback (CampaignIntroCallback);

		implay = campaign_intro.AddButton (20, cfg.scrheight-40, 70, 28, "Play");
		imback = campaign_intro.AddButton (100, cfg.scrheight-40, 60, 28, "Back");
		imquit = campaign_intro.AddButton (cfg.scrwidth-80, cfg.scrheight-40, 60, 28, "Quit");

		game.race_id = 0;
		game.course_id = CampaignInfo.GetCourseID (game.campaign_id, game.race_id);
		game.light_id = CampaignInfo.GetLightID (game.campaign_id, game.race_id);
		game.snow_id = CampaignInfo.GetSnowID (game.campaign_id, game.race_id);
		game.wind_id = CampaignInfo.GetWindID (game.campaign_id, game.race_id);
		game.location_id = CampaignInfo.GetLocationID (game.campaign_id, game.race_id);
		game.new_campaignintro = false;		
	}

	campaign_intro.SetFocus (implay);
	Music.Play ("start_screen");
}

void CampaignIntroLoop (float time_step) {
	int num_races;
	int course_id;
	string cname;
	float ww;
	string val;
//	char val[16];

	CheckGLError ();
	Audio.Update ();
	SetGLOptions (GUI);

	SetViewport ();
    ClearRenderContext ();
    SetupGuiDisplay ();

	DrawCommonTexture (TOPLEFT, 0, 0, 1.0);
	DrawCommonTexture (TOPRIGHT, cfg.scrwidth-256, 0, 1);
	DrawCommonTexture (BOTTOMLEFT, 0, cfg.scrheight-311, 1);
	DrawCommonTexture (BOTTOMRIGHT, cfg.scrwidth-256, cfg.scrheight-311, 1);
	DrawHLine (0, cfg.scrheight-55, cfg.scrwidth, cfg.scrheight, colWhite);

	FT.SetProps ("italic", 36, colDYell);
	string name = CampaignInfo.GetName (game.campaign_id);
	ww = FT.GetWidth (name.c_str()) + 40;
	float left = (cfg.scrwidth -ww) / 2;
	FT.DrawTextNY (left + 40, 80, name.c_str());
	DrawCup (left, 88, cfg.scrwidth);	

	float top = (cfg.scrheight - 200) / 2 + 40; 
	left = (cfg.scrwidth - 400) / 2;

	DrawFrame (left, top, 400, 200, cfg.scrheight, colWhite, colDBackgr, 2);
	num_races = CampaignInfo.GetNumRaces (game.campaign_id);
	for (int i=0; i<num_races; i++) {
		if (i == game.race_id) FT.SetProps ("normal", 22, colDYell);
		else FT.SetProps ("normal", 20, colWhite);
		course_id = CampaignInfo.GetCourseID (game.campaign_id, i);
		cname = CourseInfo.GetName (course_id);
		ww = FT.GetWidth (cname.c_str());		
		FT.DrawStringNY (left + 40, top + i*36 + 10, cname);
		bool tick = (i < game.race_id);
		DrawCheck (left+350, top+i*36+10, cfg.scrheight, tick);		
	}


	int maxbonus = CampaignInfo.GetBonus (game.campaign_id);
	DrawBonus ((cfg.scrwidth - maxbonus * 40) / 2, top - 60, cfg.scrheight, 
			game.bonus, maxbonus);
	DrawLevel (left+10, top +215, cfg.scrheight, game.level+1);

	DrawRacemode (left+180, top + 215, cfg.scrheight, HERRING, false);
	FT.SetProps ("normal", 18, colWhite);
	val = Int_StrN (CourseInfo.GetHerring (game.course_id, game.level));
	FT.DrawStringNY (left +240, top+220, val);

	DrawRacemode (left+290, top + 215, cfg.scrheight, TIME, false);
	val = Float_StrN (CourseInfo.GetTime (game.course_id, game.level), 1);
	FT.DrawStringNY (left +340, top+220, val);


	campaign_intro.Draw ();
	glFlush ();
    SwapBuffers ();
} 

void CampaignIntroTerm () { 
}

// --------------------------------------------------------------------
//	player menu 
// --------------------------------------------------------------------

static CMenu player_menu;
static int pmplay, pmback, pmquit;

void PlayermenuCallback (int idx, int select, float val) {
	if (idx == pmquit) WinExit (0);
	if (idx == pmback) SetGameMode (MAINMENU);
}

void PlayermenuKeys (unsigned int key, bool special, bool release, int x, int y){
	if (game.keylock) return;
	if (release) return;
	switch (key) {
		case 27: SetGameMode (MAINMENU); break;
		case SDLK_q: WinExit (0); break;
 		default: player_menu.KeyAction (key, x, y);
	}
}

void PlayermenuMouse (int button, int state, int x, int y) {
	if (game.keylock) return;
	player_menu.MouseAction (button, state, x, y);
}

void PlayermenuMotion (bool active, int x, int y) {
}

void PlayermenuInit (void) { 
	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	ShowCursor (true);

	SetMouseFunc (PlayermenuMouse);
	SetMotionFunc (PlayermenuMotion);
	SetKeyboardFunc (PlayermenuKeys);
	player_menu.SetCallback (PlayermenuCallback);

	player_menu.Reset ();
	pmplay = player_menu.AddButton (20, cfg.scrheight-40, 70, 28, "Play");
	pmback = player_menu.AddButton (100, cfg.scrheight-40, 60, 28, "Back");
	pmquit = player_menu.AddButton (cfg.scrwidth-80, cfg.scrheight-40, 60, 28, "Quit");
	player_menu.SetFocus (1);
	Music.Play ("start_screen");
}

void PlayermenuLoop (float time_step) {
	CheckGLError ();
	Audio.Update ();
	SetGLOptions (GUI);

	SetViewport ();
    ClearRenderContext ();
    SetupGuiDisplay ();
	DrawCommonTexture (TOPLEFT, 0, 0, 1.0);
	DrawCommonTexture (TOPRIGHT, cfg.scrwidth-256, 0, 1);
	DrawCommonTexture (BOTTOMLEFT, 0, cfg.scrheight-311, 1);
	DrawCommonTexture (BOTTOMRIGHT, cfg.scrwidth-256, cfg.scrheight-311, 1);
	DrawHLine (0, cfg.scrheight-55, cfg.scrwidth, cfg.scrheight, colWhite);

	DrawCommonTexture (LOGO, (cfg.scrwidth-410)/2, 40, 0.8);

	FT.SetProps ("italic", 18, colDYell);
	FT.DrawTextNY (140, 200, "This menu is not implemented yet.");
	FT.DrawTextNY (140, 225, "Here you can select a player or create a new player.");
	FT.DrawTextNY (140, 250, "Additionally you select a level and the character you");
	FT.DrawTextNY (140, 275, "want to play with.");

	player_menu.Draw ();
	glFlush ();
    SwapBuffers ();
} 

void PlayermenuTerm () { 
	player_menu.Reset (); 
}

// --------------------------------------------------------------------
//	loading
// --------------------------------------------------------------------

void LoadingInit (void) {
	ShowCursor (false);    
    SetMouseFunc (0);
    SetMotionFunc (0);
	Music.Play ("start_screen");
}

void LoadingLoop (float time_step) {
	char *mess = "Loading, please wait ...";
	if (game.argument != 1) {   // direct loading of test course
		CheckGLError ();
		Audio.Update ();
		SetGLOptions (GUI);
		ClearRenderContext ();
		SetupGuiDisplay ();
		FT.SetProps ("normal", 20, colWhite);
		float ww = FT.GetWidth (mess);
		FT.DrawTextNY ((cfg.scrwidth-ww)/2, 300, mess);
		SetViewport ();
		SwapBuffers ();
	}

	LoadCourse (); 
	LoadEnvironment ();

	// the loop function is called only once. After having loaded the 
	// course and environment the program switches immediately to the 
	// next program mode
    if (TuxStart.loaded) SetGameMode (START); else SetGameMode (RACING);
} 

void LoadingTerm () {
}

// --------------------------------------------------------------------
//	start (intro in Tuxracer)
// --------------------------------------------------------------------

void StartKeys (unsigned int key, bool special, bool release, int x, int y) {
	if (game.keylock) return;
	if (release) return;
	switch (key) {
		case 27: 
			if (game.argument == 1) WinExit (0);
			else SetGameMode (COURSEMENU); 
			break;
		case SDLK_c: cfg.show_herring = !cfg.show_herring; break;
		case SDLK_h: cfg.show_hud = !cfg.show_hud; break;
		case SDLK_s: Screenshot (); break;
		case 13: SetGameMode (RACING); break;
		default: SetGameMode (RACING); break;
	}
}

void StartInit () {
	TControl *ctrl = Player.ctrl(0);

	SDL_EnableKeyRepeat (0, 0);
	SetMouseFunc (0);
    SetMotionFunc (0);
	SetKeyboardFunc (StartKeys);
	
	ctrl->pos.x = Course.startpoint.x;
	ctrl->pos.z = Course.startpoint.y;
	ctrl->pos.y = Course.GetYCoord (ctrl->pos) + adj.radius;
    ctrl->vel = MakeVec (0, 0, -1);
	ctrl->firstorient = true;

	TuxStart.Init (ctrl->pos);	// start keyframe

    View.Reset ();
    View.SetViewMode (FOLLOW);
    View.RacingView (0.0001, ctrl); 
	Env.SetLight ();
	Env.SetFog ();	

	Particles.Init ();
	Snow.Init (game.snow_id, ctrl);
	Wind.Init (game.wind_id);
	Trackmarks.ResetCurrentTrack ();

	Race.Reset ();
	Models.ResetItems ();
	Music.Play ("racing");
}

void StartLoop (float timestep) {
	TControl *ctrl = Player.ctrl(0);

	CheckGLError ();
	Audio.Update ();
	ClearRenderContext ();

	if (TuxStart.active) TuxStart.Update (timestep, ctrl);
	else SetGameMode (RACING);

	View.RacingView (timestep, ctrl);
	Env.DrawSkybox (ctrl->viewpos);
	Env.DrawFog ();
	Env.UpdateLight ();

	DrawTerrain (ctrl->viewpos);
	Trackmarks.DrawPreviousTrack (ctrl);
	Trackmarks.DrawCurrentTrack (ctrl);
	Models.DrawObjects (ctrl);

	Wind.Update (timestep);
	Snow.Update (timestep, ctrl);
    DrawTux ();
    DrawHud (ctrl);
    SetViewport ();
    SwapBuffers ();
}

void StartTerm () {
}

// --------------------------------------------------------------------
//	racing
// --------------------------------------------------------------------

void RacingKeys (unsigned int key, bool special, bool release, int x, int y) {
	if (game.keylock) return;
	switch (key) {
		case SDLK_SPACE: if (!release) SetPlyrJumping (true); break;
		case SDLK_UP:    SetPlyrPaddling (!release); break;		
		case SDLK_DOWN:  SetPlyrBraking (!release); break;		
		case SDLK_LEFT:  SetPlyrTurnleft (!release); break;	
		case SDLK_RIGHT: SetPlyrTurnright (!release); break;	
		case SDLK_v:     SetExtPaddling (!release); break;
		case SDLK_b:     SetExtBraking (!release); break;

		case SDLK_p: if (!release) SetGameMode (PAUSED); break;	
		case SDLK_1: if (!release) View.SetViewMode (ABOVE); break;	
		case SDLK_2: if (!release) View.SetViewMode (FOLLOW); break;	
		case SDLK_3: if (!release) View.SetViewMode (BEHIND); break;	
		case SDLK_s: if (!release) Screenshot (); break;

		case SDLK_h: if (!release) cfg.show_hud     = !cfg.show_hud; break;
		case SDLK_f: if (!release) cfg.show_fps     = !cfg.show_fps; break;
		case SDLK_e: if (!release) cfg.show_energy  = !cfg.show_energy; break;
		case SDLK_c: if (!release) cfg.show_herring = !cfg.show_herring; break;
		case SDLK_ESCAPE: 
			if (!release) {
				if (game.argument==1) WinExit (0);
				else SetGameMode (COURSEMENU); 
			} break;
	}
}

void RacingInit (void) {
	TControl *ctrl = Player.ctrl(0);

	SetMouseFunc (0);
    SetMotionFunc (0);
	SetKeyboardFunc (RacingKeys);

	SetPlyrBraking (false);
	SetPlyrPaddling (false);
	SetPlyrJumping (false);
	SetPlyrTurnleft (false);
	SetPlyrTurnright (false);
	SetExtPaddling (false);
	SetExtBraking (false);

	ctrl->firstorient = true;
	ctrl->airborne = false;

    if (game.prevmode != PAUSED) {
		InitSimulation (ctrl);
		ctrl->jump_level = 0;
//	    View.Reset ();
    	View.SetViewMode (FOLLOW);
//	    View.RacingView (0.0001); 
		Env.SetLight ();	
		Env.SetFog ();		
	    Race.time = 0.0;
	}
	Music.Play ("racing");
}

void RacingLoop (float timestep) {
	TControl *ctrl = Player.ctrl(0);

	// --------- read the control variables  ----------------
	// all params are stored in an unsigned short and
	// are bit-coded. In the case of multiplaying (client server
	// system) this contol variable is sent to the server 
	// or (even better) requested by the server
	bool Paddling = ((steer &  1) > 0);
	bool Braking = ((steer &  2) > 0); 
	bool Turnleft = ((steer &  4) > 0);
	bool Turnright = ((steer &  8) > 0);
	bool Jumping = ((steer & 16) > 0); 
	bool ExtPaddling = ((steer & 32) > 0);
	bool ExtBraking = ((steer & 64) > 0); 

	if (Turnleft ^ Turnright) {
		if (Turnleft) ctrl->turn = -1; else ctrl->turn = 1;
	} else 	ctrl->turn = 0;

	ctrl->brake = Braking;
	ctrl->paddle = Paddling;
	ctrl->brakeX = ExtBraking;
	ctrl->paddleX = ExtPaddling;

	// this must be improved ...
    if  (ctrl->paddle && ctrl->is_paddling == false) {
		ctrl->is_paddling = true;
		ctrl->paddle_time = Race.time;
    }

	if (Jumping) {
		if (ctrl->jump) {
			SetPlyrJumping (false);
			ctrl->jump_level = 0;
			ctrl->jump = false;
		} else {
			if (ctrl->jump_level > 1) ctrl->jump = true;
		}
	}
	if (ctrl->jump_level < 5.0) ctrl->jump_level += timestep;
	// -------------------------------------------------------------

	// and now the actual loop
    CheckGLError ();
	Audio.Update ();
    ClearRenderContext ();

	UpdateSimulation (timestep, ctrl);
	View.RacingView (timestep, ctrl);

	Env.DrawSkybox (ctrl->viewpos);
	Env.DrawFog ();
	Env.UpdateLight ();
	DrawTerrain (ctrl->viewpos);

	Trackmarks.Add (timestep, ctrl);
	Trackmarks.DrawPreviousTrack (ctrl);
	Trackmarks.DrawCurrentTrack (ctrl);

	Models.DrawObjects (ctrl);

	Particles.GenerateBrakeParticles (timestep, ctrl);
	Particles.UpdateBrakeParticles (timestep, ctrl);
	Particles.DrawBrakeParticles (ctrl);	

	Curtain.Update (timestep, ctrl);
	Snow.Update (timestep, ctrl);
	Wind.Update (timestep);

	DrawTux ();
	DrawHud (ctrl);
	SetViewport ();
    SwapBuffers ();
    if (!ctrl->final_state) Race.time += timestep;
} 

static void RacingTerm (void) {
	Trackmarks.FinishTrack ();
}

// --------------------------------------------------------------------
//	paused
// --------------------------------------------------------------------

void PausedKeys (unsigned int key, bool special, bool release, int x, int y) {
	if (game.keylock) return;
	if (release) return;
	if (key == SDLK_s) Screenshot (); 
	else SetGameMode (RACING);
}

void PausedMouse (int button, int state, int x, int y) {
	if (game.keylock) return;
    SetGameMode (RACING);
}

void PausedMotion (bool active, int x, int y) {
	if (ModePending ()) return;
}

void PausedInit (void) {
    SetMouseFunc (PausedMouse);
    SetMotionFunc (PausedMotion);
	SetKeyboardFunc (PausedKeys);
	Music.Play ("racing");
}

void PausedLoop (float timestep) {
	TControl *ctrl = Player.ctrl(0);

    CheckGLError ();
	Audio.Update ();
	ClearRenderContext ();
	View.RacingView (0, ctrl);
	Env.UpdateLight ();
	Env.DrawSkybox (ctrl->viewpos);
	Env.DrawFog ();
	DrawTerrain (ctrl->viewpos);

	Trackmarks.DrawPreviousTrack (ctrl);
	Trackmarks.DrawCurrentTrack (ctrl);
	Models.DrawObjects (ctrl);

	Particles.UpdateBrakeParticles (timestep, ctrl);
	Particles.DrawBrakeParticles (ctrl);
	Snow.Update (timestep, ctrl);

	DrawTux ();
	DrawHud (ctrl);
    SetViewport ();
    SwapBuffers ();
} 

void PausedTerm () {
}


// --------------------------------------------------------------------
//	finish - game over
// --------------------------------------------------------------------

CKeyframe *final_frame;
static CMenu finish_menu;
int fmrepeat, fmsel, fmquit, fmmain, fmback;
static int action = 0;
static char *succ1 = "You have won this race";
static char *succ2 = "Congratulations, you have accomplished this campaign";
static char *fail1 = "Sorry, you must repeat this race";
static char *fail2 = "Sorry, you have failed. Campaign is over";
static char *train = "Training";
static char *finalmsg;
static TColor msgcol;

void FinishKeys (unsigned int key, bool special, bool release, int x, int y) {
	if (game.keylock) return;
	if (release) return;

	if (game.is_campaign) {
		switch (key) {
			case 27: 
				if (game.argument==1) WinExit (0);
				else SetGameMode (CAMPAIGNMENU); 
				break;
			case SDLK_s: Screenshot (); break;
			case SDLK_q: WinExit (0); break;
			default: finish_menu.KeyAction (key, x, y);
		}
	} else {
		switch (key) {
			case 27: 
				if (game.argument==1) WinExit (0);
				else SetGameMode (COURSEMENU); 
				break;
			case SDLK_s: Screenshot (); break;
			case SDLK_q: WinExit (0); break;
			case SDLK_r: SetGameMode (LOADING); break;
			case SDLK_c: SetGameMode (COURSEMENU); break;
			case SDLK_m: SetGameMode (MAINMENU); break;
			default: finish_menu.KeyAction (key, x, y);
		}
	}
}

void FinishMouse (int button, int state, int x, int y) {
		if (game.keylock) return;
		finish_menu.MouseAction (button, state, x, y);
}

void FinishmenuCallback (int idx, int select, float val) {
	if (game.is_campaign) {
		if (idx == fmback) {
			switch (action) {
			case 1: // sucess and last race
				game.new_campaignmenu = true;				
				SetGameMode (CAMPAIGNMENU); 
				break;
			case 2: // sucess and not last race
				game.race_id++;
				game.course_id = CampaignInfo.GetCourseID (game.campaign_id, game.race_id);
				game.light_id = CampaignInfo.GetLightID (game.campaign_id, game.race_id);
				game.snow_id = CampaignInfo.GetSnowID (game.campaign_id, game.race_id);
				game.wind_id = CampaignInfo.GetWindID (game.campaign_id, game.race_id);
				game.location_id = CampaignInfo.GetLocationID (game.campaign_id, game.race_id);
				SetGameMode (CAMPAIGNINTRO); 
				break;
			case 3: // failed but bonus left
				SetGameMode (CAMPAIGNINTRO);
				break;				
			case 4: // failed and no bonus
				game.new_campaignmenu = true;				
				SetGameMode (CAMPAIGNMENU); 
				break;
			}
		}
	} else {
		if (idx == fmrepeat) SetGameMode (LOADING);
		if (idx == fmsel) SetGameMode (COURSEMENU);
		if (idx == fmmain) SetGameMode (MAINMENU);
		if (idx == fmquit) WinExit (0);
	}
}

void FinishInit (void) {
	TControl *ctrl = Player.ctrl(0);


    SetMouseFunc (FinishMouse); 
    SetMotionFunc (0);
	SetKeyboardFunc (FinishKeys);
	ShowCursor (true);
	finish_menu.SetCallback (FinishmenuCallback);

	if (game.is_campaign) {
		if (Race.RaceSuccess(ctrl)) final_frame = &TuxSuccess;
		else final_frame = &TuxFailure;	
		final_frame->Init (ctrl->pos);

		// success:
		if (Race.RaceSuccess(ctrl)) {
			bool lastrace = (game.race_id >= CampaignInfo.GetNumRaces (game.campaign_id)-1);
			if (lastrace) {
				fmback = finish_menu.AddButton (
				(cfg.scrwidth-80)/2, cfg.scrheight-40, 80, 28, "Back");
				finalmsg = succ2;
				msgcol = colLYell;
				action = 1;
			} else {
				fmback = finish_menu.AddButton (
				(cfg.scrwidth-100)/2, cfg.scrheight-40, 100, 28, "Next race");
				msgcol = colLYell;
				finalmsg = succ1;
				action = 2;
			}
		// failed:
		} else {
			game.bonus--;
			if (game.bonus > 0) {
				fmback = finish_menu.AddButton (
				(cfg.scrwidth-100)/2, cfg.scrheight-40, 100, 28, "Repeat");
				finalmsg = fail1;
				msgcol = colOrange;
				action = 3;
			} else {
				fmback = finish_menu.AddButton (
				(cfg.scrwidth-80)/2, cfg.scrheight-40, 80, 28, "Back");
				finalmsg = fail2;
				msgcol = colOrange;
				action = 4;
			}
		}
		finish_menu.SetFocus (0);

	// single race
	} else {
		finalmsg = train;
		msgcol = colDBlue;

		if (game.racemode == 0) {
			final_frame = &TuxFinal;
		} else {
			if (Race.RaceSuccess(ctrl)) final_frame = &TuxSuccess;
			else final_frame = &TuxFailure;	
		}
		final_frame->Init (ctrl->pos);
	
		float lb = (cfg.scrwidth - 410) / 2;
		fmrepeat = finish_menu.AddButton (lb + 0, cfg.scrheight-40, 110, 28, "Race again");
		fmsel = finish_menu.AddButton (lb + 120, cfg.scrheight-40, 110, 28, "New course");
		fmmain = finish_menu.AddButton (lb + 240, cfg.scrheight-40, 110, 28, "Main menu");
		fmquit = finish_menu.AddButton (lb + 360, cfg.scrheight-40, 70, 28, "Quit");
		finish_menu.SetFocus (0);
	}
	Music.Play ("game_over");
}


void FinishLoop (float timestep) {
	TControl *ctrl = Player.ctrl(0);

    CheckGLError ();
	Audio.Update ();
	ClearRenderContext ();

	final_frame->Update (timestep, ctrl);

	View.FinishView (ctrl);
	Env.DrawSkybox (ctrl->viewpos);
	Env.DrawFog ();
	DrawTerrain (ctrl->viewpos);
	Trackmarks.DrawPreviousTrack (ctrl);
	Trackmarks.DrawCurrentTrack (ctrl);
	Models.DrawObjects (ctrl);
	Particles.UpdateBrakeParticles (timestep, ctrl);
	Particles.DrawBrakeParticles (ctrl);
	Snow.Update (timestep, ctrl);

	DrawTux ();
	DrawHud (ctrl);
		
	if (final_frame->active == false) {
	 	FT.SetProps ("bold", 24, msgcol);
		float ww = FT.GetWidth (finalmsg);
	 	FT.DrawTextNY ((cfg.scrwidth-ww) / 2, 55, finalmsg);
		DrawFinalMessage ();
		if (game.argument != 1) finish_menu.Draw ();
	}

	SetViewport ();
    SwapBuffers ();
} 

void FinishTerm () {
	Trackmarks.CopyTrack ();
	finish_menu.Reset ();
}

// --------------------------------------------------------------------
//	tools menu
// --------------------------------------------------------------------

static CMenu tools_menu;
static int tmquit, tmback, tmshape, tmframe, tmtree, tmlight;

void ToolsmenuCallback (int idx, int select, float val) {
	if (idx == tmquit) WinExit (0);
	if (idx == tmback) SetGameMode (MAINMENU);
	if (idx == tmshape) {
		game.toolmode = TUXSHAPE;
		SetGameMode (TOOLS);
	}
	if (idx == tmframe) {
//		TuxTest.Load (game.test_frame);
		game.toolmode = KEYFRAME;
		SetGameMode (TOOLS);
	}
}

void ToolsmenuKeys (unsigned int key, bool special, bool release, int x, int y){
	if (release) return;
	switch (key) {
		case 27: SetGameMode (MAINMENU); break;
		case SDLK_q: WinExit (0); break;
		case SDLK_s: 
			game.toolmode = TUXSHAPE;
			SetGameMode (TOOLS);
			break;
		case SDLK_k: 
			TuxTest.Load (game.test_frame);
			game.toolmode = KEYFRAME;
			SetGameMode (TOOLS);
			break;
 		default: tools_menu.KeyAction (key, x, y);
	}
}

void ToolsmenuMouse (int button, int state, int x, int y) {
	tools_menu.MouseAction (button, state, x, y);
}

void ToolsmenuMotion (bool active, int x, int y) {
}

void ToolsmenuInit (void) { 
	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	ShowCursor (true);

	SetMouseFunc (ToolsmenuMouse);
	SetMotionFunc (ToolsmenuMotion);
	SetKeyboardFunc (ToolsmenuKeys);
	tools_menu.SetCallback (ToolsmenuCallback);

	tools_menu.Reset ();
	tmback = tools_menu.AddButton (20, cfg.scrheight-40, 60, 28, "Back");
	tmshape = tools_menu.AddButton (90, cfg.scrheight-40, 120, 28, "Char shape");
	tmframe = tools_menu.AddButton (220, cfg.scrheight-40, 110, 28, "Keyframe");
	tmtree = tools_menu.AddButton (340, cfg.scrheight-40, 100, 28, "Treegen");
	tmlight = tools_menu.AddButton (450, cfg.scrheight-40, 70, 28, "Light");
	tmquit = tools_menu.AddButton (cfg.scrwidth-80, cfg.scrheight-40, 60, 28, "Quit");
	tools_menu.SetFocus (0);
	Music.Play ("start_screen");
}

void ToolsmenuLoop (float time_step) {
	CheckGLError ();
	SetGLOptions (GUI);

	SetViewport ();
    ClearRenderContext ();
    SetupGuiDisplay ();

	DrawHLine (0, cfg.scrheight-55, cfg.scrwidth, cfg.scrheight, colWhite);

	FT.SetProps ("normal", 18, colYellow);
	FT.DrawTextNY (140, 180, "The built-in tools are not implemented yet. ");
	FT.DrawTextNY (140, 225, "Char shape - a help for creating new characters.");
	FT.DrawTextNY (140, 250, "Keyframe - to test and edit own keyframes");
	FT.DrawTextNY (140, 275, "Treegen - the tree generator");
	FT.DrawTextNY (140, 300, "Light - a tool for adjusting the light conditions");
	FT.DrawTextNY (140, 350, "For a very first impression you can try the shape and");
	FT.DrawTextNY (140, 375, "keyframe button.");

	tools_menu.Draw ();
	glFlush ();
    SwapBuffers ();
} 

void ToolsmenuTerm () { 
	tools_menu.Reset (); 
}

// --------------------------------------------------------------------
//	register
// --------------------------------------------------------------------

void RegisterLoopFuncs () {
    SetModeFuncs (SPLASH,   SplashInit,    SplashLoop,    SplashTerm);
    SetModeFuncs (MAINMENU, MainmenuInit,  MainmenuLoop,  MainmenuTerm);
    SetModeFuncs (COURSEMENU, CoursemenuInit,  CoursemenuLoop,  CoursemenuTerm);
    SetModeFuncs (CAMPAIGNMENU,  CampaignmenuInit,  CampaignmenuLoop,  CampaignmenuTerm);
    SetModeFuncs (CAMPAIGNINTRO, CampaignIntroInit, CampaignIntroLoop, CampaignIntroTerm);
    SetModeFuncs (PLAYERMENU, PlayermenuInit,  PlayermenuLoop,  PlayermenuTerm);
    SetModeFuncs (LOADING,  LoadingInit,   LoadingLoop,   LoadingTerm);
    SetModeFuncs (START,    StartInit,     StartLoop,     StartTerm);
    SetModeFuncs (RACING,   RacingInit,    RacingLoop,    RacingTerm);
    SetModeFuncs (PAUSED,   PausedInit,    PausedLoop,    PausedTerm);
    SetModeFuncs (FINISH,   FinishInit,    FinishLoop,    FinishTerm); 
    SetModeFuncs (TOOLSMENU, ToolsmenuInit, ToolsmenuLoop, ToolsmenuTerm);

}
