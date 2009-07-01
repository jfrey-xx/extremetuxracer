// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

// this module allows the access to freetype fonts. It is based on
// FTGL, ft_font.cpp is the modified FTGL library

#ifndef FONT_H
#define FONT_H

#include "bh.h"
#include "ft_font.h"

#define MAX_FONTS 36;

// --------------------------------------------------------------------
//		CFont
// --------------------------------------------------------------------

class CFont {
private:
	FTFont *fonts[36];
	int numFonts;
	string fontindex;
	TColor color;
	float size;
	int ft;
	wchar_t *UnicodeStr (const char* s);
	void UnicodeStr (wchar_t* buff, const char* string);
public:
	CFont ();
	~CFont ();

	void SetColor (float r, float g, float b, float a);
	void SetColor (TColor col);
	void SetSize (float siz);
	void SetFont (char *font);
	void SetProps (char *font, float siz, TColor col);
	void GetTextSize (char *txt, float &x, float &y);
	void BBox (char *font, char* txt, float& llx, float& lly, float& llz, 
		float& urx, float& ury, float& urz);
	float GetWidth (const char *txt);
	
	bool LoadFontlist ();
	bool LoadFont (const char *dir, const char *filename);
	bool LoadFont (const char *path);

	void DrawText   (float x, float y, const char *text);		// normal char*
	void DrawText   (float x, float y, const wchar_t *text);	// wide char*
	void DrawString (float x, float y, const string &s);		// string
	
	// the same function with reverse y coords
	void DrawTextNY (float x, float y, const char *text);     
	void DrawTextNY (float x, float y, const wchar_t *text);  
	void DrawStringNY (float x, float y, const string &s);

	void MakeLineList (const char *source, CSPList *dest, float width);
};

extern CFont FT;

// --------------------------------------------------------------------
// The following code is from PPRacer (ETRacer)
// It works with binding tables which are not used in 
// Bunny Hill. I keep this class to show the differences
// --------------------------------------------------------------------

/*
#include <map>
#include <string>
#include "ft_font.h"

class PPFont {
private:
	static std::map <std::string, PPFont* > sm_bindings;
	FTFont *ft_font; 	
	TColor fontcol;
public:	
	PPFont (const char *fileName, unsigned int size, const TColor &color);
	PPFont (FTFont *font, const TColor &color);
	~PPFont ();

	void draw (const char *string, float x, float y);
	void draw (const char *string, TVector2 position);
	void draw(const wchar_t *string, float x, float y);
	void draw (const wchar_t *string, TVector2 position);

	float ascender();
	float descender();
	float advance(const char *string);
	float advance(const wchar_t *string);

	TColor &getColor();
	FTFont *getFTFont();
	
	static bool registerFont (const char *binding, const char *fileName, 
		unsigned int size, const TColor &color);
	static bool bindFont (const char *binding, const char *fontName);
	static bool bindFont (const char *binding, const char *fontName, const TColor& color);
	static PPFont* get (const char *binding);
	static void draw(const char *binding, const char *string, float x, float y);
	static wchar_t* utf8ToUnicode (const char* string);
	static void utf8ToUnicode (wchar_t* buff, const char* string);
};
*/


#endif


