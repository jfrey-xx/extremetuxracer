// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
//
// --------------------------------------------------------------------

#ifndef MATHLIB_H
#define MATHLIB_H

#include "bh.h"

// einige Makros: 
#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))
#define ROUND_TO_NEAREST(x) ((int) ((x)+0.5))
#define ANGLES_TO_RADIANS(x) (M_PI / 180.0 * (x) )
#define RADIANS_TO_ANGLES(x) (180.0 / M_PI * (x) )
#define RADIANS(x) (M_PI / 180.0 * (x) )
#define INVRADIANS(x) (180.0 / M_PI * (x) )

const TVector3 NullVec = {0.0, 0.0, 0.0};
const TVector3 GravVec = {0.0, -1.0, 0.0};

// vectors:
TVector3	MakeVec (float x, float y, float z);
TVector2	MakeVec2 (float x, float y);
TVector4	MakeVec4 (float x, float y, float z, float w);
TIndex2		MakeIndex2 (int i, int j);

TVector3    AddVec (const TVector3 &v1, const TVector3 &v2);
TVector3    SubVec (const TVector3 &v1, const TVector3 &v2);

TVector3	ScaleVec (float s, TVector3 v);
TVector2	ScaleVec2 (float s, TVector2 v);
float		NormVec (TVector3 &v);
float		NormVec2 (TVector2 &v);
TVector3	CrossVec (TVector3 u, TVector3 v);
float		DotVec (TVector3 v1, TVector3 v2);

float		VecLen (const TVector3 &v);
float		SquareVecLen (const TVector3 &v);
TVector3	SizeVec (float size, const TVector3 &v);
TVector3	ProjectToPlane (const TVector3 &planenml, const TVector3 &v);
void        IsolateVectors (const TVector3 &planenml, const TVector3 &v, 
	        TVector3 &parallel, TVector3 &perpendicular);
TVector3	TransformVec (TMatrix mat, TVector3 v);
TVector3	TransformNormal (TVector3 n, TMatrix mat);
TVector3	TransformPoint (TMatrix mat, TVector3 p);
TVector3	ReflectVec (const TVector3 &v, const TVector3 &norm);
float		VecDistance (TVector3 v1, TVector3 v2);
float       SquareVecDistance (TVector3 p1, TVector3 p2);

// palnes:
TPlane		MakePlane (float nx, float ny, float nz, float d);
bool		IntersectPlanes (TPlane s1, TPlane s2, TPlane s3, TVector3 *p);
float		DistToPlane (TPlane plane, TVector3 pt);

// matrices:
void MakeIdentityMat (TMatrix h);
void MultMat (TMatrix ret, TMatrix mat1, TMatrix mat2);
void TransposeMat (TMatrix mat, TMatrix trans);
void MakeRotMat (TMatrix mat, float angle, char axis);
void MakeTransMat (TMatrix mat, float x, float y, float z);
void MakeScaleMat (TMatrix mat, float x, float y, float z);
void MakefloatBasisMat (TMatrix mat, TMatrix invMat, TVector3 w1, TVector3 w2, TVector3 w3);
void MakeBasisMat (TMatrix mat,	TVector3 w1, TVector3 w2, TVector3 w3);
void RotateVecMat (TMatrix mat, TVector3 u, float angle);
void RotateVecMatY (TMatrix mat, TVector3 u, float angle);

// quaternions:
void        QuatToMat (TMatrix mat, TQuaternion q);
TVector3    RotateVec (TQuaternion q, TVector3 v);
TQuaternion MakeQuat (float x, float y, float z, float w);
TQuaternion MultQuatQuat (TQuaternion q, TQuaternion r);
TQuaternion AddQuat (TQuaternion q, TQuaternion r);
TQuaternion ScaleQuat (float s, TQuaternion q);
TQuaternion ConjugateQuat (TQuaternion q);
TQuaternion MatToQuat (TMatrix mat);
TQuaternion MakeRotQuat (TVector3 s, TVector3 t);
TQuaternion InterpolateQuat (TQuaternion q, TQuaternion r, float t);

// Gauss:
int gauss (float *matrix, int n, float *soln);

// Ode-Solver:

typedef char		TOdeData; 

// Deklaration of function pointers
typedef TOdeData*   (*PNewData) (); // pointer to ode-data
typedef int 		(*PNumEstimate) ();
typedef void 		(*PInitData) (TOdeData *, float init_val, float h );
//typedef float 		(*PNextTime) (TOdeData *, int step);
typedef float 		(*PNextValue) (TOdeData *, int step);
typedef void 		(*PUpdateEstimate) (TOdeData *, int step, float val );
typedef float 		(*PFinalEstimate) (TOdeData *);
typedef float 		(*PEstimateError) (TOdeData *);
//typedef float 		(*PTimestepExp) ();

typedef struct {
    PNewData newData;
    PNumEstimate numEstimates;
    PInitData initData;
//    PNextTime nextTime;
    PNextValue nextValue;
    PUpdateEstimate updateEstimate;
    PFinalEstimate finalEstimate;
    PEstimateError estimateError;
//    PTimestepExp timestepExp;
} TOdeSolver;

typedef struct {
    float k[4];
    float init_val;
    float h;
} TOdeData23;

TOdeSolver NewSolver23 ();


float lin_interp (const float x[], const float y[], float val, int n );

// enhancements:
void GetMatrix (TMatrixGL glmat);
void GetMatrix (TMatrix mat);

void GetTransMatrix (TMatrixGL glmat, float x, float y, float z);
void GetTransMatrix (TMatrix mat, float x, float y, float z);
void GetScaleMatrix (TMatrixGL glmat, float x, float y, float z);
void GetScaleMatrix (TMatrix mat, float x, float y, float z);
void GetRotMatrix (TMatrixGL glmat, float angle, float x, float y, float z);
void GetRotMatrix (TMatrix mat, float angle, float x, float y, float z);

void MatrixGL_Matrix (TMatrixGL glmat, TMatrix mat);
TVector3 VectorMatrixProduct (TMatrix mat, TVector3 vec);
TVector3 RotateVectorX (TVector3 v, float angle);
TVector3 RotateVectorY (TVector3 v, float angle);
TVector3 RotateVectorZ (TVector3 v, float angle);

TVector3 PlaneNormal (const TVector3 &a, const TVector3 &b, const TVector3 &c);
float   PlaneConstant (const TVector3 &pOrigin, const TVector3 &pNormal);
float   SphereIntersect3 (TVector3 center, TVector3 pos, TVector3 vel, float rad);

// random:
float xrandom (float min, float max);
float frandom ();
int    irandom (int min, int max);


#endif
