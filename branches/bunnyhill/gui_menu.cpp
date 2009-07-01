// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "gui_menu.h"
#include "gui_elements.h"
#include "font.h"
#include "textures.h"

#define MIN_BUTTSIZE  20
#define LINEHEIGHT 20
#define BARWIDTH 18

static CSPList internal_list (100);

CMenu::CMenu () {
	numWidgets = 0;
	numMembers = 0;
	for (int i=0; i<MAX_MENU_WIDGETS; i++) widgets[i] = 0;
	dragstate = false;
	focus = -1;
}

CMenu::CMenu (TMenuCallback cb) {
	numWidgets = 0;
	numMembers = 0;
	for (int i=0; i<MAX_MENU_WIDGETS; i++) widgets[i] = 0;
	dragstate = false;
	callback = cb;
	focus = -1;
}

CMenu::~CMenu () { Reset (); }

void CMenu::Reset () {
	for (int i=0; i<MAX_MENU_WIDGETS; i++)
		if (widgets[i] != 0) {
			delete widgets[i];
			widgets[i] = 0;
		}
	numWidgets = 0;
	focus = -1;
}

// --------------------------------------------------------------------
//				groups
// --------------------------------------------------------------------

bool CMenu::GroupOpen () { return (numMembers > 0); }

bool CMenu::IsMember (int idx) {
	bool res = false;
	if (numMembers > 0) {
		for (int i=0; i<numMembers; i++) {
			if (members[i] == idx) res = true;
		}	
	}
	return res;
}

void CMenu::OpenGroup (int idx) {
	if (GroupOpen()) return;
	CWidget *wi = widgets[idx];
	members[numMembers] = idx;
	numMembers++;
	if (wi->child >= 0)	{	
		CWidget *child = widgets[wi->child];
		child->active = true;
		members[numMembers] = wi->child;
		numMembers++;
		if (child->child >= 0)	{
			CWidget *grandchild = widgets[child->child];
			grandchild->active = true;
			members[numMembers] = child->child;
			numMembers++;
		}			
	}
}

void CMenu::CloseGroup () {
	if (!GroupOpen()) return;
	CWidget *wi = widgets[members[0]];
	if (wi->child >= 0)	{	
		CWidget *child = widgets[wi->child];
		child->active = false;
		if (child->child >= 0)	{
			CWidget *grandchild = widgets[child->child];
			grandchild->active = false;
		}			
	}
	numMembers = 0;
}

// --------------------------------------------------------------------
//				focus handling
// --------------------------------------------------------------------

void CMenu::SetFocus (int idx) {
	CWidget *wi = widgets[idx];

	if (focus == idx) return;
	if (idx >= numWidgets) return;
	if (GroupOpen ()) CloseGroup ();
	
	bool focus_poss;
	if (idx >= 0) {
		focus_poss = wi->focus_enabled;
	} else focus_poss = false;

	if (focus_poss) {
		if (focus>=0) widgets[focus]->has_focus = false;
		focus = idx;
		wi->has_focus = true;
	} 
}

void CMenu::NextFocus () {
	if (GroupOpen ()) CloseGroup ();
	if (focus >= 0 && focus < numWidgets) widgets[focus]->has_focus = false;
	int start = focus;
	bool possible = true;
	do {
		start++;
		if (start < numWidgets && start >= 0) {
			possible = (widgets[start]->focus_enabled && widgets[start]->active);
		} else possible = true;
	} while (!possible);

	if (start < numWidgets) {
		focus = start;
		widgets[focus]->has_focus = true;
	} else focus = -1;
}

// --------------------------------------------------------------------	
//				report function (mouse and keyboard)
// --------------------------------------------------------------------	

void CMenu::KeyAction_List (int key) {
	float offs, fact, lastpos;
	float *pos;

	if (focus < 0 || focus >= numWidgets) return;
	CWidget *wi = widgets[focus];

	bool has_scroll = (wi->child >= 0 && wi->child < numWidgets);
	CWidget *scroll;
	if (has_scroll) {
		scroll = widgets[wi->child];
		pos = &scroll->pos;
		fact = scroll->scrollfact;
		lastpos = scroll->lastpos;
	} else {
		scroll = 0;
		pos = 0;
		fact = 1;
		lastpos = 0;
	}

	switch (key) {
		case SDLK_DOWN: 
			if (wi->select < wi->list->Count()-1) wi->select++; 
			if (has_scroll) {
				offs = wi->select * LINEHEIGHT - *pos * fact;
				if (offs > wi->h - 60) *pos += (LINEHEIGHT / fact); 
				if (*pos > lastpos) *pos = lastpos;
			}
			TriggerCallback (focus, wi->select, 0.0);
			break;
		case SDLK_UP: 
			if (wi->select > 0) wi->select--; 
			if (has_scroll) {
				offs = wi->select * LINEHEIGHT - *pos * fact;
				if (offs < 40) *pos -= LINEHEIGHT / fact; 
				if (*pos < 0) *pos = 0;
			}
			TriggerCallback (focus, wi->select, 0.0);
			break;
		default: break;
	}
}

void CMenu::KeyAction (unsigned int key, int x, int y) {
	if (key == SDLK_TAB) {
		NextFocus ();
		return;
	} 
	if (focus < 0) return;

	CWidget *wi = widgets[focus];
	switch (widgets[focus]->type) {
		case W_BUTTON: 
			if (key == SDLK_RETURN) TriggerCallback (focus, 0, 0);
			break;
		case W_LIST:
			if (key == SDLK_RETURN) TriggerCallback (focus, -1, 0);
			else KeyAction_List (key);
			break;
		case W_SELBUTTON:
		case W_IMGBUTTON:
			if (key == SDLK_DOWN) {
				wi->select++;
				if (wi->select >= wi->maxSelect) wi->select = 0;			
				TriggerCallback (focus, wi->select, 0);
			}
			if (key == SDLK_UP) {
				wi->select--;
				if (wi->select < 0) wi->select = wi->maxSelect-1;			
				TriggerCallback (focus, wi->select, 0);
			}
			if (key == SDLK_RETURN) TriggerCallback (focus, -1, 0);
			break;
		default: break;
	}
}

void CMenu::MouseAction_List (int idx, int x, int y) {
	CWidget *wi = 0; 
	CWidget *child = 0;
	float offs;

	if (idx < 0 || idx >= numWidgets) return;
	wi = widgets[idx];
	if (wi->child >= 0 && wi->child < numWidgets) {
		child = widgets[wi->child];
		offs = child->pos * child->scrollfact;
	} else offs = 0; 

	float xpos = y - wi->y + offs;
	int line = (int)(xpos / LINEHEIGHT);
	if (line >=0 && line < wi->list->Count()) {
		int sel = line;
		wi->select = sel;
	} 
}

void CMenu::MouseAction (int button, bool down, int x, int y) {
	if (dragstate) dragstate = false;
	int idx = FindRegion (x, y);
 	if (down && (!IsMember(idx))) SetFocus (idx);
	if (idx < 0) return;

	CWidget *wi = widgets[idx];
	if (down) {
		switch (wi->type) {
			case W_VSCROLL: 
				dragstate = true;
				starty = y;
				dragidx = idx;
				startpos = wi->pos;
				break;
			case W_LIST: MouseAction_List (idx, x, y); 
				TriggerCallback (wi->self, wi->select, 0);
				break;
			case W_BUTTON: TriggerCallback (idx, 0, 0); break;
			case W_TEXTBLOCK: TriggerCallback (wi->self, 0, 0); break;
			case W_SELBUTTON: 
			case W_IMGBUTTON: 
				wi->select++;
				if (wi->select >= wi->maxSelect) wi->select = 0;
				TriggerCallback (wi->self, wi->select, 0);
				break;
			default: break;
		}
	}
}

void CMenu::MouseMotion (bool down, int x, int y) {
	float diff = 0;
	float newpos;
	CWidget *wi = widgets[dragidx];
	if (down) {
		if (dragstate) {
			if (wi->type == W_VSCROLL) diff = y - starty;
			newpos = MIN (MAX (0, startpos + diff), wi->lastpos);
			wi->pos = newpos;
		}
	}
}

void CMenu::TriggerCallback (int idx, int select, float val) {
	if (callback != 0) {(*callback) (idx, select, val);}
}

int CMenu::FindRegion (int x, int y) {
	int res = -1;
	int layer = -1;
	int newlayer;
	bool found;

	for (int i=0; i<numWidgets; i++) {
		found = false;
		CWidget *wi = widgets[i];

		if (wi->type == W_VSCROLL) {
			if (x > wi->x && 
				x < wi->x + wi->w && 
				y > wi->y && 
				y < wi->y + wi->h) found = true;
		} else {
			if (x > wi->x && 
				x < wi->x + wi->w && 
				y > wi->y && 
				y < wi->y + wi->h) found = true;
		}

		if (found) {
			newlayer = wi->layer;
			if (newlayer > layer && wi->active) {
				layer = newlayer;
				res = i;
			}
		}
	}
	return (res);
}


// --------------------------------------------------------------------
//					Draw
// --------------------------------------------------------------------

void CMenu::DrawButton (int idx) {
	CWidget *wi = widgets[idx];
	TRegion tex = MakeRegion (0.5, 0.75, 0, 0.125);
	if (wi->has_focus) {
		tex.left += 0.25;
		tex.right += 0.25;
	}

	TColor framecol;
	if (wi->has_focus) framecol = colWhite; else framecol = colBlack;

	DrawQuad (wi->x, wi->y, wi->w, wi->h, cfg.scrheight, framecol, 1);
	DrawTexquad (wi->x, wi->y, wi->w, wi->h, cfg.scrheight, tex, TexID(T_WIDGETS));
	TColor textcol;
	if (wi->has_focus) textcol = colBlack; else textcol = colWhite; 
	FT.SetProps ("normal", 16, textcol);
	FT.DrawStringNY (wi->x + 15, wi->y + 2, wi->label);
}

void CMenu::DrawVScroll (int idx) {
	CWidget *wi = widgets[idx];
	TRegion tex = MakeRegion (0.5, 0.75, 0, 0.125);

	TColor framecol;
	framecol = colBlack;
	TColor backcol;
	backcol = colDBackgr;
	DrawFrame (wi->x, wi->y, wi->w, wi->h, cfg.scrheight, framecol, backcol, 2);
	DrawRotatedTexquad (wi->x, wi->y+wi->pos, wi->w, 
		wi->buttsize, cfg.scrheight, tex, TexID(T_WIDGETS));
}

void CMenu::DrawList (int idx) {
	float xpos, ypos;
	CWidget *scroll = 0;
	CWidget *wi = widgets[idx];
	if (wi->child >= 0) scroll = widgets[wi->child]; 
	if (scroll) wi->offs = scroll->pos * scroll->scrollfact; else wi->offs = 0;

	TColor framecol;
	if (wi->has_focus) framecol = colWhite; else framecol = colBlack;
	TColor backcol = colLBackgr;
	TColor textcol = colBlack;
	DrawFrame (wi->x, wi->y, wi->w, wi->h, cfg.scrheight, framecol, backcol, 2);
	if (wi->list) {
		xpos = wi->x + 10;
		for (int i=0; i<wi->list->Count(); i++) {
 			if (wi->select == i) textcol = colLYell; 
			else {
				if (wi->list->Flag(i) >= 0) textcol = colBlack;
				else textcol = colGrey;
			}
			FT.SetProps ("normal", 14, textcol);
			ypos = wi->y + LINEHEIGHT * i + 1 - wi->offs;
			if (ypos>wi->y && ypos < wi->y + wi->h - LINEHEIGHT + 2) { 
				FT.DrawStringNY (xpos, ypos, wi->list->Line(i));
			}			
		}
	}
}

void CMenu::DrawTextblock (int idx) {
	float xpos, ypos;

	CWidget *wi = widgets[idx];
	CWidget *scroll = 0;
	if (wi->child >= 0) scroll = widgets[wi->child]; 
	if (scroll) wi->offs = scroll->pos * scroll->scrollfact; else wi->offs = 0;

	TColor framecol;
	framecol = colBlack;
	TColor backcol = colDBackgr;
	DrawFrame (wi->x, wi->y, wi->w, wi->h, cfg.scrheight, framecol, backcol, 2);

	if (wi->list == 0) return;
	xpos = wi->x + 10;
	float lineheight = wi->textsize + 4;
	FT.SetProps ("normal", wi->textsize, colWhite);

	for (int i=0; i<wi->list->Count(); i++) {
		ypos = wi->y + lineheight * i + 1 - wi->offs;
		if (ypos>wi->y && ypos < wi->y + wi->h - lineheight + 2) { 
			FT.DrawStringNY (xpos, ypos, wi->list->Line(i));
		}			
	}	
}

void CMenu::DrawSelButton (int idx) {
	CWidget *wi = widgets[idx];
	float left;
	if (wi->icons == 0) left = 0.125 * wi->select;
	else left = 0.125 * wi->icons[wi->select];

	float bottom = 0.125 * wi->texrow;
	TRegion tex = MakeRegion (left, left + 0.125, bottom, bottom + 0.125);

	TColor framecol;
	if (wi->has_focus) framecol = colWhite; else framecol = colBlack;
	DrawFrame (wi->x, wi->y, 32, 32, cfg.scrheight, framecol, colWhite, 2);
	DrawTexquad (wi->x, wi->y, 32, 32, cfg.scrheight, tex, TexID(T_WIDGETS));
}

void CMenu::DrawImageButton (int idx) {
	CWidget *wi = widgets[idx];
	TRegion tex = MakeRegion (0, 1, 0, 1);
	TColor framecol;
	if (wi->has_focus) framecol = colWhite; else framecol = colBlack;
	DrawFrame (wi->x, wi->y, wi->w, wi->h, cfg.scrheight, framecol, colLGrey, 2);
	DrawTexquad (wi->x, wi->y, wi->w, wi->h, cfg.scrheight, tex, wi->texid);
}

void CMenu::Draw () {
	for (int layer=0; layer<3; layer++) {
		for (int i=0; i<numWidgets; i++) {
			if (widgets[i]->layer == layer && widgets[i]->active) {
				switch (widgets[i]->type) {
					case W_BUTTON: DrawButton (i); break;
					case W_VSCROLL: DrawVScroll (i); break;
					case W_LIST: DrawList (i); break;
					case W_TEXTBLOCK: DrawTextblock (i); break;
					case W_SELBUTTON: DrawSelButton (i); break;
					case W_IMGBUTTON: DrawImageButton (i); break;
					default: break;
				}
			}
		}
	}
}

// --------------------------------------------------------------------
//				create
// --------------------------------------------------------------------

int CMenu::CreateWidget (TWidgetType type) {
	int nr = numWidgets;
	widgets[nr] = new CWidget;
	widgets[nr]->type = type;
	widgets[nr]->self = nr;
	numWidgets++;
	return nr;
}

int CMenu::AddButton (float x, float y, float w, float h, char *label) {
	if (numWidgets >= MAX_MENU_WIDGETS) return -1;

	int nr = CreateWidget (W_BUTTON);
	widgets[nr]->SetGeom (x, y, w, h);
	widgets[nr]->label = label;
	return nr;
}

int CMenu::AddVScroll (float x, float y, float w, float h) {
	if (numWidgets >= MAX_MENU_WIDGETS) return -1;

	int nr = CreateWidget (W_VSCROLL);
	widgets[nr]->SetGeom (x, y, w, h);
	widgets[nr]->pos = 0;
 	widgets[nr]->buttsize = MIN_BUTTSIZE;
	widgets[nr]->lastpos = h - MIN_BUTTSIZE;
	return nr;
}

void CMenu::AdjustScrollParams (int idx) {
	float lineheight;

	CWidget *wi = widgets[idx];
	if (wi->child < 0) return;
	CWidget *child = widgets[wi->child];
	if (wi->list == 0) return;

	lineheight = wi->textsize + 4;
	int numentries = wi->list->Count (); 
	int possentries = (int)((wi->h - 10.0) / lineheight);
	float neededplace = (numentries - possentries) * lineheight;  
	if (neededplace > 0.0) {
		if (wi->h - MIN_BUTTSIZE >= neededplace)	{	
			child->lastpos = neededplace;
			child->buttsize = wi->h - neededplace;
			child->scrollfact = 1.0;
		} else {
			child->lastpos = wi->h - MIN_BUTTSIZE;
			child->buttsize = MIN_BUTTSIZE;
			child->scrollfact = neededplace / child->lastpos;
		}
	} else {
		child->lastpos = 0.0;
		child->buttsize = wi->h;
		child->scrollfact = 1.0;
	}
	child->pos = 0;
}

int CMenu::AddList (float x, float y, float w, float h, CSPList *list, float scrollwidth) {
	if (numWidgets >= MAX_MENU_WIDGETS - 1) return -1;

	int nr = CreateWidget (W_LIST);
	widgets[nr]->SetGeom (x, y, w, h);
	widgets[nr]->list = list;
	widgets[nr]->select = 0;
	widgets[nr]->offs = 0;

	int scroll = CreateWidget (W_VSCROLL);
	widgets[scroll]->SetGeom (x + w + 6, y, scrollwidth, h);
	widgets[scroll]->focus_enabled = false;
	widgets[scroll]->layer = 0;
	widgets[scroll]->pos = 0;
	widgets[scroll]->active = true;

	widgets[scroll]->parent = nr;
	widgets[nr]->child = scroll;

	AdjustScrollParams (nr);
	return nr;
}

int CMenu::AddTextblock (float x, float y, float w, float h, float txtsize, 
		float scrollwidth) {
	if (numWidgets >= MAX_MENU_WIDGETS - 1) return -1;

	int pan = CreateWidget (W_TEXTBLOCK);
	widgets[pan]->SetGeom (x, y, w-scrollwidth-6, h);
	widgets[pan]->textsize = txtsize;
	widgets[pan]->focus_enabled = false;

	int scroll = CreateWidget (W_VSCROLL);
	widgets[scroll]->SetGeom (x + w-scrollwidth, y, scrollwidth, h);
	widgets[scroll]->focus_enabled = false;
	widgets[scroll]->layer = 1;
	widgets[scroll]->pos = 0;
	widgets[scroll]->active = true;

	widgets[scroll]->parent = pan;
	widgets[pan]->child = scroll;
	return pan;
}

int CMenu::AddSelButton (float x, float y, int row) {
	if (numWidgets >= MAX_MENU_WIDGETS) return -1;
	int nr = CreateWidget (W_SELBUTTON);
	widgets[nr]->SetGeom (x, y, 32, 32);
	widgets[nr]->texrow = row;
	return nr;	
}
	
int CMenu::AddImageButton (float x, float y, float w, float h, int num) {
	if (numWidgets >= MAX_MENU_WIDGETS) return -1;
	int nr = CreateWidget (W_IMGBUTTON);
	widgets[nr]->SetGeom (x, y, w, h);
	widgets[nr]->maxSelect = num;
	return nr;	
}

void CMenu::SetText (int idx, const char *txt) {
	if (idx < 0 || idx >=numWidgets) return;
	CWidget *wi = widgets[idx];

	wi->list = &internal_list;
	wi->list->Clear ();
	FT.SetProps ("normal", wi->textsize, colBlack);
	FT.MakeLineList (txt, wi->list, wi->w-20);
	AdjustScrollParams (idx);
}

void CMenu::SetTexid (int idx, GLuint texid) {
	if (idx < 0 || idx >=numWidgets) return;
	widgets[idx]->texid = texid;
}

void CMenu::SetIcons (int idx, int num, int *icons) {
	if (idx < 0 || idx >=numWidgets) return;
	CWidget *wi = widgets[idx];
	wi->icons = icons;
	wi->maxSelect = num;
	wi->select = 0;
}


