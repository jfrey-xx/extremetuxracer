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
#include <SFML/Graphics.hpp>
#include <GL/glu.h>
#include <fstream>
#include <cctype>


static const GLshort fullsize_texture[] = {
	0, 1,
	1, 1,
	1, 0,
	0, 0
};


// --------------------------------------------------------------------
//				class TTexture
// --------------------------------------------------------------------

bool TTexture::Load(const string& filename) {
	texture = new sf::Texture();
	texture->setSmooth(true);
	return texture->loadFromFile(filename);
}

bool TTexture::Load(const string& dir, const string& filename) {
	return Load(dir + SEP + filename);
}

bool TTexture::LoadMipmap(const string& filename, bool repeatable) {
	texture = new sf::Texture();
	texture->setSmooth(true);
	texture->setRepeated(repeatable);
	if (!texture->loadFromFile(filename))
		return false;
	///gluBuild2DMipmaps(GL_TEXTURE_2D, 4, texture->getSize().x, texture->getSize().y, GL_RGBA, GL_UNSIGNED_BYTE, texture->);
	return true;
}
bool TTexture::LoadMipmap(const string& dir, const string& filename, bool repeatable) {
	return LoadMipmap(dir + SEP + filename, repeatable);
}

void TTexture::Bind() {
	sf::Texture::bind(texture);
}

void TTexture::Draw() {
	GLint w, h;

	glEnable (GL_TEXTURE_2D);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Bind();

	glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

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

	glEnable (GL_TEXTURE_2D);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Bind();

	glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

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

	glEnable (GL_TEXTURE_2D);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Bind();

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
	if (!texture)
		return;

	GLint ww = GLint(w);
	GLint hh = GLint(h);
	GLint xx = x;
	GLint yy = Winsys.resolution.height - hh - y;

	Bind();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	if (frame > 0) {
		if (w < 1) glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &ww);
		if (h < 1) glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &hh);

		glColor(col, 1.0);

		glDisable (GL_TEXTURE_2D);
		const GLint vtx [] = {
			xx - frame, yy - frame,
			xx + ww + frame, yy - frame,
			xx + ww + frame, yy + hh + frame,
			xx - frame, yy + hh + frame
		};

		glVertexPointer(2, GL_INT, 0, vtx);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glEnable (GL_TEXTURE_2D);
	}

	glColor4f (1.0, 1.0, 1.0, 1.0);

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

CTexture::CTexture () {
	forientation = OR_TOP;
}

CTexture::~CTexture () {
	FreeTextureList();
}

void CTexture::LoadTextureList () {
	FreeTextureList();
	CSPList list (200);
	if (list.Load (param.tex_dir, "textures.lst")) {
		for (size_t i=0; i<list.Count(); i++) {
			const string& line = list.Line(i);
			string name = SPStrN (line, "name");
			int id = SPIntN (line, "id", -1);
			CommonTex.resize(max(CommonTex.size(), (size_t)id+1));
			string texfile = SPStrN (line, "file");
			bool rep = SPBoolN (line, "repeat", false);
			if (id >= 0) {
				CommonTex[id] = new TTexture();
				if (rep)
					CommonTex[id]->LoadMipmap(param.tex_dir, texfile, rep);
				else
					CommonTex[id]->Load(param.tex_dir, texfile);

				Index[name] = CommonTex[id];
			} else Message ("wrong texture id in textures.lst");
		}
	} else Message ("failed to load common textures");
}

void CTexture::FreeTextureList () {
	for (size_t i=0; i<CommonTex.size(); i++) {
		delete CommonTex[i];
	}
	CommonTex.clear();
	Index.clear();
}

TTexture* CTexture::GetTexture (size_t idx) const {
	if (idx >= CommonTex.size()) return NULL;
	return CommonTex[idx];
}

TTexture* CTexture::GetTexture (const string& name) const {
	return Index.at(name);
}

bool CTexture::BindTex (size_t idx) {
	if (idx >= CommonTex.size()) return false;
	CommonTex[idx]->Bind();
	return true;
}

bool CTexture::BindTex (const string& name) {
	try {
		Index.at(name)->Bind();
	} catch (...) {
		return false;
	}
	return true;
}

// ---------------------------- Draw ----------------------------------

void CTexture::Draw (size_t idx) {
	if (CommonTex.size() > idx)
		CommonTex[idx]->Draw();
}

void CTexture::Draw (const string& name) {
	Index[name]->Draw();
}

void CTexture::Draw (size_t idx, int x, int y, float size) {
	if (CommonTex.size() > idx)
		CommonTex[idx]->Draw(x, y, size, forientation);
}

void CTexture::Draw (const string& name, int x, int y, float size) {
	Index[name]->Draw(x, y, size, forientation);
}

void CTexture::Draw (size_t idx, int x, int y, int width, int height) {
	if (CommonTex.size() > idx)
		CommonTex[idx]->Draw (x, y, width, height, forientation);
}

void CTexture::Draw (const string& name, int x, int y, int width, int height) {
	Index[name]->Draw (x, y, width, height, forientation);
}

void CTexture::DrawFrame (size_t idx, int x, int y, double w, double h, int frame, const TColor& col) {
	if (CommonTex.size() > idx)
		CommonTex[idx]->DrawFrame (x, y, w, h, frame, col);
}

void CTexture::DrawFrame (const string& name, int x, int y, double w, double h, int frame, const TColor& col) {
	Index[name]->DrawFrame (x, y, w, h, frame, col);
}

void CTexture::SetOrientation (Orientation orientation) {
	forientation = orientation;
}

// -------------------------- numeric strings -------------------------

void CTexture::DrawNumChr (char c, int x, int y, int w, int h, const TColor& col) {
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
		texleft, 1,
		texright, 1,
		texright, 0,
		texleft, 0
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

void CTexture::DrawNumStr (const string& s, int x, int y, float size, const TColor& col) {
	if (!BindTex ("ziff032")) {
		Message ("DrawNumStr: missing texture");
		return;
	}
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_TEXTURE_2D);
	int qw = (int)(22 * size);
	int qh = (int)(32 * size);

	glColor(col);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	for (size_t i=0; i < s.size(); i++) {
		DrawNumChr (s[i], x + (int)i*qw, y, qw, qh, col);
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

// --------------------------------------------------------------------

//				screenshot
// --------------------------------------------------------------------

// 0 ppm, 1 tga, 2 bmp
#define SCREENSHOT_PROC 2

void ScreenshotN () {
	sf::Texture tex;
	tex.update(Winsys.window);
	sf::Image img = tex.copyToImage();

	string path = param.screenshot_dir;
	path += SEP;
	path += Course.CourseList[g_game.course_id].dir;
	path += "_";
	path += GetTimeString();
	int type = SCREENSHOT_PROC;

	switch (type) {
		case 0:
			path += ".ppm";
			img.saveToFile(path);
			break;
		case 1:
			path += ".tga";
			img.saveToFile(path);
			break;
		case 2:
			path += ".bmp";
			img.saveToFile(path);
			break;
	}
}
