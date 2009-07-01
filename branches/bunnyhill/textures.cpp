// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "textures.h"

// --------------------------------------------------------------------
//	Image, for loading png textures
// --------------------------------------------------------------------

Image::Image () { status = false; }

Image::~Image () {
	if (status) free (data);
}

bool Image::LoadPng (const char *filepath) {
	SDL_Surface* sdlImage;
	unsigned char *sdlData;
	
	sdlImage = IMG_Load (filepath);
   	if (sdlImage == 0) {
   		printf ("could not load image %s\n", filepath);
		return false;
   	}

	status = true;
	strcpy (name, filepath);
	
	nx    = sdlImage->w;
	ny    = sdlImage->h;
	depth = sdlImage->format->BytesPerPixel;
	pitch = sdlImage->pitch;
	data  = (unsigned char *) malloc (pitch * ny * sizeof (unsigned char));

   	if (SDL_MUSTLOCK (sdlImage)) {
   	    if (SDL_LockSurface (sdlImage) < 0) {
	   		printf ("mustlock error \n");
			return false;
		};
   	}

	sdlData = (unsigned char *) sdlImage->pixels;
	for (int y=0; y<ny; y++) {
		for (int x=0; x<pitch; x++) {
			data[y * pitch + x] = sdlData[(ny-1-y) * pitch + x];	
		}
	}
   	
	if (SDL_MUSTLOCK (sdlImage)) SDL_UnlockSurface (sdlImage);
	SDL_FreeSurface (sdlImage);
	return true;
}

bool Image::LoadPng (const char *dir, const char *filename) {
	string path = dir;
	path += SEP;
	path += filename;
	return LoadPng (path.c_str());
}

// --------------------------------------------------------------------
//	textures
// --------------------------------------------------------------------

#define MAX_COMMON_TEX 64
GLuint CommonTextures[MAX_COMMON_TEX];

// index string only for numstr digits
static  string idxstr = "[0]0[1]1[2]2[3]3[4]4[5]5[6]6[7]7[8]8[9]9[:]10[ ]11";

bool BindCommonTex (int idx) {
	if (idx >= 0) {
		glBindTexture (GL_TEXTURE_2D, CommonTextures[idx]);
		return true;
	} else return false;
}

GLuint TexID (int idx) {
	if (idx >= MAX_COMMON_TEX || idx < 0) return 0;
	return CommonTextures[idx];
}

void LoadCommonTextures () {
	int id;
	string texfile;
	string line;

	CSPList list (200);
	if (list.Load (cfg.texturedir, "textures.lst")) {
		for (int i=0; i<list.Count(); i++) {
			line = list.Line (i);
			id = SPIntN (line, "id", 0);
			texfile = SPStrN (line, "file", "");
			CommonTextures[id] = LoadMenuTexture (cfg.texturedir.c_str(), texfile.c_str());
		}
	} else Message ("failed to load common textures");
}

void DrawCommonTexture (int idx, int x, int y, float size) {
    TVector2 bl, tr;
    GLint w, h;
    TRect rr;

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (CommonTextures[idx] < 1) return;
	glBindTexture (GL_TEXTURE_2D, CommonTextures[idx]);
	glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);


	rr.width  = (int)(w * size);
	rr.height = (int)(h * size);
	if (x == -1) rr.left = (cfg.scrwidth - rr.width) / 2; 
	else rr.left = x;
	rr.top = y;

	bl.x = rr.left;
	bl.y = cfg.scrheight - rr.top - rr.height;
	tr.x = rr.left + rr.width;
	tr.y = cfg.scrheight - rr.top;

    glColor4f (1.0, 1.0, 1.0, 1.0);
	glBegin (GL_QUADS);
	    glTexCoord2f (0, 0); glVertex2f (bl.x, bl.y);
	    glTexCoord2f (1, 0); glVertex2f (tr.x, bl.y);
	    glTexCoord2f (1, 1); glVertex2f (tr.x, tr.y);
	    glTexCoord2f (0, 1); glVertex2f (bl.x, tr.y);
	glEnd();
}

void DrawTexture (int texid, int x, int y, int width, int height) {
    TVector2 bl, tr;
    GLint w, h;
    TRect rr;

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture (GL_TEXTURE_2D, texid);

	if (width > 0) w = width;
	else glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	if (height > 0) h = height;
	else glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

	rr.width  = w;
	rr.height = h;
	if (x == -1) rr.left = (cfg.scrwidth - rr.width) / 2; 
	else rr.left = x;
	rr.top = y;

	bl.x = rr.left;
	bl.y = cfg.scrheight - rr.top - rr.height;
	tr.x = rr.left + rr.width;
	tr.y = cfg.scrheight - rr.top;

    glColor4f (1.0, 1.0, 1.0, 1.0);
	glBegin (GL_QUADS);
	    glTexCoord2f (0, 0); glVertex2f (bl.x, bl.y);
	    glTexCoord2f (1, 0); glVertex2f (tr.x, bl.y);
	    glTexCoord2f (1, 1); glVertex2f (tr.x, tr.y);
	    glTexCoord2f (0, 1); glVertex2f (bl.x, tr.y);
	glEnd();
}

int LoadTexture (const char *filename, bool repeatable) {
    Image texImage;
	GLuint texid;

	if (texImage.LoadPng (filename) == false) return 0;
	glGenTextures (1, &texid);
	glBindTexture (GL_TEXTURE_2D, texid);		
    glPixelStorei (GL_UNPACK_ALIGNMENT, 4);
 
   if  (repeatable) {
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

	GLenum format;
	if (texImage.depth == 3) format = GL_RGB; 
	else format = GL_RGBA;

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 

	gluBuild2DMipmaps (GL_TEXTURE_2D, texImage.depth, texImage.nx,
		       texImage.ny, format, GL_UNSIGNED_BYTE, texImage.data);

    return texid;    
}

int LoadTexture (const char *dir, const char *filename, bool repeatable) {
	string path = dir;
	path += SEP;
	path += filename;
	return LoadTexture (path.c_str(), repeatable);
}

int LoadMenuTexture (const char *filename) {
    Image texImage;
	GLuint texid;

	if (texImage.LoadPng (filename) == false) return 0;
	glGenTextures (1, &texid);
	glBindTexture (GL_TEXTURE_2D, texid);		
    glPixelStorei (GL_UNPACK_ALIGNMENT, 4);
 
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	GLenum format;
	if (texImage.depth == 3) format = GL_RGB; 
	else format = GL_RGBA;

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	glTexImage2D (GL_TEXTURE_2D, 0, texImage.depth, texImage.nx,
		       texImage.ny, 0, format, GL_UNSIGNED_BYTE, texImage.data);

    return texid;    
}

int LoadMenuTexture (const char *dir, const char *filename) {
	string path = dir;
	path += SEP;
	path += filename;
	return LoadMenuTexture (path.c_str());
}

void DrawNumChr (char c, int x, int y, int w, int h, TColor col) {
    float texleft, texright, texw;
	char chrname[2]; 	
    TVector2 bl, tr;
	chrname[0]= c;
	chrname[1]= 0;

	int idx = SPIntN (idxstr, chrname, 11);	

	// quad coords
	bl.x = x;
	bl.y = cfg.scrheight - y - h;
	tr.x = x + w * 0.9;
	tr.y = cfg.scrheight - y;

	// texture coords
	texw = 22.0 / 256.0;
	texleft = idx * texw;
	texright = (idx + 1) * texw;

	glColor4f (col.r, col.g, col.b, col.a);
	glBegin (GL_QUADS);
	    glTexCoord2f (texleft, 0); glVertex2f (bl.x, bl.y);
	    glTexCoord2f (texright, 0); glVertex2f (tr.x, bl.y);
	    glTexCoord2f (texright, 1); glVertex2f (tr.x, tr.y);
	    glTexCoord2f (texleft, 1); glVertex2f (bl.x, tr.y);
	glEnd();

} 

void DrawNumStr (const char *s, int x, int y, float size, TColor col) {
	if (!BindCommonTex (NUMERIC_FONT)) return;

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_TEXTURE_2D);
	int qw = (int)(22 * size);
	int qh = (int)(32 * size);

	for (int i=0; i<int(strlen(s)); i++) {
		DrawNumChr (s[i], x + i*qw, y, qw, qh, col); 
	}
}








