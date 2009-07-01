// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
//
// --------------------------------------------------------------------

#ifndef KEYFRAME_H
#define KEYFRAME_H

#include "bh.h"

#define MAX_KEY_FRAMES 128

typedef struct {
    TVector3 pos;
    float	time;
    float	yaw;      
    float	pitch;    
    float	l_shldr;
    float	r_shldr;
    float	l_hip;
    float	r_hip;
	float	l_knee;
	float	r_knee;
	float	l_ankle;
	float	r_ankle;
	float	neck;
	float	head;
} TKeyframe; 

class CKeyframe {
private:
	float keytime;
	int numFrames;
	TKeyframe frames[MAX_KEY_FRAMES];
	TVector3 refpos;
	int keyidx;

	float interp (float frac, float v1, float v2);
	void InterpolateKeyframe (int idx, float frac);
public:
	CKeyframe ();
	~CKeyframe ();

	bool active;
	bool loaded;
	void Init (TVector3 ref_pos);
	void Reset ();
	void Update (float timestep, TControl *ctrl);
	void TestUpdate (float timestep);
	void Load (string filename);
};

extern CKeyframe TuxStart;
extern CKeyframe TuxFailure;
extern CKeyframe TuxSuccess;
extern CKeyframe TuxFinal;

// the test keyframe is only for editing keyframes
extern CKeyframe TuxTest;

#endif
