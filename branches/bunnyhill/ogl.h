// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
//
// --------------------------------------------------------------------

// common functions related to OpenGL

#ifndef OGL_H
#define OGL_H

#include "bh.h"

typedef enum {
    GUI,
    GAUGE_BARS,
    TEXFONT,
    COURSE,
    TREES,
	MODELS,
    PARTICLES,
    TUX,
    TUX_SHADOW,
    SKY,
    FOG_PLANE,
    TRACK_MARKS,
} TRenderMode;

void InitGL (); 
void SetViewport ();
void SetViewport2 ();
void SetViewport (int width, int height);
void ClearRenderContext ();
void SetupGuiDisplay ();
void SetupGuiDisplay2 ();
void SetGLOptions (TRenderMode mode);
int  ReadGLOption ();
void PrintGLInfo ();
void CheckGLError ();
void SetupTexGen (float scale);
void SetMaterial (TColor diff, TColor spec, float exp);
void SetMaterial (TColor diff, TColor amb, TColor spec, float exp);

// The following pointers used by quadtree, they point to OpenGL extensions
// They don't seem to have any remarkable effect, so I've commented out
// the code in this unit as well as the function calls in quadtree
// (in DrawTris).

// extern PFNGLLOCKARRAYSEXTPROC glLockArraysEXT_p;
// extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT_p;

#endif
