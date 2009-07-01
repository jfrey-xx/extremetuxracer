// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#ifndef GUI_MENU_H
#define GUI_MENU_H

#include "bh.h"
#include "gui_elements.h"

#define MAX_MENU_WIDGETS 64

typedef void (*TMenuCallback) (int idx, int select, float val);

class CMenu {
private:
	int numWidgets;
 	CWidget *widgets[MAX_MENU_WIDGETS];	
	int textinfo;
	TMenuCallback callback;

	// dragging
	bool dragstate;
	float starty;
	float startpos;
	int dragidx;

	// focus handling
	int focus;
	void NextFocus ();

	// opening childs in a group
	int members[8];
	int numMembers;
	bool GroupOpen ();
	bool IsMember (int idx);
	void OpenGroup (int idx);
	void CloseGroup ();

	void TriggerCallback (int idx, int select, float val);
	int  FindRegion (int x, int y);
	void DrawButton (int idx);
	void DrawVScroll (int idx);
	void DrawList (int idx);
	void DrawTextblock (int idx);
	void DrawSelButton (int idx);
	void DrawImageButton (int idx);
	void AdjustScrollParams (int idx);
public:
	CMenu ();
	CMenu (TMenuCallback cb);
	~CMenu ();	
	void Reset ();
 	void Draw ();

	void KeyAction_List (int key);
	void KeyAction (unsigned int key, int x, int y);
	void MouseAction_List (int idx, int x, int y);
	void MouseAction (int button, bool down, int x, int y);
	void MouseMotion (bool down, int x, int y);
	void SetCallback (TMenuCallback cb) {callback = cb;}

	int  CreateWidget (TWidgetType type);
	int  AddButton (float x, float y, float w, float h, char *label);
	int  AddVScroll (float x, float y, float w, float h);
	int  AddList (float x, float y, float w, float h, CSPList *list, float scrollwidth);
	int  AddTextblock (float x, float y, float w, float h, float txtsize, float scrollwidth);
	int  AddSelButton (float x, float y, int row);
	int  AddImageButton (float x, float y, float w, float h, int num);

	void SetText (int idx, const char *txt);
	void SetIcons (int idx, int num, int *icons);
	void SetFocus (int idx);
	void SetTexid (int idx, GLuint texid);
	bool IsEmpty () { return (numWidgets < 1); }
};

#endif


