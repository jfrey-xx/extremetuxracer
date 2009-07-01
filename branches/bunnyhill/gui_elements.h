// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

// This is the base module for widgets

#ifndef GUI_ELEMENTS_H
#define GUI_ELEMENTS_H

#include "bh.h"

typedef enum {
	W_NONE,
	W_BUTTON,
	W_VSCROLL,
	W_LIST,
	W_PULLDOWN,
	W_TEXTBLOCK,
	W_SELBUTTON,
	W_IMGBUTTON,
} TWidgetType;

class CWidget {	
private:
	void SetDefaults ();
public:
	CWidget ();
	CWidget (float xpos, float ypos, float width, float height);
	~CWidget () {}

	TWidgetType type;		
	float x, y, w, h;

	// later perhaps with access functions
 	bool   focus_enabled;	// only main elementes can get the focus (default true)
	bool   has_focus;		// true, if the element has focus (default false)
	int    layer;			// drawing layer, 0 = lowest
	string label;			// button text
	CSPList *list;			// pointer for W_LIST
	GLuint texid;
	bool active;			// drawable if true

	// setting variables
	float pos;				// current position of scroll elements
	float lastpos;			// last position of scroll elements
	float buttsize;			// current height of scroll buttons (changes)
	int   select;			// point on a selected entry in W_List
	float offs;				
	float scrollfact;		// scroll position vs. list position
	float textsize;
	int maxSelect;
	int *icons;
	
	// some indices
	int  self;				// own index in widget array
	int  parent;			
	int  child;				

	int texrow;
	void SetGeom (float xpos, float ypos, float width, float height);
};

#endif


