/* --------------------------------------------------------------------
EXTREME TUXRACER

Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
Copyright (C) 2010 Extreme Tuxracer Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
---------------------------------------------------------------------*/

#ifdef HAVE_CONFIG_H
#include <etr_config.h>
#endif

#include "textures.h"
#include "spx.h"
#include "course.h"
#include "winsys.h"
#include "ogl.h"
#include <SDL/SDL_image.h>
#include <GL/glu.h>
#include <fstream>
#include <cctype>


static const GLshort fullsize_texture[] = {
	0, 0,
	1, 0,
	1, 1,
	0, 1
};

// --------------------------------------------------------------------
//				class CImage
// --------------------------------------------------------------------

CImage::CImage() {
	data = NULL;
	nx = 0;
	ny = 0;
	depth = 0;
	pitch = 0;
}

CImage::~CImage() {
	DisposeData();
}

void CImage::DisposeData() {
	delete[] data;
	data = NULL;
}

bool CImage::LoadPng(const char *filepath, bool mirroring) {
	SDL_Surface *sdlImage;
	unsigned char *sdlData;

	sdlImage = IMG_Load(filepath);
	if (sdlImage == 0) {
		Message("could not load image", filepath);
		return false;
	}

	nx    = sdlImage->w;
	ny    = sdlImage->h;
	depth = sdlImage->format->BytesPerPixel;
	pitch = sdlImage->pitch;
	DisposeData();
	data  = new unsigned char[pitch * ny];

	if (SDL_MUSTLOCK(sdlImage)) {
		if (SDL_LockSurface(sdlImage) < 0) {
			SDL_FreeSurface(sdlImage);
			Message("mustlock error");
			return false;
		};
	}

	sdlData = (unsigned char *) sdlImage->pixels;

	if (mirroring) {
		for (int y=0; y<ny; y++) {
			memcpy(data + y*pitch, sdlData + (ny-1-y)*pitch, pitch);
		}
	} else {
		memcpy(data, sdlData, ny*pitch);
	}

	if (SDL_MUSTLOCK(sdlImage)) SDL_UnlockSurface(sdlImage);
	SDL_FreeSurface(sdlImage);
	return true;
}

bool CImage::LoadPng(const char *dir, const char *filename, bool mirroring) {
	string path = dir;
	path += SEP;
	path += filename;
	return LoadPng(path.c_str(), mirroring);
}

// ------------------ read framebuffer --------------------------------

bool CImage::ReadFrameBuffer_PPM() {
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	nx = viewport[2];
	ny = viewport[3];
	depth = 3;

	DisposeData();
	data  = new unsigned char[nx * ny * depth];

	glReadBuffer(GL_FRONT);

	for (int i=0; i<viewport[3]; i++) {
		glReadPixels(viewport[0], viewport[1] + viewport[3] - 1 - i,
		             viewport[2], 1, GL_RGB, GL_UNSIGNED_BYTE, data + viewport[2] * i * 3);
	}

	return true;
}

void CImage::ReadFrameBuffer_TGA() {
	nx = Winsys.resolution.width;
	ny = Winsys.resolution.height;
	depth = 3;

	DisposeData();
	data  = new unsigned char[nx * ny * depth];

	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, nx, ny, GL_BGR, GL_UNSIGNED_BYTE, data);
}

void CImage::ReadFrameBuffer_BMP() {
	nx = Winsys.resolution.width;
	ny = Winsys.resolution.height;
	depth = 4;

	DisposeData();
	data  = new unsigned char[nx * ny * depth];
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, nx, ny, GL_BGRA, GL_UNSIGNED_BYTE, data);
}

// ---------------------------

void CImage::WritePPM(const char *filepath) {
	if (data == NULL)
		return;

	std::ofstream file(filepath);

	file << "P6\n# A Raw PPM file"
	     << "\n# width\n" << nx
	     << "\n# height\n" << ny
	     << "\n# max component value\n255"<< std::endl;

	file.write(reinterpret_cast<char*>(data), depth * nx * ny);
}

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct TTgaHeader {
	int8_t  tfType;
	int8_t  tfColorMapType;
	int8_t  tfImageType;
	int8_t  tfColorMapSpec[5];
	int16_t tfOrigX;
	int16_t tfOrigY;
	int16_t tfWidth;
	int16_t tfHeight;
	int8_t  tfBpp;
	int8_t  tfImageDes;
#ifdef _MSC_VER
};
#pragma pack(pop)
#else
} __attribute__((packed));
#endif

void CImage::WriteTGA(const char *filepath) {
	if (data == NULL)
		return;

	TTgaHeader header;

	header.tfType = 0;
	header.tfColorMapType = 0;
	header.tfImageType = 2;
	for (int i=0; i<5; i++) header.tfColorMapSpec[i] = 0;
	header.tfOrigX = 0;
	header.tfOrigY = 0;
	header.tfWidth = static_cast<int16_t>(nx);
	header.tfHeight = static_cast<int16_t>(ny);
	header.tfBpp = 24;
	header.tfImageDes = 0;

	std::ofstream out(filepath, std::ios_base::out|std::ios_base::binary);
	out.write(reinterpret_cast<char*>(&header), sizeof(TTgaHeader));
	out.write(reinterpret_cast<char*>(data), 3 * nx * ny);
}

#define BF_TYPE 0x4D42             // "MB"

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct TBmpHeader {
	uint16_t bfType;           // identifier of bmp format
	uint32_t bfSize;           // size of file, including the headers
	uint16_t bfReserved1;      // reserved, always 0
	uint16_t bfReserved2;      // reserved, always 0
	uint32_t bfOffBits;        // offset to bitmap data
#ifdef _MSC_VER
};
#else
} __attribute__((packed));
#endif

struct TBmpInfo {
	uint32_t biSize;           // size of info header, normally 40
	int32_t  biWidth;          // width
	int32_t  biHeight;         // height
	uint16_t biPlanes;         // number of color planes, normally 1
	uint16_t biBitCount;       // Number of bits per pixel (8 * depth)
	uint32_t biCompression;    // type of compression, normally 0 = no compr.
	uint32_t biSizeImage;      // size of data
	int32_t  biXPelsPerMeter;  // normally 0
	int32_t  biYPelsPerMeter;  // normally 0
	uint32_t biClrUsed;        // normally 0
	uint32_t biClrImportant;   // normally 0
#ifdef _MSC_VER
};
#pragma pack(pop)
#else
} __attribute__((packed));
#endif

void CImage::WriteBMP(const char *filepath) {
	if (data == NULL)
		return;

	int infosize = 40;
	int width = nx;
	int height = ny;
	int imgsize = nx * ny * depth;
	int bitcnt = 8 * depth; // 24 or 32
	unsigned int bitsize;
	// (width * bitcnt + 7) / 8 = width * depth
	if (imgsize == 0) bitsize = (width * bitcnt + 7) / 8 * height;
	else bitsize = imgsize;

	TBmpHeader header;
	header.bfType = BF_TYPE;
	header.bfSize = 14 + infosize + bitsize;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits = sizeof(TBmpHeader) + sizeof(TBmpInfo);

	TBmpInfo info;
	info.biSize = infosize;
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;
	info.biBitCount = bitcnt;
	info.biCompression = 0;
	info.biSizeImage = imgsize;
	info.biXPelsPerMeter = 0;
	info.biYPelsPerMeter= 0;
	info.biClrUsed = 0;
	info.biClrImportant = 0;

	std::ofstream out(filepath, std::ios_base::out|std::ios_base::binary);
	if (!out) {
		Message("could not open bmp file", filepath);
		return;
	}

	out.write(reinterpret_cast<char*>(&header), sizeof(TBmpHeader));
	out.write(reinterpret_cast<char*>(&info), sizeof(TBmpInfo));

	out.write(reinterpret_cast<char*>(data), bitsize);
}

// --------------------------------------------------------------------
//				class TTexture
// --------------------------------------------------------------------

TTexture::~TTexture() {
	glDeleteTextures(1, &id);
}

bool TTexture::Load(const string& filename) {
	CImage texImage;

	if (texImage.LoadPng(filename.c_str(), true) == false)
		return false;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	GLenum format;
	if (texImage.depth == 3) format = GL_RGB;
	else format = GL_RGBA;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D
	(GL_TEXTURE_2D, 0, texImage.depth, texImage.nx,
	 texImage.ny, 0, format, GL_UNSIGNED_BYTE, texImage.data);
	return true;
}
bool TTexture::Load(const string& dir, const string& filename) {
	return Load(dir + SEP + filename);
}

bool TTexture::LoadMipmap(const string& filename, bool repeatable) {
	CImage texImage;
	if (texImage.LoadPng(filename.c_str(), true) == false)
		return false;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if (repeatable) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	} else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	GLenum format;
	if (texImage.depth == 3) format = GL_RGB;
	else format = GL_RGBA;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	gluBuild2DMipmaps(GL_TEXTURE_2D, texImage.depth, texImage.nx, texImage.ny, format, GL_UNSIGNED_BYTE, texImage.data);
	return true;
}
bool TTexture::LoadMipmap(const string& dir, const string& filename, bool repeatable) {
	return LoadMipmap(dir + SEP + filename, repeatable);
}

void TTexture::Bind() {
	glBindTexture(GL_TEXTURE_2D, id);
}

void TTexture::Draw() {
	GLint w, h;

	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, id);

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	const GLshort vtx[] = {
		0, 0,
		w, 0,
		w, h,
		0, h
	};
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_SHORT, 0, vtx);
	glTexCoordPointer(2, GL_SHORT, 0, fullsize_texture);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void TTexture::Draw(int x, int y, float size, Orientation orientation) {
	GLint w, h;
	GLfloat width, height, top, bott, left, right;

	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, id);

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

	width  = w * size;
	height = h * size;

	if (orientation == OR_TOP) {
		top = Winsys.resolution.height - y;
		bott = top - height;

	} else {
		bott = y;
		top = bott + height;
	}
	if (x >= 0) left = x;
	else left = (Winsys.resolution.width - width) / 2;
	right = left + width;

	glColor4f(1.0, 1.0, 1.0, 1.0);
	const GLfloat vtx[] = {
		left, bott,
		right, bott,
		right, top,
		left, top
	};
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, vtx);
	glTexCoordPointer(2, GL_SHORT, 0, fullsize_texture);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void TTexture::Draw(int x, int y, float width, float height, Orientation orientation) {
	GLfloat top, bott, left, right;

	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, id);

	if (orientation == OR_TOP) {
		top = Winsys.resolution.height - y;
		bott = top - height;
	} else {
		bott = y;
		top = bott + height;
	}
	if (x >= 0) left = x;
	else left = (Winsys.resolution.width - width) / 2;
	right = left + width;

	glColor4f(1.0, 1.0, 1.0, 1.0);
	const GLfloat vtx[] = {
		left, bott,
		right, bott,
		right, top,
		left, top
	};
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, vtx);
	glTexCoordPointer(2, GL_SHORT, 0, fullsize_texture);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void TTexture::DrawFrame(int x, int y, double w, double h, int frame, const TColor& col) {
	if (id < 1)
		return;

	GLint ww = GLint(w);
	GLint hh = GLint(h);
	GLint xx = x;
	GLint yy = Winsys.resolution.height - hh - y;

	glBindTexture(GL_TEXTURE_2D, id);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	if (frame > 0) {
		if (w < 1) glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &ww);
		if (h < 1) glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &hh);

		glColor(col, 1.0);

		glDisable(GL_TEXTURE_2D);
		const GLint vtx [] = {
			xx - frame, yy - frame,
			xx + ww + frame, yy - frame,
			xx + ww + frame, yy + hh + frame,
			xx - frame, yy + hh + frame
		};

		glVertexPointer(2, GL_INT, 0, vtx);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glEnable(GL_TEXTURE_2D);
	}

	glColor4f(1.0, 1.0, 1.0, 1.0);

	const GLshort vtx[] = {
		xx, yy,
		xx + ww, yy,
		xx + ww, yy + hh,
		xx, yy + hh
	};

	glVertexPointer(2, GL_SHORT, 0, vtx);
	glTexCoordPointer(2, GL_SHORT, 0, fullsize_texture);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

// --------------------------------------------------------------------
//				class CTexture
// --------------------------------------------------------------------

CTexture Tex;

CTexture::CTexture() {
	forientation = OR_TOP;
}

CTexture::~CTexture() {
	FreeTextureList();
}

void CTexture::LoadTextureList() {
	FreeTextureList();
	CSPList list(200);
	if (list.Load(param.tex_dir, "textures.lst")) {
		for (size_t i=0; i<list.Count(); i++) {
			const string& line = list.Line(i);
			string name = SPStrN(line, "name");
			int id = SPIntN(line, "id", -1);
			CommonTex.resize(max(CommonTex.size(), (size_t)id+1));
			string texfile = SPStrN(line, "file");
			bool rep = SPBoolN(line, "repeat", false);
			if (id >= 0) {
				CommonTex[id] = new TTexture();
				if (rep)
					CommonTex[id]->LoadMipmap(param.tex_dir, texfile, rep);
				else
					CommonTex[id]->Load(param.tex_dir, texfile);
			} else Message("wrong texture id in textures.lst");
		}
	} else Message("failed to load common textures");
}

void CTexture::FreeTextureList() {
	for (size_t i=0; i<CommonTex.size(); i++) {
		delete CommonTex[i];
	}
	CommonTex.clear();
}

TTexture* CTexture::GetTexture(size_t idx) const {
	if (idx >= CommonTex.size()) return NULL;
	return CommonTex[idx];
}

bool CTexture::BindTex(size_t idx) {
	if (idx >= CommonTex.size()) return false;
	CommonTex[idx]->Bind();
	return true;
}

// ---------------------------- Draw ----------------------------------

void CTexture::Draw(size_t idx) {
	if (CommonTex.size() > idx)
		CommonTex[idx]->Draw();
}

void CTexture::Draw(size_t idx, int x, int y, float size) {
	if (CommonTex.size() > idx)
		CommonTex[idx]->Draw(x, y, size, forientation);
}

void CTexture::Draw(size_t idx, int x, int y, int width, int height) {
	if (CommonTex.size() > idx)
		CommonTex[idx]->Draw(x, y, width, height, forientation);
}

void CTexture::DrawFrame(size_t idx, int x, int y, double w, double h, int frame, const TColor& col) {
	if (CommonTex.size() > idx)
		CommonTex[idx]->DrawFrame(x, y, w, h, frame, col);
}

void CTexture::SetOrientation(Orientation orientation) {
	forientation = orientation;
}

// -------------------------- numeric strings -------------------------

void CTexture::DrawNumChr(char c, int x, int y, int w, int h, const TColor& col) {
	int idx;
	if (isdigit(c)) {
		char chrname[2] = {c, '\0'};
		idx = atoi(chrname);
	} else if (c == ':')
		idx = 10;
	else if (c == ' ')
		idx = 11;
	else
		return;

	// texture coords
	float texw = 22.0 / 256.0;
	float texleft = idx * texw;
	float texright = (idx + 1) * texw;

	const GLfloat tex[] = {
		texleft, 0,
		texright, 0,
		texright, 1,
		texleft, 1
	};
	const GLfloat vtx[] = {
		x,           Winsys.resolution.height - y - h,
		x + w * 0.9, Winsys.resolution.height - y - h,
		x + w * 0.9, Winsys.resolution.height - y,
		x,           Winsys.resolution.height - y
	};

	glVertexPointer(2, GL_FLOAT, 0, vtx);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void CTexture::DrawNumStr(const string& s, int x, int y, float size, const TColor& col) {
	if (!BindTex(NUMERIC_FONT)) {
		Message("DrawNumStr: missing texture");
		return;
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	int qw = (int)(22 * size);
	int qh = (int)(32 * size);

	glColor(col);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	for (size_t i=0; i < s.size(); i++) {
		DrawNumChr(s[i], x + (int)i*qw, y, qw, qh, col);
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

// --------------------------------------------------------------------
//				screenshot
// --------------------------------------------------------------------

// 0 ppm, 1 tga, 2 bmp
#define SCREENSHOT_PROC 2

void ScreenshotN() {
	CImage image;
	string path = param.screenshot_dir;
	path += SEP;
	path += g_game.course->dir;
	path += "_";
	path += GetTimeString();
	int type = SCREENSHOT_PROC;

	switch (type) {
		case 0:
			path += ".ppm";
			image.ReadFrameBuffer_PPM();
			image.WritePPM(path.c_str());
			break;
		case 1:
			path += ".tga";
			image.ReadFrameBuffer_TGA();
			image.WriteTGA(path.c_str());
			break;
		case 2:
			path += ".bmp";
			image.ReadFrameBuffer_BMP();
			image.WriteBMP(path.c_str());
			break;
	}
}
