/* 
 * PPRacer 
 * Copyright (C) 2004-2005 Volker Stroebel <volker@planetpenguin.de>
 *
 * Copyright (C) 1999-2001 Jasmin F. Patry
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef _TRACK_MARKS_H_
#define _TRACK_MARKS_H_


#include "pp_types.h"
#include "player.h"
#include "course_mgr.h"
#include "course_load.h"

#undef TRACK_TRIANGLES

#define TRACK_WIDTH  0.7
#define MAX_TRACK_MARKS 1000
#define MAX_CONTINUE_TRACK_DIST TRACK_WIDTH*4
#define MAX_CONTINUE_TRACK_TIME .1
#define SPEED_TO_START_TRENCH 0.0
#define SPEED_OF_DEEPEST_TRENCH 10

#ifdef TRACK_TRIANGLES
  #define TRACK_HEIGHT 0.1
  #define MAX_TRACK_DEPTH 10
  #define MAX_TRIS MAX_TRACK_MARKS
#else
  #define TRACK_HEIGHT 0.08
  #define MAX_TRACK_DEPTH 0.7
#endif

typedef enum track_types_t {
    TRACK_HEAD,
    TRACK_MARK,
    TRACK_TAIL,
    NUM_TRACK_TYPES
} track_types_t;

typedef struct track_quad_t {
    pp::Vec3d v1, v2, v3, v4;
    pp::Vec2d t1, t2, t3, t4;
    pp::Vec3d n1, n2, n3, n4;
    track_types_t track_type;
	int terrain;
    double alpha;
} track_quad_t;

typedef struct track_marks_t {
    track_quad_t quads[MAX_TRACK_MARKS];
    int current_mark;
    int next_mark;
    double last_mark_time;
    pp::Vec3d last_mark_pos;
} track_marks_t;

#ifdef TRACK_TRIANGLES

typedef struct track_tris_t {
    triangle_t tri[MAX_TRIS];
    track_types_t *track_type[MAX_TRIS];
    double *alpha[MAX_TRIS];
    int first_mark;
    int next_mark;
    int current_start;
    int current_end;
    int num_tris;
} track_tris_t;

typedef struct track_tri_t {
    pp::Vec3d v1, v2, v3;
} track_tri_t;

#endif


class Track_marks {
	private:
		#ifdef TRACK_TRIANGLES
		void draw_tri( triangle_t *tri, double alpha );
		void draw_tri_tracks( void );
		
		void add_tri_tracks_from_tri( pp::Vec3d p1, pp::Vec3d p2, pp::Vec3d p3,
				     pp::Vec2d t1, pp::Vec2d t2, pp::Vec2d t3,
				     track_types_t *track_type, double *alpha );
				     
		void add_tri_tracks_from_quad( track_quad_t *q );
		#endif
		
	public:
		track_marks_t track_marks;
		bool continuing_track;
		
		#ifdef TRACK_TRIANGLES
		track_tris_t track_tris;
		#endif
		
		void init_track_marks(void);
		void draw_track_marks(void);
		void add_track_mark( Player& plyr );
		void break_track_marks(void);
		
		//static terrain_tex_t terrain_texture[NUM_TERRAIN_TYPES];
		//static unsigned int num_terrains;
};

extern Track_marks track_marks_array[NUM_PLAYERS];

#endif /* _TRACK_MARKS_H_ */
