// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

// this unit contains all about snow and wind, it is still work in progress

#ifndef PARTICLES_H
#define PARTICLES_H

#include "bh.h"

// --------------------------------------------------------------------
//					brake particles
// --------------------------------------------------------------------

#define PARTICLES_PER_GROUP 8
#define NUM_PARTICLE_GROUPS 20

typedef struct {
	TVector3 pos;
	TVector3 vel;
	float size;
	int type; // part of the texture
	bool visible;
} TParticle; 

typedef struct {
	TVector2 lb;
	TVector2 tr;
} TPartCoord;

class CParticles {
private:
	TPartCoord TexCoords[4];	
 	TParticle BrakeParticles[NUM_PARTICLE_GROUPS * PARTICLES_PER_GROUP * 2 + 1];
	int PartRegister[NUM_PARTICLE_GROUPS];
	int part_ptr;
	int numGroupParticles;
	int numHalfgroupParticles;
	int numGroups;

	void DrawBillboard (TVector3 pos, float size, TPartCoord tex, 
		bool world_axis, TControl *ctrl);
	void DrawParticle (TVector3 pos, float size, TPartCoord tex);	
	void NewBrakeParticles (float speed, TVector3 pos, TVector3 vel, 
		int num, bool left, TControl *ctrl);
public:
	CParticles ();
	~CParticles ();
	void Init ();
	void SetColor (TColor col);
	void UpdateBrakeParticles (float timestep, TControl *ctrl);
	void DrawBrakeParticles (TControl *ctrl);
	void GenerateBrakeParticles (float timestep, TControl *ctrl);
};

extern CParticles Particles;

// --------------------------------------------------------------------
//					snow particles
// --------------------------------------------------------------------

#define MAX_SNOW_PARTICLES 2000

typedef struct {
	int numParticles[3];
	float xrange[3];
	float ytop[3];
	float yrange[3];
	float zback[3];
	float zrange[3];
	float minSize[3];
	float maxSize[3];
	float speed[3];
	float ydrift[3];
	float zdrift[3];
} TSnowParams;

typedef struct {
    TVector3 pt;
    float   size;
    TVector3 vel;
    TVector2 tex_min;
    TVector2 tex_max;
} TSnowParticle;

typedef struct {
	float left;
	float right;
	float bottom;
	float top;
	float front;
	float back;
} TSnowArea;

class CSnow {
private:
	bool snowing;
	TVector3 snow_lastpos;
	TSnowParams params;
	TSnowParticle PartArr[3][MAX_SNOW_PARTICLES];
	TSnowArea area[3];

 	void UpdateAreas (TControl *ctrl);
	void GenerateSnow (TControl *ctrl);
	void MakeSnowParticle (int ar, int i);
	void WriteParams (float *bl, float near, float middle, float far);
	void SetParams (int grade);
	void Draw (int ar, bool rotate, TControl *ctrl);
public:
	CSnow ();
	~CSnow ();
	void Init (int snow_id, TControl *ctrl);
	void Update (float timestep, TControl *ctrl);
};

extern CSnow Snow;

// --------------------------------------------------------------------
//					snow curtain
// --------------------------------------------------------------------
// snow curtains are used to get more flakes without slowing down the
// game. They must be displayed in the far, in the near area we need
// to draw single flakes which need much performance. For the medium area
// another kind of snow is planned. 

#define MAX_CURTAIN_COLS 16
#define MAX_CURTAIN_ROWS 8

typedef struct {
    TVector3 pt;
	float angle;
	float height;
	float zrandom;
} TCurtain;

class CCurtain {
private:
	TCurtain curtains[3][MAX_CURTAIN_COLS][MAX_CURTAIN_ROWS];
	void Draw (TControl *ctrl);
	int numCols[3];
	int numRows[3];
	float zdist[3];
	float size[3];
	float speed[3];
	float horzspeed[3];
	float angledist[3];
	float startangle[3];
	float lastangle[3];
	float minheight[3];
	bool enabled[3];
	int chg[3][MAX_CURTAIN_ROWS];

	void GenerateCurtains (TControl *ctrl);
	void CurtainVec (float angle, float zdist, float &x, float &z);
public:
	CCurtain ();
	~CCurtain ();
	void Update (float timestep, TControl *ctrl);
	void Init (TControl *ctrl, bool near, bool middle, bool far);
};

extern CCurtain Curtain;

// --------------------------------------------------------------------
//					wind
// --------------------------------------------------------------------

/* Parameter:

[min_base_speed] 20 
[max_base_speed] 50
	Zwischen diesen beiden Grenzwerten wird per Zufall speed (lokal) berechnet.
	Das ist die Anfangsgeschwindigkeit.
[min_speed_var] 30 
[max_speed_var] 30
	Zwischen den beiden Grenzwerten wird per Zufall var (lokal) berechnet
	Aus var und speed ergibt sich minSpeed und maxSpeed (params). Damit schwankt
	die Geschwindigkeit um speed - var bis speed + var. Das sind die Grenzen, in
	denen sich speed bewegt.

[min_base_angle] 70 
[max_base_angle] 170 
	Zwischen diesen Grenzwerten wird per Zufall angle (lokal) berechnet
[alt_angle] 180
	in 50% der Fälle wird alt_angle zu ange addiert
[min_angle_var] 10 
[max_angle_var] 90
	Zwischen den beiden Grenzwerten wird per Zufall var (lokal) berechnet
	Aus var ergibt sich minAngle und maxAngle. Das Prinzip ist ähnlich wie
	bei der Geschwindigkeit.

// die folgenden Parameter werden direkt übernommen
[min_speed_change] 0.1 
[max_speed_change] 0.5
	Change gibt die Größe der Schwankungen in einem Berechnungszeitraum an. 
	Der Wert wird ebenfalls zufällig zwischen den beiden Grenzwerten 
	berechnet und gilt dann für das komplette Rennen.
[min_angle_change] 0.1 
[max_angle_change] 1
	Dasselbe für den Winkel.
[top_speed] 80 
[top_probability] 10 
[null_probability] 10 
*/

typedef struct {
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
} TWindParams;


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
	~CWind ();

	void Update (float timestep);
	void Init (int wind_id);
	bool Windy () { return windy; }
	float Angle () { return WAngle; }
	float Speed () { return WSpeed; }
	TVector3 WindDrift () { return WVector; }
};

extern CWind Wind;

#endif
