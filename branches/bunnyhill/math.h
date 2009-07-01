// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 2004-2005 Volker Stroebel <volker@planetpenguin.de>
//
// --------------------------------------------------------------------

/*
This library is intended to make the class-orientated functions of ETR (PPRacer)
available. I don't use these functions since I can't see any advantage compared with
the native functions. Quite the contrary: The usage is more difficult and
less clear. Above all these classes are not really consistent. For example, is a 
matrix part of the quaternion class or belongs the quaternion to the matrix class?
In my opinion, such reciprocal or circularly dependencies are not the real destination 
of classes though it is common practice.

However, I've taken the code from ETR and put in this library for the case that
anyone prefers the class-orientated functions. This unit is not listed in the
MAKEFILE.
*/


#ifndef MATH_H
#define MATH_H

#include "bh.h"

// --------------------------------------------------------------------
//					native
// --------------------------------------------------------------------

TVector2 MakeVector2 (float x, float y);
TVector3 MakeVector3 (float x, float y, float z);

// predeclaration of all maths classes
class CVector2;
class CVector3;
class CMatrix;
class CQuaternion;

// --------------------------------------------------------------------
//					class CVector2
// --------------------------------------------------------------------

class CVector2 {
private:
public:
	CVector2 (void);
	CVector2 (const float x, const float y);
	CVector2 (const float *v);	

	TVector2 get ();
	void set (TVector2 v);
	void print ();

	float x;   
	float y;	
		
	friend CVector2 operator+ (const CVector2 &v1,const CVector2 &v2);
	friend CVector2 operator- (const CVector2 &v1,const CVector2 &v2);				
};

// --------------------------------------------------------------------
//					class CVector3
// --------------------------------------------------------------------

class CVector3 {
private:
public:
	float x;
	float y;
	float z;		

	CVector3 ();
	CVector3 (const float x, const float y, const float z);
	CVector3 (const float *v);	

	float normalize ();
	float length () const;
	float length2 () const;
	void print ();
	void set (TVector3 v);
	TVector3 get ();

	friend CVector3 operator+ (const CVector3 &v1, const CVector3 &v2);
	friend CVector3 operator- (const CVector3 &v1, const CVector3 &v2);
	friend CVector3 operator* (const CVector3 &v, const float scalar); 
	friend CVector3 operator* (const float scalar, const CVector3 &v);
	friend float  operator* (const CVector3 &v1, const CVector3 &v2);   // dot product
	friend CVector3 operator^ (const CVector3 &v1, const CVector3 &v2); // cross product
};

// --------------------------------------------------------------------
//					class CMatrix
// --------------------------------------------------------------------

class CMatrix {
private:
public:
	float data[4][4];
	CMatrix () {};
	CMatrix (const CQuaternion quat);	

	CMatrix operator* (const CMatrix matrix) const;
		
	void makeIdentity (void);
	void makeRotation (const float angle, const char axis);
	void makeTranslation (const float x, const float y, const float z);
	void makeScaling (const float x, const float y, const float z);
	void makeRotationAboutVector (const CVector3 u, const float angle);

	void transpose (const CMatrix &mat);
	CVector3 transformVector (const CVector3 v) const;
	CVector3 transformPoint (const CVector3 p) const;
		
	static void makeChangeOfBasisCMatrix (CMatrix &mat, CMatrix &invMat, 
		const CVector3 v1, const CVector3 v2, const CVector3 v3);	
};

// --------------------------------------------------------------------
//					class CQuaternion
// --------------------------------------------------------------------

class CQuaternion {
private:
public:
	float x;
	float y;
	float z;
	float w;

	CQuaternion () {};
	CQuaternion (const float x, const float y, const float z, const float w);	
	CQuaternion (const CVector3 &s, const CVector3 &t);
	CQuaternion (const CMatrix mat);
	
	void set (const float x, const float y, const float z, const float w);	

	CQuaternion conjugate (void) const;
	CVector3 rotate (const CVector3 &v) const;
	
	CQuaternion operator* (const CQuaternion &quat) const;

	static CQuaternion interpolate (const CQuaternion &q, CQuaternion r, float t);		
};

#endif




