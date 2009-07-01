// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 2004-2005 Volker Stroebel <volker@planetpenguin.de>
//
// --------------------------------------------------------------------

#ifndef IMAGE_H
#define IMAGE_H

#include "bh.h"

// This module is for loading and saving images. At the moment
// it is only used for screenshots.
// The loading functions for textures are in textures.cpp

class CImage {
public:
	CImage (int width=0, int height=0, int depth=3);
	~CImage (void);
	
	unsigned short width, height;
	unsigned char depth;
    unsigned char *data;
		
	bool Save (const char *filename) const;
	static CImage *Load (const char *filename);
};


class CPPMWriter {
public:
	CPPMWriter (const char *fileName, const CImage &image);
	~CPPMWriter () {};
};

void Screenshot ();

#endif


