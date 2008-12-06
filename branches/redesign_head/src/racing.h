/* 
 * PPRacer 
 * Copyright (C) 2004-2005 Volker Stroebel <volker@planetpenguin.de>
 * 
 * Copyright (C) 1999-2001 Jasmin F. Patry
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef _RACING_H_
#define _RACING_H_

#include "loop.h"

#include "racer.h"
#include "humanracer.h"
#include <vector>

class Racing : public GameMode
{
public:
	Racing();
	~Racing();

	void loop(float timeStep);
  void updatePhysics(float);
  void renderView(void);

	bool keyboardEvent(SDLKey key, bool release);
	bool keyPressEvent(SDLKey key);

private:
  // Use a vector because we'll only be adding to the end, and
  // don't need to traverse backwards through it.
  std::vector<Racer*> mRacers;

public: //eek!
  //these need to go away
	bool m_rightTurn;
	bool m_leftTurn;
	bool m_trickModifier;
	bool m_paddling;
	bool m_charging;
	bool m_braking;
	double m_chargeStartTime;
	int m_lastTerrain;
	
	void calcJumpAmt( double time_step );
};


#endif
