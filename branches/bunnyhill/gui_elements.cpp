// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "gui_elements.h"
#include "font.h"

// --------------------------------------------------------------------
//					CWidget
// --------------------------------------------------------------------

CWidget::CWidget () {
	SetDefaults ();
}

CWidget::CWidget (float xpos, float ypos, float width, float height) {
	x = xpos;
	y = ypos;
	w = width;
	h = height;
	SetDefaults ();
}

void CWidget::SetGeom (float xpos, float ypos, float width, float height) {
	x = xpos;
	y = ypos;
	w = width;
	h = height;
}

void CWidget::SetDefaults () {
	focus_enabled = true;	
	has_focus = false;		
	layer = 0;				
	type = W_NONE;			
	active = true;			

	pos = 0;				
	lastpos = 0;			
	scrollfact = 1;			
	select = 0;				
	buttsize = 20;
	textsize = 16;
	maxSelect = 0;
	icons = 0;
	texid = 0;

	self = parent = child = -1;
	label = "";	
	list = 0;	
}






