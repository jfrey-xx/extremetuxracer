// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "info.h"
#include "textures.h"

// --------------------------------------------------------------------
//					course
// --------------------------------------------------------------------

CCourseInfo CourseInfo; 

CCourseInfo::CCourseInfo () { Init(); }
CCourseInfo::~CCourseInfo () { Reset(); }

void CCourseInfo::Init () {
	for (int i=0; i<MAX_COURSE_INFO; i++) {
		info[i].name = "";
		info[i].desc = "";
		info[i].texid = 0;
	}
	numInfo = 0;
}

void CCourseInfo::Reset () {
	for (int i=0; i<MAX_COURSE_INFO; i++) {
		info[i].name = "";
		info[i].dir = "";
		info[i].desc = "";
		if (info[i].texid >= 0) {
			glDeleteTextures (1, &info[i].texid);
			info[i].texid = 0;
		}
	}
	numInfo = 0;
}

void CCourseInfo::Load () {
	int i;
	string line;
	string dir;
	string path;
	TVector3 vec;

	CSPList list(128);

	if (!list.Load (cfg.common_coursedir, "courses.lst")) {
		Message ("could not load courses.lst");
		WinExit (0);
	}
	if (list.Count () < 1) {
		Message ("the course list is empty");
		WinExit (0);
	}

	for (i=0; i<list.Count(); i++) {
		line = list.Line(i);	
		dir = SPStrN (line, "dir", "none");
		path = MakePathStr (cfg.common_coursedir, dir);
		if (DirExists (path.c_str())) {
			info[numInfo].dir = dir;
			info[numInfo].texid = LoadMenuTexture (path.c_str(), "preview.png");
			info[numInfo].name = SPStrN (line, "name", "unknown");
			info[numInfo].desc = SPStrN (line, "desc", "no description");
			info[numInfo].is_public = SPIntN (line, "public", 1);
			vec = SPVector3N (line, "herring", MakeVec (0,0,0));
			info[numInfo].herring[0] = (int) vec.x;
			info[numInfo].herring[1] = (int) vec.y;
			info[numInfo].herring[2] = (int) vec.z;
			vec = SPVector3N (line, "time", MakeVec (120,120,120));
			info[numInfo].time[0] = vec.x;
			info[numInfo].time[1] = vec.y;
			info[numInfo].time[2] = vec.z;
			numInfo++;
		}
	}	
}

string CCourseInfo::GetName (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].name;
}

string CCourseInfo::GetDir (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].dir;
}

string CCourseInfo::GetDesc (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].desc;
}

const char *CCourseInfo::GetDescC (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].desc.c_str();
}

bool CCourseInfo::IsPublic (int idx) {
	if (idx < 0 || idx >= numInfo) return true;
	return info[idx].is_public;
}

GLuint CCourseInfo::GetTexid (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].texid;
}

int CCourseInfo::Count () { return numInfo; }

int CCourseInfo::GetHerring (int idx, int level) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].herring[level];
}

float CCourseInfo::GetTime (int idx, int level) {
	if (idx < 0 || idx >= numInfo) return 0.0;
	return info[idx].time[level];
}

int CCourseInfo::GetCourseIdx (const char *dirname) {
	int res = -1;
	for (int i=0; i<numInfo; i++) {
		if (strcmp (dirname, info[i].dir.c_str()) == 0) {
			res = i; break;
		}		
	}
	return res;
}

// --------------------------------------------------------------------
//					campaign
// --------------------------------------------------------------------

CCampaignInfo CampaignInfo; 

CCampaignInfo::CCampaignInfo () { Init(); }
CCampaignInfo::~CCampaignInfo () { Reset(); }

void CCampaignInfo::Init () {
	for (int i=0; i<MAX_CAMPAIGN_INFO; i++) {
		info[i].name = "";
		info[i].desc = "";
		info[i].texid = 0;
		info[i].bonus = 0;
	}
	numInfo = 0;
}

void CCampaignInfo::Reset () {
	for (int i=0; i<MAX_CAMPAIGN_INFO; i++) {
		info[i].name = "";
		info[i].dir = "";
		info[i].desc = "";
		if (info[i].texid >= 0) {
			glDeleteTextures (1, &info[i].texid);
			info[i].texid = 0;
		}
	}
	numInfo = 0;
}

bool CCampaignInfo::LoadRaces (TCampaignInfo *cinfo, string path) {
	CSPList list(MAX_CAMPAIGN_RACES);
	string line;
	int i, cidx, id;
	string cdir;
	string cloc;
	string clight;

	if (!list.Load (path, "races.lst")) {
		Message ("could not load races.lst");
		return false;
	}

	cinfo->numRaces = 0;	
	for (i=0; i<list.Count(); i++) {
		line = list.Line(i);	
		cdir = SPStrN (line, "race", "unknown");
		cidx = CourseInfo.GetCourseIdx (cdir.c_str());

		if (cidx >= 0) {
			cinfo->races[cinfo->numRaces].course_id = cidx;
			cloc = SPStrN (line, "location", "");
			cinfo->races[cinfo->numRaces].location_id = EnvInfo.GetEnvID (cloc);
			clight = SPStrN (line, "light", "");
			cinfo->races[cinfo->numRaces].light_id = EnvInfo.GetLightID (cloc, clight);
	
			id = SPIntN (line, "snow", 0);
			if (id < 0 || id > 3) id = 0;
			cinfo->races[cinfo->numRaces].snow_id = id;
			id = SPIntN (line, "wind", 0);
			if (id < 0 || id > 3) id = 0;
			cinfo->races[cinfo->numRaces].wind_id = id;
			cinfo->numRaces++;
		}
	}
	return (cinfo->numRaces > 0);	
}

void CCampaignInfo::Load () {
	int i;
	string line;
	string dir;
	string path;
	CSPList list(128);

	if (!list.Load (cfg.common_campaigndir, "campaigns.lst")) {
		Message ("could not load campaigns.lst");
		return;
	}
	if (list.Count () < 1) {
		Message ("the Campaign list is empty");
		return;
	}

	numInfo = 0;
	for (i=0; i<list.Count(); i++) {
		line = list.Line(i);	
		dir = SPStrN (line, "dir", "none");
		path = MakePathStr (cfg.common_campaigndir, dir);	
		if (FileExists (path.c_str())) { 
			info[numInfo].dir = dir;
			info[numInfo].texid = LoadMenuTexture (path.c_str(), "preview.png");
			info[numInfo].name = SPStrN (line, "name", "unknown");
			info[numInfo].desc = SPStrN (line, "desc", "no description");
			info[numInfo].bonus = SPIntN (line, "bonus", 4);
			info[numInfo].racemode = SPIntN (line, "mode", 3);
			if (LoadRaces (&info[numInfo], path)) {
				numInfo++;
			}
		}
	}	
}

GLuint CCampaignInfo::GetTexid (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].texid;
}

string CCampaignInfo::GetName (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].name;
}

string CCampaignInfo::GetDir (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].dir;
}

string CCampaignInfo::GetDesc (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].desc;
}

const char *CCampaignInfo::GetDescC (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].desc.c_str();
}

int CCampaignInfo::GetBonus (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].bonus;
}

int CCampaignInfo::GetRaceMode (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].racemode;
}

int CCampaignInfo::Count () { return numInfo; }

int CCampaignInfo::GetCourseID (int idx, int crace) {
	if (idx < 0 || idx >= numInfo) return -1;
	if (crace < 0 || crace >= info[idx].numRaces) return 0;
	return info[idx].races[crace].course_id;
}

int CCampaignInfo::GetLocationID (int idx, int crace) {
	if (idx < 0 || idx >= numInfo) return -1;
	if (crace < 0 || crace >= info[idx].numRaces) return 0;
	return info[idx].races[crace].location_id;
}

int CCampaignInfo::GetLightID (int idx, int crace) {
	if (idx < 0 || idx >= numInfo) return -1;
	if (crace < 0 || crace >= info[idx].numRaces) return 0;
	return info[idx].races[crace].light_id;
}

int CCampaignInfo::GetSnowID (int idx, int crace) {
	if (idx < 0 || idx >= numInfo) return -1;
	if (crace < 0 || crace >= info[idx].numRaces) return 0;
	return info[idx].races[crace].snow_id;
}

int CCampaignInfo::GetWindID (int idx, int crace) {
	if (idx < 0 || idx >= numInfo) return -1;
	if (crace < 0 || crace >= info[idx].numRaces) return 0;
	return info[idx].races[crace].wind_id;
}

int CCampaignInfo::GetNumRaces (int idx) {
	if (idx < 0 || idx >= numInfo) return 0;
	return info[idx].numRaces;
}
// --------------------------------------------------------------------
//					environment
// --------------------------------------------------------------------

CEnvInfo EnvInfo;

CEnvInfo::CEnvInfo () {Init ();}

CEnvInfo::~CEnvInfo () {Reset ();}

void CEnvInfo::Init () {
	for (int i=0; i<MAX_ENV_INFO; i++) {
		info[i].location = "";
		for (int j=0; j<4; j++) info[i].lights[j] = "";
	}
	numInfo = 0;
	lightindex = "[sunny]0[cloudy]1[foggy]4[sunset]2[evening]5[night]3";
	envindex[0] = 0;
}

void CEnvInfo::Reset () {
	for (int i=0; i<MAX_ENV_INFO; i++) {
		info[i].location = "";
		for (int j=0; j<8; j++) {
			info[i].lights[j] = "";
		}
	}
}

void CEnvInfo::AppendLight (int envidx, string &line, string tag, int lightidx) {
	string lightstr;
	lightstr = SPStrN (line, tag, "");
	
	if (lightstr.size() > 0) {
		TEnvInfo *inf = &info[envidx];
		inf->lights[inf->numLights] = lightstr;
		inf->icons[inf->numLights] = lightidx;
		inf->numLights++;
	}
}

void CEnvInfo::Load () {
	string line;
	CSPList list(40);
	string path;
	string val;

	if (!list.Load (cfg.env_dir, "environment.lst")) {
		Message ("could not load environment.lst");
		WinExit (0);
	}

	numInfo = 0;
	for (int i=0; i<list.Count(); i++) {
		line = list.Line(i);	
		info[numInfo].location = SPNewCharN (line, "location");
		
		info[numInfo].numLights = 0;
		AppendLight (numInfo, line, "sunny",   0);		
		AppendLight (numInfo, line, "cloudy",  1);		
		AppendLight (numInfo, line, "foggy",   4);		
		AppendLight (numInfo, line, "sunset",  2);		
		AppendLight (numInfo, line, "evening", 5);		
		AppendLight (numInfo, line, "night",   3);		

		path = cfg.env_dir;
		path += SEP;
		path += info[numInfo].location;

		info[numInfo].texid = LoadMenuTexture (path.c_str(), "preview.png");
		envindex += "[";
		envindex += info[numInfo].location;
		envindex += "]";
		envindex += Int_StrN (numInfo);
		numInfo++;
	}	
}

string CEnvInfo::GetDir (int location, int light) {
	string res = "";
	if (location < 0 || location >= numInfo) return "";
	if (light < 0 || light >= info[location].numLights) return "";
	res += cfg.env_dir;
	res += SEP;
	res += info[location].location;
	res += SEP;
	res += info[location].lights[light];
	return res;
}

int *CEnvInfo::GetIcons (int location) {
	if (location < 0 || location >= numInfo) return 0;
	return info[location].icons;
}

int CEnvInfo::GetNumLights (int location) {
	if (location < 0 || location >= numInfo) return 0;
	return info[location].numLights;
}

GLuint CEnvInfo::GetTexid (int location) {
	if (location < 0 || location >= numInfo) return 0;
	return info[location].texid;
}

int CEnvInfo::GetEnvID (string loc) {
	return SPIntN (envindex, loc, 0);
}

int CEnvInfo::GetLightID (string loc, string light) {
 	int envid = GetEnvID (loc);
	int lightid = SPIntN (lightindex, light, 0);
	
	int res = 0;
	for (int i=0; i<GetNumLights (envid); i++) {
		if (lightid == info[envid].icons[i]) {
			res = i; break;
		}
	}
	return res;
}
