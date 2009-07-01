// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

// The trackmarks code has been completely rewritten to make
// the code more flexible. It's not implemented yet but principally
// it's possible to store a track in a file and reload it even after
// having restarted the program.

// todo:
// - materials can probably be optimized
// - storing and loading of tracks
// - paging with DrawPreviousTrack
// - checking the neighbour terrains 

// At the moment there are two tracks, the current track of course
// and 1 previous track. In racing_term the current track is copied
// to the previous track.

#include "trackmarks.h"
#include "ogl.h"
#include "course.h"
#include "textures.h"

// --------------------------------------------------------------------
//					settings
// --------------------------------------------------------------------

#define TRACK_WIDTH  0.7
#define TRACK_HEIGHT 0.2 // sorgt dafür, dass Track nicht unter Terrain
#define MAX_TRACK_DEPTH 0.7
#define NUM_CURRENT_MARKS 40 
#define FORWARD_TRACK_AREA 50
#define BACKWARD_TRACK_AREA 5

CTrackmarks Trackmarks;  // extern declared

CTrackmarks::CTrackmarks () {}
CTrackmarks::~CTrackmarks () {}

// --------------------------------------------------------------------
//					material
// --------------------------------------------------------------------

void CTrackmarks::SetTrackMaterial (TColor diff, TColor spec, float exp) {
	float mat_amb_diff[4];
	float mat_specular[4];

	mat_amb_diff[0] = diff.r;
	mat_amb_diff[1] = diff.g;
	mat_amb_diff[2] = diff.b;
	mat_amb_diff[3] = diff.a; 

	mat_specular[0] = spec.r;
	mat_specular[1] = spec.g;
	mat_specular[2] = spec.b;
	mat_specular[3] = spec.a;

	glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
	glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, exp);
	glColor4f (diff.r, diff.g, diff.b, diff.a);
} 

void CTrackmarks::ResetCurrentTrack () {
	mode = TRACKBREAK;
	CurrTrack.numQuads = 0;
	last_pos = NullVec;
	last_dir = MakeVec2 (0, -1);
}

void CTrackmarks::ResetLastTrack () {
	PrevTrack.numQuads = 0;
}

void CTrackmarks::DrawQuad (TTrackQuad *quad) {
	GLuint texid = 0;
    TColor trackcol = colWhite;
	TColor blendcol = colWhite;

	texid = Course.TerrTypes[quad->terr].tracktexid;
	glBindTexture (GL_TEXTURE_2D, texid);

	trackcol.a = quad->alpha;
	blendcol.a = 0;
	if ((quad->track_type == TRACK_HEAD) || (quad->track_type == TRACK_TAIL)) { 
		glBegin(GL_QUADS);
			if (quad->track_type == TRACK_HEAD) {
				SetTrackMaterial (blendcol, colBlack, 0.0);
			} else {
				SetTrackMaterial (trackcol, colBlack, 0.0);
			}
			glNormal3f (quad->n1.x, quad->n1.y, quad->n1.z);
			glTexCoord2f (quad->t1.x, quad->t1.y);
			glVertex3f (quad->v1.x, quad->v1.y, quad->v1.z);
	
			glNormal3f (quad->n2.x, quad->n2.y, quad->n2.z);
			glTexCoord2f (quad->t2.x, quad->t2.y);
			glVertex3f (quad->v2.x, quad->v2.y, quad->v2.z);

			if (quad->track_type == TRACK_HEAD) {
				SetTrackMaterial (trackcol, colBlack, 0.0);
			} else {
				SetTrackMaterial (blendcol, colBlack, 0.0);
			}
			glNormal3f (quad->n4.x, quad->n4.y, quad->n4.z);
			glTexCoord2f (quad->t4.x, quad->t4.y);
			glVertex3f (quad->v4.x, quad->v4.y, quad->v4.z);
	
			glNormal3f (quad->n3.x, quad->n3.y, quad->n3.z);
			glTexCoord2f (quad->t3.x, quad->t3.y);
			glVertex3f (quad->v3.x, quad->v3.y, quad->v3.z);
	
		glEnd();
	} else {
		SetTrackMaterial (trackcol, colBlack, 1.0);
		glBegin (GL_QUAD_STRIP);

			glNormal3f (quad->n2.x, quad->n2.y, quad->n2.z);
			glTexCoord2f (quad->t2.x, quad->t2.y);
			glVertex3f (quad->v2.x, quad->v2.y, quad->v2.z);

			glNormal3f (quad->n1.x, quad->n1.y, quad->n1.z);
			glTexCoord2f (quad->t1.x, quad->t1.y);
			glVertex3f (quad->v1.x, quad->v1.y, quad->v1.z);

			glNormal3f (quad->n4.x, quad->n4.y, quad->n4.z);
			glTexCoord2f (quad->t4.x, quad->t4.y);
			glVertex3f (quad->v4.x, quad->v4.y, quad->v4.z);

			glNormal3f (quad->n3.x, quad->n3.y, quad->n3.z);
			glTexCoord2f (quad->t3.x, quad->t3.y);
			glVertex3f (quad->v3.x, quad->v3.y, quad->v3.z);

		glEnd();
	}
}

void CTrackmarks::DrawCurrentTrack (TControl *ctrl) {
	if (!cfg.draw_trackmarks) return;
	int firstmark = 0;
	TTrackQuad *quad;

	SetGLOptions (TRACK_MARKS); 
    glColor4f (0, 0, 0, 1);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (ctrl->deviation < 0.5) {
		firstmark = 0;
	} else {
//		firstmark = CurrTrack.numQuads - NUM_CURRENT_MARKS;
//		if (firstmark < 0) firstmark = 0;
		firstmark = 0;
	}
	for (int qq=firstmark; qq<CurrTrack.numQuads; qq++) {
		quad = &CurrTrack.Quads[qq];
		if (quad->v3.z > ctrl->pos.z - FORWARD_TRACK_AREA && 
			quad->v1.z < ctrl->pos.z + BACKWARD_TRACK_AREA) {
			DrawQuad (quad);
		}
	}
}

void CTrackmarks::DrawPreviousTrack (TControl *ctrl) {
	if (!cfg.draw_previous_track) return;

	TTrackQuad *quad;
	SetGLOptions (TRACK_MARKS); 
    glColor4f (0, 0, 0, 1);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	for (int qq=0; qq < PrevTrack.numQuads; qq++) {
		quad = &PrevTrack.Quads[qq];
		if (quad->v3.z > ctrl->pos.z - FORWARD_TRACK_AREA && 
			quad->v1.z < ctrl->pos.z + BACKWARD_TRACK_AREA) {
			DrawQuad (quad);
		}
	}
}

void CTrackmarks::Add (float timestep, TControl *ctrl) {
	if (!cfg.draw_trackmarks) return;
	if (CurrTrack.numQuads >= MAX_TRACK_MARKS) return;

	TTrackQuad *quad, *prevquad;
	TVector3 left_vector, right_vector, left_wing, right_wing, cross, vel;
	float left_y, right_y, speed, dir_chg, dist_from_surface, track_depth; 
	TPlane surf_plane;
	TVector3 vv;
	TVector2 dir;
	bool trackterrain = true;
	bool terr_changed = false;
	float step_width = 0.5;

	if (CurrTrack.numQuads > 1) {
		dir.x = ctrl->dir.x;
		dir.y = ctrl->dir.z;
		NormVec2 (dir);
		dir_chg = fabs (last_dir.x - dir.x) + fabs (last_dir.y - dir.y);
		step_width = VecLen (SubVec (ctrl->pos, last_pos));
		if (cfg.optimize_trackmarks && step_width < 1.0 && dir_chg < 0.01) return;
		last_pos = ctrl->pos;
	} else {
		last_pos = ctrl->pos;
		step_width = 0.2;
	}
	last_dir = dir;
	
	// first we check the terrain
	int terr = Course.GetTrackTerrainIdx (ctrl->pos);
	if (!Course.TerrTypes[terr].trackmarks) trackterrain = false;
	terr_changed = (terr != last_terr);
	last_terr = terr;

	// next we check the distance from surface
	if (trackterrain) {
		surf_plane = Course.GetCoursePlane (ctrl->pos);
    	dist_from_surface = DistToPlane (surf_plane, ctrl->pos) - adj.radius;
		track_depth = Course.TerrTypes[terr].depth * 2;
		if (dist_from_surface > track_depth) trackterrain = false;
	}

	// at last we compute the wing params and check the roll angle
	if (trackterrain) {
		vel = ctrl->vel;
		speed = NormVec (vel);
	
		cross = CrossVec (vel, MakeVec (0, 1, 0));
		left_vector = ScaleVec (TRACK_WIDTH / 2.0, cross);
		right_vector = ScaleVec (-TRACK_WIDTH / 2.0, cross);
		left_wing =  SubVec (ctrl->pos, left_vector);
		right_wing = SubVec (ctrl->pos, right_vector);
		left_y = Course.GetYCoord (left_wing);
		right_y = Course.GetYCoord (right_wing);
		if (fabs (left_y-right_y) > MAX_TRACK_DEPTH) trackterrain = false;
	}

	// now we know whether a trackmark has to be added or not
	// first the case that no mark has to be added
	if (!trackterrain) {
		if (mode == TRACKBREAK) {
			return;
		} else {
			CurrTrack.Quads[CurrTrack.numQuads-1].track_type = TRACK_TAIL;
			mode = TRACKBREAK;
			return;
		}
	}

	if (mode == TRACKCONT && terr_changed) {
		CurrTrack.Quads[CurrTrack.numQuads-1].track_type = TRACK_TAIL;
		mode = TRACKBREAK;
	}

	// at this point we have to add a new trackmark
	if (mode == TRACKBREAK) {
		// we create a temp quad for the coordinates
		quad = &temp_quad;

		quad->v3 = MakeVec (left_wing.x, left_y + TRACK_HEIGHT, left_wing.z);
		quad->v4 = MakeVec (right_wing.x, right_y + TRACK_HEIGHT, right_wing.z);

		vv = Course.GetCourseNormal (quad->v3);
		quad->n3.x = vv.x;
		quad->n3.y = vv.y;
		quad->n3.z = vv.z;
		vv = Course.GetCourseNormal (quad->v4);
		quad->n4.x = vv.x;
		quad->n4.y = vv.y;
		quad->n4.z = vv.z;

	    quad->t3= MakeVec2 (0.0, 1.0);
	    quad->t4= MakeVec2 (1.0, 1.0);

		mode = TRACKSTART;
	} else {
	    quad = &CurrTrack.Quads[CurrTrack.numQuads];
		if (mode == TRACKSTART) prevquad = &temp_quad;
		else prevquad = &CurrTrack.Quads[CurrTrack.numQuads-1];

		// take the backward params from the previous quad
	    quad->v1 = prevquad->v3;
    	quad->v2 = prevquad->v4;
	    quad->n1 = prevquad->n3;
	    quad->n2 = prevquad->n4;
	    quad->t1 = prevquad->t3; 
	    quad->t2 = prevquad->t4;

		// compute the front vertices
		quad->v3 = MakeVec (left_wing.x, left_y + TRACK_HEIGHT, left_wing.z);
		quad->v4 = MakeVec (right_wing.x, right_y + TRACK_HEIGHT, right_wing.z);

		// compute the front normals
		vv = Course.GetCourseNormal (quad->v3);
		quad->n3.x = vv.x;
		quad->n3.y = vv.y;
		quad->n3.z = vv.z;
		vv = Course.GetCourseNormal (quad->v4);
		quad->n4.x = vv.x;
		quad->n4.y = vv.y;
		quad->n4.z = vv.z;


		float tex_end = step_width;
	    quad->t3 = MakeVec2 (0.0, quad->t1.y + tex_end);
    	quad->t4 = MakeVec2 (1.0, quad->t2.y + tex_end);

		if (mode == TRACKSTART) {
			quad->track_type = TRACK_HEAD;
			mode = TRACKCONT;
		} else quad->track_type = TRACK_MARK; // TRACKCONT

		quad->terr = terr;
		float alpha = 1 - dist_from_surface / track_depth;
		quad->alpha = MIN (1, MAX (0, alpha));

		CurrTrack.numQuads++;
	} 
}

void CTrackmarks::FinishTrack () {
	if (CurrTrack.numQuads > 0)
	CurrTrack.Quads[CurrTrack.numQuads-1].track_type = TRACK_TAIL;
}

void CTrackmarks::CopyTrack () {

	for (int i=0; i<CurrTrack.numQuads; i++) {
		PrevTrack.Quads[i] = CurrTrack.Quads[i];
	}
	PrevTrack.numQuads = CurrTrack.numQuads;
}






