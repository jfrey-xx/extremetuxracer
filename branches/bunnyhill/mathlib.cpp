// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
//
// --------------------------------------------------------------------

#include "mathlib.h"

TVector3 MakeVec (float x, float y, float z) {
    TVector3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

TVector2 MakeVec2 (float x, float y){
    TVector2 result;
    result.x = x;
    result.y = y;
    return result;
}

TVector4 MakeVec4 (float x, float y, float z, float w) {
    TVector4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}

TIndex2 MakeIndex2 (int i, int j) {
    TIndex2 result;
    result.i = i;
    result.j = j;
    return result;
}

float DotVec (TVector3 v1, TVector3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float VecLen (const TVector3 &v) {
	return sqrt (v.x * v.x + v.y * v.y + v.z * v.z);
}

float SquareVecLen (const TVector3 &v) {
	return (v.x * v.x + v.y * v.y + v.z * v.z);
}

TVector3 SizeVec (float size, const TVector3 &v) {
	if (v.x == 0 && v.y == 0 && v.z == 0) return v;
	float len = VecLen (v);
	return ScaleVec (size / len, v);
}

TVector3 ScaleVec (float s, TVector3 v){
    TVector3 rval;
    rval.x = s * v.x;
    rval.y = s * v.y;
    rval.z = s * v.z;
    return rval;
}

TVector2 ScaleVec2 (float s, TVector2 v) {
    TVector2 rval;
    rval.x = s * v.x;
    rval.y = s * v.y;
    return rval;
}

TVector3 AddVec (const TVector3 &v1, const TVector3 &v2) {
    TVector3 result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}

TVector3 SubVec (const TVector3 &v1, const TVector3 &v2) {
    TVector3 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

TVector3 ProjectToPlane (const TVector3 &planenml, const TVector3 &v) {
	// planenml must be normalized
	// result is a vector parallel to the plane
    float dotprod = DotVec (planenml, v);
	TVector3 nmlcomp = ScaleVec (dotprod, planenml);
    return SubVec (v, nmlcomp);
} 

void IsolateVectors (const TVector3 &planenml, const TVector3 &v, 
		TVector3 &parallel, TVector3 &perpendicular) {
	// planenml must be normalized
	// v is divided in projection part and perpendicular part
    float dotprod = DotVec (planenml, v);
	perpendicular = ScaleVec (dotprod, planenml);
    parallel = SubVec (v, perpendicular);
} 

float NormVec2 (TVector2 &v) {
	if (v.x == 0 && v.y == 0) return 0.0;
	float denom = sqrt (v.x * v.x + v.y * v.y);
	v = ScaleVec2 (1.0 / denom, v);
	return denom;
}

float NormVec (TVector3 &v) {
	if (v.x == 0 && v.y == 0 && v.z == 0) return 0.0;
	float denom = sqrt (v.x * v.x + v.y * v.y + v.z * v.z);
	v = ScaleVec (1.0 / denom, v);
	return denom;
}

TVector3 CrossVec (TVector3 u, TVector3 v) {
    TVector3 ret;
    ret.x = u.y * v.z - u.z * v.y;
    ret.y = u.z * v.x - u.x * v.z;
    ret.z = u.x * v.y - u.y * v.x;
    return ret;
}

TVector3 ReflectVec (const TVector3 &v, const TVector3 &norm) {
	TVector3 res;
	float dotprod = DotVec (v, norm);
	res.x = v.x - 2 * norm.x * dotprod;
	res.y = v.y - 2 * norm.y * dotprod;
	res.z = v.z - 2 * norm.z * dotprod;
	return res;
}

float VecDistance (TVector3 v1, TVector3 v2) {
	float xdist = v1.x - v2.x;
	float ydist = v1.y - v2.y;
	float zdist = v1.z - v2.z; 
	return sqrt (xdist * xdist + ydist * ydist + zdist * zdist);
}

float SquareVecDistance (TVector3 p1, TVector3 p2) {
	float xdist = p1.x - p2.x;
	float ydist = p1.y - p2.y;
	float zdist = p1.z - p2.z; 
	return (xdist * xdist + ydist * ydist + zdist * zdist);
}


TVector3 TransformVec(TMatrix mat, TVector3 v){
    TVector3 r;
    r.x = v.x * mat[0][0] + v.y * mat[1][0] + v.z * mat[2][0];
    r.y = v.x * mat[0][1] + v.y * mat[1][1] + v.z * mat[2][1];
    r.z = v.x * mat[0][2] + v.y * mat[1][2] + v.z * mat[2][2];
    return r;
}

TVector3 TransformNormal(TVector3 n, TMatrix mat){
    TVector3 r;
    r.x = n.x * mat[0][0] + n.y * mat[0][1] + n.z * mat[0][2];
    r.y = n.x * mat[1][0] + n.y * mat[1][1] + n.z * mat[1][2];
    r.z = n.x * mat[2][0] + n.y * mat[2][1] + n.z * mat[2][2];
    return r;
}

TVector3 TransformPoint(TMatrix mat, TVector3 p){
    TVector3 r;
    r.x = p.x * mat[0][0] + p.y * mat[1][0] + p.z * mat[2][0];
    r.y = p.x * mat[0][1] + p.y * mat[1][1] + p.z * mat[2][1];
    r.z = p.x * mat[0][2] + p.y * mat[1][2] + p.z * mat[2][2];
    r.x += mat[3][0];
    r.y += mat[3][1];
    r.z += mat[3][2];
    return r;
}

TPlane MakePlane (float nx, float ny, float nz, float d){
    TPlane tmp;
    tmp.nml.x = nx;
    tmp.nml.y = ny;
    tmp.nml.z = nz;
    tmp.d = d;

    return tmp;
}

bool IntersectPlanes (TPlane s1, TPlane s2, TPlane s3, TVector3 *p){
    float A[3][4];
    float x[3];
    float retval;

    A[0][0] =  s1.nml.x;
    A[0][1] =  s1.nml.y;
    A[0][2] =  s1.nml.z;
    A[0][3] = -s1.d;

    A[1][0] =  s2.nml.x;
    A[1][1] =  s2.nml.y;
    A[1][2] =  s2.nml.z;
    A[1][3] = -s2.d;

    A[2][0] =  s3.nml.x;
    A[2][1] =  s3.nml.y;
    A[2][2] =  s3.nml.z;
    A[2][3] = -s3.d;

    retval = gauss ((float*) A, 3, x);

    if  (retval != 0) {
		return False;
    } else {
		p->x = x[0];
		p->y = x[1];
		p->z = x[2];
		return True;
    }
}

float DistToPlane (TPlane plane, TVector3 pt) {
    return 
		plane.nml.x * pt.x +
		plane.nml.y * pt.y +
		plane.nml.z * pt.z +
		plane.d;
}

void MakeIdentityMat(TMatrix h){
    int i,j;
  
    for (i= 0 ; i< 4 ; i++)
	for (j= 0 ; j< 4 ; j++)
	    h[i][j]= (i==j);
}

void MultMat (TMatrix ret, TMatrix mat1, TMatrix mat2){
    int i, j;
    TMatrix r;
    for (i=0; i<4; i++)
	for (j=0; j<4; j++)
	    r[j][i] = 
			mat1[0][i] * mat2[j][0] +
			mat1[1][i] * mat2[j][1] +
			mat1[2][i] * mat2[j][2] +
			mat1[3][i] * mat2[j][3];

    for (i=0; i<4; i++)
	for (j=0; j<4; j++)
	    ret[i][j] = r[i][j];
}

void TransposeMat (TMatrix mat, TMatrix trans) {
    int i, j;
    TMatrix r;

    for (i=0; i<4; i++)
	for (j=0; j<4; j++)
	    r[j][i] = mat[i][j];

    for (i=0; i<4; i++)
	for (j=0; j<4; j++)
	    trans[i][j] = r[i][j];
}

void MakeRotMat (TMatrix mat, float angle, char axis) {
    float sinv, cosv;
    sinv = sin (ANGLES_TO_RADIANS (angle));
    cosv = cos (ANGLES_TO_RADIANS (angle));

    MakeIdentityMat (mat);

    switch (axis) {
    case 'x':
        mat[1][1] = cosv;
        mat[2][1] = -sinv;
        mat[1][2] = sinv;
        mat[2][2] = cosv;
        break;

    case 'y':
        mat[0][0] = cosv;
        mat[2][0] = sinv;
        mat[0][2] = -sinv;
        mat[2][2] = cosv;
        break;

    case 'z': 
        mat[0][0] = cosv;
        mat[1][0] = -sinv;
        mat[0][1] = sinv;
        mat[1][1] = cosv;
        break;
    }
} 

void MakeTransMat (TMatrix mat, float x, float y, float z){
    MakeIdentityMat (mat);
    mat[3][0] = x;
    mat[3][1] = y;
    mat[3][2] = z;
} 

void MakeScaleMat (TMatrix mat, float x, float y, float z){
    MakeIdentityMat (mat);
    mat[0][0] = x;
    mat[1][1] = y;
    mat[2][2] = z;
} 

void MakefloatBasisMat (TMatrix mat, TMatrix invMat, TVector3 w1, TVector3 w2, TVector3 w3){
    MakeIdentityMat (mat);
    mat[0][0] = w1.x;
    mat[0][1] = w1.y;
    mat[0][2] = w1.z;
    mat[1][0] = w2.x;
    mat[1][1] = w2.y;
    mat[1][2] = w2.z;
    mat[2][0] = w3.x;
    mat[2][1] = w3.y;
    mat[2][2] = w3.z;

    MakeIdentityMat (invMat);
    invMat[0][0] = w1.x;
    invMat[1][0] = w1.y;
    invMat[2][0] = w1.z;
    invMat[0][1] = w2.x;
    invMat[1][1] = w2.y;
    invMat[2][1] = w2.z;
    invMat[0][2] = w3.x;
    invMat[1][2] = w3.y;
    invMat[2][2] = w3.z;
} 

void MakeBasisMat (TMatrix mat, TVector3 w1, TVector3 w2, TVector3 w3){
    MakeIdentityMat (mat);
    mat[0][0] = w1.x;
    mat[0][1] = w1.y;
    mat[0][2] = w1.z;
    mat[1][0] = w2.x;
    mat[1][1] = w2.y;
    mat[1][2] = w2.z;
    mat[2][0] = w3.x;
    mat[2][1] = w3.y;
    mat[2][2] = w3.z;
} 

// old name: "make_rotation_about_vector_matrix" - oh yes 
void RotateVecMat (TMatrix mat, TVector3 u, float angle) {
    TMatrix rx, irx, ry, iry;
    float a, b, c, d;

    a = u.x;
    b = u.y;
    c = u.z;
    d = sqrt (b*b + c*c);

    if  (d < 0.0001) {
        if  (a < 0) MakeRotMat (mat, -angle, 'x');
        else MakeRotMat (mat, angle, 'x');
        return;
    } 

    MakeIdentityMat (rx);
    MakeIdentityMat (irx);
    MakeIdentityMat (ry);
    MakeIdentityMat (iry);

    rx[1][1] = c/d;
    rx[2][1] = -b/d;
    rx[1][2] = b/d;
    rx[2][2] = c/d;

    irx[1][1] = c/d;
    irx[2][1] = b/d;
    irx[1][2] = -b/d;
    irx[2][2] = c/d;

    ry[0][0] = d;
    ry[2][0] = -a;
    ry[0][2] = a;
    ry[2][2] = d;

    iry[0][0] = d;
    iry[2][0] = a;
    iry[0][2] = -a;
    iry[2][2] = d;

    MakeRotMat (mat, angle, 'z');

    MultMat (mat, mat, ry);
    MultMat (mat, mat, rx);
    MultMat (mat, iry, mat);
    MultMat (mat, irx, mat);
} 

void RotateVecMatY (TMatrix mat, TVector3 u, float angle) {
    TMatrix rx, irx, ry, iry;
    float a, b, c, d;

    a = u.x;
    b = u.y;
    c = u.z;
    d = sqrt (b*b + c*c);

    if  (d < 0.0001) {
        if  (a < 0) MakeRotMat (mat, -angle, 'y');
        else MakeRotMat (mat, angle, 'y');
        return;
    } 

    MakeIdentityMat (rx);
    MakeIdentityMat (irx);
    MakeIdentityMat (ry);
    MakeIdentityMat (iry);

    rx[1][1] = c/d;
    rx[2][1] = -b/d;
    rx[1][2] = b/d;
    rx[2][2] = c/d;

    irx[1][1] = c/d;
    irx[2][1] = b/d;
    irx[1][2] = -b/d;
    irx[2][2] = c/d;

    ry[0][0] = d;
    ry[2][0] = -a;
    ry[0][2] = a;
    ry[2][2] = d;

    iry[0][0] = d;
    iry[2][0] = a;
    iry[0][2] = -a;
    iry[2][2] = d;

    MakeRotMat (mat, angle, 'y');

    MultMat (mat, mat, ry);
    MultMat (mat, mat, rx);
    MultMat (mat, iry, mat);
    MultMat (mat, irx, mat);
} 

TQuaternion MakeQuat (float x, float y, float z, float w){
    TQuaternion q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}

TQuaternion MultQuatQuat (TQuaternion q, TQuaternion r){
    TQuaternion res;
    res.x = q.y * r.z - q.z * r.y + r.w * q.x + q.w * r.x;
    res.y = q.z * r.x - q.x * r.z + r.w * q.y + q.w * r.y;
    res.z = q.x * r.y - q.y * r.x + r.w * q.z + q.w * r.z;
    res.w = q.w * r.w - q.x * r.x - q.y * r.y - q.z * r.z;
    return res;
}

TQuaternion AddQuat (TQuaternion q, TQuaternion r){
    TQuaternion res; 
    res.x = q.x + r.x;
    res.y = q.y + r.y;
    res.z = q.z + r.z;
    res.w = q.w + r.w;
    return res;
}

TQuaternion ConjugateQuat (TQuaternion q){
    TQuaternion res;
    res.x = -1 * q.x;
    res.y = -1 * q.y;
    res.z = -1 * q.z;
    res.w = q.w;

    return res;
}

TQuaternion ScaleQuat (float s, TQuaternion q){
    TQuaternion res;
    res.x = s * q.x;
    res.y = s * q.y;
    res.z = s * q.z;
    res.w = s * q.w;

    return res;
}

void QuatToMat (TMatrix mat, TQuaternion q){
    mat[0][0] = 1.0 - 2.0 *  (q.y * q.y + q.z * q.z);
    mat[1][0] =       2.0 *  (q.x * q.y - q.w * q.z);
    mat[2][0] =       2.0 *  (q.x * q.z + q.w * q.y);

    mat[0][1] =       2.0 *  (q.x * q.y + q.w * q.z);
    mat[1][1] = 1.0 - 2.0 *  (q.x * q.x + q.z * q.z);
    mat[2][1] =       2.0 *  (q.y * q.z - q.w * q.x);

    mat[0][2] =       2.0 *  (q.x * q.z - q.w * q.y);
    mat[1][2] =       2.0 *  (q.y * q.z + q.w * q.x);
    mat[2][2] = 1.0 - 2.0 *  (q.x * q.x + q.y * q.y);

    mat[3][0] = mat[3][1] = mat[3][2] = 0.0;
    mat[0][3] = mat[1][3] = mat[2][3] = 0.0;
    mat[3][3] = 1.0;
}

TQuaternion MatToQuat (TMatrix m){
    TQuaternion res;
    float  tr, s, q[4];
    int    i, j, k;

    static int nxt[3] = {1, 2, 0};

    tr = m[0][0] + m[1][1] + m[2][2];

    if (tr > 0.0) {
	s = sqrt (tr + 1.0);
	res.w = 0.5 * s;
	s = 0.5 / s;
	res.x = (m[1][2] - m[2][1]) * s;
	res.y = (m[2][0] - m[0][2]) * s;
	res.z = (m[0][1] - m[1][0]) * s;
    } else {                
	i = 0;
	if (m[1][1] > m[0][0]) i = 1;
	if (m[2][2] > m[i][i]) i = 2;
	j = nxt[i];
	k = nxt[j];

	s = sqrt (m[i][i] - m[j][j] - m[k][k] + 1.0);
                       
	q[i] = s * 0.5;
                             
	if (s != 0.0) s = 0.5 / s;

	q[3] = (m[j][k] - m[k][j]) * s;
	q[j] = (m[i][j] + m[j][i]) * s;
	q[k] = (m[i][k] + m[k][i]) * s;

	res.x = q[0];
	res.y = q[1];
	res.z = q[2];
	res.w = q[3];
    }

    return res;
}

TQuaternion MakeRotQuat (TVector3 s, TVector3 t){
    TQuaternion res;
    TVector3 u;
    float cos2phi, sin2phi;
    float cosphi, sinphi;

    u = CrossVec (s, t);
    sin2phi = NormVec (u);

    if  (sin2phi < 0.0001) {
	res = MakeQuat (0., 0., 0., 1.);
    } else {
	cos2phi = DotVec (s, t);

	sinphi = sqrt ( (1 - cos2phi) / 2.0);
	cosphi = sqrt ( (1 + cos2phi) / 2.0);

	res.x = sinphi * u.x;
	res.y = sinphi * u.y;
	res.z = sinphi * u.z;
	res.w = cosphi;
    }

    return res;
}

TQuaternion InterpolateQuat (TQuaternion q, TQuaternion r, float t){
    TQuaternion res;
    float cosphi;
    float sinphi;
    float phi;
    float scale0, scale1;

    cosphi = q.x * r.x + q.y * r.y + q.z * r.z + q.w * r.w;

    if  (cosphi < 0.0) {
	cosphi = -cosphi;
	r.x = -r.x;
	r.y = -r.y;
	r.z = -r.z;
	r.w = -r.w;
    }

    if  (1.0 - cosphi > 0.0001) {
	phi = acos (cosphi);
	sinphi = sin (phi);
	scale0 = sin (phi *  (1.0 - t)) / sinphi;
	scale1 = sin (phi * t) / sinphi;
    } else {
	scale0 = 1.0 - t;
	scale1 = t;
    }

    res.x = scale0 * q.x + scale1 * r.x; 
    res.y = scale0 * q.y + scale1 * r.y; 
    res.z = scale0 * q.z + scale1 * r.z; 
    res.w = scale0 * q.w + scale1 * r.w; 

    return res;
}

TVector3 RotateVec (TQuaternion q, TVector3 v){
    TVector3 res;
    TQuaternion res_q;
    TQuaternion qs;
    TQuaternion p;

    p.x = v.x;
    p.y = v.y;
    p.z = v.z;
    p.w = 1.0;

    qs.x = -q.x;
    qs.y = -q.y;
    qs.z = -q.z;
    qs.w =  q.w;

    res_q = MultQuatQuat (q, MultQuatQuat (p, qs));
    res.x = res_q.x;
    res.y = res_q.y;
    res.z = res_q.z;

    return res;
}


// --------------------------------------------------------------------
//				 gauss
// --------------------------------------------------------------------


unsigned short order (float *matrix, int n, int pivot);
void elim (float *matrix, int n, int pivot);
void backsb (float *matrix, int n, float *soln);


int gauss(float *matrix, int n, float *soln){
    int pivot=0;
    unsigned short error=0;

    while ((pivot<(n-1)) && (!error)) {
		if(!(error = order(matrix,n,pivot))) {
		    elim(matrix,n,pivot);
		    pivot++;
		}
    }
    if (error) {
		return 1;
    } else {
		backsb(matrix, n, soln);
    }
    return 0;
}


unsigned short order (float *matrix, int n, int pivot){
    int row, rmax, k;
    float temp;
    unsigned short error=0;

    rmax = pivot;

    for (row=pivot+1; row<n; row++) {
		if (fabs(*(matrix+row*(n+1)+pivot)) > fabs(*(matrix+rmax*(n+1)+pivot)))
		    rmax = row;
	}

    if (fabs(*(matrix+rmax*(n+1)+pivot)) < 0.0001)
		error = 1;
    else if (rmax != pivot) {
		for (k=0; k<(n+1); k++)
		{
		    temp = *(matrix+rmax*(n+1)+k);
		    *(matrix+rmax*(n+1)+k) = *(matrix+pivot*(n+1)+k);
		    *(matrix+pivot*(n+1)+k) = temp;
		}
    }
    return error;
}

void elim (float *matrix, int n, int pivot){
    int row, col;
    float factor;

    for (row = pivot+1; row < n; row++) {
		factor = (*(matrix+row*(n+1)+pivot))/(*(matrix+pivot*(n+1)+pivot));
		*(matrix+row*(n+1)+pivot)=0.0;
		for (col=pivot+1l; col<n+1; col++) {
		    *(matrix+row*(n+1)+col) = *(matrix+row*(n+1)+col) - 
			(*(matrix+pivot*(n+1)+col))*factor;
		}
    }
}


void backsb (float *matrix, int n, float *soln){
    int row, col;

    for (row = n-1; row >=0; row--){
		for (col = n-1; col >= row+1; col--) {
		    *(matrix+row*(n+1)+(n)) = *(matrix+row*(n+1)+n) - 
			(*(soln+col))*(*(matrix+row*(n+1)+col));
		}
		*(soln+row) = (*(matrix+row*(n+1)+n))/(*(matrix+row*(n+1)+row));
    }
}

// --------------------------------------------------------------------
//					ode solver
// --------------------------------------------------------------------

//const float TimeExp23      = 1.0/3.0;
const float TimeMatrix23[] = { 0.0, 0.5, 0.75, 1.0 };
const float ErrorMatrix23[]= {-5.0/72.0, 1.0/12.0, 1.0/9.0, -1.0/8.0 };
const float CoeffMatrix23[][4] = {
    {0.0, 0.5, 0.00, 0.22222222},
    {0.0, 0.0, 0.75, 0.33333333},
    {0.0, 0.0, 0.00, 0.44444444},
    {0.0, 0.0, 0.00, 0.0} 
};


TOdeData *NewData23 () {
    TOdeData23 *data;
    data = (TOdeData23*)malloc(sizeof(TOdeData23));
    return (TOdeData*) data;
}

int numEstimates23() {return 4; }

void initData23 (TOdeData *p, float init_val, float h) {
    TOdeData23 *data = (TOdeData23*)p;
    data->init_val = init_val;
    data->h = h;
}

/*
float nextTime23(TOdeData *p, int step) {
    TOdeData23 *data = (TOdeData23*)p;
    return TimeMatrix23[step] * data->h;
}
*/
float nextValue23 (TOdeData *p, int step) {
    TOdeData23 *data = (TOdeData23*)p;
    float val = data->init_val;
    int i;

    for  (i=0; i<step; i++) val += CoeffMatrix23[i][step] * data->k[i];
    return val;
}

void updateEstimate23(TOdeData *p, int step, float val) {
    TOdeData23 *data = (TOdeData23*)p;
    data->k[step] = data->h * val;
}

float finalEstimate23(TOdeData *p) {
    TOdeData23 *data = (TOdeData23*)p;
    float val = data->init_val;
    int i;

    for (i=0; i<3; i++) val += CoeffMatrix23[i][3] * data->k[i];
    return val;
}

float estimateError23 (TOdeData *p){
    TOdeData23 *data = (TOdeData23*)p;
    float err=0.;
    int i;

    for  (i=0; i<4; i++) err += ErrorMatrix23[i] * data->k[i];
    return fabs(err);
}

/*
float timestepExp23(){
    return TimeExp23;
}
*/
TOdeSolver NewSolver23 (){
    TOdeSolver s; 
    s.newData = NewData23;
    s.numEstimates = numEstimates23;
    s.initData = initData23;
//    s.nextTime = nextTime23;
    s.nextValue = nextValue23;
    s.updateEstimate = updateEstimate23;
    s.finalEstimate = finalEstimate23;
    s.estimateError = estimateError23;
//    s.timestepExp = timestepExp23;
    return s;
}

float lin_interp (const float x[], const float y[], float val, int n){
    int i;
    float m, b;

    if (val < x[0]) i = 0;
    else if (val >= x[n-1]) i = n-2;
    else for (i=0; i<n-1; i++) if (val < x[i+1]) break;

    m = (y[i+1] - y[i]) / (x[i+1] - x[i]);
    b = y[i] - m * x[i];

    return m * val + b;
}

// --------------------------------------------------------------------
//					enhancements
// --------------------------------------------------------------------

void GetMatrix (TMatrixGL glmat) {
	glGetFloatv (GL_MODELVIEW_MATRIX, glmat);
}


void GetMatrix (TMatrix mat) {
	TMatrixGL glmat;
	glGetFloatv (GL_MODELVIEW_MATRIX, glmat);
	MatrixGL_Matrix (glmat, mat);
}


void MatrixGL_Matrix (TMatrixGL glmat, TMatrix mat) {
	int i,j;
	for (j=0; j<4; j++) {
		for (i=0; i<4; i++) mat[j][i] = glmat[i*4+j];	
	}
}

void GetTransMatrix (TMatrixGL glmat, float x, float y, float z) {
	glPushMatrix ();
	glLoadIdentity ();
	glTranslatef (x, y, z);
	GetMatrix (glmat);				
	glPopMatrix ();
}

void GetTransMatrix (TMatrix mat, float x, float y, float z) {
	TMatrixGL glmat;
	GetTransMatrix (glmat, x, y, z);
	MatrixGL_Matrix (glmat, mat);
}

void GetScaleMatrix (TMatrixGL glmat, float x, float y, float z) {
	glPushMatrix ();
	glLoadIdentity ();
	glScalef (x, y, z);
	GetMatrix (glmat);				
	glPopMatrix ();
}

void GetScaleMatrix (TMatrix mat, float x, float y, float z) {
	TMatrixGL glmat;
	GetScaleMatrix (glmat, x, y, z);
	MatrixGL_Matrix (glmat, mat);
}

void GetRotMatrix (TMatrixGL glmat, float angle, float x, float y, float z) {
	glPushMatrix ();
	glLoadIdentity ();
	glRotatef (angle, x, y, z);
	GetMatrix (glmat);				
	glPopMatrix ();
}

void GetRotMatrix (TMatrix mat, float angle, float x, float y, float z) {
	TMatrixGL glmat;
	GetRotMatrix (glmat, angle, x, y, z);
	MatrixGL_Matrix (glmat, mat);
}

TVector3 VectorMatrixProduct (TMatrix mat, TVector3 vec) {
	TVector3 ret;
	ret.x = mat[0][0]*vec.x + mat[0][1]*vec.y + mat[0][2]*vec.z + mat[0][3];
	ret.y = mat[1][0]*vec.x + mat[1][1]*vec.y + mat[1][2]*vec.z + mat[1][3];
	ret.z = mat[2][0]*vec.x + mat[2][1]*vec.y + mat[2][2]*vec.z + mat[2][3];
	return ret;
}

TVector3 RotateVectorX (TVector3 v, float angle) {
	TMatrix mat;
	GetRotMatrix (mat, angle, 1, 0, 0);	
	return VectorMatrixProduct (mat, v);
}

TVector3 RotateVectorY (TVector3 v, float angle) {
	TMatrix mat;
	GetRotMatrix (mat, angle, 0, 1, 0);	
	return VectorMatrixProduct (mat, v);
}

TVector3 RotateVectorZ (TVector3 v, float angle) {
	TMatrix mat;
	GetRotMatrix (mat, angle, 0, 0, 1);	
	return VectorMatrixProduct (mat, v);
}

// --------------------------------------------------------------------
//					collision
// --------------------------------------------------------------------

TVector3 PlaneNormal (const TVector3 &a, const TVector3 &b, const TVector3 &c) {
	TVector3 res;
	TVector3 ab = SubVec (b,a);
	TVector3 ac = SubVec (c,a);
	res = CrossVec (ab, ac);
	NormVec (res);
	return res;
}

float PlaneConstant (const TVector3 &pOrigin, const TVector3 &pNormal) {
	return -(DotVec (pNormal, pOrigin));
}

float SphereIntersect3 (TVector3 center, TVector3 pos, TVector3 vel, float rad) {
	center.y = pos.y = vel.y = 0;
	TVector3 q = SubVec (center, pos);
	float c = VecLen (q);
	float v = DotVec (q, vel);
	float d = rad * rad - (c * c - v * v);
	
	if (d < 0.0) return -1;
	return v - sqrt(d);
}


float xrandom (float min, float max) {
	return (float)rand () / RAND_MAX * (max - min) + min; 
}

float frandom () {
	return (float)rand () / RAND_MAX; 
}

int irandom (int min, int max) {
	return (int)xrandom ((float)min, (float)max + 0.999999);
}












