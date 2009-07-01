// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "particles.h"
#include "course.h"
#include "ogl.h"
#include "textures.h"
#include "env.h"
#include "view.h"
#include "player.h"

// --------------------------------------------------------------------
//					settings
// --------------------------------------------------------------------

// brake particles, initial constants
const float BR_MINSIZE = 0.04;
const float BR_MAXSIZE = 0.12;
const float BR_XVARIANCE = 0.5;
const float BR_ZVARIANCE = 0.5;
const float BR_MINY = 0.5;
const float BR_MAXY = 1.5;
const float BR_SPEED_Y = 0.1;

// brake particles, update constants
const float BR_VEL = 2.0;
const float BR_GRAV = 0.5;
const float BR_ZCOMP = 0.01;
const float BR_SIZEREDUCT = 0.95;

const float SNOW_WIND_DRIFT = 0.2;

// --------------------------------------------------------------------
// 					class CParticles (common)
// --------------------------------------------------------------------

CParticles Particles;

CParticles::CParticles () {};
CParticles::~CParticles () {};

void CParticles::Init () {
	if (!cfg.draw_particles) return;

	numGroups = NUM_PARTICLE_GROUPS;
	numHalfgroupParticles = PARTICLES_PER_GROUP;
	numGroupParticles = PARTICLES_PER_GROUP * 2;

	TexCoords[0].lb = MakeVec2 (0.0, 0.875);
	TexCoords[0].tr = MakeVec2 (0.125, 1.0);
	TexCoords[1].lb = MakeVec2 (0.125, 0.875);
	TexCoords[1].tr = MakeVec2 (0.25, 1.0);
	TexCoords[2].lb = MakeVec2 (0.0, 0.75);
	TexCoords[2].tr = MakeVec2 (0.125, 0.875);
	TexCoords[3].lb = MakeVec2 (0.125, 0.75);
	TexCoords[3].tr = MakeVec2 (0.25, 0.875);

	for (int i=0; i<numGroups; i++) PartRegister[i] = -1;
	part_ptr = 0;
}


// ----------------------- DrawBillboard ------------------------------

void CParticles::DrawBillboard 
	(TVector3 pos, float size, TPartCoord tex, bool world_axis, TControl *ctrl) {
    TVector3 pt, xvec, yvec, zvec;

	xvec.x = ctrl->viewmat[0][0];
	xvec.y = ctrl->viewmat[0][1];
	xvec.z = ctrl->viewmat[0][2];

    if  (world_axis) {
		yvec = MakeVec (0, 1, 0);
		xvec = ProjectToPlane (yvec, xvec);
		NormVec (xvec);
		zvec = CrossVec (xvec, yvec);
    } else {
		yvec.x = ctrl->viewmat[1][0];
		yvec.y = ctrl->viewmat[1][1];
		yvec.z = ctrl->viewmat[1][2];
		zvec.x = ctrl->viewmat[2][0];
		zvec.y = ctrl->viewmat[2][1];
		zvec.z = ctrl->viewmat[2][2];
    }

    glBegin (GL_QUADS);
		pt = AddVec (pos, ScaleVec (-size / 2.0, xvec));
		pt = AddVec (pt, ScaleVec (-size / 2.0, yvec));

		glNormal3f (zvec.x, zvec.y, zvec.z);
		glTexCoord2f (tex.lb.x, tex.lb.y); 
		glVertex3f (pt.x, pt.y, pt.z);
	
		pt = AddVec (pt, ScaleVec (size, xvec));
		glTexCoord2f (tex.tr.x, tex.lb.y); 
		glVertex3f (pt.x, pt.y, pt.z);
	
		pt = AddVec (pt, ScaleVec (size, yvec));
		glTexCoord2f (tex.tr.x, tex.tr.y); 
		glVertex3f (pt.x, pt.y, pt.z);
	
		pt = AddVec (pt, ScaleVec (-size, xvec));
		glTexCoord2f (tex.lb.x, tex.tr.y); 
		glVertex3f (pt.x, pt.y, pt.z);
	glEnd();
}

// --------------------- DrawParticle ---------------------------------

void CParticles::DrawParticle (TVector3 pos, float size, TPartCoord tex) {
	glPushMatrix ();
	glTranslatef (pos.x, pos.y, pos.z);
	glBegin (GL_QUADS);
		glTexCoord2f (tex.lb.x, tex.lb.y); glVertex3f (0, 0, 0);
		glTexCoord2f (tex.tr.x, tex.lb.y); glVertex3f (size, 0, 0);
		glTexCoord2f (tex.tr.x, tex.tr.y); glVertex3f (size, size, 0);
		glTexCoord2f (tex.lb.x, tex.tr.y); glVertex3f (0, size, 0);
	glEnd();
	glPopMatrix ();
}

// --------------------------------------------------------------------
//				brake particles (if braking or turning)
// --------------------------------------------------------------------

void CParticles::DrawBrakeParticles (TControl *ctrl) {
	TVector3 pt;
	float size;
	TPartCoord tex;
	int start, i, j, group, idx;

	if (!cfg.draw_particles) return;

	SetGLOptions (PARTICLES);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	BindCommonTex (T_WIDGETS);
	glPushMatrix ();
	

	glColor4fv (Env.GetGLParticleColor ());

	for (i=0; i<numGroups; i++) {
		group = PartRegister[i];		
		if (group >= 0) {
			start = numGroupParticles * group;
			for (j=0; j<numGroupParticles; j++) {
				idx = start + j;
				if (BrakeParticles[idx].visible) {
					pt = BrakeParticles[idx].pos; 
					size = BrakeParticles[idx].size;
					tex = TexCoords[BrakeParticles[idx].type];
					DrawBillboard (pt, size, tex, false, ctrl);
					// a faster but less nice alternative:
					// DrawParticle (pt, size, tex); 
				}
			}
		}
	}
	glPopMatrix ();
}

void CParticles::UpdateBrakeParticles (float timestep, TControl *ctrl) {
	int start, i, j, group, idx;
	TVector3 diff;

	if (!cfg.draw_particles) return;

	for (i=0; i<numGroups; i++) {
		group = PartRegister[i];		
		if (group >= 0) {
			start = numGroupParticles * group;
			for (j=0; j<numGroupParticles; j++) {
				idx = start + j;
				if (BrakeParticles[idx].visible) {
					if (ctrl->final_state) {
						diff = ScaleVec (timestep * BR_VEL * 0.8, BrakeParticles[idx].vel);
						BrakeParticles[idx].pos = AddVec (BrakeParticles[idx].pos, diff);
						BrakeParticles[idx].vel.z -= BR_ZCOMP * ctrl->nmlspeed;
						BrakeParticles[idx].vel.y -= BR_GRAV * 1.6;
						BrakeParticles[idx].size *= BR_SIZEREDUCT;
					} else {			
						diff = ScaleVec (timestep * BR_VEL, BrakeParticles[idx].vel);
						BrakeParticles[idx].pos = AddVec (BrakeParticles[idx].pos, diff);
						BrakeParticles[idx].vel.z -= BR_ZCOMP * ctrl->nmlspeed;
						BrakeParticles[idx].vel.y -= BR_GRAV;
						BrakeParticles[idx].size *= BR_SIZEREDUCT;
					}
				}
			}
		}
	}
}

void CParticles::NewBrakeParticles (float speed, TVector3 pos, TVector3 vel, int num, 
		bool right, TControl *ctrl) {
	int i, idx, start;
	TVector3 vv;
	
	if (num > numHalfgroupParticles) num = numHalfgroupParticles;
	start = part_ptr * numGroupParticles;
	if (right) start += numHalfgroupParticles;

	for (i=0; i<num; i++) {
		idx = start + i; 
		BrakeParticles[idx].pos = pos;
		BrakeParticles[idx].type = irandom (0, 3);
		BrakeParticles[idx].size = xrandom (BR_MINSIZE, BR_MAXSIZE);
		BrakeParticles[idx].visible = true;

		vv = vel;
		if (!ctrl->final_state) {
			vv.x += xrandom (-BR_XVARIANCE, BR_XVARIANCE);
			vv.y += xrandom (BR_MINY, BR_MAXY) + speed * BR_SPEED_Y;
			vv.z += xrandom (-BR_ZVARIANCE, BR_ZVARIANCE);
		} else {
			vv.x += xrandom (-BR_XVARIANCE, BR_XVARIANCE);
			vv.y += xrandom (BR_MINY, BR_MAXY) + 0.8 + speed * BR_SPEED_Y;
			vv.z += xrandom (-BR_ZVARIANCE, BR_ZVARIANCE) - 1.5;
		}		
		BrakeParticles[idx].vel = vv;
	}
}

void CParticles::GenerateBrakeParticles (float timestep, TControl *ctrl) {
	TVector3 xvec, rpos, lpos, rvel, lvel;
	TMatrix rot_mat;
	int num_parts; 
	int left_parts;
	int right_parts;

	if (!cfg.draw_particles) return;

	// ................................................................
	for (int i=numGroups-1; i>0; i--) PartRegister[i] = PartRegister[i-1];
	PartRegister[0] = -1;
	// ................................................................

	int turning = 0;
	if (ctrl->turn < -0.5) turning = 1;
	else if (ctrl->turn > 0.5) turning = 2;

	// return if no particles have to be generated
	if (ctrl->airborne) return;
	if (!ctrl->brake && turning==0) return;
	if (!Course.ParticleTerrain (ctrl->pos)) return;

	// -------------------- generate new particles --------------------

	xvec = CrossVec (ctrl->dir, ctrl->surf_nml);
	float angle = MAX (20, MIN (40, ctrl->nmlspeed * 1.5));

	// calculate the position of the wings
	rpos = AddVec (ctrl->pos, ScaleVec (adj.radius * 1, xvec));
	lpos = AddVec (ctrl->pos, ScaleVec (-adj.radius * 1, xvec));

	// and now the exact position where the wings touch the surface		
	rpos.y -= adj.radius;
	lpos.y -= adj.radius;
		// perhaps a more exact alternative:
		// float surf_y = Course.GetYCoord (ctrl[pl].pos);
		// rpos.y = lpos.y = surf_y;
	if (turning > 0) {
		rpos.x -= 0.1;
		lpos.x += 0.1;
	}
	rpos.z += 0.3;
	lpos.z += 0.3;


	// calculate the number of particles
	left_parts = right_parts = 0;
	num_parts = MAX (2, MIN (6, (int)ctrl->nmlspeed/4));
	if (!ctrl->brake) {
		if (turning == 1) left_parts = num_parts;
		else right_parts = num_parts;		
	} else {
		switch (turning) {
			case 0: left_parts = right_parts = num_parts; break;
			case 1: left_parts = num_parts + 2; break;
			case 2: right_parts = num_parts + 2; break;
		}
	}

	// after RotateVecMat lvel and rvel are still normalized
	RotateVecMat (rot_mat, ctrl->dir, -angle);
	lvel = TransformVec (rot_mat, ctrl->surf_nml);
	RotateVecMat (rot_mat, ctrl->dir, angle); 
	rvel = TransformVec (rot_mat, ctrl->surf_nml);

	num_parts = 3;
	if (ctrl->final_state) num_parts = 8;

	int start = part_ptr * numGroupParticles;
	for (int i=0; i<numGroupParticles; i++) BrakeParticles[start+i].visible = false;
	if (left_parts>0) NewBrakeParticles 
			(ctrl->nmlspeed, lpos, lvel, left_parts, false, ctrl);
	if (right_parts>0) NewBrakeParticles 
			(ctrl->nmlspeed, rpos, rvel, right_parts, true, ctrl);

	// ................................................................
 	PartRegister[0] = part_ptr;
	part_ptr ++;
	if (part_ptr >= numGroups) part_ptr = 0;
	// ................................................................
}

// ====================================================================
//							snow on the course
// 
// there are 2 global functions:
//
// Init - call this function in the init function of the start mode
// after the snow_id has been selected
//
// Update - call this function in the related loop functions
// ====================================================================

CSnow Snow; // extern declared

CSnow::CSnow () {
	snowing = false;
}

CSnow::~CSnow () {}

void CSnow::WriteParams (float *bl, float near, float middle, float far) {
	bl[0] = near; bl[1] = middle; bl[2] = far;
} 

void CSnow::SetParams (int grade) {
	if (grade == 0) {	
		params.numParticles[0] = 300;
		params.numParticles[1] = 300;
		params.numParticles[2] = 500;
		WriteParams (params.xrange, 16, 40, 60);
		WriteParams (params.ytop, 3.5, 6, 12);
		WriteParams (params.yrange, 4, 15, 40);
		WriteParams (params.zback, -2, 6, 26);
		WriteParams (params.zrange, 8, 20, 60);
		WriteParams (params.minSize, 0.02, 0.05, 0.3);
		WriteParams (params.maxSize, 0.04, 0.1, 0.5);
		WriteParams (params.speed, 8, 6, 6);
		WriteParams (params.ydrift, 0.6, 0.6, 0.6);
		WriteParams (params.zdrift, 0.6, 0.6, 0.6);

	} else if (grade == 1) {	
		params.numParticles[0] = 400;
		params.numParticles[1] = 600;
		params.numParticles[2] = 0;
		WriteParams (params.xrange, 16, 40, 120);
		WriteParams (params.ytop, 3.5, 6, 12);
		WriteParams (params.yrange, 4, 15, 40);
		WriteParams (params.zback, -2, 6, 26);
		WriteParams (params.zrange, 8, 20, 60);
		WriteParams (params.minSize, 0.04, 0.15, 0.35);
		WriteParams (params.maxSize, 0.08, 0.3, 0.7);
		WriteParams (params.speed, 10, 7, 7);
		WriteParams (params.ydrift, 0.6, 0.6, 0.6);
		WriteParams (params.zdrift, 0.6, 0.6, 0.6);
	} else {
		params.numParticles[0] = 500;
		params.numParticles[1] = 500;
		params.numParticles[2] = 0;
		WriteParams (params.xrange, 16, 40, 60);
		WriteParams (params.ytop, 3.5, 6, 12);
		WriteParams (params.yrange, 4, 12, 40);
		WriteParams (params.zback, -2, 6, 26);
		WriteParams (params.zrange, 8, 20, 60);

		WriteParams (params.minSize, 0.06, 0.3, 0.6);
		WriteParams (params.maxSize, 0.12, 0.6, 1.2);
		WriteParams (params.speed, 12, 8, 8);
		WriteParams (params.ydrift, 0.6, 0.6, 0.6);
		WriteParams (params.zdrift, 0.6, 0.6, 0.6);
	}
}

void CSnow::MakeSnowParticle (int ar, int i) {
    float p_dist;
    int type;
	PartArr[ar][i].pt.x = xrandom (area[ar].left, area[ar].right);  
	PartArr[ar][i].pt.y = -xrandom (area[ar].top, area[ar].bottom);
	PartArr[ar][i].pt.z = area[ar].back - frandom () * (area[ar].back - area[ar].front);

	p_dist = frandom ();
    PartArr[ar][i].size = xrandom (params.minSize[ar], params.maxSize[ar]);
	PartArr[ar][i].vel.x = 0;
	PartArr[ar][i].vel.z = 0;
	PartArr[ar][i].vel.y = -PartArr[ar][i].size * params.speed[ar];	
    
	type = (int) (frandom () * 3.9999);

/*		PartArr[ar][i].tex_min = MakeVec2  (0.0, 0.75);
		PartArr[ar][i].tex_max = MakeVec2  (0.25, 1.0);*/

/*		PartArr[ar][i].tex_min = MakeVec2  (0, 0);
		PartArr[ar][i].tex_max = MakeVec2  (1, 1);*/

	// the texture coords assume the texture as a part of a 
	// larger texture
	if (type == 0) {
		PartArr[ar][i].tex_min = MakeVec2  (0.0, 0.875);
		PartArr[ar][i].tex_max = MakeVec2  (0.125, 1.0);
    } else if (type == 1) {
		PartArr[ar][i].tex_min = MakeVec2  (0.125, 0.875);
		PartArr[ar][i].tex_max = MakeVec2  (0.25, 1.0);
	} else if (type == 2) {
		PartArr[ar][i].tex_min = MakeVec2  (0.0, 0.75);
		PartArr[ar][i].tex_max = MakeVec2  (0.125, 0.875);
    } else {
		PartArr[ar][i].tex_min = MakeVec2  (0.125, 0.75);
		PartArr[ar][i].tex_max = MakeVec2  (0.25, 0.875);
    }
}

void CSnow::UpdateAreas (TControl *ctrl) {
	for (int ar=0; ar<3; ar++) {
		area[ar].left = ctrl->pos.x - params.xrange[ar] / 2;
		area[ar].right = area[ar].left + params.xrange[ar];
		area[ar].back = ctrl->pos.z - params.zback[ar];
		area[ar].front = area[ar].back - params.zrange[ar];
		area[ar].top = ctrl->pos.y + params.ytop[ar];
		area[ar].bottom = area[ar].top - params.yrange[ar];	
	}
}

void CSnow::GenerateSnow (TControl *ctrl) {
	if (!snowing) return;
	UpdateAreas (ctrl);	
	snow_lastpos = ctrl->pos;
	for (int ar=0; ar<3; ar++) {		
		for (int i=0; i<params.numParticles[ar]; i++) MakeSnowParticle (ar, i);
	}
}

void CSnow::Draw (int ar, bool rotate, TControl *ctrl) {
    TVector2 *tex_min, *tex_max;
    TVector3 *pt;
	float size;
    int i;
	TPlane lp = View.GetLeftPlane ();
	TPlane rp = View.GetRightPlane ();
	float dir_angle (atan (ctrl->vel.x / ctrl->vel.z) * 180 / 3.14159);

	SetGLOptions (PARTICLES);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	BindCommonTex (T_WIDGETS);  
	glColor4fv (Env.GetGLParticleColor ());

	for  (i=0; i < params.numParticles[ar]; i++) {
	    pt = &PartArr[ar][i].pt;
	    size = PartArr[ar][i].size;
	    tex_min = &PartArr[ar][i].tex_min;
	    tex_max = &PartArr[ar][i].tex_max;

		if ((DistToPlane (lp, *pt) < 0) && (DistToPlane (rp, *pt) < 0)) {
			glPushMatrix();
			glTranslatef (pt->x, pt->y, pt->z);
			if (rotate) glRotatef (dir_angle, 0, 1, 0);
			glBegin (GL_QUADS);
				glTexCoord2f (tex_min->x, tex_min->y);
				glVertex3f (0, 0, 0);
				glTexCoord2f (tex_max->x, tex_min->y);
				glVertex3f (size, 0, 0);
				glTexCoord2f (tex_max->x, tex_max->y);
				glVertex3f (size, size, 0);
				glTexCoord2f (tex_min->x, tex_max->y);
				glVertex3f (0, size, 0);
			glEnd();
			glPopMatrix();
		}
	} 
} 

void CSnow::Update (float timestep, TControl *ctrl) {
    int i;
	float ydiff, zdiff;
		
	if (!snowing) return;
	UpdateAreas (ctrl);

	zdiff = ctrl->pos.z - snow_lastpos.z;
	if (game.mode != FINISH) {
		ydiff = ctrl->pos.y - snow_lastpos.y;
	} else ydiff = 0; 

	TVector3 winddrift = ScaleVec (SNOW_WIND_DRIFT, Wind.WindDrift ());
	float xcoeff = winddrift.x * timestep;
	float ycoeff = (ydiff * params.ydrift[1]) + (winddrift.z * 0.025);	
	float zcoeff = (winddrift.z * timestep) + (zdiff * params.zdrift[1]);

	for (int ar=0; ar<3; ar++) {
		for (i=0; i < params.numParticles[ar]; i++) {
			PartArr[ar][i].pt.x += xcoeff;
			PartArr[ar][i].pt.y += PartArr[ar][i].vel.y * timestep + ycoeff;
			PartArr[ar][i].pt.z += zcoeff;

			if (PartArr[ar][i].pt.y < area[ar].bottom) {
				PartArr[ar][i].pt.y += params.yrange[ar];	
			} else if (PartArr[ar][i].pt.x < area[ar].left) {
				PartArr[ar][i].pt.x += params.xrange[ar];
			} else if (PartArr[ar][i].pt.x > area[ar].right) {
				PartArr[ar][i].pt.x -= params.xrange[ar];
			} else if (PartArr[ar][i].pt.y > area[ar].top) {
				PartArr[ar][i].pt.y -= params.yrange[ar];			
			} else if (PartArr[ar][i].pt.z < area[ar].front) {
				PartArr[ar][i].pt.z += params.zrange[ar];		
			} else if (PartArr[ar][i].pt.z > area[ar].back) {
				PartArr[ar][i].pt.z -= params.zrange[ar];		
			} 
		}		
		if (ar == 0) Draw (ar, true, ctrl);
		else Draw (ar, false, ctrl);
	}
	snow_lastpos = ctrl->pos;
	Curtain.Update (timestep, ctrl);
} 

void CSnow::Init (int snow_id, TControl *ctrl) {
	if (snow_id < 1 || snow_id > 3) {
		snowing = false;
		return;
	}
	snowing = true;
	SetParams (snow_id - 1);	
	GenerateSnow (ctrl);
	switch (snow_id) {
		case 1: Curtain.Init (ctrl, false, false, false); break;
		case 2: Curtain.Init (ctrl, false, true, false); break;
		case 3: Curtain.Init (ctrl, false, true, true); break;
	}
}

// --------------------------------------------------------------------
//					snow curtain
// --------------------------------------------------------------------

typedef struct {
	float min;
	float max;
	float curr;
	float step;
	bool forward;
} TChange;

TChange changes[6];

void InitChanges () {
 	for (int i=0; i<6; i++) {
		changes[i].min = xrandom (-0.15, -0.05);
		changes[i].max = xrandom (0.05, 0.15);
		changes[i].curr = (changes[i].min + changes[i].max) / 2;
		changes[i].step = 0.001;
		changes[i].forward = 1;
	}
}

void UpdateChanges () {
	TChange *ch;
	for (int i=0; i<6; i++) {
		ch = &changes[i];
		if (ch->forward) {
			ch->curr += ch->step;
			if (ch->curr > ch->max) {
				ch->forward = false;
			}
		} else {
			ch->curr -= ch->step;
			if (ch->curr < ch->min) {
				ch->forward = true;
			}
		}
	}
}

// ----------------------------------------------------------
CCurtain Curtain; // extern declared

CCurtain::CCurtain () {}

CCurtain::~CCurtain () {}

void CCurtain::CurtainVec (float angle, float zdist, float &x, float &z) {
	x = zdist  * sin (angle * 3.14159 / 180);
	if (angle > 90 || angle < -90) z = sqrt (zdist * zdist - x * x);
	else z = -sqrt (zdist * zdist - x * x);
}

void CCurtain::Draw (TControl *ctrl) {
    TVector3 *pt;
//	TPlane lp = View.GetLeftPlane ();
//	TPlane rp = View.GetRightPlane ();
//	float dir_angle (atan (ctrl->vel.x / ctrl->vel.z) * 180 / 3.14159);

	SetGLOptions (PARTICLES);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	BindCommonTex (4);  //3
	glColor4fv (Env.GetGLParticleColor ());
	glNormal3f (0, 0, -1);


	for (int i=0; i<3; i++) {
		if (enabled[i]) {	
			float halfsize = size[i] / 2;
			for (int co=0; co<numCols[i]; co++) {
				for (int row=0; row<numRows[i]; row++) {
					pt = &curtains[i][co][row].pt;
					glPushMatrix();
					glTranslatef (pt->x, pt->y, pt->z);
					glRotatef (-curtains[i][co][row].angle, 0, 1, 0);
					glBegin (GL_QUADS);
						glTexCoord2f (0, 0);
						glVertex3f (-halfsize, -halfsize, 0);
						glTexCoord2f (1, 0);
						glVertex3f (halfsize, -halfsize, 0);
						glTexCoord2f (1, 1);
						glVertex3f (halfsize, halfsize, 0);
						glTexCoord2f (0, 1);
						glVertex3f (-halfsize, halfsize, 0);
					glEnd();
					glPopMatrix();
				}
			}
		}
	}
}

void CCurtain::GenerateCurtains (TControl *ctrl) {
	InitChanges ();

	if (enabled[0]) {	
		numRows[0] = 3;
		zdist[0] = 25;
		size[0] = 15;
		speed[0] = 0.1;
		startangle[0] = -100;
		horzspeed[0] = 0.0;
		minheight[0] = -10; 
	
		angledist[0] = atan (size[0] / 2 / zdist[0]) * 360 / 3.14159;
		numCols[0] = (int)(-2 * startangle[0] / angledist[0]) + 1;
		if (numCols[0] > MAX_CURTAIN_COLS) numCols[0] = MAX_CURTAIN_COLS;
		lastangle[0] = startangle[0] + (numCols[0]-1) * angledist[0];
		for (int i=0; i<numRows[0]; i++) chg[0][i] = irandom (0, 5);		
	}
	if (enabled[1]) {	
		numRows[1] = 3;
		zdist[1] = 40;
		size[1] = 20;
		speed[1] = 0.05;
		startangle[1] = -100;
		horzspeed[1] = 0.0;
		minheight[1] = -10; 
	
		angledist[1] = atan (size[1] / 2 / zdist[1]) * 360 / 3.14159;
		numCols[1] = (int)(-2 * startangle[1] / angledist[1]) + 1;
		if (numCols[1] > MAX_CURTAIN_COLS) numCols[1] = MAX_CURTAIN_COLS;
		lastangle[1] = startangle[1] + (numCols[1]-1) * angledist[1];
		for (int i=0; i<numRows[1]; i++) chg[1][i] = irandom (0, 5);		
	}
	if (enabled[2]) {	
		numRows[2] = 3;
		zdist[2] = 60;
		size[2] = 30;
		speed[2] = 0.1;
		startangle[2] = -100;
		horzspeed[2] = 0.0;
		minheight[2] = -10; 
	
		angledist[2] = atan (size[2] / 2 / zdist[2]) * 360 / 3.14159;
		numCols[2] = (int)(-2 * startangle[2] / angledist[2]) + 1;
		if (numCols[2] > MAX_CURTAIN_COLS) numCols[2] = MAX_CURTAIN_COLS;
		lastangle[2] = startangle[2] + (numCols[2]-1) * angledist[2];
		for (int i=0; i<numRows[2]; i++) chg[2][i] = irandom (0, 5);		
	}
	float x, z;
	TCurtain *curt = 0; 

	for (int i=0; i<3; i++) {
		if (enabled[i]) {
			for (int co=0; co<numCols[i]; co++) {
				for (int row=0; row<numRows[i]; row++) {
					curt = &curtains[i][co][row];
					curt->height = minheight[i] + row * size[i];
					curt->angle = co * angledist[i] + startangle[i];
					CurtainVec (curt->angle, zdist[i], x, z);
					curt->pt.x = ctrl->pos.x + x;
					curt->pt.z = ctrl->pos.z + z;
					curt->pt.y = ctrl->pos.y + curt->height;
				}
			}
		}
	}
}

void CCurtain::Update (float timestep, TControl *ctrl) {
	float x, z;
	TCurtain *curt = 0; 
	TVector3 drift = Wind.WindDrift ();

	UpdateChanges ();
	for (int i=0; i<3; i++) {
		if (enabled[i]) {
			for (int co=0; co<numCols[i]; co++) {
				for (int row=0; row<numRows[i]; row++) {
					curt = &curtains[i][co][row];
		
					curt->angle += horzspeed[i];
					curt->angle += changes[chg[i][row]].curr;
					curt->angle += drift.x / 200;
					if (curt->angle > lastangle[i] + angledist[i]) curt->angle = startangle[i];
					if (curt->angle < startangle[i] - angledist[i]) curt->angle = lastangle[i];
		
					CurtainVec (curt->angle, zdist[i], x, z);
					curt->pt.x = ctrl->pos.x + x ;
					curt->pt.z = ctrl->pos.z + z;
					curt->height -= speed[i];
					curt->pt.y = ctrl->pos.y + curt->height;
					if (curt->height < minheight[i] - size[i]) curt->height += numRows[i] * size[i];		
				}
			}
		}
	}
	Draw (ctrl);
}

void CCurtain::Init (TControl *ctrl, bool near, bool middle, bool far) {
	enabled[0] = near;
	enabled[1] = middle;
	enabled[2] = far;
	GenerateCurtains (ctrl);
}

// --------------------------------------------------------------------
//					wind
// --------------------------------------------------------------------

#define UPDATE_TIME 0.04

CWind Wind;

CWind::CWind () {
	windy = false;
	CurrTime = 0.0;
	WVector = MakeVec (0, 0, 0);
}

CWind::~CWind () {}

void CWind::SetParams (int grade) {
	float min_base_speed = 0; 
	float max_base_speed = 0;
	float min_speed_var = 0;
	float max_speed_var = 0;
	float min_base_angle = 0; 
	float max_base_angle = 0; 
	float min_angle_var = 0;
	float max_angle_var = 0;
	float alt_angle = 0;

	if (grade == 0) {
		min_base_speed = 20; 
		max_base_speed = 35;
		min_speed_var = 20;
		max_speed_var = 20;
		params.minChange = 0.1;
		params.maxChange = 0.3;

		min_base_angle = 70; 
		max_base_angle = 110; 
		min_angle_var = 0;
		max_angle_var = 90;
		params.minAngleChange = 0.1;
		params.maxAngleChange = 1.0;

		params.topSpeed = 100;
		params.topProbability = 0;
		params.nullProbability = 6;
		alt_angle = 180;
	} else if (grade == 1) {
		min_base_speed = 30; 
		max_base_speed = 60;
		min_speed_var = 40;
		max_speed_var = 40;
		params.minChange = 0.1;
		params.maxChange = 0.5;

		min_base_angle = 70; 
		max_base_angle = 110; 
		min_angle_var = 0;
		max_angle_var = 90;
		params.minAngleChange = 0.1;
		params.maxAngleChange = 1.0;

		params.topSpeed = 100;
		params.topProbability = 0;
		params.nullProbability = 10;
		alt_angle = 180;
	} else {
		min_base_speed = 40; 
		max_base_speed = 80;
		min_speed_var = 30;
		max_speed_var = 60;
		params.minChange = 0.1;
		params.maxChange = 1.0;

		min_base_angle = 0; 
		max_base_angle = 180; 
		min_angle_var = 180;
		max_angle_var = 360;
		params.minAngleChange = 0.1;
		params.maxAngleChange = 1.0;

		params.topSpeed = 100;
		params.topProbability = 10;
		params.nullProbability = 10;
		alt_angle = 0;
	}

	float speed, var, angle;

	speed = xrandom (min_base_speed, max_base_speed);		
	var = xrandom (min_speed_var, max_speed_var) / 2;
	params.minSpeed = speed - var;
	params.maxSpeed = speed + var;
	if (params.minSpeed < 0) params.minSpeed = 0;
	if (params.maxSpeed > 100) params.maxSpeed = 100;

	angle = xrandom (min_base_angle, max_base_angle);	
	if (xrandom (0, 100) > 50) angle = angle + alt_angle;	
	var = xrandom (min_angle_var, max_angle_var) / 2;
	params.minAngle = angle - var;
	params.maxAngle = angle + var;

}


void CWind::CalcDestSpeed () {	
	float rand = xrandom (0, 100);
	if (rand > (100 - params.topProbability)) {
		DestSpeed = xrandom (params.maxSpeed, params.topSpeed);
		WindChange = params.maxChange;
	} else if (rand < params.nullProbability) {
		DestSpeed = 0.0;
		WindChange = xrandom (params.minChange, params.maxChange);
	} else {
		DestSpeed = xrandom (params.minSpeed, params.maxSpeed);
		WindChange = xrandom (params.minChange, params.maxChange);
	}

	if (DestSpeed > WSpeed) SpeedMode = 1; else SpeedMode = 0;
}

void CWind::CalcDestAngle () {
	DestAngle = xrandom (params.minAngle, params.maxAngle);
	AngleChange = xrandom (params.minAngleChange, params.maxAngleChange);

	if (DestAngle > WAngle) AngleMode = 1; else AngleMode = 0;
}
		

void CWind::Update (float timestep) {
	float xx, zz;
	
	if (!windy) return;	
	// the wind needn't be updated in each frame
	CurrTime = CurrTime + timestep;
	if (CurrTime > UPDATE_TIME) {
		CurrTime = 0.0;
		
		if (SpeedMode == 1) { // current speed lesser than destination speed
			if (WSpeed < DestSpeed) {
				WSpeed = WSpeed + WindChange;		
			} else CalcDestSpeed ();		
		} else {
			if (WSpeed > DestSpeed) {
				WSpeed = WSpeed - WindChange;		
			} else CalcDestSpeed ();							
		}	
		if (WSpeed > params.topSpeed) WSpeed = params.topSpeed;
		if (WSpeed < 0) WSpeed = 0;
	
	
		if (AngleMode == 1) {
			if (WAngle < DestAngle) {
				WAngle = WAngle + AngleChange;
			} else CalcDestAngle ();
		} else {
			if (WAngle > DestAngle) {
				WAngle = WAngle - AngleChange;
			} else CalcDestAngle ();
		}
		if (WAngle > params.maxAngle) WAngle = params.maxAngle;
		if (WAngle < params.minAngle) WAngle = params.minAngle;
			
		xx = sin (WAngle * 3.14159 / 180);
		zz = sqrt (1 - xx * xx);
		if ((WAngle > 90 && WAngle < 270) || (WAngle > 450 && WAngle < 630)) {
			zz = -zz;
		}

		WVector.x = WSpeed * xx;
		WVector.z = WSpeed * zz * 0.2;
	}
}

void CWind::Init (int wind_id) {
	if (wind_id < 1 || wind_id > 3) {
		windy = false;
		WVector = MakeVec (0, 0, 0);
		WAngle = 0;
		WSpeed = 0;
		return;
	}
	windy = true;;
	SetParams (wind_id -1);
	WSpeed = xrandom (params.minSpeed, (params.minSpeed + params.maxSpeed) / 2);
	WAngle = xrandom (params.minAngle, params.maxAngle);
	CalcDestSpeed ();
	CalcDestAngle ();
}

