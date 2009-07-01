// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

// This unit is a container for OpenGL-orientated functions

#ifndef GLLIB_H
#define GLLIB_H

#include "bh.h"

// -------------------------- common ----------------------------------

TRegion MakeRegion (float l, float r, float b, float t);

void DrawHLine (float x, float y, float w, 
		float scrheight, TColor col);
void DrawTexquad (float x, float y, float w, float h,
		float scrheight, TRegion tex, GLuint texid);
void DrawRotatedTexquad (float x, float y, float w, float h,
		float scrheight, TRegion tex, GLuint texid);
void DrawQuad (float x, float y, float w, float h,
		float scrheight, TColor col, int frame);
void DrawFrame (TRegion r, TColor framecol, TColor backcol);
void DrawFrame (float x, float y, float w, float h, 
		float scrheight, TColor framecol, TColor backcol, int frame);

void DrawViewport (float x, float y, float w, float h,
		float scrheight, TColor framecol, TColor backcol);
void ResetViewport ();

// --------------- special with widgets.png ---------------------------

void DrawLevel (float x, float y, float scrheight, int level);
void DrawRacemode (float x, float y, float scrheight, int mode, bool active);
void DrawBonus (float x, float y, float scrheight, int num, int max);
void DrawCup (float x, float y, float scrheight);
void DrawCheck (float x, float y, float scrheight, bool check);

#endif

