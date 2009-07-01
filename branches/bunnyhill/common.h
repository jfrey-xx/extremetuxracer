// --------------------------------------------------------------------
// common stuff, more or less useful
// I think this unit will be superfluous anytime
// --------------------------------------------------------------------

#ifndef COMMON_H
#define COMMON_H

#include "bh.h"

using namespace std;

// --------------------------------------------------------------------
//				color utils
// --------------------------------------------------------------------

extern const TColor colWhite;
extern const TColor colDYell;
extern const TColor colYellow;
extern const TColor colLYell;
extern const TColor colOrange;
extern const TColor colRed;
extern const TColor colGrey;
extern const TColor colLGrey;
extern const TColor colBlack;
extern const TColor colBlue;
extern const TColor colLBlue;
extern const TColor colDBlue;
extern const TColor colBackgr;
extern const TColor colDBackgr;
extern const TColor colMBackgr;
extern const TColor colLBackgr;
extern const TColor colMess;
extern const TColor colSky;

TColor	MakeColor (float r, float g, float b, float a);

// --------------------------------------------------------------------
//				print utils
// --------------------------------------------------------------------
// some simple functions to print out values on the 
// terminal. Only used for development. 

void	PrintInt (const int val);
void	PrintInt (char *s, const int val);
void	PrintStr (const char *val);
void	PrintString (string s);
void	PrintFloat (const float val);
void	PrintFloat8 (const float val);
void	PrintFloat (char *s, const float val);
void	PrintBool (const bool val);
void	PrintPointer (void *p);
void	PrintVector (const TVector3 v);
void	PrintVector4 (const TVector4 v);
void    PrintColor (const TColor);
void	PrintVector2 (const TVector2 v);
void	PrintVector (char *s, const TVector3 v);
void	PrintMatrix (TMatrix mat);
void	PrintMatrixGL (TMatrixGL glmat);
void	PrintQuaternion (TQuaternion q);

// --------------------------------------------------------------------
//				file utils
// --------------------------------------------------------------------

bool    FileExists (const char *filename);
bool    DirExists (const char *dirname);

// the following functions should work on Windows, too
// though they are not particularly for Window

bool    DirExists (const char *dirname);
bool    DirExistsWin (const char *dirname);

// --------------------------------------------------------------------
//				message utils
// --------------------------------------------------------------------

// at the moment the messages are written on the terminal.
// It's intended to write them in a file, so the user can see what was wrong
// and perhaps report errors
void    Message (const char *msg, const char *desc);
void    Message (const char *msg);
void	MessageN (string a, string b);

#endif 


