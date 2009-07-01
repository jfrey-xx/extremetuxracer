// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#ifndef COLLISION_H
#define COLLISION_H

#include "bh.h"

#define MAX_TERR_TRIANGLES 100
#define MAX_MODEL_TRIANGLES 1000
#define MAX_MODEL_QUADS 400
#define MAX_TREE_TRIANGLES 100

// this struct is returned after collision detection.
// It contains all information needed by the physics
typedef struct {
	TVector3 pos;
	TVector3 vel;
	bool ready;
	int type;
	float dist;
	bool hit;
	TVector3 intersect;
	TVector3 nml;
	float collcos;
	int bounce;
	int msg;	
	int poly;
	int object;  		// 1 = terrain, 2 = model poly, 3 = tree 
	int itemidx;
} TCollDetection;

// --------------------------------------------------------------------

class CCollision {
private:
	TTriangleC TerrTriangles[MAX_TERR_TRIANGLES];
	int numTerrTriangles;

	TTriangleC ModelTriangles[MAX_MODEL_TRIANGLES];
	int numModelTriangles;

	TQuadC ModelQuads[MAX_MODEL_QUADS];
	int numModelQuads;

	TTriangleC TreeTriangles[MAX_TREE_TRIANGLES];
	int numTreeTriangles;

	float   FTotalLen;		// remaining motion until ready
	float   FMoved;			// this motion is already done
	float   FSpeed;			// for calculating the return vector

	// local, for collision detection, updated in each frame
	TVector3 FPos;			// position of Tux
	TVector3 FVel;			// velocity vector
	TVector3 FDir;			// normalized velocity
	float   FLen;			// length of the vel vector
	float   FLenlen;		// square length
	
	TVector3 FIntersect;	// point of intersection
 	float   FDistance;		// distance to the intersection point
	TVector3 FPlaneNml;		// normal of the polygon plane
	float   FPlaneConst;	// plane constant of the polygon
	float   FRad;			// Radius of the bounding sphere of Tux

	float PlaneIntersect (TVector3 pOrigin, TVector3 pNormal, TVector3 p, TVector3 dir);
	bool   InsidePolygon (TVector3 *v, float num, TVector3 p);
	float EdgeIntersect (TVector3 vert1, TVector3 vert2, float max, TVector3 *cp);
	float VertexIntersect (TVector3 vert, float max);
	int    PolygonIntersect (TVector3 *v, int num_vert, bool with_edges);
	int    PolygonDistance (TVector3 *v, int num_vert);
public:
	CCollision ();
	~CCollision ();

	void Init (TVector3 position, TVector3 velocity, float radius);
 	TCollDetection CheckModelCollision ();
	void CheckSurface (TControl *ctr);

	void ResetTerrPolygons ();		
	void AddTerrTriangle (TTriangleC tri);		// full collision

	void ResetModelPolygons ();	
	void AddModelTriangle (TTriangleC tri);		// full collision
	void AddModelQuad (TQuadC quad);			// full collision

	void ResetTreePolygons ();
	void AddTreeTriangle (TTriangleC tri);		// special collision

	bool FCollided;

};

extern CCollision Collision;
#endif









