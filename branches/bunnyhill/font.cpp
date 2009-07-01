// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "font.h"
#include "common.h"

// --------------------------------------------------------------------
// First some common function used for textboxes and called by
// CFont::MakeLineList. Theses bundle of functions generates
// a SPList from a textstring and adapts the lines to the textbox
// Maybe gui_menu.cpp is a better place

void MakeWordList (CSPList *wordlist, const char *s) {
	char dest[2000];
	char val[16];
	dest[0]=0;
	int ch = 0;
	int wd = 1;
	dest[ch] = '['; ch++;
	dest[ch] = '0'; ch++;
	dest[ch] = ']'; ch++;

	for (unsigned int i=0; i<strlen(s); i++) {
		if (s[i] == ' ') {
			Int_CharN (val, wd); 
			wd++;
			dest[ch] = '['; 
			ch++;
			for (unsigned int j=0; j<strlen(val); j++) {
				dest[ch] = val[j]; 
				ch++;
			} 
			dest[ch] = ']'; 
			ch++;
		} else {
			dest[ch] = s[i]; 
			ch++;
		}
	}
	dest[ch] = 0;

	string entry, istr;
	string dest2 (dest);
//	string *dest2 = new string (dest);
	for (int i=0; i<wd; i++) {
		istr = Int_StrN (i);
		entry = SPStrN (dest2, istr, "");	
		entry += ' ';
		wordlist->Add (entry);
	}
}

float GetWordLng (const char *word) {
	return FT.GetWidth (word) + 4;		// +4: some space reserve at EOL
}

int MakeLine (int first, CSPList *wordlist, CSPList *linelist, float width) {
	if (first >= wordlist->Count()) return 999;
	float wordlng;

//	string line;
	int last = first-1;
	float lng = 0;

	wordlng = GetWordLng (wordlist->LineC(first));
	bool ready = false;
	do {
		last++;
		lng += wordlng;
		if (last >= wordlist->Count()-1) ready = true;
		if (!ready) {
			wordlng = GetWordLng (wordlist->LineC (last+1));
			if (!ready && lng + wordlng >= width) ready = true;  
		}
	} while (!ready);

	string line = "";
	for (int j=first; j<=last; j++) line += wordlist->Line(j);
	linelist->Add (line);
	return last;
}

// --------------------------------------------------------------------
//				CFont
// --------------------------------------------------------------------

CFont FT;	// external declared

wchar_t *CFont::UnicodeStr (const char *s) {
	wchar_t ch;
	int len = strlen (s);
	wchar_t *res = new wchar_t [len+1];
	int j=0;
		
	for (int i=0; i < len; ++i, ++j ) {
		ch = ((const unsigned char *)s)[i];
		if (ch >= 0xF0) {
			ch  =  (wchar_t) (s[i] & 0x07)   << 18;
			ch |=  (wchar_t) (s[++i] & 0x3F) << 12;
			ch |=  (wchar_t) (s[++i] & 0x3F) << 6;
			ch |=  (wchar_t) (s[++i] & 0x3F);
		} else
			if (ch >= 0xE0) {
				ch  =  (wchar_t) (s[i] & 0x0F)   << 12;
				ch |=  (wchar_t) (s[++i] & 0x3F) << 6;
			ch |=  (wchar_t) (s[++i] & 0x3F);
			} else
				if (ch >= 0xC0) {
					ch  =  (wchar_t) (s[i] & 0x1F) << 6;
					ch |=  (wchar_t) (s[++i] & 0x3F);
				}
				res[j] = ch;
	}
	res[j] = 0;
	return res;
}

void CFont::UnicodeStr (wchar_t *buff, const char *string) {
	wchar_t ch;
	int len = strlen(string);
	int j=0;
		
	for (int i=0; i < len; ++i, ++j ) {
		ch = ((const unsigned char *)string)[i];
		if ( ch >= 0xF0 ) {
			ch  =  (wchar_t)(string[i]&0x07)   << 18;
			ch |=  (wchar_t)(string[++i]&0x3F) << 12;
			ch |=  (wchar_t)(string[++i]&0x3F) << 6;
			ch |=  (wchar_t)(string[++i]&0x3F);
		} else
		if ( ch >= 0xE0 ) {
			ch  =  (wchar_t)(string[i]&0x0F)   << 12;
			ch |=  (wchar_t)(string[++i]&0x3F) << 6;
			ch |=  (wchar_t)(string[++i]&0x3F);
		} else
		if ( ch >= 0xC0 ) {
			ch  =  (wchar_t)(string[i]&0x1F) << 6;
			ch |=  (wchar_t)(string[++i]&0x3F);
		}
		buff[j] = ch;
	}
	buff[j] = 0;
}

CFont::CFont () {
	for (int i=0; i<12; i++) fonts[i] = 0;
	numFonts = 0;
	SetColor (1.0, 1.0, 1.0, 1.0);
	size = 20;
	fontindex = "";
}

CFont::~CFont () {
	for (int i=0; i<12; i++) {
		if (fonts[i] != 0) {
			delete (fonts[i]);
			fonts[i] = 0;
		}
	}
	numFonts = 0;
}	FTGLPixmapFont *fonts[12];


void CFont::SetColor (float r, float g, float b, float a) {
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
}	

void CFont::SetColor (TColor col) { color = col; }
void CFont::SetSize (float siz) { size = siz; }

void CFont::SetFont (char *font) {
	int idx = SPIntN (fontindex, font, -1);
	if (idx >= numFonts) idx = -1;	
	ft = idx;
}

void CFont::SetProps (char *font, float siz, TColor col) {
	SetFont (font);
	SetSize (siz);
	SetColor (col);	
}

bool CFont::LoadFontlist () {
	string fontfile;
	string name;
	string line;

	CSPList list(40);
	if (!list.Load (cfg.font_dir, "fonts.lst")) return false;
	for (int i=0; i<list.Count(); i++) {
		line = list.Line(i);
		fontfile = SPStrN (line, "file", "");
		name = SPStrN (line, "name", "");
		if (LoadFont (cfg.font_dir.c_str(), fontfile.c_str())) {
			SPAddIntN (fontindex, name, numFonts);
			numFonts++; 
		}
	}
	return true;
}

bool CFont::LoadFont (const char *path) {
	if (numFonts >= 12) return false;	
	fonts[numFonts] = new FTGLPixmapFont (path);
	if (fonts[numFonts]->Error()) {
		Message ("Failed to open font \n");
		return false;
	}
	fonts[numFonts]->FaceSize (18);
	fonts[numFonts]->CharMap (ft_encoding_unicode);
	return true;
}

bool CFont::LoadFont (const char *dir, const char *filename) {
	string path = dir;
	path += SEP;
	path += filename;
	return LoadFont (path.c_str());
}

void CFont::DrawText (float x, float y, const char *text) {
	if (ft < 0) return;
	glPushMatrix();
	fonts[ft]->FaceSize ((int)size);
	glColor4f (color.r, color.g, color.b, color.a);
//	glTranslatef(x, y, 0);				// for FTGLTextureFont
	glRasterPos2i ((int)x, (int)y);		// for FTGLPixmapFont
	fonts[ft]->Render (UnicodeStr (text));
	glPopMatrix();
}

void CFont::DrawString (float x, float y, const string &s) {
	if (ft < 0) return;
	glPushMatrix();
	fonts[ft]->FaceSize ((int)size);
	glColor4f (color.r, color.g, color.b, color.a);
//	glTranslatef(x, y, 0);
	glRasterPos2i ((int)x, (int)y);
	fonts[ft]->Render (s.c_str());
	glPopMatrix();
}

void CFont::DrawText (float x, float y, const wchar_t *text) {
	if (ft < 0) return;
	glPushMatrix();
	fonts[ft]->FaceSize ((int)size);
	glColor4f (color.r, color.g, color.b, color.a);
//	glTranslatef(x, y, 0);
	glRasterPos2i ((int)x, (int)y);
	fonts[ft]->Render (text);
	glPopMatrix();
}


void CFont::DrawTextNY (float x, float y, const char *text) {
	if (ft < 0) return;
	float ny = cfg.scrheight - size - y;
	DrawText (x, ny, text);
}

void CFont::DrawTextNY (float x, float y, const wchar_t *text) {
	if (ft < 0) return;
	float ny = cfg.scrheight - size - y;
	DrawText (x, ny, text);
}

void CFont::DrawStringNY (float x, float y, const string &s) {
	if (ft < 0) return;
	float ny = cfg.scrheight - size - y;
	DrawString (x, ny, s);
}

void CFont::BBox (char *font, char *txt, float &llx, float &lly, 
		float &llz, float &urx, float &ury, float &urz) {
	float gg;
	fonts[ft]->BBox (txt, llx, lly, llz, urx, ury, gg);
}

void CFont::GetTextSize (char *txt, float &x, float &y) {
	if (ft < 0) { x = 0; y = 0; return; }
	float llx, lly, llz, urx, ury, urz;
	fonts[ft]->FaceSize ((int)size);
	fonts[ft]->BBox (txt, llx, lly, llz, urx, ury, urz);
	x = urx - llx;
	y = ury - lly;
}

float CFont::GetWidth (const char *txt) {
	if (ft < 0) { 
		Message ("wrong default font");
		return 0.0; 
	}
	float llx, lly, llz, urx, ury, urz;
	fonts[ft]->FaceSize ((int)size);
	fonts[ft]->BBox (txt, llx, lly, llz, urx, ury, urz);
	return urx - llx;
}

void CFont::MakeLineList (const char *source, CSPList *line_list, float width) {
	CSPList wordlist(1000);
	MakeWordList (&wordlist, source);

	int last = -1;
	do { last = MakeLine (last+1, &wordlist, line_list, width); } 
	while (last < wordlist.Count()-1);
}

// --------------------------------------------------------------------
// The following code is from PPRacer (ETRacer)
// It works with binding tables which are not used in 
// Bunny Hill. I keep this class to show the differences.
// Maybe a developer will prefer the PPRacer code.
// --------------------------------------------------------------------

/*
PPFont::PPFont (const char *fileName, unsigned int size, const TColor &color)
: ft_font(NULL), fontcol(color) {
	ft_font = new FTGLTextureFont (fileName);
	ft_font->FaceSize (size);
	ft_font->CharMap (ft_encoding_unicode);	
}

PPFont::PPFont (FTFont *font, const TColor &color)
 : ft_font (font), fontcol (color) {}

PPFont::~PPFont() {}

void PPFont::draw (const char *string, float x, float y) {
	const wchar_t* u_string;
	u_string = PPFont::utf8ToUnicode (string);	

	glPushMatrix();	
		glColor4dv ((double*)&fontcol);
		glTranslatef (x, y, 0);
		ft_font->Render (u_string);
	glPopMatrix();

	delete u_string;
}

void PPFont::draw(const char* string, TVector2 position) {
	draw(string, position.x, position.y);
}

void PPFont::draw(const wchar_t *string, float x, float y) {
	glPushMatrix();
	{
		glColor4dv ((double*)&fontcol );
		glTranslatef(x, y, 0);
		ft_font->Render(string);
	}
	glPopMatrix();
}

void PPFont::draw(const wchar_t *string, TVector2 position) {
	draw(string, position.x, position.y);
}

float PPFont::ascender() {
	return ft_font->Ascender();
}

float PPFont::descender() {
	return ft_font->Descender();
}

float PPFont::advance(const char* string) {
	const wchar_t* u_string;
	u_string = PPFont::utf8ToUnicode(string);	
	
	float adv = ft_font->Advance(u_string);
	
	delete u_string;
	return adv;	
}

float PPFont::advance(const wchar_t* string) {
	return ft_font->Advance(string);	
}

TColor &PPFont::getColor() {
	return fontcol;
}

FTFont *PPFont::getFTFont() {
	return ft_font;
}

std::map<std::string, PPFont*> PPFont::sm_bindings;

bool PPFont::registerFont(const char *binding, const char *fileName, 
		unsigned int size, const TColor &color) {
	sm_bindings[binding] = new PPFont (fileName, size, color);
	return true;
}

bool PPFont::bindFont(const char *binding, const char *fontName) {
	PPFont *font = PPFont::get(fontName);
	if (font==NULL) return false;
	sm_bindings[binding] = new PPFont (font->getFTFont(), font->getColor());
	return true;
}	

bool PPFont::bindFont (const char *binding, const char *fontName, const TColor &color) {
	PPFont *font = PPFont::get (fontName);
	if (font==0) return false;
	sm_bindings[binding] = new PPFont (font->getFTFont(), color);
	return true;
}

PPFont *PPFont::get(const char* binding) {
	if(binding==NULL) return NULL;
	
	std::map<std::string, PPFont*>::iterator it;
	it = sm_bindings.find(binding);
	if (it != sm_bindings.end()) return (*it).second;
	else return NULL;
}

void PPFont::draw (const char* binding, const char *string, float x, float y) {
	PPFont* font = PPFont::get (binding);
	
	if(font!=NULL) font->draw (string, x, y);
}

wchar_t* PPFont::utf8ToUnicode (const char* string) {
	wchar_t ch;
	int len = strlen(string);
	wchar_t *u_string = new wchar_t[len+1];
	int j=0;
		
	for (int i=0; i < len; ++i, ++j ) {
		ch = ((const unsigned char *)string)[i];
		if ( ch >= 0xF0 ) {
			ch  =  (wchar_t)(string[i]&0x07) << 18;
			ch |=  (wchar_t)(string[++i]&0x3F) << 12;
			ch |=  (wchar_t)(string[++i]&0x3F) << 6;
			ch |=  (wchar_t)(string[++i]&0x3F);
		} else
		if ( ch >= 0xE0 ) {
			ch  =  (wchar_t)(string[i]&0x0F) << 12;
			ch |=  (wchar_t)(string[++i]&0x3F) << 6;
			ch |=  (wchar_t)(string[++i]&0x3F);
		} else
		if ( ch >= 0xC0 ) {
			ch  =  (wchar_t)(string[i]&0x1F) << 6;
			ch |=  (wchar_t)(string[++i]&0x3F);
		}
		u_string[j] = ch;
	}
	u_string[j] = 0;
	
	return u_string;
}

void PPFont::utf8ToUnicode (wchar_t* buff, const char* string) {
	wchar_t ch;
	int len = strlen(string);
	int j=0;
		
	for (int i=0; i < len; ++i, ++j ) {
		ch = ((const unsigned char *)string)[i];
		if ( ch >= 0xF0 ) {
			ch  =  (wchar_t)(string[i]&0x07) << 18;
			ch |=  (wchar_t)(string[++i]&0x3F) << 12;
			ch |=  (wchar_t)(string[++i]&0x3F) << 6;
			ch |=  (wchar_t)(string[++i]&0x3F);
		} else
		if ( ch >= 0xE0 ) {
			ch  =  (wchar_t)(string[i]&0x0F) << 12;
			ch |=  (wchar_t)(string[++i]&0x3F) << 6;
			ch |=  (wchar_t)(string[++i]&0x3F);
		} else
		if ( ch >= 0xC0 ) {
			ch  =  (wchar_t)(string[i]&0x1F) << 6;
			ch |=  (wchar_t)(string[++i]&0x3F);
		}
		buff[j] = ch;
	}
	buff[j] = 0;
}
*/



