// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 2004-2005 Volker Stroebel <volker@planetpenguin.de>
//
// --------------------------------------------------------------------

#include "image.h"
#include <fstream>
#include "course.h"

// --------------------------------------------------------------------
//				class CPPMWriter
// --------------------------------------------------------------------

CPPMWriter::CPPMWriter (const char *fileName, const CImage &image) {
	std::ofstream file;
	file.open (fileName);

	file << "P6\n# A Raw PPM file"
		<< "\n# width\n" << int(image.width)
		<< "\n# height\n" << int(image.height)
		<< "\n# max component value\n255"<< std::endl;

	file.write ((const char*) image.data, image.width * image.height * image.depth);
	file.close ();
}

// --------------------------------------------------------------------
//				class CImage
// --------------------------------------------------------------------

CImage::CImage (int width, int height, int depth) {
	if (width>0 && height>0 && depth>0 && depth<4) { 
		this->width=width;
		this->height=height;
		this->depth=depth;
		this->data = new unsigned char [width * height * depth];
		return;
	} else {
		this->width=0;
		this->height=0;
		this->depth=0;
		this->data=0;
		return;
	}
}

CImage::~CImage(void) {
	if (data != 0) delete data;	
}

CImage *CImage::Load (const char *fileName) {
	CImage *image = 0;
	int len = strlen(fileName);
	
	if (!strcmp (fileName + len-3, "png")) {
		// Reading has to be implemented later:
		// CImage* image = new ReaderPNG (fileName);
		
		if (image->data != 0){
			return image;		
		} else {
			delete image;
			return 0;
		}
	} else return 0;		
}

bool CImage::Save (const char *fileName) const {
	CPPMWriter writer (fileName, *this);
	return true;
}

// --------------------------------------------------------------------
//					screenshot
// --------------------------------------------------------------------

bool TakeScreenshot (const char *fileName) {
	int viewport[4];
	glGetIntegerv (GL_VIEWPORT, viewport);
	glReadBuffer (GL_FRONT);
	
	CImage image (viewport[2], viewport[3], 3);
	
	image.width = viewport[2];
	image.height = viewport[3];
	image.depth = 3;
	
	for (int i=0; i<viewport[3]; i++){
		glReadPixels (viewport[0], viewport[1] + viewport[3] - 1 - i,
			viewport[2], 1, GL_RGB, GL_UNSIGNED_BYTE, image.data + viewport[2] * i * 3);
	}
	return image.Save (fileName);
}

// before saving the screenshot the name is generated in this way:
// first the course name, then a random number 0 ... 9999999
// the probability of equal numbers is quite low.
void Screenshot () {
	string buff = cfg.screenshot_dir;
	buff += SEP;
	buff += Course.name;
	buff += '_';
	buff += Int_StrN (irandom (0, 999999), 6);
    if (!TakeScreenshot (buff.c_str())) Message ("screenshot failed");
} 






