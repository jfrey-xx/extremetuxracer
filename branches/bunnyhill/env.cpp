// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "env.h"
#include "textures.h"
#include "ogl.h"
#include "course.h"
#include "view.h"
#include "info.h"

// --------------------------------------------------------------------
//					defaults
// --------------------------------------------------------------------

static float def_amb[]     = {0.45, 0.53, 0.75, 1.0};    
static float def_diff[]    = {1.0, 0.9, 1.0, 1.0};    
static float def_spec[]    = {0.6, 0.6, 0.6, 1.0};    
static float def_pos[]     = {1, 2, 2, 0.0};    

static float def_fogcol[]  = {0.9, 0.9, 1.0, 0.0};
static float def_partcol[] = {0.8, 0.8, 0.9, 0.0};

TuxEnv Env;

TuxEnv::TuxEnv () {}
TuxEnv::~TuxEnv () {}

void TuxEnv::Init () {
	// lights:
	default_light.is_on = true;
	for (int i=0; i<4; i++) { 
		default_light.ambient[i]  = def_amb[i];
		default_light.diffuse[i]  = def_diff[i];
		default_light.specular[i] = def_spec[i];
		default_light.position[i] = def_pos[i];
	}
	ResetLight ();

	// fog (including particle color)
	default_fog.is_on = true;
	default_fog.mode = GL_LINEAR;
	default_fog.start = 20.0;
	default_fog.end = 70.0;
	default_fog.height = 0;
	for (int i=0; i<4; i++) { 
		default_fog.color[i] =  def_fogcol[i];
		default_fog.part_color[i] =  def_partcol[i];
	}
	ResetFog ();

	// skybox
	for (int i=0; i<6; i++) Skybox[i] = 0;
}

TColor TuxEnv::GetParticleColor () {
	TColor res;
	res.r = fog.part_color[0];
	res.g = fog.part_color[1];
	res.b = fog.part_color[2];
	res.a = fog.part_color[3];
	return res;
}

float *TuxEnv::GetGLParticleColor () {
	return fog.part_color;
}

// --------------------------------------------------------------------
//					skybox
// --------------------------------------------------------------------

void TuxEnv::ResetSkybox () {
	for (int i=0; i<6; i++) {
		if (Skybox[i] != 0) glDeleteTextures (1, &Skybox[i]);
		Skybox[i] = 0;
	}
}

void TuxEnv::DrawSkybox (TVector3 pos) {
	SetGLOptions (SKY);
  
	glColor4f (1.0, 1.0, 1.0, 1.0);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glPushMatrix();
	glTranslatef (pos.x, pos.y, pos.z);
	
	// front
	glBindTexture (GL_TEXTURE_2D, Skybox[0]);
	glBegin(GL_QUADS);
		glTexCoord2f (0.0, 0.0); glVertex3f (-1, -1, -1);
		glTexCoord2f (1.0, 0.0); glVertex3f ( 1, -1, -1);
		glTexCoord2f (1.0, 1.0); glVertex3f ( 1,  1, -1);
		glTexCoord2f (0.0, 1.0); glVertex3f (-1,  1, -1);
	glEnd();
	
	// left
	glBindTexture (GL_TEXTURE_2D, Skybox[1]);
	glBegin(GL_QUADS);
		glTexCoord2f (0.0, 0.0); glVertex3f (-1, -1,  1);
		glTexCoord2f (1.0, 0.0); glVertex3f (-1, -1, -1);
		glTexCoord2f (1.0, 1.0); glVertex3f (-1,  1, -1);
		glTexCoord2f (0.0, 1.0); glVertex3f (-1,  1,  1);
	glEnd();
	
	// right
	glBindTexture (GL_TEXTURE_2D, Skybox[2]);
	glBegin(GL_QUADS);
		glTexCoord2f (0.0, 0.0); glVertex3f (1, -1, -1);
		glTexCoord2f (1.0, 0.0); glVertex3f (1, -1,  1);
		glTexCoord2f (1.0, 1.0); glVertex3f (1,  1,  1);
		glTexCoord2f (0.0, 1.0); glVertex3f (1,  1, -1);
	glEnd();

	// normally, the following textures are unvisible
	// see config.cpp (use full_skybox)

	if (cfg.full_skybox) {
		// top
		glBindTexture (GL_TEXTURE_2D, Skybox[3]);
		glBegin(GL_QUADS);
			glTexCoord2f (0.0, 0.0); glVertex3f (-1, 1, -1);
			glTexCoord2f (1.0, 0.0); glVertex3f ( 1, 1, -1);
			glTexCoord2f (1.0, 1.0); glVertex3f ( 1, 1,  1);
			glTexCoord2f (0.0, 1.0); glVertex3f (-1, 1,  1);
		glEnd();
		
		// bottom
		glBindTexture (GL_TEXTURE_2D, Skybox[4]);
		glBegin(GL_QUADS);
			glTexCoord2f (0.0, 0.0); glVertex3f (-1, -1,  1);
			glTexCoord2f (1.0, 0.0); glVertex3f ( 1, -1,  1);
			glTexCoord2f (1.0, 1.0); glVertex3f ( 1, -1, -1);
			glTexCoord2f (0.0, 1.0); glVertex3f (-1, -1, -1);
		glEnd();
		
		// back
		glBindTexture (GL_TEXTURE_2D, Skybox[5]);
		glBegin(GL_QUADS);
			glTexCoord2f (0.0, 0.0); glVertex3f ( 1, -1, 1);
			glTexCoord2f (1.0, 0.0); glVertex3f (-1, -1, 1);
			glTexCoord2f (1.0, 1.0); glVertex3f (-1,  1, 1);
			glTexCoord2f (0.0, 1.0); glVertex3f ( 1,  1, 1);
		glEnd();
	}
	
	glPopMatrix();
}

// --------------------------------------------------------------------
//					light
// --------------------------------------------------------------------

void TuxEnv::ResetLight () {
	lights[0] = default_light;
	for (int i=1; i<4; i++) lights[i].is_on = false;
	glDisable (GL_LIGHT1);
	glDisable (GL_LIGHT2);
	glDisable (GL_LIGHT3);
}

void TuxEnv::SetLight () {
	glLightfv (GL_LIGHT0, GL_POSITION, lights[0].position);
	glLightfv (GL_LIGHT0, GL_AMBIENT, lights[0].ambient);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, lights[0].diffuse);
	glLightfv (GL_LIGHT0, GL_SPECULAR, lights[0].specular);
	glEnable  (GL_LIGHT0);
	if (lights[1].is_on) {
		glLightfv (GL_LIGHT1, GL_POSITION, lights[1].position);
		glLightfv (GL_LIGHT1, GL_AMBIENT, lights[1].ambient);
		glLightfv (GL_LIGHT1, GL_DIFFUSE, lights[1].diffuse);
		glLightfv (GL_LIGHT1, GL_SPECULAR, lights[1].specular);
		glEnable  (GL_LIGHT1);
	}
	if (lights[2].is_on) {
		glLightfv (GL_LIGHT2, GL_POSITION, lights[2].position);
		glLightfv (GL_LIGHT2, GL_AMBIENT, lights[2].ambient);
		glLightfv (GL_LIGHT2, GL_DIFFUSE, lights[2].diffuse);
		glLightfv (GL_LIGHT2, GL_SPECULAR, lights[2].specular);
		glEnable  (GL_LIGHT2);
	}
	if (lights[3].is_on) {
		glLightfv (GL_LIGHT3, GL_POSITION, lights[3].position);
		glLightfv (GL_LIGHT3, GL_AMBIENT, lights[3].ambient);
		glLightfv (GL_LIGHT3, GL_DIFFUSE, lights[3].diffuse);
		glLightfv (GL_LIGHT3, GL_SPECULAR, lights[3].specular);
		glEnable  (GL_LIGHT3);
	}
	glEnable  (GL_LIGHTING);
}

void TuxEnv::UpdateLight () {
	SetLight ();
}

// --------------------------------------------------------------------
//					fog
// --------------------------------------------------------------------

void TuxEnv::ResetFog () { 
	fog = default_fog; 
}

void TuxEnv::SetFog () {
    glEnable (GL_FOG);
    glFogi   (GL_FOG_MODE, fog.mode);
    glFogf   (GL_FOG_START, fog.start);
    glFogf   (GL_FOG_END, fog.end);
    glFogfv  (GL_FOG_COLOR, fog.color);
//	glHint   (GL_FOG_HINT, GL_FASTEST );
	glHint   (GL_FOG_HINT, GL_NICEST);
}

void TuxEnv::DrawFog () {
    TPlane bottom_plane, top_plane;
    TVector3 left, right, vpoint;
    TVector3 topleft, topright;
    TVector3 bottomleft = NullVec; 
	TVector3 bottomright = NullVec;
    float height;

	if (!fog.is_on) return;

	// the clipping planes are calculated by view frustum (view.cpp)
	leftclip = View.GetLeftPlane (); 
	rightclip = View.GetRightPlane ();
	farclip = View.GetFarPlane ();
	bottomclip = View.GetBottomPlane ();

	// --------------- calculate the planes ---------------------------

    float slope = tan (ANGLES_TO_RADIANS (Course.angle));
    TPlane left_edge_plane = MakePlane (1.0, 0.0, 0.0, 0.0);
    TPlane right_edge_plane = MakePlane (-1.0, 0.0, 0.0, Course.width);

    bottom_plane.nml = MakeVec (0.0, 1, -slope);
    height = Course.baseheight + 0;
    bottom_plane.d = -height * bottom_plane.nml.y;

    top_plane.nml = bottom_plane.nml;
    height = Course.maxheight + fog.height;
    top_plane.d = -height * top_plane.nml.y;

    if (!IntersectPlanes (bottom_plane, farclip, leftclip,  &left)) return;
    if (!IntersectPlanes (bottom_plane, farclip, rightclip, &right)) return;
    if (!IntersectPlanes (top_plane,    farclip, leftclip,  &topleft)) return;
    if (!IntersectPlanes (top_plane,    farclip, rightclip, &topright)) return;
    if (!IntersectPlanes (bottomclip,   farclip, leftclip,  &bottomleft)) return;
    if (!IntersectPlanes (bottomclip,   farclip, rightclip, &bottomright)) return;
    
	TVector3 leftvec  = SubVec (topleft, left);
    TVector3 rightvec = SubVec (topright, right);

	// --------------- draw the fog plane -----------------------------

	SetGLOptions (FOG_PLANE);
    glEnable (GL_FOG);
	
	// only the alpha channel is used
	float bottom_dens[4]     = {0, 0, 0, 1.0};
	float top_dens[4]        = {0, 0, 0, 0.9};
	float leftright_dens[4]  = {0, 0, 0, 0.3};
	float top_bottom_dens[4] = {0, 0, 0, 0.0};

    glBegin (GL_QUAD_STRIP);
	    glColor4fv (bottom_dens);
	    glVertex3f (bottomleft.x, bottomleft.y, bottomleft.z);
    	glVertex3f (bottomright.x, bottomright.y, bottomright.z);
	    glVertex3f (left.x, left.y, left.z);
    	glVertex3f (right.x, right.y, right.z);

	    glColor4fv (top_dens);
    	glVertex3f (topleft.x, topleft.y, topleft.z);
    	glVertex3f (topright.x, topright.y, topright.z);
	
	    glColor4fv (leftright_dens);
    	vpoint = AddVec (topleft, leftvec);
	    glVertex3f (vpoint.x, vpoint.y, vpoint.z);
    	vpoint = AddVec (topright, rightvec);
	    glVertex3f (vpoint.x, vpoint.y, vpoint.z);
		
	    glColor4fv (top_bottom_dens);
	    vpoint = AddVec (topleft, ScaleVec (3.0, leftvec));
    	glVertex3f (vpoint.x, vpoint.y, vpoint.z);
	    vpoint = AddVec (topright, ScaleVec (3.0, rightvec));
    	glVertex3f (vpoint.x, vpoint.y, vpoint.z);
    glEnd();
}

void SetGLValues (TVector4 source, float *dest) {
	dest[0] = source.x;
	dest[1] = source.y;
	dest[2] = source.z;
	dest[3] = source.w;
}

void TuxEnv::LoadLight () {
	string idxstr = "[fog]-1[0]0[1]1[2]2[3]3[4]4[5]5[6]6";
	string line;
	string item;
	int i, idx;	

	CSPList list(24);
	if (list.Load (dir.c_str(), "light.lst")) {
		for (i=0; i<list.Count(); i++) {
			line = list.Line(i);
			item = SPStrN (line, "light", "none");
			idx = SPIntN (idxstr, item, -1);
			if (idx < 0) {
				fog.is_on = SPBoolN (line, "fog", true);
				fog.start = SPFloatN (line, "fogstart", 20);
				fog.end = SPFloatN (line, "fogend", cfg.forwardclip);
				fog.height = SPFloatN (line, "fogheight", 0);
				SPArrN (line, "fogcol", fog.color, 4, 1);
				SPArrN (line, "partcol", fog.part_color, 4, 1);
			} else if (idx < 4){
				lights[idx].is_on = true;
				SPArrN (line, "amb", lights[idx].ambient, 4, 1);
				SPArrN (line, "diff", lights[idx].diffuse, 4, 1);
				SPArrN (line, "spec", lights[idx].specular, 4, 1);
				SPArrN (line, "pos", lights[idx].position, 4, 1);
			}
		}
	} else {
		Message ("could not load light file", "");
	}
}

void TuxEnv::Reset () {
	ResetSkybox ();
	ResetLight ();
	ResetFog ();
}

bool TuxEnv::Load () {
	string tempdir;
	tempdir = EnvInfo.GetDir (game.location_id, game.light_id);
	dir = tempdir;

	Skybox[0] = LoadTexture (dir.c_str(), "front.png", false);
	Skybox[1] = LoadTexture (dir.c_str(), "left.png", false);
	Skybox[2] = LoadTexture (dir.c_str(), "right.png", false);
	if (cfg.full_skybox) {
		Skybox[3] = LoadTexture (dir.c_str(), "top.png", false);
		Skybox[4] = LoadTexture (dir.c_str(), "bottom.png", false);
		Skybox[5] = LoadTexture (dir.c_str(), "back.png", false);
	}
	LoadLight ();
	return true;
}

// --------------------------------------------------------------------
//					Load Environment (global)
// --------------------------------------------------------------------

static int EnvID = -1;

void LoadEnvironment () {
	int env_id = game.location_id * 100 + game.light_id;

	if (env_id != EnvID) { 
		if (EnvID >= 0) Env.Reset ();
		if (Env.Load ()) EnvID = env_id;
		else Message ("could not load environment", "");
	}
}



