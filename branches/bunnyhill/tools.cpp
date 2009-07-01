#include "tools.h"
#include "ogl.h"
#include "textures.h"
#include "env.h"
#include "tux.h"
#include "keyframe.h"

//static GLuint pave, grass, trans, smooth, test1, test2, tree;

void InitOpengl () {

/*
	pave = LoadTexture ("data", "pave01.png", false);
	grass = LoadTexture ("data", "grass01.png", false);
	trans = LoadTexture ("data", "trans.png", false);
	smooth = LoadTexture ("data", "smooth.png", false);
	test1 = LoadTexture ("data", "test1.png", false);
	test2 = LoadTexture ("data", "test2.png", false);
	tree = LoadTexture ("data", "tree1.png", false);
*/
//	GLint tmu;
//	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &tmu);
//	PrintInt (tmu);
}

/* --------------------------------------------------------------------
	glDisable (GL_BLEND);
	glEnable (GL_BLEND);
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHTING);
	glBindTexture (GL_TEXTURE_2D, grass); 

	glBlendFunc (GL_ZERO, GL_ZERO);
	glBlendFunc (GL_ONE, GL_ZERO);
	glBlendFunc (GL_ZERO, GL_ONE);
	glBlendFunc (GL_ONE, GL_ONE);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	glBlendFunc (GL_ONE, GL_ONE);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GL_ONE   GL_ZERO   
	GL_SRC_ALPHA             GL_DST_ALPHA
	GL_ONE_MINUS_RSC_ALPHA   GL_ONE_MINUS_DST_ALPHA   
	GL_SRC_COLOR             GL_DST_COLOR
	GL_ONE_MINUS_RSC_COLOR   GL_ONE_MINUS_DST_COLOR   
----------------------------------------------------------------------*/

static float xposition = 0;
static float yposition = 0;
static float zposition = -2;
static bool xplus = false;
static bool xminus = false;
static bool yplus = false;
static bool yminus = false;
static bool zplus = false;
static bool zminus = false;

static float aa = 0;
void TuxMonitor () {
	glPushMatrix ();
	Env.SetLight ();
	glTranslatef (xposition-0.5, yposition, zposition);	
	ResetTuxRoot ();
	SetTuxStandingOrientation2 (-0);
	ResetTuxJoints ();
	DrawTux ();

	glTranslatef (1, 0, 0);	
	ResetTuxRoot ();
 	SetTuxStandingOrientation2 (aa);
	ResetTuxJoints ();
	SetTuxAnimation2 ();
	aa += 2;
	DrawTux ();

	glPopMatrix ();
}

static bool keyrun = false;

void KeyMonitor (float timestep) {
	if (TuxTest.active) TuxTest.TestUpdate (timestep);
	else if (keyrun) {
		TuxTest.Init (NullVec);
		keyrun = false;
	}

	glPushMatrix ();
	Env.SetLight ();
	glTranslatef (xposition, yposition-0.5, zposition-1);	
	DrawTux ();
	glPopMatrix ();
}

void ToolsKeys (unsigned int key, bool special, bool release, int x, int y) {
	switch (key) {
		case 27: if (!release) SetGameMode (TOOLSMENU); break;
		case 13: if (!release) keyrun = true; break;
		case SDLK_UP:       yplus = (!release); break;
		case SDLK_DOWN:     yminus = (!release); break;
		case SDLK_LEFT:     xminus = (!release); break;
		case SDLK_RIGHT:    xplus = (!release); break;
		case SDLK_PAGEUP:   zminus = (!release); break;
		case SDLK_PAGEDOWN: zplus = (!release); break;
	}
}

static void ToolsMouse (int button, int state, int x, int y) {
}

void ToolsInit (void) {
    SetMouseFunc (ToolsMouse); 
    SetMotionFunc (0);
	SetKeyboardFunc (ToolsKeys);
	InitOpengl ();
	switch (game.toolmode) {
		case TUXSHAPE: break;
		case KEYFRAME: TuxTest.Init (NullVec); break;
		case TREEGEN: break;
	}
}


void ToolsLoop (float timestep) {
	if (xplus)  xposition += 0.02;
	if (yplus)  yposition += 0.02;
	if (zplus)  zposition += 0.02;
	if (xminus) xposition -= 0.02;
	if (yminus) yposition -= 0.02;
	if (zminus) zposition -= 0.02;

	CheckGLError ();
	ClearRenderContext ();
	switch (game.toolmode) {
		case TUXSHAPE: TuxMonitor (); break;
		case KEYFRAME: KeyMonitor (timestep); break;
		case TREEGEN: break;
	}
	SetViewport ();
	SwapBuffers ();
} 

void ToolsTerm () {}

void RegisterToolFuncs () {
    SetModeFuncs (TOOLS, ToolsInit, ToolsLoop, ToolsTerm);
}




