// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "race.h"
#include "course.h"
#include "info.h"

// --------------------------------------------------------------------
//					setting
// --------------------------------------------------------------------

#define NML_TIME_POINTS 200
#define TIME_RANGE 0.2
#define NML_HERRING_POINTS 50
#define HERRING_RANGE 0.2
#define NML_POINTS 200

// --------------------------------------------------------------------

CRace Race;

CRace::CRace () {}
CRace::~CRace () {}

void CRace::Reset () {
	herring = 0;
	time = 0.0;
	score = 0;
	way = 0.0;
}

void CRace::Update (TControl *ctrl) {
	float step = VecLen (SubVec (ctrl->pos, ctrl->lastpos));
	way += step;
}

bool CRace::HerringSuccess () {
	int idx = game.course_id;
	switch (game.racemode) {
		case TIME_HERRING: 
			return (herring >= CourseInfo.GetHerring (idx, game.level)); break;
		case TRAINING: return true; break;
		case SCORE: return true; break;
		default: return true; 
	}
}

bool CRace::TimeSuccess (TControl *ctrl) {
	float timelimit;
	int idx = game.course_id;
	
	if (ctrl->final_state || game.mode == FINISH) {
		timelimit = CourseInfo.GetTime (idx, game.level);
	} else {
		timelimit = CourseInfo.GetTime (idx, game.level) * 
			(Course.startpoint.y - ctrl->pos.z) / 
			(Course.play_length - Course.startpoint.y);
	}

	switch (game.racemode) {
		case TRAINING: return true; break;
 		case TIME_HERRING: return (timelimit > time); break;
		case SCORE: return true; break;
		default: return true;
	}
}

bool CRace::RaceSuccess (TControl *ctrl) {
	switch (game.racemode) {
		case TRAINING: return true; break;
		case TIME_HERRING: return (TimeSuccess(ctrl) && HerringSuccess()); break;
		case SCORE: return true; break;
		default: return true;
	}
}

int CRace::ReachedLevel () {
	int res = 0;
	int idx = game.course_id;
	if (CourseInfo.GetTime (idx, 0) > time && 
		herring >= CourseInfo.GetHerring (idx, 0)) res=1;
	if (CourseInfo.GetTime (idx, 1) > time && 
		herring >= CourseInfo.GetHerring (idx, 0)) res=2;
	if (CourseInfo.GetTime (idx, 2) > time && 
		herring >= CourseInfo.GetHerring (idx, 0)) res=3;
	return res;
}

int CRace::ScorePoints () {
	float herring_points, herring_weight;
	float time_points, time_weight;
	int idx = game.course_id;

	if (CourseInfo.GetTime (idx, 1) > 9) {
		time_weight = NML_TIME_POINTS / (TIME_RANGE * CourseInfo.GetTime (idx, 1)); 
		time_points = (CourseInfo.GetTime (idx, 1) - time) * time_weight;
	} else return 0;

	if (CourseInfo.GetHerring (idx, 1) > 9) {
		herring_weight = NML_HERRING_POINTS / (HERRING_RANGE * CourseInfo.GetHerring (idx, 1));
		herring_points = (herring - CourseInfo.GetHerring (idx, 1)) * herring_weight;
	} else herring_points = 0;

	float score = time_points + herring_points + NML_POINTS;
	if (score < 0) score = 0;
	return (int) score;
}
