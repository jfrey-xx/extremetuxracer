// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 2004-2005 Volker Stroebel <volker@planetpenguin.de>
//
// --------------------------------------------------------------------

#include "math.h"

// --------------------------------------------------------------------
//					CVector2
// --------------------------------------------------------------------

TVector2 MakeVector2 (float x, float y) {
    TVector2 result;
    result.x = x;
    result.y = y;
    return result;
}

CVector2::CVector2 () : x(0.0), y(0.0) {}	
CVector2::CVector2 (const float x, const float y) : x(x), y(y) {}
CVector2::CVector2 (const float *v) : x(v[0]), y(v[1]) {}

void CVector2::set (TVector2 v) { x = v.x; y = v.y; }
TVector2 CVector2::get () { return MakeVector2 (x, y); }
void CVector2::print () { printf ("Vector: %.4f  %.4f \n", x, y); } 

CVector2 operator+ (const CVector2 &v1, const CVector2 &v2) {
	return CVector2 (v1.x + v2.x, v1.y + v2.y);
}

CVector2 operator- (const CVector2 &v1,const CVector2 &v2) {
	return CVector2 (v1.x - v2.x, v1.y - v2.y);
}

// --------------------------------------------------------------------
//					class CVector3
// --------------------------------------------------------------------

TVector3 MakeVector3 (float x, float y, float z) {
    TVector3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

CVector3::CVector3 () : x(0.0), y(0.0), z(0.0){}
CVector3::CVector3 (const float x, const float y, const float z) : x(x), y(y), z(z){}
CVector3::CVector3 (const float *v) : x(v[0]), y(v[1]), z(v[2]){}

float CVector3::normalize () {
    float len = length();
	if (len > 0.0) {
		x /= len;
		y /= len;
		z /= len;
	}                
	return (len);
};

float CVector3::length () const {
	return sqrt (x*x + y*y + z*z);
}

float CVector3::length2 () const {
	return x*x + y*y + z*z;
}

void CVector3::print () {
	printf ("Vector: %.4f  %.4f  %.4f \n", x, y, z); 
} 

void CVector3::set (TVector3 v) { x = v.x; y = v.y; z = v.z; }
TVector3 CVector3::get () { return MakeVector3 (x, y, z); }

CVector3 operator+ (const CVector3 &v1,const CVector3 &v2) {
	return CVector3 (v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

CVector3 operator- (const CVector3 &v1,const CVector3 &v2) {
	return CVector3 (v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

CVector3 operator* (const CVector3 &v, const float scalar) {
	return CVector3 (v.x * scalar, v.y * scalar, v.z * scalar);
}

CVector3 operator* (const float scalar, const CVector3 &v) {
	return CVector3 (v.x * scalar, v.y * scalar, v.z * scalar);
}

float operator* (const CVector3 &v1, const CVector3 &v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

CVector3 operator^ (const CVector3 &v1, const CVector3 &v2) {
	return CVector3 (
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}

// --------------------------------------------------------------------
//					CMatrix
// --------------------------------------------------------------------

CMatrix::CMatrix (const CQuaternion quat) {
    data[0][0] = 1.0 - 2.0 * ( quat.y * quat.y + quat.z * quat.z );
    data[1][0] =       2.0 * ( quat.x * quat.y - quat.w * quat.z );
    data[2][0] =       2.0 * ( quat.x * quat.z + quat.w * quat.y );

    data[0][1] =       2.0 * ( quat.x * quat.y + quat.w * quat.z );
    data[1][1] = 1.0 - 2.0 * ( quat.x * quat.x + quat.z * quat.z );
    data[2][1] =       2.0 * ( quat.y * quat.z - quat.w * quat.x );

	data[0][2] =       2.0 * ( quat.x * quat.z - quat.w * quat.y );
    data[1][2] =       2.0 * ( quat.y * quat.z + quat.w * quat.x );
    data[2][2] = 1.0 - 2.0 * ( quat.x * quat.x + quat.y * quat.y );

    data[3][0] = data[3][1] = data[3][2] = 0.0;
    data[0][3] = data[1][3] = data[2][3] = 0.0;
    data[3][3] = 1.0;
}

void CMatrix::makeIdentity(void) {
	for (int i=0; i<4; i++){
		for (int j=0; j<4; j++) data[i][j]=(i==j);	
	}
}	

void CMatrix::makeRotation (const float angle, const char axis) {
	float sinv = sin (ANGLES_TO_RADIANS (angle));
    float cosv = cos (ANGLES_TO_RADIANS (angle));

    makeIdentity();

    switch (axis) {
		case 'x':
			data[1][1] = cosv;
			data[2][1] = -sinv;
			data[1][2] = sinv;
			data[2][2] = cosv;
			break;
	
		case 'y':
			data[0][0] = cosv;
			data[2][0] = sinv;
			data[0][2] = -sinv;
			data[2][2] = cosv;
			break;
	
		case 'z': 
			data[0][0] = cosv;
			data[1][0] = -sinv;
			data[0][1] = sinv;
			data[1][1] = cosv;
			break;
	
		default: {}
    }
}

void CMatrix::makeTranslation(const float x, const float y, const float z) {
	makeIdentity();
    data[3][0] = x;
    data[3][1] = y;
    data[3][2] = z;
}

void CMatrix::makeScaling(const float x, const float y, const float z ) {
	makeIdentity();
    data[0][0] = x;
    data[1][1] = y;
    data[2][2] = z;
}

void CMatrix::makeRotationAboutVector (const CVector3 u, const float angle ) {
	CMatrix rx, irx, ry, iry;
	
    float a = u.x;
    float b = u.y;
    float c = u.z;
	float d = sqrt (b*b + c*c);

    if (d < 0.0001) {
        if (a < 0) makeRotation (-angle, 'x');
        else makeRotation (angle, 'x');
        return;
    } 

    rx.makeIdentity();
    irx.makeIdentity();
    ry.makeIdentity();
    iry.makeIdentity();

    rx.data[1][1] = c/d;
    rx.data[2][1] = -b/d;
    rx.data[1][2] = b/d;
    rx.data[2][2] = c/d;

    irx.data[1][1] = c/d;
    irx.data[2][1] = b/d;
    irx.data[1][2] = -b/d;
    irx.data[2][2] = c/d;

    ry.data[0][0] = d;
    ry.data[2][0] = -a;
    ry.data[0][2] = a;
    ry.data[2][2] = d;

    iry.data[0][0] = d;
    iry.data[2][0] = a;
    iry.data[0][2] = -a;
    iry.data[2][2] = d;

    makeRotation (angle, 'z');

    *this = (*this) * ry;
    *this = (*this) * rx;
    *this = iry * (*this);
    *this = irx * (*this);
}


void CMatrix::transpose (const CMatrix& matrix) {
	for( int i= 0 ; i< 4 ; i++ ){
		for( int j= 0 ; j< 4 ; j++ ){
	    	data[j][i] = matrix.data[i][j];
		}
	}
}

CMatrix CMatrix::operator* (const CMatrix matrix) const {
	CMatrix ret;
 
    for( int i= 0 ; i< 4 ; i++ )
	for( int j= 0 ; j< 4 ; j++ )
	    ret.data[j][i]=
		data[0][i] * matrix.data[j][0] +
		data[1][i] * matrix.data[j][1] +
		data[2][i] * matrix.data[j][2] +
		data[3][i] * matrix.data[j][3];
   	return ret;
}

CVector3 CMatrix::transformVector (const CVector3 v) const {   
	return CVector3 (
		v.x * data[0][0] + v.y * data[1][0] + v.z * data[2][0],
    	v.x * data[0][1] + v.y * data[1][1] + v.z * data[2][1],
    	v.x * data[0][2] + v.y * data[1][2] + v.z * data[2][2]
	);
}
	
		
CVector3 CMatrix::transformPoint (const CVector3 p) const {
	return CVector3 (
    	p.x * data[0][0] + p.y * data[1][0] + p.z * data[2][0] + data[3][0],
    	p.x * data[0][1] + p.y * data[1][1] + p.z * data[2][1] + data[3][1],
    	p.x * data[0][2] + p.y * data[1][2] + p.z * data[2][2] + data[3][2]
    );
}

void CMatrix::makeChangeOfBasisCMatrix (CMatrix& mat,
	CMatrix& invMat, const CVector3 w1, const CVector3 w2, const CVector3 w3) 
{
	mat.makeIdentity();
    mat.data[0][0] = w1.x;
    mat.data[0][1] = w1.y;
    mat.data[0][2] = w1.z;
    mat.data[1][0] = w2.x;
    mat.data[1][1] = w2.y;
    mat.data[1][2] = w2.z;
    mat.data[2][0] = w3.x;
    mat.data[2][1] = w3.y;
    mat.data[2][2] = w3.z;

    invMat.makeIdentity();
    invMat.data[0][0] = w1.x;
    invMat.data[1][0] = w1.y;
    invMat.data[2][0] = w1.z;
    invMat.data[0][1] = w2.x;
    invMat.data[1][1] = w2.y;
    invMat.data[2][1] = w2.z;
    invMat.data[0][2] = w3.x;
    invMat.data[1][2] = w3.y;
    invMat.data[2][2] = w3.z;
}

// --------------------------------------------------------------------
//					CQuaternion
// --------------------------------------------------------------------

CQuaternion::CQuaternion
	(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w)
{}	
	
CQuaternion::CQuaternion(const CVector3& s, const CVector3& t) {
    CVector3 u = s^t;
	if (u.normalize() < 0.0001){
		x=0.0;
		y=0.0;
		z=0.0;
		w=1.0;
    }else{
		float cos2phi = s*t;
		float sinphi = sqrt( ( 1 - cos2phi ) / 2.0 );
		float cosphi = sqrt( ( 1 + cos2phi ) / 2.0 );

		x = sinphi * u.x;
		y = sinphi * u.y;
		z = sinphi * u.z;
		w = cosphi;
    }
}

// source: http://www.gamasutra.com/features/19980703/quaternions_01.htm

CQuaternion::CQuaternion (const CMatrix mat) {
    static int nxt[3] = {1, 2, 0};
    float tr = mat.data[0][0] + mat.data[1][1] + mat.data[2][2];

    // check the diagonal
   	if (tr > 0.0) {
		float s = sqrt (tr + 1.0);
		w = 0.5 * s;
		s = 0.5 / s;
		x = (mat.data[1][2] - mat.data[2][1]) * s;
		y = (mat.data[2][0] - mat.data[0][2]) * s;
		z = (mat.data[0][1] - mat.data[1][0]) * s;
    } else {                
		// diagonal is negative
		int i = 0;
		if (mat.data[1][1] > mat.data[0][0]) i = 1;
		if (mat.data[2][2] > mat.data[i][i]) i = 2;
		int j = nxt[i];
		int k = nxt[j];

		float s = sqrt (mat.data[i][i] - mat.data[j][j] - mat.data[k][k] + 1.0);
        
		float q[4];		
		q[i] = s * 0.5;
                             
		if (s != 0.0) s = 0.5 / s;

		q[3] = (mat.data[j][k] - mat.data[k][j]) * s;
		q[j] = (mat.data[i][j] + mat.data[j][i]) * s;
		q[k] = (mat.data[i][k] + mat.data[k][i]) * s;

		x = q[0];
		y = q[1];
		z = q[2];
		w = q[3];
	}
}

void CQuaternion::set (const float x, const float y, const float z, const float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}


CQuaternion CQuaternion::conjugate(void) const {
	return CQuaternion(-x, -y, -z, w);	
}

CVector3 CQuaternion::rotate( const CVector3& v ) const {
    CQuaternion p(v.x,v.y,v.z,1.0);
    CQuaternion res_q = (*this)*(p*conjugate());
	return CVector3(res_q.x,res_q.y,res_q.z);
}

CQuaternion CQuaternion::operator*(const CQuaternion& quat) const {
	return CQuaternion(
		y * quat.z - z * quat.y + quat.w * x + w * quat.x,
		z * quat.x - x * quat.z + quat.w * y + w * quat.y,
		x * quat.y - y * quat.x + quat.w * z + w * quat.z,
		w * quat.w - x * quat.x - y * quat.y - z * quat.z
	);
}

CQuaternion CQuaternion::interpolate (const CQuaternion& q, CQuaternion r,float t) {
	CQuaternion res;
    float cosphi;
    float sinphi;
    float phi;
    float scale0, scale1;

    cosphi = q.x * r.x + q.y * r.y + q.z * r.z + q.w * r.w;

    // adjust signs (if necessary) 
    if (cosphi < 0.0 ) {
	cosphi = -cosphi;
	r.x = -r.x;
	r.y = -r.y;
	r.z = -r.z;
	r.w = -r.w;
    }

    if (1.0 - cosphi > 0.0001) {
		// standard case -- slerp 
		phi = acos( cosphi );
		sinphi = sin( phi );
		scale0 = sin( phi * ( 1.0 - t ) ) / sinphi;
		scale1 = sin( phi * t ) / sinphi;
    } else {
		// use linear interpolation to avoid division by zero
		scale0 = 1.0 - t;
		scale1 = t;
    }

    res.x = scale0 * q.x + scale1 * r.x; 
    res.y = scale0 * q.y + scale1 * r.y; 
    res.z = scale0 * q.z + scale1 * r.z; 
    res.w = scale0 * q.w + scale1 * r.w; 

    return res;
}




