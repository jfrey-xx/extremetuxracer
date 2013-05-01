/* --------------------------------------------------------------------
EXTREME TUXRACER

Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
Copyright (C) 2010 Extreme Tuxracer Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
---------------------------------------------------------------------*/

#ifndef PARTICLES_H
#define PARTICLES_H

#include "bh.h"
#include <vector>

// --------------------------------------------------------------------
//					snow for menu screens
// --------------------------------------------------------------------

void init_ui_snow (void );
void update_ui_snow (double time_step);
void push_ui_snow (const TVector2& pos );
void draw_ui_snow (void );
void make_ui_snow (const TVector2& pos );
void reset_ui_snow_cursor_pos (const TVector2& pos );

// --------------------------------------------------------------------
//					snow particles during race
// --------------------------------------------------------------------

void create_new_particles (const TVector3& loc, TVector3 vel, int num );
void update_particles (double time_step );
void clear_particles ();
void draw_particles (CControl *ctrl );
void generate_particles (CControl *ctrl, double dtime, const TVector3& pos, double speed);

// --------------------------------------------------------------------
//					snow flakes for short distances
// --------------------------------------------------------------------

struct TFlake {
    TVector3 pt;
    float   size;
    TVector3 vel;
    TVector2 tex_min;
    TVector2 tex_max;

	void Draw(const TPlane& lp, const TPlane& rp, bool rotate_flake, float dir_angle) const;
};

struct TFlakeArea {
	float xrange;
	float ytop;
	float yrange;
	float zback;
	float zrange;
	float minSize;
	float maxSize;
	float speed;
	bool  rotate_flake;
	
	float left;
	float right;
	float bottom;
	float top;
	float front;
	float back;

	vector<TFlake> flakes;

	TFlakeArea(
		int num_flakes, 
		float xrange,
		float ytop,
		float yrange,
		float zback,
		float zrange,
		float minSize,
		float maxSize,
		float speed,
		bool  rotate);
	void Draw(CControl* ctrl) const;
	void Update(float timestep, float xcoeff, float ycoeff, float zcoeff);
};

class CFlakes {
private:
	TVector3 snow_lastpos;
	vector<TFlakeArea> areas;
	void MakeSnowFlake (size_t ar, size_t i);
	void GenerateSnowFlakes (CControl *ctrl);
	void UpdateAreas (CControl *ctrl);
public:
	void Init (int grade, CControl *ctrl);
	void Reset ();
	void Update (double timestep, CControl *ctrl);
	void Draw (CControl *ctrl) const;
};

// --------------------------------------------------------------------
//					snow clouds for medium distances
// --------------------------------------------------------------------
// intended for future versions

// --------------------------------------------------------------------
//					snow curtains for greater distances
// --------------------------------------------------------------------

#define MAX_CURTAIN_COLS 16
#define MAX_CURTAIN_ROWS 8

struct TCurtainElement {
    TVector3 pt;
	float angle;
	float height;
	float zrandom;
};

struct TCurtain {
	TCurtainElement curtains[MAX_CURTAIN_COLS][MAX_CURTAIN_ROWS];
	int chg[MAX_CURTAIN_ROWS];	// for each row

	unsigned int numCols;
	unsigned int numRows;
	float zdist;
	float size;
	float speed;
	float angledist;
	float startangle;
	float lastangle;
	float minheight;
	int texture;
	
	TCurtain(
		int num_rows,
		float z_dist,
		float tex_size,
		float base_speed,
		float start_angle,
		float min_height,
		int curt_texture);
	void SetStartParams(CControl* ctrl);
	void Draw() const;
	void Update(float timestep, const TVector3& drift, CControl* ctrl);

private:
	static void CurtainVec (float angle, float zdist, float &x, float &z);
};

class CCurtain {
private:
	vector<TCurtain> curtains;

	void SetStartParams (CControl *ctrl);
public:
	void Init (CControl *ctrl);
	void Update (float timestep, CControl *ctrl);
	void Draw (CControl *ctrl);
	void Reset ();
};

// --------------------------------------------------------------------
//					wind
// --------------------------------------------------------------------

struct TWindParams {
	float minSpeed;
	float maxSpeed;
	float minChange;
	float maxChange;

	float minAngle;
	float maxAngle;
	float minAngleChange;
	float maxAngleChange;

	float topSpeed;
	float topProbability;
	float nullProbability;
};

class CWind {
private:
	bool windy;
	float CurrTime;
	TWindParams params;
	// if dest > curr state the modes are 1, otherwise 0
	int SpeedMode;
	int AngleMode;

	float WSpeed;
	float WAngle;
	TVector3 WVector;

	float DestSpeed;
	float DestAngle;
	float WindChange;
	float AngleChange;

	void SetParams (int grade);
	void CalcDestSpeed ();
	void CalcDestAngle ();
public:
	CWind ();

	void Update (float timestep);
	void Init (int wind_id);
	bool Windy () const { return windy; }
	float Angle () const { return WAngle; }
	float Speed () const { return WSpeed; }
	const TVector3& WindDrift () const { return WVector; }
};

extern CWind Wind;

// --------------------------------------------------------------------
// 			Acess functions
// --------------------------------------------------------------------

void InitSnow (CControl *ctrl);
void UpdateSnow (double timestep, CControl *ctrl);
void DrawSnow (CControl *ctrl);
void InitWind ();
void UpdateWind (double timestep, CControl *ctrl);

#endif