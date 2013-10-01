/* --------------------------------------------------------------------
EXTREME TUXRACER

Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
Copyright (C) 2010 Extreme Tuxracer Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
---------------------------------------------------------------------*/

#ifdef HAVE_CONFIG_H
#include <etr_config.h>
#endif

#include "view.h"
#include "course.h"
#include "ogl.h"
#include "physics.h"
#include "winsys.h"

#define MIN_CAMERA_HEIGHT  1.5
#define ABSOLUTE_MIN_CAMERA_HEIGHT  0.3
#define CAMERA_ANGLE_ABOVE_SLOPE 10
#define PLAYER_ANGLE_IN_CAMERA 20
#define MAX_CAMERA_PITCH 40
#define BEHIND_ORBIT_TIME_CONSTANT 0.06
#define BEHIND_ORIENT_TIME_CONSTANT 0.06
#define FOLLOW_ORBIT_TIME_CONSTANT 0.06
#define FOLLOW_ORIENT_TIME_CONSTANT 0.06
#define MAX_INTERPOLATION_VALUE 0.3
#define BASELINE_INTERPOLATION_SPEED 4.5
#define NO_INTERPOLATION_SPEED 2.0
#define CAMERA_DISTANCE_INCREMENT 2

static TMatrix stationary_matrix;
static bool is_stationary = false;
static bool shall_stationary = false;

void SetStationaryCamera (bool stat) {
	if (stat == false) {
		is_stationary = false;
		shall_stationary = false;
	} else {
		shall_stationary = true;
	}
}

static double camera_distance = 4.0;
void IncCameraDistance (double timestep) {
	camera_distance += timestep * CAMERA_DISTANCE_INCREMENT;
}

void SetCameraDistance (double val) {camera_distance = val;}


void set_view_mode (CControl *ctrl, TViewMode mode) {ctrl->viewmode = mode;}

TVector3d interpolate_view_pos (const TVector3d& ctrl_pos1, const TVector3d& ctrl_pos2,
                                double max_vec_angle,
                                const TVector3d& pos1, const TVector3d& pos2,
                                double dist, double dt,
                                double time_constant) {
	static TVector3d y_vec(0.0, 1.0, 0.0);

	TVector3d vec1 = pos1 - ctrl_pos1;
	TVector3d vec2 = pos2 - ctrl_pos2;

	vec1.Norm();
	vec2.Norm();

	TQuaternion q1 = MakeRotationQuaternion (y_vec, vec1);
	TQuaternion q2 = MakeRotationQuaternion (y_vec, vec2);
	double alpha = min (MAX_INTERPOLATION_VALUE, 1.0 - exp  (-dt / time_constant));
	q2 = InterpolateQuaternions (q1, q2, alpha);

	vec2 = RotateVector (q2, y_vec);
	double theta = RADIANS_TO_ANGLES (M_PI/2 - acos (DotProduct (vec2, y_vec)));
	if (theta > max_vec_angle) {
		TVector3d axis = CrossProduct (y_vec, vec2);
		axis.Norm();
		TMatrix rot_mat;
		RotateAboutVectorMatrix (rot_mat, axis, theta-max_vec_angle);
		vec2 = TransformVector (rot_mat, vec2);
	}
	return ctrl_pos2 + dist * vec2;
}

void interpolate_view_frame (const TVector3d& up1, const TVector3d& dir1,
                             TVector3d *p_up2, TVector3d *p_dir2,
                             double dt, double time_constant) {
	TMatrix cob_mat1, inv_cob_mat1;
	TMatrix cob_mat2, inv_cob_mat2;

	TVector3d z1 = -1.0 * dir1;
	z1.Norm();
	TVector3d y1 = ProjectToPlane (z1, up1);
	y1.Norm();
	TVector3d x1 = CrossProduct (y1, z1);

	MakeBasismatrix_Inv (cob_mat1, inv_cob_mat1, x1, y1, z1);
	TQuaternion q1 = MakeQuaternionFromMatrix (cob_mat1);
	TVector3d z2 = -1.0 * *p_dir2;
	z2.Norm();
	TVector3d y2 = ProjectToPlane (z2, *p_up2);
	y2.Norm();
	TVector3d x2 = CrossProduct (y2, z2);

	MakeBasismatrix_Inv (cob_mat2, inv_cob_mat2, x2, y2, z2);
	TQuaternion q2 = MakeQuaternionFromMatrix (cob_mat2);
	double alpha = min (MAX_INTERPOLATION_VALUE, 1.0 - exp (-dt / time_constant));
	q2 = InterpolateQuaternions (q1, q2, alpha);
	MakeMatrixFromQuaternion (cob_mat2, q2);

	p_up2->x = cob_mat2[1][0];
	p_up2->y = cob_mat2[1][1];
	p_up2->z = cob_mat2[1][2];

	p_dir2->x = -cob_mat2[2][0];
	p_dir2->y = -cob_mat2[2][1];
	p_dir2->z = -cob_mat2[2][2];
}

void setup_view_matrix (CControl *ctrl, bool save_mat) {
	TMatrix view_mat;

	TVector3d view_z = -1.0 * ctrl->viewdir;
	TVector3d view_x = CrossProduct (ctrl->viewup, view_z);
	TVector3d view_y = CrossProduct (view_z, view_x);
	view_z.Norm();
	view_x.Norm();
	view_y.Norm();

	MakeIdentityMatrix (ctrl->view_mat);

	ctrl->view_mat[0][0] = view_x.x;
	ctrl->view_mat[0][1] = view_x.y;
	ctrl->view_mat[0][2] = view_x.z;

	ctrl->view_mat[1][0] = view_y.x;
	ctrl->view_mat[1][1] = view_y.y;
	ctrl->view_mat[1][2] = view_y.z;

	ctrl->view_mat[2][0] = view_z.x;
	ctrl->view_mat[2][1] = view_z.y;
	ctrl->view_mat[2][2] = view_z.z;

	ctrl->view_mat[3][0] = ctrl->viewpos.x;
	ctrl->view_mat[3][1] = ctrl->viewpos.y;
	ctrl->view_mat[3][2] = ctrl->viewpos.z;
	ctrl->view_mat[3][3] = 1;

	TransposeMatrix (ctrl->view_mat, view_mat);

	view_mat[0][3] = 0;
	view_mat[1][3] = 0;
	view_mat[2][3] = 0;

	TVector3d viewpt_in_view_frame = TransformPoint (view_mat, ctrl->viewpos);

	view_mat[3][0] = -viewpt_in_view_frame.x;
	view_mat[3][1] = -viewpt_in_view_frame.y;
	view_mat[3][2] = -viewpt_in_view_frame.z;

	if (save_mat) {
		memcpy(stationary_matrix, view_mat, 16*sizeof(**view_mat));
	}
	glLoadIdentity();
	glMultMatrixd ((double*) view_mat);
}

TVector3d MakeViewVector () {
	double course_angle = Course.GetCourseAngle();
	double rad = ANGLES_TO_RADIANS (
	                 course_angle -
	                 CAMERA_ANGLE_ABOVE_SLOPE +
	                 PLAYER_ANGLE_IN_CAMERA);
	TVector3d res(0, sin(rad), cos(rad));
	return camera_distance * res;
}

void update_view (CControl *ctrl, double dt) {
	if (is_stationary) {
		glLoadIdentity();
		glMultMatrixd ((double*) stationary_matrix);
		return;
	}

	TVector3d view_pt(0,0,0);
	TVector3d view_dir;
	TMatrix rot_mat;

	static const TVector3d y_vec(0.0, 1.0, 0.0);
	static const TVector3d mz_vec(0.0, 0.0, -1.0);

	double speed = ctrl->cvel.Length();
	double time_constant_mult = 1.0 /
	                            clamp (0.0,
	                                   (speed - NO_INTERPOLATION_SPEED) / (BASELINE_INTERPOLATION_SPEED - NO_INTERPOLATION_SPEED),
	                                   1.0);

	TVector3d vel_dir = ctrl->cvel;
	vel_dir.Norm();

	TVector3d view_vec = MakeViewVector();

	switch (ctrl->viewmode) {
		case BEHIND: {
			TVector3d vel_proj = ProjectToPlane (y_vec, vel_dir);
			vel_proj.Norm();
			TQuaternion rot_quat = MakeRotationQuaternion (mz_vec, vel_proj);
			view_vec = RotateVector (rot_quat, view_vec);
			view_pt = ctrl->cpos + view_vec;
			double ycoord = Course.FindYCoord (view_pt.x, view_pt.z);

			if (view_pt.y < ycoord + MIN_CAMERA_HEIGHT) {
				view_pt.y = ycoord + MIN_CAMERA_HEIGHT;
			}

			if (ctrl->view_init) {
				for (int i=0; i<2; i++) {
					view_pt = interpolate_view_pos (ctrl->cpos, ctrl->cpos,
					                                MAX_CAMERA_PITCH, ctrl->viewpos,
					                                view_pt, camera_distance, dt,
					                                BEHIND_ORBIT_TIME_CONSTANT *
					                                time_constant_mult);
				}
			}

			ycoord = Course.FindYCoord (view_pt.x, view_pt.z);
			if (view_pt.y < ycoord + ABSOLUTE_MIN_CAMERA_HEIGHT) {
				view_pt.y = ycoord + ABSOLUTE_MIN_CAMERA_HEIGHT;
			}

			view_vec = view_pt - ctrl->cpos;
			TVector3d axis = CrossProduct (y_vec, view_vec);
			axis.Norm();
			RotateAboutVectorMatrix (rot_mat, axis, PLAYER_ANGLE_IN_CAMERA);
			view_dir = -1.0 * TransformVector (rot_mat, view_vec);

			if (ctrl->view_init) {
				for (int i=0; i<2; i++) {
					TVector3d up_dir(0, 1, 0);
					interpolate_view_frame (ctrl->viewup, ctrl->viewdir,
					                        &up_dir, &view_dir, dt,
					                        BEHIND_ORIENT_TIME_CONSTANT);
				}
			}
			break;
		}

		case FOLLOW: { // normale Einstellung
			TVector3d vel_proj = ProjectToPlane (y_vec, vel_dir);
			vel_proj.Norm();
			TQuaternion rot_quat = MakeRotationQuaternion (mz_vec, vel_proj);
			view_vec = RotateVector (rot_quat, view_vec);
			view_pt = ctrl->cpos + view_vec;
			double ycoord = Course.FindYCoord (view_pt.x, view_pt.z);
			if (view_pt.y < ycoord + MIN_CAMERA_HEIGHT) {
				view_pt.y = ycoord + MIN_CAMERA_HEIGHT;
			}

			if (ctrl->view_init) {
				for (int i=0; i<2; i++) {
					view_pt = interpolate_view_pos (ctrl->plyr_pos, ctrl->cpos,
					                                MAX_CAMERA_PITCH, ctrl->viewpos,
					                                view_pt, camera_distance, dt,
					                                FOLLOW_ORBIT_TIME_CONSTANT *
					                                time_constant_mult);
				}
			}
			ycoord = Course.FindYCoord (view_pt.x, view_pt.z);
			if (view_pt.y < ycoord + ABSOLUTE_MIN_CAMERA_HEIGHT) {
				view_pt.y = ycoord + ABSOLUTE_MIN_CAMERA_HEIGHT;
			}

			view_vec = view_pt - ctrl->cpos;
			TVector3d axis = CrossProduct (y_vec, view_vec);
			axis.Norm();
			RotateAboutVectorMatrix (rot_mat, axis,
			                         PLAYER_ANGLE_IN_CAMERA);
			view_dir = -1.0 * TransformVector (rot_mat, view_vec);

			if (ctrl->view_init) {
				for (int i=0; i<2; i++) {
					TVector3d up_dir(0, 1, 0);
					interpolate_view_frame (ctrl->viewup, ctrl->viewdir,
					                        &up_dir, &view_dir, dt,
					                        FOLLOW_ORIENT_TIME_CONSTANT);
				}
			}
			break;
		}

		case ABOVE: {
			view_pt = ctrl->cpos + view_vec;
			double ycoord = Course.FindYCoord (view_pt.x, view_pt.z);
			if (view_pt.y < ycoord + MIN_CAMERA_HEIGHT) {
				view_pt.y = ycoord + MIN_CAMERA_HEIGHT;
			}

			view_vec = view_pt - ctrl->cpos;
			MakeRotationMatrix (rot_mat, PLAYER_ANGLE_IN_CAMERA, 'x');
			view_dir = -1.0 * TransformVector (rot_mat, view_vec);
			break;
		}

		default:
			Message ("code not reached");
			return;
	}


	ctrl->viewpos = view_pt;
	ctrl->viewdir = view_dir;
	ctrl->viewup = TVector3d(0, 1, 0);
	ctrl->plyr_pos = ctrl->cpos;
	ctrl->view_init = true;

	if (shall_stationary) {
		setup_view_matrix (ctrl, true);
		is_stationary = true;
	} else {
		setup_view_matrix (ctrl, false);
	}
}


// --------------------------------------------------------------------
//					viewfrustum
// --------------------------------------------------------------------

static TPlane frustum_planes[6];
static char p_vertex_code[6];


void SetupViewFrustum (const CControl *ctrl) {
	double aspect = (double) Winsys.resolution.width /Winsys.resolution.height;

	double near_dist = NEAR_CLIP_DIST;
	double far_dist = param.forward_clip_distance;
	TVector3d origin(0., 0., 0.);
	double half_fov = ANGLES_TO_RADIANS (param.fov * 0.5);
	double half_fov_horiz = atan (tan (half_fov) * aspect);

	frustum_planes[0] = TPlane(0, 0, 1, near_dist);
	frustum_planes[1] = TPlane(0, 0, -1, -far_dist);
	frustum_planes[2]
	    = TPlane(-cos(half_fov_horiz), 0, sin(half_fov_horiz), 0);
	frustum_planes[3]
	    = TPlane(cos(half_fov_horiz), 0, sin(half_fov_horiz), 0);
	frustum_planes[4]
	    = TPlane(0, cos(half_fov), sin(half_fov), 0);
	frustum_planes[5]
	    = TPlane(0, -cos(half_fov), sin(half_fov), 0);

	for (int i=0; i<6; i++) {
		TVector3d pt = TransformPoint (ctrl->view_mat,
		                               origin + -frustum_planes[i].d * frustum_planes[i].nml);

		frustum_planes[i].nml = TransformVector (
		                            ctrl->view_mat, frustum_planes[i].nml);

		frustum_planes[i].d = -DotProduct (
		                          frustum_planes[i].nml,
		                          pt - origin);
	}

	for (int i=0; i<6; i++) {
		p_vertex_code[i] = 0;

		if (frustum_planes[i].nml.x > 0) p_vertex_code[i] |= 4;
		if (frustum_planes[i].nml.y > 0) p_vertex_code[i] |= 2;
		if (frustum_planes[i].nml.z > 0) p_vertex_code[i] |= 1;
	}
}

clip_result_t clip_aabb_to_view_frustum (const TVector3d& min, const TVector3d& max) {
	bool intersect = false;

	for (int i=0; i<6; i++) {
		TVector3d p = min;
		TVector3d n = max;

		if (p_vertex_code[i] & 4) {
			p.x = max.x;
			n.x = min.x;
		}

		if (p_vertex_code[i] & 2) {
			p.y = max.y;
			n.y = min.y;
		}

		if (p_vertex_code[i] & 1) {
			p.z = max.z;
			n.z = min.z;
		}

		if (DotProduct (n, frustum_planes[i].nml) + frustum_planes[i].d > 0) {
			return NotVisible;
		}

		if (DotProduct (p, frustum_planes[i].nml) + frustum_planes[i].d > 0) {
			intersect = true;
		}
	}
	if (intersect) return SomeClip;
	return NoClip;
}

const TPlane& get_far_clip_plane() { return frustum_planes[1]; }
const TPlane& get_left_clip_plane() { return frustum_planes[2]; }
const TPlane& get_right_clip_plane() { return frustum_planes[3]; }
const TPlane& get_bottom_clip_plane() { return frustum_planes[5]; }