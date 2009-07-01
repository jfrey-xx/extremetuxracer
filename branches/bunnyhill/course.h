// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

// the course class contains all (!) geometrical params and arrays
// related to the course - including the load functons or functions
// for calculating the course properties at a given point

#ifndef COURSE_H
#define COURSE_H

#include "bh.h"

//  the arrays in CCourseData:
//	elevation: [nx * ny]      height points, LoadElevMap (Quadtree)
//	vertices:  [nx * ny]      vertex vectors, CalcVertices (OpenGL)     
//	nmls:      [nx * ny]      vertex normals, CalcNmls (OpenGL)
//	colors:    [nx * ny]      color values, CalcColors (OpenGL und Quadtree)
//	iarr:      [nx-1 * ny-1]  vertex indices for 2 triangles (brute force rendering)
//	workarr:   [nx-1 * ny-1]  work array (brute force)
//	terrain_v: [nx * ny]      terrain codes, LoadTerrainMap (for Quadtree)
//	terrain:   [nx-1 * ny-1]  terrain codes, LoadTerrainMap (for brute force)

#define ELEV(x,y) (elevation[(x) + nx*(y)] )
#define XCD(x) ((float)(x) / (nx-1.0) * course_width)
#define ZCD(y) (-(float)(y) / (ny-1.0) * course_length)
#define NMLPOINT(x,y) MakeVec (XCD(x), ELEV(x,y), ZCD(y) )
#define COURSE_VERTEX(x,y) MakeVec( (float)(x)/(nx-1.)*width, \
	ELEV((x),(y)), -(float)(y)/(ny-1.)*length ) 

#define CBASE_HEIGHT 127
#define MAX_TERR_TYPES 64

typedef struct {
	string name;
	string texture;
	string track_texture;
	GLuint texid;
	GLuint tracktexid;

	string sound;
	TColor col;
	float friction;
	float depth;
	int vol_type;
	bool shiny;
	bool particles;
	bool trackmarks;
} TTerrType;

typedef struct {int a, b, c, d, e, f; } TQuadIndex;

class CCourseData {
private:
	void      CalcNormals ();
	void      CalcVertices ();
	void      CalcColors ();
	void      CalcIndices ();
	bool      LoadElevMap ();
	bool      LoadTerrainMap ();

	int       GetTerrain (unsigned char pixel[]);
	TVector3  GetVertex (int x, int y);
	GLuint    VertIdx (int x, int y);
	void      GetQuadIndex (const TVector3 pos, int &qx, int &qy);
	void      GetTrianglePair (int x, int y, TTriangleC &tri1, TTriangleC &tri2);
	void      GetBarycentrics (const TVector3 pos, 
		      TIndex2 &idx0, TIndex2 &idx1, TIndex2 &idx2, float &u, float &v);
	void      GetTriangleIndices (const TVector3 pos,
		      TIndex2 &idx0, TIndex2 &idx1, TIndex2 &idx2);
public:
	string name;
	string dir;

	string start_frame;
	string success_frame;
	string final_frame;
	string failure_frame;

	TTerrType   TerrTypes[MAX_TERR_TYPES];
	int         numTerrTypes;

	// arrays with global access:
	float       *elevation;
	TQuadIndex  *iarr;
	TQuadIndex  *workarr;
	char        *terrain_v;
	char        *terrain_q;	
	// the vnc-array:
	TVector3   *vertices;				
	TVector3   *nmls;
	TIntColorGL *colors;

	int nx, ny, qnx, qny;
	float scale;
	float angle;
	float width;
	float length;
	float play_width;
	float play_length;
 	float descent;
	float slope;
	float speed;
	TVector2 startpoint;
	float baseheight;
	float maxheight;

	CCourseData ();
	~CCourseData ();
	void      Init ();
	void      Reset ();
	bool      Load (int id);
	void      LoadTerrainTypes ();
	bool      LoadCourseParams ();

	void      GetIndices (const TVector3 pos, int &x0, int &y0, int &x1, int &y1);
	TVector3  GetCourseNormal (const TVector3 pos);
	float     GetYCoord (const TVector3 pos);
	void      GetSurfaceWeights (const TVector3 pos, float weights[]);
	int       GetTerrainIdx (TVector3 pos, float level);
	int       GetTerrainIdx (TVector3 pos);
	int       GetTrackTerrainIdx (TVector3 pos);
	TPlane    GetCoursePlane (TVector3 pos);
	float     GetTerrainFriction (TVector3 pos);
	void      AddPolygons (TControl *ctrl);
	bool      ParticleTerrain (TVector3 pos);
};

extern CCourseData Course;

void LoadCourse ();

#endif
