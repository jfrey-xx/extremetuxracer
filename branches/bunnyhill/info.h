// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#ifndef INFO_H
#define INFO_H

#include "bh.h"

// the race and environment information is read after programstart. 
// All information is stored in different info classes 

// --------------------------------------------------------------------
//				course info
// --------------------------------------------------------------------

#define MAX_COURSE_INFO 128

typedef struct {
	string name;
	string desc;
	string dir;
	GLuint texid;
	int herring[3];
	float time[3];
	bool is_public;
} TCourseInfo;

class CCourseInfo {
private:
	TCourseInfo info[MAX_COURSE_INFO];	
	int numInfo;
public:
	CCourseInfo ();
	~CCourseInfo ();
	void Init ();
	void Reset ();

	void   Load ();
	string GetName (int idx);
	string GetDir (int idx);
	string GetDesc (int idx);
	const char *GetDescC (int idx);
	GLuint GetTexid (int idx);
	bool   IsPublic (int idx);
	int    GetHerring (int idx, int level);
	float  GetTime (int idx, int level);
	int    Count ();
	int    GetCourseIdx (const char *dirname);
};

// --------------------------------------------------------------------
//				campaign info
// --------------------------------------------------------------------

#define MAX_CAMPAIGN_INFO 16
#define MAX_CAMPAIGN_RACES 8

typedef struct {
	int course_id;
	int location_id;
	int light_id;
	int snow_id;
	int wind_id;
} TCampaignRace;

typedef struct {
	string name;
	string desc;
	string dir;
	GLuint texid;
	int bonus;
	int racemode;
	TCampaignRace races[MAX_CAMPAIGN_RACES];
	int numRaces;
} TCampaignInfo;

class CCampaignInfo {
private:
	TCampaignInfo info[MAX_CAMPAIGN_INFO];	
	int numInfo;

	bool LoadRaces (TCampaignInfo *cinfo, string path);
public:
	CCampaignInfo ();
	~CCampaignInfo ();
	void Init ();
	void Reset ();

	void   Load ();
	string GetName (int idx);
	string GetDir (int idx);
	string GetDesc (int idx);
	const char *GetDescC (int idx);
	GLuint GetTexid (int idx);
	int    GetBonus (int idx);
	int    GetRaceMode (int idx);
	int    Count ();

	int GetNumRaces (int idx);
	int GetCourseID (int idx, int crace);
	int GetLocationID (int idx, int crace);
	int GetLightID (int idx, int crace);
	int GetSnowID (int idx, int crace);
	int GetWindID (int idx, int crace);
};

// --------------------------------------------------------------------
//				environment info
// --------------------------------------------------------------------

#define MAX_ENV_INFO 32

typedef struct {
	string location;
	string lights[8];
	int  icons[8];		// contains the light IDs
	int  numLights;
	GLuint texid;
} TEnvInfo;

class CEnvInfo {
private:
	TEnvInfo info[MAX_ENV_INFO];
	int numInfo;
	void AppendLight (int envidx, string &line, string tag, int lightidx);
	string envindex;
	string lightindex;
public:
	CEnvInfo ();
	~CEnvInfo ();
	void Init ();
	void Reset ();

	void Load ();
	string GetDir (int location, int light);
	int *GetIcons (int location);
	int  GetNumLights (int location);
	int  GetNumInfo () { return numInfo; };
	GLuint GetTexid (int location);
	int GetEnvID (string loc);
	int GetLightID (string loc, string light);
};

extern CCourseInfo CourseInfo;
extern CCampaignInfo CampaignInfo;
extern CEnvInfo EnvInfo;

#endif



