/* 
 * Extreme Tux Racer
 * Copyright (C) 2008 Steven Bell <botsnlinux@gmail.com>
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
#ifndef _HUMANRACER_H_
#define _HUMANRACER_H_

#include "racer.h"

class HumanRacer : public Racer
{
public:
  HumanRacer();

  //! Overrides function from Racer
  void updatePosition(float timeStep);

  //! Overrides function from Racer
  void keyboardEvent(SDLKey key, bool release);



private:





  //! Turn animation amount.  Floating-point value from -1 to +1
	float mTurnAnimation;

private: //helper functions
  void updateTurnAnimation(float);


};

#endif
