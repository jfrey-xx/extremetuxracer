// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#ifndef SP_H
#define SP_H

#include "bh.h"
using namespace std;

/*
This new SP library uses the string class instead of C-strings. Like
the old library SPX it contains some additional functions for
string manipulation. 

The data types are declared in bh.h
Some helpful functions for printing out the results (in a terminal)
are defined in common.cpp

Most functions end with "N" to distinguish them from the old functions.
For compatibiliy there are some old functions left.

A lot of functions require a def parameter. That's a default value which
is returned is something fails in the function. The tag param is the field 
identifier (e.g. [name]) but without the brackets []. If the tag can't 
be found in the string the function will return the def value.

Count is used to format a string. If the param is an integer value, count
defines the length of the returned string. Example: with count = 4 the 
integer value 23 is returned as "0023". If the result is an float value,
count defines the fixed amount of numbers behind the point. 
*/

// ----- elementary string functions ----------------------------------
string   MakePathStr  (string src, string add);
void     SInsertN     (string &s, unsigned int pos, const string ins);
void     SDeleteN     (string &s, unsigned int pos, int count);
int      SPosN        (string &s, const string find);
void     STrimLeftN   (string &s);
void     STrimRightN  (string &s);
void     STrimN       (string &s);

// ----- conversion functions -----------------------------------------
void     Int_StrN     (string &s, const int val);
string   Int_StrN     (const int val);
string   Int_StrN     (const int val, const int count);
void     Float_StrN   (string &s, const float val, const int count);
string   Float_StrN   (const float val, const int count);
int      Str_IntN     (const string &s, const int def);
bool     Str_BoolN    (const string &s, const bool def);
bool     Str_BoolNX   (const string &s, const bool def);
float    Str_FloatN   (const string &s, const float def);
TVector2 Str_Vector2N (const string &s, const TVector2 def);
TVector2 Str_Vector2N (const string &s);
TVector3 Str_Vector3N (const string &s, const TVector3 def);
TVector3 Str_Vector3N (const string &s);
TVector4 Str_Vector4N (const string &s, const TVector4 def);
TColor   Str_ColorN   (const string &s, const TColor def);
void     Str_ArrN     (const string &s, float *arr, int count, float def);
string   Bool_StrN    (const bool val);

// ----- SP functions for parsing lines --------------------------------
bool     SPExistsN    (string &s, const string &tag);
int      SPPosN       (string &s, const string &tag);

string   SPItemN      (string &s, const string &tag);
void     SPItemN      (string &s, const string &tag, string &item);
string   SPStrN       (string &s, const string &tag, const string def);
int      SPIntN       (string &s, const string &tag, const int def);
bool     SPBoolN      (string &s, const string &tag, const bool def);
bool     SPBoolNX     (string &s, const string &tag, const bool def);
float    SPFloatN     (string &s, const string &tag, const float def);
TVector2 SPVector2N   (string &s, const string &tag, const TVector2 def);
TVector3 SPVector3N   (string &s, const string &tag, const TVector3 def);
TVector2 SPVector2N   (string &s, const string &tag);
TVector3 SPVector3N   (string &s, const string &tag);
TVector4 SPVector4N   (string &s, const string &tag, const TVector4 def);
TColor   SPColorN     (string &s, const string &tag, const TColor def);
void     SPArrN       (string &s, const string &tag, float *arr, int count, float def);
int      SPEnumN      (string &s, const string &tag, int def);

// ----- compatibility ------------------------------------------------
void     Int_CharN    (char *s, const int val);
void     Int_CharN    (char *s, const int val, const int cnt);
char    *NewStrN      (const char *s);
void     SPCharN      (string &s, const string &tag, char *result);
char    *SPNewCharN   (string &s, const string &tag);

// ----- making SP strings --------------------------------------------
void     SPAddIntN    (string &s, const string &tag, const int val);
void     SPAddFloatN  (string &s, const string &tag, const float val, int count);
void     SPAddStrN    (string &s, const string &tag, const string &val);
void     SPAddVec3N   (string &s, const string &tag, const TVector3 val, int count);
void     SPAddIndx3N  (string &s, const string &tag, const TIndex3 val);
void     SPAddIndx4N  (string &s, const string &tag, const TIndex4 val);
void	 SPAddBoolN   (string &s, const string &tag, const bool val);

// ----- manipulating SP strings --------------------------------------
void     SPSetIntN    (string &s, const string &tag, const int val);
void     SPSetFloatN  (string &s, const string &tag, const float val, int count);
void     SPSetStrN    (string &s, const string &tag, const string &val);

// --------------------------------------------------------------------
//		 string list
// --------------------------------------------------------------------

/*
The stringlist class CSPList is easy to use. The constructor gets the number of
lines. Make sure that this value is great enough, otherwise a textfile might not
be read completely. Remember that an element is a pointer (4 byte), so you can 
be generous. Additionally this class is mostly temporary used, within a function,
and the memory is disposed at the end of the function.

A common method of usage is (example):

void LoadTextfile () {
	CSPList list (4000);
	string line;
	
	if (list.Load (filepath) == false) {
		// error handling
	} else {
		for (int i=0; i<list.Count(); i++) {
			line = list.Line (i);
			anyInt = SPIntN (line, "cnt", 0);
(x)			anyFloat = SPFloatN (line, "height", 3.0);
			...
		}
	}
}

The line (x) shows a meaningful usage of the default. The def is set to 3.0
(height of a tree, for example). If the tag doesn't exist in the line or
if it is written wrong anyFloat is set to 3.0. In many cases the program
will work with such def values. Even more: if the desired value is the same
as the default, the line needn't consist an related entry. 

To create a file from a given set of different values is as easy:
Build the lines with SPAdd... or manually, e.g.:
	line = "[name] " + anyName + ...

Then insert the line in a CSPList with list.Add (line), and
finally save the list with list.Save (filepath). 
*/

class CSPList  {
private:
	string *flines;
	int fmax;
	int fcount;
	int  *fflag;
public:
	CSPList (int maxlines);
	~CSPList ();
	string Line (int idx);
	const char *LineC (int idx);
	int  Count ();
	void Clear ();
	void Add (string line);
	void Add (string line, int flag);
	void Append (string line, int idx);
	void Print ();
	bool Load (const string &filepath);
	bool Load (string dir, string filename);
	bool Save (const string &filepath);
	bool Save (string dir, string filename);

	int Flag (int idx);
	void SetFlag (int idx, int flag); 
	void MakeIndex (string &index, const string &tag);
};

void SetEnum (string s);

#endif


