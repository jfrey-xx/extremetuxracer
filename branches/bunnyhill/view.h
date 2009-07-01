// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
//
// --------------------------------------------------------------------

#ifndef VIEWMODE_H
#define VIEWMODE_H

#include "bh.h"
#include "player.h"

typedef enum {INSIDE, INTERSECT, OUTSIDE} TClipRes;

class TuxView {
private:
 	TPlane basplanes[6]; // vorausberechnete Planebasis
	TPlane fplanes[6];  
	char pvc[6];    
	bool initialized;
	TViewMode viewmode;

	void SetupViewMatrix (TControl *ctrl);
	void UpdateView (float timestep);
	void SetupFrustum (TControl *ctrl);
public:
	TuxView ();
	~TuxView ();
	void Init ();

	void RacingView (float timestep, TControl *ctrl);
	void StartView (TVector3 refpos, TControl *ctrl);
	void FinishView (TControl *ctrl);

	TClipRes ClipToFrustum (TVector3 min, TVector3 max);
	TClipRes clip_aabb_to_view_frustum (TVector3 min, TVector3 max);
	void SetViewMode (TViewMode mode) {viewmode = mode;}	
	void Reset () {initialized = false;}
	TPlane GetLeftPlane ()   {return fplanes[2];}
	TPlane GetRightPlane ()  {return fplanes[3];}
	TPlane GetBottomPlane () {return fplanes[5];}
	TPlane GetFarPlane ()    {return fplanes[1];}
	void setup_view_frustum (TControl *ctrl);
};

extern TuxView View;

#endif






