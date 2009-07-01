// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
//
// --------------------------------------------------------------------

#include "hud.h"
#include "ogl.h"
#include "textures.h"
#include "models.h"
#include "race.h"
#include "course.h"
#include "font.h"
#include "particles.h"
#include "info.h"

#define H_SIZE 128
#define H_WIDTH 128.0
#define GREEN_FRAC 0.5
#define GREEN_MAX 60
#define YELL_MAX 100
#define YELL_FRAC 0.25
#define RED_MAX 160
#define RED_FRAC 0.25
#define H_BASE_ANGLE 225
#define H_MAX_ANGLE 45
#define H_CIRCLE_DIV 12

#define ENERGY_GAUGE_BOTTOM 3.0
#define ENERGY_GAUGE_HEIGHT 103.0
#define ENERGY_GAUGE_CENTER_X 71.0
#define ENERGY_GAUGE_CENTER_Y 55.0
#define SPEEDBAR_OUTER_RADIUS 71

static float energy_background_color[] = {0.2, 0.2, 0.2, 0.0};
static float energy_foreground_color[] = {0.54, 0.59, 1.00, 0.5};
static float speedbar_background_color[] = {0.2, 0.2, 0.2, 0.0};
static float hud_white[] = {1.0, 1.0, 1.0, 1.0};

void GetTimeComponents (float time, int *min, int *sec, int *hundr) {
    *min = (int) (time / 60);
    *sec = ((int) time) % 60;
    *hundr = ((int) (time * 100 + 0.5) ) % 100;
}

void DrawMessageFrame (float x, float y, float w, float h, int line, 
		TColor backcol, TColor framecol, float transp) {
	float yy = cfg.scrheight - y - h;
 
	if (x < 0) 	x = (cfg.scrwidth - w) / 2;

	glPushMatrix();
	glDisable (GL_TEXTURE_2D);
    
	glColor4f (framecol.r, framecol.g, framecol.b, transp); 
	glTranslatef (x, yy, 0);
	glBegin (GL_QUADS );
	    glVertex2f (0, 0 );
	    glVertex2f (w, 0 );
	    glVertex2f (w, h );
	    glVertex2f (0, h );
	glEnd();

	glColor4f (backcol.r, backcol.g, backcol.b, transp);
	glBegin (GL_QUADS );
	    glVertex2f (0 + line, 0 + line );
	    glVertex2f (w - line, 0 + line );
	    glVertex2f (w - line, h - line );
	    glVertex2f (0 + line, h - line );
	glEnd();

	glEnable (GL_TEXTURE_2D);
    glPopMatrix();
}

void DrawFinalMessage () {
	TColor backcol = MakeColor (1, 1, 1, 1);
	TColor framecol = MakeColor (0.7, 0.7, 1, 1);
	float leftframe = (cfg.scrwidth - 400) / 2;
	float topframe = 120;
	string line;
	string valstr;
	string valstr2;
	char *mm[4];
	mm[0] = "NONE";
	mm[1] = "EASY";
	mm[2] = "MEDIUM";
	mm[3] = "DIFFICULT";

  	DrawMessageFrame (leftframe, topframe, 400, 150, 4, backcol, framecol, 0.5);
 	FT.SetProps ("normal", 17, colBlack);

	line = "Score:  ";
 	FT.DrawStringNY (leftframe+80, topframe+15, line);
	line = Int_StrN (Race.ScorePoints());
	line += "  pts";
 	FT.DrawStringNY (leftframe+240, topframe+15, line);

	line = "Herring:  ";
 	FT.DrawStringNY (leftframe+80, topframe+40, line);
	line = Int_StrN (Race.herring);
	if (game.racemode == TIME_HERRING) {
		line += "  (";
		line += Int_StrN (CourseInfo.GetHerring (game.course_id, game.level));
		line += ")";
	}
 	FT.DrawStringNY (leftframe+240, topframe+40, line);

	line = "Time:  ";
 	FT.DrawStringNY (leftframe+80, topframe+65, line);
	line = Float_StrN (Race.time, 2);
	line += "  s";
	if (game.racemode == TIME_HERRING) {
		line += "  (";
		line += Float_StrN (CourseInfo.GetTime (game.course_id, game.level), 2);
		line += ")";
	}
 	FT.DrawStringNY (leftframe+240, topframe+65, line);
		
	line = "Path length:  ";
 	FT.DrawStringNY (leftframe+80, topframe+90, line);
	line = Float_StrN (Race.way, 2);
	line += "  m";
 	FT.DrawStringNY (leftframe+240, topframe+90, line);
		
	line = "Average speed:  ";
 	FT.DrawStringNY (leftframe+80, topframe+115, line);		
	line = Float_StrN (Race.way / Race.time * 3.6, 2);
	line += "  km/h"; 
 	FT.DrawStringNY (leftframe+240, topframe+115, line);		
}


// --------------------------------------------------------------------
//				speed
// --------------------------------------------------------------------

TVector2 NewFanPoint (float angle) {
    TVector2 pt;
    pt.x = ENERGY_GAUGE_CENTER_X + cos (ANGLES_TO_RADIANS (angle)) *
	SPEEDBAR_OUTER_RADIUS;
    pt.y = ENERGY_GAUGE_CENTER_Y + sin (ANGLES_TO_RADIANS (angle)) *
	SPEEDBAR_OUTER_RADIUS;
    return pt;
}

void StartFan () {
    TVector2 pt;
    glBegin (GL_TRIANGLE_FAN);
    glVertex2f (ENERGY_GAUGE_CENTER_X, ENERGY_GAUGE_CENTER_Y);
    pt = NewFanPoint (H_BASE_ANGLE); 
    glVertex2f (pt.x, pt.y);
}

void DrawPartitialFan (float fraction) {
    int i;
    TVector2 pt;

    float angle = H_BASE_ANGLE + (H_MAX_ANGLE - H_BASE_ANGLE) * fraction;

    int divs = (int)((H_BASE_ANGLE - angle) * H_CIRCLE_DIV / 360.0);
    float cur_angle = H_BASE_ANGLE;
    float angle_incr = 360.0 / H_CIRCLE_DIV;
    bool trifan = false;

    for (i=0; i<divs; i++) {
		if (!trifan) {
		    StartFan();
	    	trifan = true;
		}
		cur_angle -= angle_incr;
		pt = NewFanPoint (cur_angle);
		glVertex2f (pt.x, pt.y);
    }

    if (cur_angle > angle + 0.0001) {
		cur_angle = angle;
		if (!trifan) {
		    StartFan();
	    	trifan = true;
		}
		pt = NewFanPoint (cur_angle);
		glVertex2f (pt.x, pt.y);
    }

    if (trifan) {
		glEnd();
		trifan = false;
    }
}

void DrawGauge (float speed, float energy) {
    float xplane[4] = {1.0 / H_SIZE, 0.0, 0.0, 0.0};
    float yplane[4] = {0.0, 1.0 / H_SIZE, 0.0, 0.0};

	BindCommonTex (5); // energy mask
    glTexGenfv (GL_S, GL_OBJECT_PLANE, xplane);
    glTexGenfv (GL_T, GL_OBJECT_PLANE, yplane);

    glPushMatrix();
	glTranslatef (cfg.scrwidth - H_WIDTH - 10, 10, 0);

	if (cfg.show_energy && game.mode != FINISH) {
		glColor4fv (energy_background_color);
		float y = ENERGY_GAUGE_BOTTOM + energy * ENERGY_GAUGE_HEIGHT;
		glBegin (GL_QUADS);
			glVertex2f (0.0, y);
			glVertex2f (H_SIZE, y);
			glVertex2f (H_SIZE, H_SIZE);
			glVertex2f (0.0, H_SIZE);
		glEnd();
		glColor4fv (energy_foreground_color);
		glBegin (GL_QUADS);
			glVertex2f (0.0, 0.0);
			glVertex2f (H_SIZE, 0.0);
			glVertex2f (H_SIZE, y);
			glVertex2f (0.0, y);
		glEnd();
	}

	float fact1 = RED_FRAC / (RED_MAX - YELL_MAX);
	float fact2 = YELL_FRAC / (YELL_MAX - GREEN_MAX);
	float spfrac = 0.0;
	if  (speed > adj.max_paddling_speed) {
	    spfrac = GREEN_FRAC;
	    if  (speed > YELL_MAX) {
			spfrac += YELL_FRAC;
			if (speed > RED_MAX) spfrac += RED_FRAC;
			else  spfrac += (speed - YELL_MAX) * fact1;
	    } else spfrac += (speed - GREEN_MAX) * fact2;	    
	} else spfrac += speed / GREEN_MAX * GREEN_FRAC;

	glColor4fv (speedbar_background_color);
	BindCommonTex (6); // speed mask
	DrawPartitialFan (1.0);
	glColor4fv (hud_white);
	DrawPartitialFan (MIN (1.0, spfrac));

	glColor4fv (hud_white);
	BindCommonTex (7);  // gauge_outline
	glBegin (GL_QUADS);
	    glVertex2f (0.0, 0.0);
	    glVertex2f (H_SIZE, 0.0);
	    glVertex2f (H_SIZE, H_SIZE);
	    glVertex2f (0.0, H_SIZE);
	glEnd();
    glPopMatrix();
}

void DrawSpeed (TControl *ctrl) {
	float speed;
	float energy;
	string speedstr;

	if (game.mode == START) {
		speed = 0.0;
	} else {
		if (!ctrl->final_state) speed = ctrl->realspeed; 
		else speed = Race.final_speed;
	}

	energy = MAX (0, (ctrl->jump_level - 1) / 4);
	speedstr = Int_StrN ((int)speed, 3);

    SetGLOptions (GAUGE_BARS);
	DrawGauge (speed, energy);
	SetGLOptions (TEXFONT);
	DrawNumStr (speedstr.c_str(), cfg.scrwidth - 100, cfg.scrheight - 85, 1, colWhite);
}

// --------------------------------------------------------------------
//					time
// --------------------------------------------------------------------

static void DrawTime (TControl *ctrl) {
    int min, sec, hundr;

	GetTimeComponents (Race.time, &min, &sec, &hundr);
	string timestr = Int_StrN (min, 2);
	timestr += ':';
	timestr += Int_StrN (sec, 2);
	DrawNumStr (timestr.c_str(), 65, 10, 1, colWhite);
	timestr = Int_StrN (hundr, 2);
	DrawNumStr (timestr.c_str(), 181, 10, 0.7, colWhite);
	DrawRacemode (10, 10, cfg.scrheight, TIME, !Race.TimeSuccess(ctrl));
}

// --------------------------------------------------------------------
//					herring
// --------------------------------------------------------------------

void DrawHerring () {
	string herringstr;
	herringstr = Int_StrN (Race.herring, 3);
	DrawNumStr (herringstr.c_str(), cfg.scrwidth-80, 10, 1, colWhite);
	DrawRacemode (cfg.scrwidth-150, 10, cfg.scrheight, HERRING, !Race.HerringSuccess());
}

// --------------------------------------------------------------------
//					fps
// --------------------------------------------------------------------

const  int   maxFrames = 40;
static int   numFrames = 0;
static float averagefps = 0;
static float sumTime = 0;

void DrawFps () {
	if (game.mode == FINISH || game.mode == START) return;
	string fpsstr;
	TColor col;

	if (numFrames >= maxFrames) {
		averagefps = 1/ sumTime * maxFrames;
		numFrames = 0;
		sumTime = 0;
	} else {
		sumTime += game.frametime;
		numFrames++;
	}
	if (averagefps < 1) return;

	if (cfg.show_fps) {
		if (averagefps >= 35) col = colWhite; else col = colRed;
		fpsstr = Float_StrN (averagefps, 0);
		DrawNumStr (fpsstr.c_str(), (cfg.scrwidth - 60) / 2, cfg.scrheight - 40, 1, col);
	} else {
		if (averagefps < 30)
			DrawQuad (4, cfg.scrheight - 14, 10, 10, cfg.scrheight, colRed, 0);
		else if (averagefps < 35)
			DrawQuad (4, cfg.scrheight - 14, 10, 10, cfg.scrheight, colOrange, 0);
		else if (averagefps < 40)
			DrawQuad (4, cfg.scrheight - 14, 10, 10, cfg.scrheight, colYellow, 0);
	}
}

void DrawWind (float dir, float speed, TControl *ctrl) {
	string windstr;
	if (game.wind_id < 1) return;

	DrawCommonTexture (8, 0, cfg.scrheight-140, 1.0);
    glDisable (GL_TEXTURE_2D );


	float alpha, red, blue, len;
	len = 45;
	if (speed <= 50) {
		alpha = speed / 50;
		red = 0;
	} else {
		alpha = 1.0;
		red = (speed - 50) / 50;
	}
	blue = 1.0 - red;

	glPushMatrix ();
	glColor4f (red, 0, blue, alpha);
	glTranslatef (72, 66, 0);
	glRotatef (dir, 0, 0, 1);
	glBegin (GL_QUADS);
	    glVertex2f (-5, 0.0);
	    glVertex2f (5, 0.0);
	    glVertex2f (5, -len);
	    glVertex2f (-5, -len);
	glEnd();
	glPopMatrix ();

	// direction indicator
	glPushMatrix ();
	glColor4f (0, 0.5, 0, 1.0);
	glTranslatef (72, 66, 0);
	glRotatef (ctrl->dir_angle+180, 0, 0, 1);
	glBegin (GL_QUADS);
	    glVertex2f (-2, 0.0);
	    glVertex2f (2, 0.0);
	    glVertex2f (2, -50);
	    glVertex2f (-2, -50);
	glEnd();
	glPopMatrix ();

    glEnable (GL_TEXTURE_2D );

 	DrawCommonTexture (21, 64, cfg.scrheight - 74, 1.0);
	windstr = Int_StrN ((int)speed, 3);
	DrawNumStr (windstr.c_str(), 47, cfg.scrheight - 55, 0.8, colWhite);
}


// --------------------------------------------------------------------
//					hud
// --------------------------------------------------------------------

void DrawHud (TControl *ctrl) {
	if (!cfg.show_hud) return;

    SetupGuiDisplay ();      // ortho perspective
    DrawSpeed (ctrl);
	DrawHerring ();
    DrawTime (ctrl);
    DrawFps ();
	DrawWind (Wind.Angle (), Wind.Speed (), ctrl);
}



