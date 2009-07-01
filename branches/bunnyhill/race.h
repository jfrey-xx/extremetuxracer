// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

// The unit race.cpp controls the challenge-orientated processes during
// the race. The algorithm for calculating the score should be reworked.

#ifndef RACE_H
#define RACE_H

#include "bh.h"

class CRace {
private:

public:
	CRace ();
	~CRace ();

	float time;
	int herring;
	int score;	
	float way;
	float final_speed;

	void Update (TControl *ctrl);		// calculate the way, called in physics.cpp
	void Reset ();						// set current time, herrings ... to 0
	bool HerringSuccess (); 			// true if enough herrings are catched 
	bool TimeSuccess (TControl *ctrl);	// true if estimated time is short enough
	bool RaceSuccess (TControl *ctrl);	// final result 
	int ScorePoints ();					// algorithm for calculating the score
	int ReachedLevel ();
};

#endif

extern CRace Race;
