// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#ifndef TRACKMARKS_H
#define TRACKMARKS_H

#include "bh.h"

#define MAX_TRACK_MARKS 2000

typedef enum {
    TRACK_HEAD,
    TRACK_MARK,
    TRACK_TAIL,
} TTrackType;

typedef struct {
    TVector3 v1, v2, v3, v4;
    TVector2 t1, t2, t3, t4;
    TVector3 n1, n2, n3, n4;
    TTrackType track_type;
	int terr;
    float alpha;
} TTrackQuad;

typedef enum {
	TRACKBREAK,
	TRACKSTART,
	TRACKCONT,
} TTrackMode;

typedef struct {
	int numQuads;
	TTrackQuad Quads[MAX_TRACK_MARKS];
} TTrack;

// --------------------------------------------------------------------

class CTrackmarks {
private:
	TTrack CurrTrack;
	TTrack PrevTrack;
	TTrackMode mode;
	TTrackQuad temp_quad;
	TVector3 last_pos;
	TVector2 last_dir;
	int last_terr;

	void SetTrackMaterial (TColor diff, TColor spec, float exp);
	void DrawQuad (TTrackQuad *quad);
public:
	CTrackmarks ();
	~CTrackmarks ();

	void ResetCurrentTrack ();
	void ResetLastTrack ();
	void DrawCurrentTrack (TControl *ctrl);
	void DrawPreviousTrack (TControl *ctrl);
	void Add (float timestep, TControl *ctrl);
	void FinishTrack ();
	void CopyTrack ();
};

extern CTrackmarks Trackmarks;

#endif
