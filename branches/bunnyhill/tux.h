// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
//
// --------------------------------------------------------------------

// Parts of the Tux code are still from Tuxracer though there have been
// a lot of modifications. I think, more modifications are necessary.
// So it's not very efficient to call the character nodes with their 
// names. Anyhow, sometime the sphere-orientated character shapes should be
// replaced with real 3D models, to get better performance
// and to make more different shapes possible.


#ifndef TUX_H
#define TUX_H

#include "bh.h"

typedef struct {
    TColor diffuse;
    TColor specular;
    float exp;
} TTuxMaterial2;

typedef struct NodeStruct2 {
    struct NodeStruct2 *parent;
    struct NodeStruct2 *next;
    struct NodeStruct2 *child;

    char *name;
	bool visible;
    TMatrix trans;
    TMatrix invtrans;   
	float radius;
	int divisions;
    TTuxMaterial2 *mat;
} TCharNode2;

void DrawTux ();

bool GetCharNode2 (char *node_name, TCharNode2 **node);
bool ResetCharNode2 (char *node_name);
bool TranslateCharNode2 (char *node_name, TVector3 vec);
bool TransformCharNode2 (char *node_name, TMatrix mat, TMatrix invmat);
bool RotateCharNode2 (char *node_name, float axis, float angle);

void ResetTuxRoot ();
void ResetTuxJoints (); 

void SetTuxOrientation2 (TVector3 dir, TVector3 up, float angle);
void SetTuxOrientation2 (TVector3 vel, float angle);
void SetTuxStandingOrientation2 (float angle);
void SetTuxAnimation2 ();
void AdjustTuxOrientation2 (float timestep, TControl *ctrl);
void AdjustTuxAnimation2 (TControl *ctrl);
void AdjustTuxJoints2 (float turnFact, bool isBraking, 
		float paddling_factor, float speed,
		TVector3 net_force, float flap_factor);

// making it common:

void LoadCharacter ();	
void SetCharacterPosition (TVector3 pos, float timestep, TControl *ctrl);
void SetCharacterOrientation (float timestep, TControl *ctrl);
void SetCharacterAnimation (TControl *ctrl);

#endif
