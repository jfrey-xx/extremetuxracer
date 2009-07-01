// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "player.h"

// externals:
TSteerCtrl steer;
CPlayer Player;

CPlayer::CPlayer () { numPlayers = 1; }
CPlayer::~CPlayer () {}

TControl *CPlayer::ctrl (int plyr) {
	if (plyr < 0 || plyr >= numPlayers) return 0;
	return &players[plyr].control;
}

void SetPlyrPaddling (bool val) {
	if (val==true) steer = steer | 1;
	else steer = steer & 254;
}

void SetPlyrBraking (bool val) {
	if (val==true) steer = steer | 2;
	else steer = steer & 253;
}

void SetPlyrTurnleft (bool val) {
	if (val==true) steer = steer | 4;
	else steer = steer & 251;
}

void SetPlyrTurnright (bool val) {
	if (val==true) steer = steer | 8;
	else steer = steer & 247;
}

void SetPlyrJumping (bool val) {
	if (val==true) steer = steer | 16;
	else steer = steer & 239;
}

void SetExtPaddling (bool val) {
	if (val==true) steer = steer | 32;
	else steer = steer & 223;
}

void SetExtBraking (bool val) {
	if (val==true) steer = steer | 64;
	else steer = steer & 191;
}



















