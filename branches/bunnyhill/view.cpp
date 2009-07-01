// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
//
// --------------------------------------------------------------------

#include "view.h"
#include "physics.h"
#include "course.h"

TuxView View; 

// --------------------------------------------------------------------
//					settings
// --------------------------------------------------------------------
// for view vector:
#define CAMERA_DISTANCE 4.0			/*4.0*/
#define CAMERA_ANGLE 10				/*10*/
#define PLAYER_ANGLE 15				/*20*/

#define MIN_CAMERA_HEIGHT 1.5		/*1.5*/
#define ABS_MIN_CAMERA_HEIGHT 0.3	/*0.3*/
#define MAX_CAMERA_PITCH 40			/*40*/
#define TIME_CONSTANT 0.06			/*0.06*/
#define MAX_INTERPOLATION 0.3		/*0.3*/
#define INTERP_SPEED 4.5			/*4.5*/
#define NO_INTERP_SPEED 2.0			/*2.0*/

// --------------------------------------------------------------------
//					view frustum
// --------------------------------------------------------------------

void TuxView::Init () {
	// Basis-Planes for view frustum:
/*    float aspect = (float) cfg.scrwidth / cfg.scrheight;
	float near_dist = cfg.nearclip;	
	float far_dist  = cfg.forwardclip;	
 	float fieldofview = cfg.fov;		
    float hfov = ANGLES_TO_RADIANS (fieldofview * 0.5);
    float horiz = atan (tan (hfov) * aspect); 
    basplanes[0] = MakePlane (0, 0, 1, near_dist);			// near
    basplanes[1] = MakePlane (0, 0, -1, -far_dist);			// far
    basplanes[2] = MakePlane (-cos(horiz),0, sin(horiz),0);	// left
    basplanes[3] = MakePlane (cos(horiz),0, sin(horiz),0);	// right
    basplanes[4] = MakePlane (0,  cos(hfov), sin(hfov), 0);	// top
    basplanes[5] = MakePlane (0, -cos(hfov), sin(hfov), 0);	// bottom*/
}


void TuxView::SetupFrustum (TControl *ctrl) {
    int i;
    TVector3 pt, vt;

	for (i=0; i<6; i++) {
		vt = ScaleVec (-basplanes[i].d, basplanes[i].nml);
		pt = TransformPoint (ctrl->viewmat, vt);
		fplanes[i].nml = TransformVec (ctrl->viewmat, basplanes[i].nml);
		fplanes[i].d = -DotVec (fplanes[i].nml, pt);
    }

    for (i=0; i<6; i++) {
		pvc[i] = 0;
		if  (fplanes[i].nml.x > 0) pvc[i] |= 4;
		if  (fplanes[i].nml.y > 0) pvc[i] |= 2;
		if  (fplanes[i].nml.z > 0) pvc[i] |= 1;
    }
}

void TuxView::setup_view_frustum (TControl *ctrl) {
    float aspect = (float) cfg.scrwidth / cfg.scrheight;

    int i;
    TVector3 pt;
    TVector3 origin = NullVec;
    float half_fov = ANGLES_TO_RADIANS (cfg.fov * 0.5);
    float half_fov_horiz = atan (tan (half_fov) * aspect); 

    fplanes[0] = MakePlane (0, 0, 1, 0.1); // 0.1 = neardist
    fplanes[1] = MakePlane (0, 0, -1, -cfg.forwardclip);
    fplanes[2] = MakePlane (-cos (half_fov_horiz), 0, sin (half_fov_horiz), 0);
    fplanes[3] = MakePlane (cos (half_fov_horiz), 0, sin (half_fov_horiz), 0);
    fplanes[4] = MakePlane (0, cos (half_fov), sin (half_fov), 0);
    fplanes[5] = MakePlane (0, -cos (half_fov), sin (half_fov), 0);
    
	for (i=0; i<6; i++) {
		pt = TransformPoint (ctrl->viewmat, 
	    	AddVec (origin, ScaleVec (-fplanes[i].d, fplanes[i].nml)));
		fplanes[i].nml = TransformVec (ctrl->viewmat, fplanes[i].nml);
		fplanes[i].d = -DotVec (fplanes[i].nml, SubVec (pt, origin));
    }

    for (i=0; i<6; i++) {
		pvc[i] = 0;
		if  (fplanes[i].nml.x > 0) pvc[i] |= 4;
		if  (fplanes[i].nml.y > 0) pvc[i] |= 2;
		if  (fplanes[i].nml.z > 0) pvc[i] |= 1;
    }
}


TClipRes TuxView::ClipToFrustum (TVector3 min, TVector3 max) {
    TVector3 fmax, fmin;
	int is = 0;
	
    for (int i=0; i<4; i++) {
		fmin = min; 
		fmax = max; 
		if  (pvc[i] & 4) { fmin.x = max.x; fmax.x = min.x; }
 		if  (pvc[i] & 2) { fmin.y = max.y; fmax.y = min.y; }
		if  (pvc[i] & 1) { fmin.z = max.z; fmax.z = min.z; }

		if (DotVec (fmax, fplanes[i].nml) + fplanes[i].d > 0) return OUTSIDE;
		if (DotVec (fmin, fplanes[i].nml) + fplanes[i].d > 0) is++;
    }
    if (is > 0) return INTERSECT;
    return INSIDE;
}

TClipRes TuxView::clip_aabb_to_view_frustum (TVector3 min, TVector3 max) {
    TVector3 n, p;
    int i;
    bool intersect = false;

    for (i=0; i<6; i++) {
	p = MakeVec (min.x, min.y, min.z);
	n = MakeVec (max.x, max.y, max.z);

		if  (pvc[i] & 4) {
		    p.x = max.x;
	    	n.x = min.x;
		}

		if  (pvc[i] & 2) {
		    p.y = max.y;
	    	n.y = min.y;
		}
	
		if  (pvc[i] & 1) {
		    p.z = max.z;
	    	n.z = min.z;
		}

		if (DotVec (n, fplanes[i].nml) + fplanes[i].d > 0){
		    return OUTSIDE;
		}

		if (DotVec (p, fplanes[i].nml) + fplanes[i].d > 0){
	    	intersect = true;
		}
    }
    if  (intersect) return INTERSECT;
    return INSIDE;
}

// --------------------------------------------------------------------
//					interpolations
// --------------------------------------------------------------------

TVector3 InterpolateViewPos (
			TVector3 lastpos, 
			TVector3 pos2,
			float timestep,
			float timeconst,
			TControl *ctrl)
{
    static TVector3 y_vec = {0.0, 1.0, 0.0};
    TMatrix rot_mat;

    float alpha = MIN (MAX_INTERPOLATION, 1.0 - exp (-timestep / timeconst));

    TVector3 vec1 = SubVec (ctrl->viewpos, lastpos);
    NormVec (vec1);
    TVector3 vec2 = SubVec (pos2, ctrl->pos);
    NormVec (vec2);

    TQuaternion q1 = MakeRotQuat (y_vec, vec1);
    TQuaternion q2 = MakeRotQuat (y_vec, vec2);
    q2 = InterpolateQuat (q1, q2, alpha);
    vec2 = RotateVec (q2, y_vec);

    float theta = INVRADIANS (M_PI / 2 - acos (DotVec (vec2, y_vec)));
    if (theta > MAX_CAMERA_PITCH) {
		TVector3 axis = CrossVec (y_vec, vec2);
		NormVec (axis);
		RotateVecMat (rot_mat, axis, theta - MAX_CAMERA_PITCH);
		vec2 = TransformVec (rot_mat, vec2);
    }

    return AddVec (ctrl->pos, ScaleVec (CAMERA_DISTANCE, vec2));
}

// ------------------------ View Frame --------------------------------

void InterpolateViewFrame (TVector3 &dir, float timestep, 
			float timeconst, TControl *ctrl) {
    TMatrix mat1, mat2;

    float alpha = MIN (MAX_INTERPOLATION, 1.0 - exp (-timestep / timeconst));
	TVector3 upvec = MakeVec (0, 1, 0);

    TVector3 z1 = ScaleVec (-1.0, ctrl->lastdir);
    NormVec (z1);
    TVector3 y1 = ProjectToPlane (z1, upvec);
    NormVec (y1);
    TVector3 x1 = CrossVec (y1, z1);
    MakeBasisMat (mat1, x1, y1, z1);
    TQuaternion q1 = MatToQuat (mat1);

    TVector3 z2 = ScaleVec (-1.0, dir);
    NormVec (z2);
    TVector3 y2 = ProjectToPlane (z2, upvec);
    NormVec (y2);
    TVector3 x2 = CrossVec (y2, z2);
    MakeBasisMat (mat2, x2, y2, z2);
    TQuaternion q2 = MatToQuat (mat2);

    q2 = InterpolateQuat (q1, q2, alpha);
    QuatToMat (mat2, q2);

    dir.x = -mat2[2][0];
    dir.y = -mat2[2][1];
    dir.z = -mat2[2][2];
}

// --------------------------------------------------------------------
//					Class TuxView
// --------------------------------------------------------------------

TuxView::TuxView () {}
TuxView::~TuxView () {}

void TuxView::SetupViewMatrix (TControl *ctrl) {
    TVector3 vx, vy, vz;
    TMatrix vmat;
    TVector3 viewpt_in_view_frame;
	
    vz = ScaleVec (-1, ctrl->lastdir);
    vx = CrossVec (MakeVec (0, 1, 0), vz);
    vy = CrossVec (vz, vx);
    NormVec (vz);
    NormVec (vx);
    NormVec (vy);

    MakeIdentityMat (ctrl->viewmat);

    ctrl->viewmat[0][0] = vx.x;
    ctrl->viewmat[0][1] = vx.y;
    ctrl->viewmat[0][2] = vx.z;

    ctrl->viewmat[1][0] = vy.x;
    ctrl->viewmat[1][1] = vy.y;
    ctrl->viewmat[1][2] = vy.z;

    ctrl->viewmat[2][0] = vz.x;
    ctrl->viewmat[2][1] = vz.y;
    ctrl->viewmat[2][2] = vz.z;
    ctrl->viewmat[3][0] = ctrl->viewpos.x;
    ctrl->viewmat[3][1] = ctrl->viewpos.y;
    ctrl->viewmat[3][2] = ctrl->viewpos.z;
    ctrl->viewmat[3][3] = 1;
    
    TransposeMat (ctrl->viewmat, vmat);

    vmat[0][3] = 0;
    vmat[1][3] = 0;
    vmat[2][3] = 0;
    
    viewpt_in_view_frame = TransformPoint (vmat, ctrl->viewpos);
    
    vmat[3][0] = -viewpt_in_view_frame.x;
    vmat[3][1] = -viewpt_in_view_frame.y;
    vmat[3][2] = -viewpt_in_view_frame.z;
    
    glLoadIdentity();
	glMultMatrixf ((float*) vmat);
}

// -------------------------- MakeViewVector --------------------------

TVector3 MakeViewVector () {
	TVector3 res;
 	float angle = Course.angle - CAMERA_ANGLE + PLAYER_ANGLE;
	res = MakeVec (0, sin (RADIANS (angle)), cos (RADIANS (angle)));
	res = ScaleVec (CAMERA_DISTANCE, res);
	return res;
}

void TuxView::RacingView (float timestep, TControl *ctrl) {
	TVector3 viewpoint;
    TVector3 viewdir;
    float ycoord;
    TVector3 axis;
    TMatrix rotmat;
    TVector3 y_vec;
    TVector3 mz_vec;
    TVector3 vel_proj;
    TQuaternion rot_quat;
	int i;
//**---------------------int pl = 0;

    TVector3 veldir = ctrl->vel;
    float speed = NormVec (veldir);
	float denom = (speed - NO_INTERP_SPEED) / (INTERP_SPEED - NO_INTERP_SPEED);
	if (denom>1) denom = 1;
	if (denom<0.1) denom = 0.1;
    float pos_const = TIME_CONSTANT / denom;
	float frame_const = TIME_CONSTANT;	

	TVector3 viewvec = MakeViewVector ();

	if (viewmode == ABOVE) {
		viewpoint = AddVec (ctrl->pos, viewvec);
        ycoord = Course.GetYCoord (viewpoint);
        if  (viewpoint.y < ycoord + MIN_CAMERA_HEIGHT) 
            viewpoint.y = ycoord + MIN_CAMERA_HEIGHT;

		viewvec = SubVec (viewpoint, ctrl->pos);
		MakeRotMat (rotmat, PLAYER_ANGLE, 'x');
		viewdir = ScaleVec (-1.0, TransformVec (rotmat, viewvec));

	} else { 
		y_vec = MakeVec (0, 1, 0);
		mz_vec = MakeVec (0, 0, -1);
		vel_proj = ProjectToPlane (y_vec, veldir);
		NormVec (vel_proj);
		rot_quat = MakeRotQuat (mz_vec, vel_proj);
		viewvec = RotateVec (rot_quat, viewvec);
		viewpoint = AddVec (ctrl->pos, viewvec);
	    ycoord = Course.GetYCoord (viewpoint);
    	if (viewpoint.y < ycoord + MIN_CAMERA_HEIGHT) viewpoint.y = ycoord + MIN_CAMERA_HEIGHT;

		TVector3 refpoint;
		if (viewmode == BEHIND) refpoint = ctrl->pos;
		else refpoint = ctrl->lastpos;

		if  (initialized) {
	    	for  (i=0; i<2; i++) {
				viewpoint = InterpolateViewPos (
					refpoint, 
					viewpoint, 
					timestep, pos_const, ctrl);
	  		}
		}

		ycoord = Course.GetYCoord (viewpoint);
		if (viewpoint.y < ycoord + ABS_MIN_CAMERA_HEIGHT) 
			viewpoint.y = ycoord + ABS_MIN_CAMERA_HEIGHT;

		viewvec = SubVec (viewpoint, ctrl->pos);
		axis = CrossVec (y_vec, viewvec);
		NormVec (axis);
		RotateVecMat (rotmat, axis, PLAYER_ANGLE);
		viewdir = ScaleVec (-1.0, TransformVec (rotmat, viewvec));

		if  (initialized) {
			for  (i=0; i<2; i++) {
				InterpolateViewFrame (
					viewdir, 
					timestep, frame_const, ctrl);
 			}
		}
	}

    ctrl->lastdir = viewdir;
    ctrl->lastpos = ctrl->pos;
	ctrl->viewpos = viewpoint;
    initialized = true;
	SetupViewMatrix (ctrl);
	setup_view_frustum (ctrl);
}

// --------------------------------------------------------------------
//					special views
// --------------------------------------------------------------------

void TuxView::FinishView (TControl *ctrl) {
	SetupViewMatrix (ctrl);
	setup_view_frustum (ctrl);
}

void TuxView::StartView (TVector3 refpos, TControl *ctrl) {
	ctrl->viewpos = refpos;
	ctrl->viewpos.y += 2.4;
	ctrl->viewpos.z = refpos.z + 3.5 ;

	ctrl->lastdir = MakeVec (0, -1.0757, -4.0779);
    initialized = true;

	SetupViewMatrix (ctrl);
	setup_view_frustum (ctrl);
}





