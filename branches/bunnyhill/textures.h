// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#ifndef TEXTURES_H
#define TEXTURES_H

#include "bh.h"

// Following some often used index numbers of the common textures. You have 
// to access and bind them with BindCommonTex (int idx), wheras idx is in repect to
// the texture array. Idx is not (!) the OpenGL texture id.

#define T_WIDGETS 0
#define NUMERIC_FONT 1
#define LOGO 2
#define ETR_LOGO 10
#define T_PARTICLES 3
#define T_ENVMAP 13
#define TOPLEFT 15
#define TOPRIGHT 16
#define BOTTOMLEFT 17
#define BOTTOMRIGHT 18

typedef struct {
	int left;
	int top;
	int width;
	int height;
} TRect;

class Image {
private:
	bool status;
public:
    Image ();
	~Image ();
	bool LoadPng (const char *filepath);
	bool LoadPng (const char *dir, const char *filepath);
		
	char name[128];
    int nx;
    int ny; 
    int depth;
    int pitch;
    unsigned char *data;
};

bool   BindCommonTex (int idx);
GLuint TexID (int idx);

// Mipmaps are used for terrain textures, for different levels of
// detail (LOD). Menu textures don't need mipmaps, so they are loaded
// by glTexImage2D, that's faster, takes less memory space and allows
// pics without arbritary sizes (not power of 2)

void LoadCommonTextures ();

// texture load with gluBuild2DMipmaps
int	 LoadTexture (const char *filename, bool repeatable);
int	 LoadTexture (const char *dir, const char *filename, bool repeatable);

// texture load with glTexImage2D
int  LoadMenuTexture (const char *filename);
int  LoadMenuTexture (const char *dir, const char *filename);

// draw about index array
void DrawCommonTexture (int idx, int x, int y, float size);
// direct draw with original texture id
void DrawTexture (int texid, int x, int y, int width, int height);

// for drawing the numbers in hud.cpp:
void DrawNumStr (const char *s, int x, int y, float fact, TColor col);

#endif
