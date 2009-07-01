// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#ifndef ENV_H
#define ENV_H

#include "bh.h"

typedef struct {
    bool is_on;
	GLint mode;
    float start;
    float end;
	float height;
    float color[4];
	float part_color[4]; 
} TFog;

typedef struct {
	bool is_on;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float position[4];	
} TLight;


class TuxEnv {
private:
	string dir;

	TLight default_light;
	TLight lights[4];
	TFog default_fog;
	TFog fog;

	GLuint Skybox[6];

	TPlane leftclip; 
	TPlane rightclip;
	TPlane farclip;
	TPlane bottomclip;

public:
	TuxEnv ();
	~TuxEnv ();
	void Init ();
	TColor GetParticleColor ();
	float *GetGLParticleColor ();

	void ResetLight ();
	void SetLight ();
	void UpdateLight ();

	void ResetFog ();
	void SetFog ();
	void DrawFog ();	

	void ResetSkybox ();
	void DrawSkybox (TVector3 pos);

	void LoadLight ();
	bool Load ();
	void Reset ();
};

extern TuxEnv Env;

void LoadEnvironment ();

#endif
