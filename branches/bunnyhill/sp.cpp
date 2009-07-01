// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "sp.h"
#include <sstream>
#include <iomanip>

static char cbuffer[4096];
static string EnumStr;

void SetEnum (string s) { EnumStr = s;}


// --------------------------------------------------------------------
//					elementary string functions
// --------------------------------------------------------------------

string MakePathStr (string src, string add) {
	string res = src;
	res += SEP;
	res += add;
	return res;
}

void SInsertN (string &s, unsigned int pos, const string ins) {
	if (pos > s.size()) pos = s.size();
	s.insert (pos, ins);
}

void SDeleteN (string &s, unsigned int pos, int count) {
	if (pos > s.size()) pos = s.size();
	s.erase (pos, count);
}

int SPosN (string &s, const string find) {
	return s.find (find);
}

void STrimLeftN (string &s) {
	int i = 0;
	while (s[i] == ' ' || s[i] == '\t') i++;
	if (i > 0) SDeleteN (s, 0, i);
}

void STrimRightN (string &s) {
	int i = s.size() -1;
	while ((s[i] == ' ' || s[i] == '\t') && i >= 0) i--;
	s.erase (i+1);	
}

void STrimN (string &s) {
	STrimLeftN (s);
	STrimRightN (s);
}

// --------------------------------------------------------------------
//				conversion functions
// --------------------------------------------------------------------

void Int_StrN (string &s, const int val) {
	ostringstream os;
	os << val;
	s = os.str();
}

string Int_StrN (const int val) {
	ostringstream os;
	os << val;
	return os.str();
}

string Int_StrN (const int val, const int count) {
	ostringstream os;
	os << val;
	string s = os.str();
	while (s.size() < (unsigned int)count) SInsertN (s, 0, "0");
	return s;
}

void Float_StrN (string &s, const float val, const int count) {
	ostringstream os;
	os << setprecision(count) << fixed << val;
	s = os.str();
}

string Float_StrN (const float val, const int count) {
	ostringstream os;
	os << setprecision(count) << fixed << val;
	return os.str();
}

int Str_IntN (const string &s, const int def) {
	int val;
	istringstream is(s);
	is >> val;
	if (is.fail()) return def; else return val;
}

bool Str_BoolN (const string &s, const bool def) {
	int val;
	istringstream is(s);
	is >> val;
	if (is.fail()) return def;
	return (val != 0);
}

bool Str_BoolNX (const string &s, const bool def) {
	string decode = "[0]0[1]1[true]1[false]0";
	string valstr;
	if (def == true) valstr = SPStrN (decode, s, "1");
	else valstr = SPStrN (decode, s, "0");
	int val;
	istringstream is(valstr);
	is >> val;
	if (is.fail()) return def;
	return (val != 0);
}

float Str_FloatN (const string &s, const float def) {
	float val;
	istringstream is(s);
	is >> val;
	if (is.fail()) return def; else return val;
}

TVector2 Str_Vector2N (const string &s, const TVector2 def) {
	float x, y;
	istringstream is(s);
	is >> x >> y;
	if (is.fail()) return def;
	else return MakeVec2 (x, y);
}

TVector2 Str_Vector2N (const string &s) {
	float x, y;
	istringstream is(s);
	is >> x >> y;
	if (is.fail()) return MakeVec2 (0, 0);
	else return MakeVec2 (x, y);
}

TVector3 Str_Vector3N (const string &s, const TVector3 def) {
	float x, y, z;
	istringstream is(s);
	is >> x >> y >> z;
	if (is.fail()) return def;
	else return MakeVec (x, y, z);
}

TVector3 Str_Vector3N (const string &s) {
	float x, y, z;
	istringstream is(s);
	is >> x >> y >> z;
	if (is.fail()) return MakeVec (0, 0, 0);
	else return MakeVec (x, y, z);
}

TVector4 Str_Vector4N (const string &s, const TVector4 def) {
	float x, y, z, w;
	istringstream is(s);
	is >> x >> y >> z >> w;
	if (is.fail()) return def;
	else return MakeVec4 (x, y, z, w);
}

TColor Str_ColorN (const string &s, const TColor def) {
	float r, g, b, a;
	istringstream is(s);
	is >> r >> g >> b >> a;
	if (is.fail()) return def;
	else return MakeColor (r, g, b, a);
}

void Str_ArrN (const string &s, float *arr, int count, float def) {
	istringstream is(s);
	switch (count) {
		case 1: is >> arr[0]; break;
		case 2: is >> arr[0] >> arr[1]; break;
		case 3: is >> arr[0] >> arr[1] >> arr[2]; break;
		case 4: is >> arr[0] >> arr[1] >> arr[2] >> arr[3]; break;
		default: break;
	}
	if (is.fail()) for (int i=0; i<count; i++) arr[i] = def;
}

string Bool_StrN (const bool val) {
	if (val == true) return "true"; else return "false";
}

// --------------------------------------------------------------------
//				SP functions for parsing lines
// --------------------------------------------------------------------

string SPItemN (string &s, const string &tag) {
	int i = 0;
	unsigned int ii = 0;
	string item = "";
	if (s.size() == 0 || tag.size() == 0) return item;

	string tg = "[" + tag + "]";
	i = SPosN (s, tg);
	if (i < 0) return item;
	ii = i + tg.size();
	while (s[ii] != '[' && s[ii] != '#' && ii < s.size()) {
		item += s[ii];
		ii++;
	}
 	return item;
}

void SPItemN (string &s, const string &tag, string &item) {
	int i = 0;
	unsigned int ii = 0;

	item = "";
	if (s.size() == 0 || tag.size() == 0) return;

	string tg = "[" + tag + "]";
	i = SPosN (s, tg);
	if (i < 0) return;
	ii = i + tg.size();
	while (s[ii] != '[' && s[ii] != '#' && ii < s.size()) {
		item += s[ii];
		ii++;
	}
}

string SPStrN (string &s, const string &tag, const string def) {
	string item = SPItemN (s, tag);
	if (item.size() < 1) return def;
	STrimN (item);
	return item;
}

int SPIntN (string &s, const string &tag, const int def) {
	return (Str_IntN (SPItemN (s, tag), def));
}

int SPEnumN (string &s, const string &tag, int def) {
	string item = SPItemN (s, tag);	
	STrimN (item);
	if (item.size() < 1) return def;
	return SPIntN (EnumStr, item, def);;
}

bool SPBoolN (string &s, const string &tag, const bool def) {
	return (Str_BoolN (SPItemN (s, tag), def));
}

bool SPBoolNX (string &s, const string &tag, const bool def) {
	string item = SPItemN (s, tag);
	STrimN (item);
	return Str_BoolNX (item, def);
}

float SPFloatN (string &s, const string &tag, const float def) {
	return (Str_FloatN (SPItemN (s, tag), def));
}

TVector2 SPVector2N (string &s, const string &tag, const TVector2 def) {
	return (Str_Vector2N (SPItemN (s, tag), def));
}

TVector2 SPVector2N (string &s, const string &tag) {
	return (Str_Vector2N (SPItemN (s, tag)));
}

TVector3 SPVector3N (string &s, const string &tag, const TVector3 def) {
	return (Str_Vector3N (SPItemN (s, tag), def));
}

TVector3 SPVector3N (string &s, const string &tag) {
	return (Str_Vector3N (SPItemN (s, tag)));
}

TVector4 SPVector4N (string &s, const string &tag, const TVector4 def) {
	return (Str_Vector4N (SPItemN (s, tag), def));
}

TColor SPColorN (string &s, const string &tag, const TColor def) {
	return (Str_ColorN (SPItemN (s, tag), def));
}

void SPArrN (string &s, const string &tag, float *arr, int count, float def) {
	Str_ArrN (SPItemN (s, tag), arr, count, def);
}

bool SPExistsN  (string &s, const string &tag) {
	string tg = "[" + tag + "]";
	int i = SPosN (s, tg);
	if (i < 0) return false;
	return true;
}

int SPPosN (string &s, const string &tag) {
	string tg = "[" + tag + "]";
	return SPosN (s, tg);
}

// --------------------------------------------------------------------
// 			compatibility
// --------------------------------------------------------------------

void SInsertO (char *s, int pos, const char *str) {
	char buff[5012];
	int lng = strlen (s);
	if (pos > lng) pos = lng;
	if (pos == lng) {
		strcat (s, str);	
	} else {
		memmove (buff, s+pos, lng-pos+1);
		s[pos] = '\0';
		strcat (s, str);
		strcat (s, buff);
	}
}

void Int_CharN (char *s, const int val) {
	sprintf (s, "%i", val);
}

void Int_CharN (char *s, const int val, const int cnt) {
	sprintf (s, "%i", val);
	while (strlen (s) < (unsigned int)cnt) SInsertO (s, 0, "0");
}

char *NewStrN (const char *s) {
    char *dest;
    dest = (char *) malloc (sizeof(char) * (strlen(s) + 1));
    if (dest == 0) Message ("malloc failed");
    strcpy (dest, s);
    return dest;
}

void SPCharN (string &s, const string &tag, char *result) {
	string item = SPItemN (s, tag);
	if (item.size() < 1) return;
	STrimN (item);
	strcpy (result, item.c_str());
}

char *SPNewCharN (string &s, const string &tag) {
	string item = SPItemN (s, tag);
	if (item.size() < 1) return 0;
	STrimN (item);
	return NewStrN (item.c_str());
}

// ------------------ add ---------------------------------------------

void SPAddIntN (string &s, const string &tag, const int val) {
	s += "[";
	s += tag;
	s += "]";
	s += Int_StrN (val);
}

void SPAddFloatN (string &s, const string &tag, const float val, int count) {
	s += "[";
	s += tag;
	s += "]";
	s += Float_StrN (val, count);
}

void SPAddStrN (string &s, const string &tag, const string &val) {
	s += "[";
	s += tag;
	s += "]";
	s += val;
}

void SPAddVec3N (string &s, const string &tag, const TVector3 val, int count) {
	s += "[";
	s += tag;
	s += "]";
	s += Float_StrN (val.x, count);
	s += " ";
	s += Float_StrN (val.y, count);
	s += " ";
	s += Float_StrN (val.z, count);
}

void SPAddIndx3N  (string &s, const string &tag, const TIndex3 val) {
	s += "[";
	s += tag;
	s += "]";
	s += Int_StrN (val.i);
	s += ' ';
	s += Int_StrN (val.j);
	s += ' ';
	s += Int_StrN (val.k);
}

void SPAddIndx4N  (string &s, const string &tag, const TIndex4 val) {
	s += '[';
	s += tag;
	s += ']';
	s += Int_StrN (val.i);
	s += ' ';
	s += Int_StrN (val.j);
	s += ' ';
	s += Int_StrN (val.k);
	s += ' ';
	s += Int_StrN (val.l);
}

void SPAddBoolN (string &s, const string &tag, const bool val) {
	s += '[';
	s += tag;
	s += ']';
	if (val == true) s += "true"; else s+= "false";
}

// --------------------------------------------------------------------

void SPSetIntN (string &s, const string &tag, const int val) {
	int pos = SPPosN (s, tag);
	if (pos >= 0) {
		int ipos = pos + tag.size() + 2;
		string item = SPItemN (s, tag);
		if (item.size() > 0) SDeleteN (s, ipos, item.size()); 
		SInsertN (s, ipos, Int_StrN (val));
	} else SPAddIntN (s, tag, val);
}

void SPSetFloatN (string &s, const string &tag, const float val, int count) {
	int pos = SPPosN (s, tag);
	if (pos >= 0) {
		int ipos = pos + tag.size() + 2;
		string item = SPItemN (s, tag);
		if (item.size() > 0) SDeleteN (s, ipos, item.size()); 
		SInsertN (s, ipos, Float_StrN (val, count));
	} else SPAddFloatN (s, tag, val, count);
}

void SPSetStrN (string &s, const string &tag, const string &val) {
	int pos = SPPosN (s, tag);
	if (pos >= 0) {
		int ipos = pos + tag.size() + 2;
		string item = SPItemN (s, tag);
		if (item.size() > 0) SDeleteN (s, ipos, item.size()); 
		SInsertN (s, ipos, val);
	} else SPAddStrN (s, tag, val);
}

// --------------------------------------------------------------------
//					class CSPList
// --------------------------------------------------------------------


CSPList::CSPList (int maxlines) {
	fmax = maxlines;
	flines = new string [maxlines];
	fflag = new int [maxlines];
	fcount = 0;
}

CSPList::~CSPList () {
	if (flines != 0) delete []flines;
	delete[]fflag;
}

string CSPList::Line (int idx) {
	if (idx < 0 || idx >= fcount) return "";
	return flines[idx];
}

const char *CSPList::LineC (int idx) {
	if (idx < 0 || idx >= fcount) return "";
	return flines[idx].c_str();
}

int CSPList::Count () { return fcount; }

void CSPList::Clear () { 
	fcount = 0; 
}

void CSPList::Add (string line) {
	if (fcount < fmax) {
		flines[fcount] = line; 	
		fcount++;
	}
}

void CSPList::Add (string line, int flag) {
	if (fcount < fmax) {
		flines[fcount] = line; 	
		fflag[fcount] = flag;
		fcount++;
	}
}

void CSPList::Append (string line, int idx) {
	if (idx < 0 || idx >= fcount) return;
	flines[idx] += line;
}

void CSPList::SetFlag (int idx, int flag) {
	if (idx < 0 || idx >= fcount) return;
	fflag[idx] = flag;
}

int CSPList::Flag (int idx) {
	if (idx < 0 || idx >= fcount) return 0;
	return fflag[idx];
}

void CSPList::Print () {
	for (int i=0; i<fcount; i++) cout << flines[i] << endl;
}

bool CSPList::Load (const string &filepath) {
	FILE *tempfile;
	string line;
	bool valid;

	tempfile = fopen (filepath.c_str(), "r");
	if (tempfile == 0) {
		Message ("CSPList::Load - unable to open");
		return false;
	} else {
	    while (fgets (cbuffer, 4000, tempfile)) {
			line = cbuffer;
			STrimN (line);

			// delete new line char if in string
			int npos = line.rfind ('\n');
			if (npos >= 0) SDeleteN (line, npos, 1);

			valid = true;
			if (line.size() < 1) valid = false;	// empty line
			if (line[0] == '#') valid = false;	// comment line
			
			if (valid) {
				if (fcount < fmax) {
					if (cbuffer[0] == '*' || fcount < 1) {
						Add (line);
					} else {
						Append (line, fcount-1);
					}
				} else {
					Message ("CSPList::Load - not enough lines");
					return false;
				}
			}
    	}
		fclose (tempfile); 
		return true;
	}
}

bool CSPList::Load (string dir, string filename) {
	return Load (dir + SEP + filename);
}

bool CSPList::Save (const string &filepath) {
	FILE *tempfile;
	string line;

	tempfile = fopen (filepath.c_str(), "w");
	if (tempfile == 0) {
		Message ("CSPList::Save - unable to open");
		return false;
	} else {
		for (int i=0; i<fcount; i++) {
			line = flines[i] + '\n';
			fputs (line.c_str(), tempfile);
		}
		fclose (tempfile);
		return true;
	}
}

bool CSPList::Save (string dir, string filename) {
	return Save (dir + SEP + filename);
}

void CSPList::MakeIndex (string &index, const string &tag) {
	index = "";
	string item;

	for (int i=0; i<fcount; i++) {
		item = SPItemN (flines[i], tag);
		STrimN (item);
		if (item.size() > 0) {
			index += "[";
			index += item;			
			index += "]";
			index += Int_StrN (i);
		}
	}
}


