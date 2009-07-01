// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "gllib.h"
#include "textures.h"

void DrawHLine (float x, float y, float w, 
		float scrheight, TColor col) {
	glDisable (GL_TEXTURE_2D);
    glColor4f (col.r, col.g, col.b, col.a);
	glBegin (GL_LINES);
		glVertex2f (x,   scrheight-y);
		glVertex2f (x+w, scrheight-y);
	glEnd();

	glEnable (GL_TEXTURE_2D);
}

TRegion MakeRegion (float l, float r, float b, float t) {
	TRegion res;
	res.left = l;
	res.right = r;
	res.bottom = b;
	res.top = t;
	return res;
}

void DrawTexquad (float x, float y, float w, float h,
		float scrheight, TRegion tex, GLuint texid) {
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture (GL_TEXTURE_2D, texid);

	glEnable (GL_TEXTURE_2D);
    glColor4f (1.0, 1.0, 1.0, 1.0);
	glBegin (GL_QUADS);
	    glTexCoord2f (tex.left, tex.bottom); 
			glVertex2f (x, cfg.scrheight-y-h);
	    glTexCoord2f (tex.right, tex.bottom); 
			glVertex2f (x+w, cfg.scrheight-y-h);
	    glTexCoord2f (tex.right, tex.top); 
			glVertex2f (x+w, cfg.scrheight-y);
	    glTexCoord2f (tex.left, tex.top); 
			glVertex2f (x, cfg.scrheight-y);
	glEnd();
}

void DrawRotatedTexquad (float x, float y, float w, float h,
		float scrheight, TRegion tex, GLuint texid) {
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture (GL_TEXTURE_2D, texid);

	glEnable (GL_TEXTURE_2D);
    glColor4f (1.0, 1.0, 1.0, 1.0);
	glBegin (GL_QUADS);
	    glTexCoord2f (tex.left, tex.top); 
			glVertex2f (x, cfg.scrheight-y-h);
	    glTexCoord2f (tex.left, tex.bottom); 
			glVertex2f (x+w, cfg.scrheight-y-h);
	    glTexCoord2f (tex.right, tex.bottom); 
			glVertex2f (x+w, cfg.scrheight-y);
	    glTexCoord2f (tex.right, tex.top); 
			glVertex2f (x, cfg.scrheight-y);
	glEnd();
}

void DrawQuad (float x, float y, float w, float h,
		float scrheight, TColor col, int frame) {
	glDisable (GL_TEXTURE_2D);
    glColor4f (col.r, col.g, col.b, col.a);
	glBegin (GL_QUADS);
		glVertex2f (x-frame,   scrheight-y-h-frame);
		glVertex2f (x+w+frame, scrheight-y-h-frame);
		glVertex2f (x+w+frame, scrheight-y+frame);
		glVertex2f (x-frame,   scrheight-y+frame);
	glEnd();
	glEnable (GL_TEXTURE_2D);
}

void DrawFrame (float x, float y, float w, float h, 
		float scrheight, TColor framecol, TColor backcol, int frame) {
	glDisable (GL_TEXTURE_2D);
    glColor4f (framecol.r, framecol.g, framecol.b, framecol.a);
	glBegin (GL_QUADS);
		glVertex2f (x-frame,   scrheight-y-frame-h);
		glVertex2f (x+frame+w, scrheight-y-frame-h);
		glVertex2f (x+frame+w, scrheight-y+frame);
		glVertex2f (x-frame,   scrheight-y+frame);
	glEnd();

    glColor4f (backcol.r, backcol.g, backcol.b, backcol.a);
	glBegin (GL_QUADS);
		glVertex2f (x,  scrheight-y-h);
		glVertex2f (x+w,scrheight-y-h);
		glVertex2f (x+w,scrheight-y);
		glVertex2f (x,  scrheight-y);
	glEnd();

	glEnable (GL_TEXTURE_2D);
}

void DrawFrame (TRegion r, TColor framecol, TColor backcol) {
	glDisable (GL_TEXTURE_2D);
    glColor4f (framecol.r, framecol.g, framecol.b, framecol.a);
	glBegin (GL_QUADS);
		glVertex2f (r.left  -2, cfg.scrheight - r.bottom -2);
		glVertex2f (r.right +2, cfg.scrheight - r.bottom -2);
		glVertex2f (r.right +2, cfg.scrheight - r.top    +2);
		glVertex2f (r.left  -2, cfg.scrheight - r.top    +2);
	glEnd();
    glColor4f (backcol.r, backcol.g, backcol.b, backcol.a);
	glBegin (GL_QUADS);
		glVertex2f (r.left, cfg.scrheight - r.bottom);
		glVertex2f (r.right, cfg.scrheight - r.bottom);
		glVertex2f (r.right, cfg.scrheight - r.top);
		glVertex2f (r.left, cfg.scrheight - r.top);
	glEnd();

	glEnable (GL_TEXTURE_2D);
}

void DrawViewport (float x, float y, float w, float h,
		float scrheight, TColor framecol, TColor backcol) {

	int xpos = (int)(x);
	int ypos = (int)(scrheight - y - h);

	glViewport (xpos, ypos, (int)w, (int)h);
	glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
	gluPerspective (45, w/h, 5, 70);
    glLoadIdentity ();
 	gluOrtho2D (0, w, 0, h);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

	glDisable (GL_TEXTURE_2D);
		glColor4f (framecol.r, framecol.g, framecol.b, framecol.a);
		glBegin (GL_QUADS);
			glVertex2f (0, 0);
			glVertex2f (0, h);
			glVertex2f (w, h);
			glVertex2f (w, 0);
		glEnd();
		glColor4f (backcol.r, backcol.g, backcol.b, backcol.a);
		glBegin (GL_QUADS);
			glVertex2f (2, 2);
			glVertex2f (w-2, 2);
			glVertex2f (w-2, h-2);
			glVertex2f (2, h-2);
		glEnd();
	glEnable (GL_TEXTURE_2D);


}

void ResetViewport () {
	glViewport (0, 0, (GLint)cfg.scrwidth, (GLint)cfg.scrheight);
	glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
	gluPerspective (45, cfg.scrwidth/cfg.scrheight, 5, 70);
    glLoadIdentity ();
 	gluOrtho2D (0, cfg.scrwidth, 0, cfg.scrheight);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
}

// ====================================================================

static const TRegion star_tex = {0.5, 0.625, 0.625, 0.75};
static const TRegion white_herr_tex = {0.5, 0.75, 0.875, 1.0};
static const TRegion yell_herr_tex = {0.5, 0.75, 0.75, 0.875};
static const TRegion white_clock_tex = {0.75, 0.875, 0.875, 1.0};
static const TRegion yell_clock_tex = {0.75, 0.875, 0.75, 0.875};
static const TRegion bonus_plus_tex = {0.875, 1.0, 0.875, 1.0};
static const TRegion bonus_minus_tex = {0.875, 1.0, 0.75, 0.875};
static const TRegion cup_tex = {0.75, 0.875, 0.625, 0.75};
static const TRegion check_tex = {0.875, 1.0, 0.625, 0.75};
static const TRegion tick_tex = {0.875, 1.0, 0.5, 0.625};

void DrawLevel (float x, float y, float scrheight, int level) {
	for (int i=0; i<level; i++) 
		DrawTexquad (x+i*24, y, 32, 32, scrheight, star_tex, TexID(T_WIDGETS));
}

void DrawRacemode (float x, float y, float scrheight, int mode, bool active) {
	TRegion hregion, tregion;
	if (active) {
		hregion = yell_herr_tex;
		tregion = yell_clock_tex;
	} else {
		hregion = white_herr_tex;
		tregion = white_clock_tex;
	}
		
	switch (mode) {
		case TIME: 
			DrawTexquad (x, y, 32, 32, scrheight, tregion, TexID(T_WIDGETS));
			break;
		case HERRING: 
			DrawTexquad (x, y, 64, 32, scrheight, hregion, TexID(T_WIDGETS));
			break;
		case TIME_HERRING: 
			DrawTexquad (x, y, 24, 24, scrheight, tregion, TexID(T_WIDGETS));
			DrawTexquad (x+36, y, 48, 24, scrheight, hregion, TexID(T_WIDGETS));
			break;
	}
}

void DrawBonus (float x, float y, float scrheight, int num, int max) {
	for (int i=0; i<max; i++) {
		if (i < num)
			DrawTexquad (x+i*40, y, 32, 32, scrheight, bonus_plus_tex, TexID(T_WIDGETS));
		else
			DrawTexquad (x+i*40, y, 32, 32, scrheight, bonus_minus_tex, TexID(T_WIDGETS));
	}
}

void DrawCup (float x, float y, float scrheight) {
	DrawTexquad (x, y, 32, 32, scrheight, cup_tex, TexID(T_WIDGETS));
}

void DrawCheck (float x, float y, float scrheight, bool check) {
	DrawTexquad (x, y, 32, 32, scrheight, check_tex, TexID(T_WIDGETS));
	if (check) DrawTexquad (x, y, 32, 32, scrheight, tick_tex, TexID(T_WIDGETS));
}



