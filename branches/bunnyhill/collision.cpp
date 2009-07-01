// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "collision.h"
#include "models.h"

// collision types:
// -1 none
//  1 polygon face
//  2 polygon edge
//  3 polygon vertex
//  4 cylinder

CCollision Collision; // extern declaration

CCollision::CCollision () {}		// constructor (if needed)
CCollision::~CCollision () {}		// destructor (if needed)

// --------------------------------------------------------------------
//					settings
// --------------------------------------------------------------------

// in respect to the bounce angle, the last value is for frontal crash.
// These settings control the behaviour at collision.
const float MODEL_SPEED_REDUCTION[6] = {1.0, 0.7, 0.5, 0.4, 0.2, 0.1};
const float TERR_SPEED_REDUCTION[6]  = {1.0, 0.9, 0.8, 0.6, 0.2, 0.2};
const float MODEL_REFLEXION [6]      = {1.1, 1.1, 1.1, 1.2, 1.3, 1.5};
const float TERR_REFLEXION [6]       = {1.0, 1.0, 1.1, 1.2, 1.3, 1.5};

// settings for tree collision:
const bool  HIDE_COLLIDED_TREES = false;
const float MIN_DIRCHANGE = 0.02;
const float MAX_DIRCHANGE = 0.20;

// --------------------------------------------------------------------
//						private
// --------------------------------------------------------------------

void CCollision::AddTerrTriangle (TTriangleC tri) {
	if (numTerrTriangles >= MAX_TERR_TRIANGLES) return;
	TerrTriangles[numTerrTriangles] = tri;
	numTerrTriangles++;
}

void CCollision::AddModelTriangle (TTriangleC tri) {
	if (numModelTriangles >= MAX_MODEL_TRIANGLES) return;
	ModelTriangles[numModelTriangles] = tri;
 	numModelTriangles++;
}

void CCollision::AddModelQuad (TQuadC quad) {
	if (numModelQuads >= MAX_MODEL_QUADS) return;
	ModelQuads[numModelQuads] = quad;
 	numModelQuads++;
}

void CCollision::AddTreeTriangle (TTriangleC tri) {
	if (numTreeTriangles >= MAX_TREE_TRIANGLES) return;
	TreeTriangles[numTreeTriangles] = tri;
 	numTreeTriangles++;
}

float CCollision::PlaneIntersect 
		(TVector3 pOrigin, TVector3 pNormal, TVector3 p, TVector3 dir) {
	float d = -(DotVec (pNormal, pOrigin));
	float num = DotVec (pNormal, p) + d;
	float denom = DotVec (pNormal, dir);
	return -(num / denom);
}

bool CCollision::InsidePolygon (TVector3 *v, float num, TVector3 p) {
	float plane;
	TVector3 normal;
	int i, ii;
	bool outside = false;
	
	for (i=0; i<num; i++) {
		if (i == num-1) ii = 0; else ii = i + 1;
	
		if (!outside) {
			normal = CrossVec (SubVec (v[i], v[ii]), FPlaneNml);
			plane = PlaneConstant (v[i], normal);
			if (DotVec (p, normal) + plane < 0.0) outside = true;
		}
	}			
	return (!outside);
}

// --------------------- Edge Intersect -------------------------------

float CCollision::EdgeIntersect (TVector3 vert1, TVector3 vert2,
		float max, TVector3 *cp) {
	
	TVector3 edge = SubVec (vert2, vert1); 
	TVector3 btv  = SubVec (vert1, FPos);
	float len   = SquareVecLen (edge);		
	float dot1  = DotVec (edge, FVel);
	float dot2  = DotVec (edge, btv);
	float dot3  = DotVec (FVel, btv);
	float a     = len * -FLenlen + dot1 * dot1;
	float b     = 2.0 * (len * dot3 - dot1 * dot2);
	float c     = len * (FRad * FRad - SquareVecLen (btv)) + dot2 * dot2;			
	float d = b * b - 4.0 * a * c;
	if (d < 0.0) return -1;
	
	float root = (-b + sqrt (d)) / 2 / a;
	if (root > 0 && root < max) {
		float f = (dot1 * root - dot2) / len;
		if (f >= 0.0 && f <= 1.0) {
			*cp = AddVec (vert1, ScaleVec (f, edge));
			return root;
		}
	}
	return -1;
}

// ------------------- Vertex Intersect -------------------------------

float CCollision::VertexIntersect (TVector3 vert, float max) {
	float b = 2.0 * (DotVec (FVel, SubVec (FPos, vert)));
	float c = SquareVecDistance (vert, FPos) - FRad * FRad;
	float d = b * b - 4.0 * FLenlen * c;
	if (d < 0.0) return -1;
	
	float root = (-b - sqrt (d)) / 2 / FLenlen;
	if (root > 0.0 && root < max) return root;
	return -1;
}

// --------------------- Polygon Intersect ----------------------------
// the main collision function.
// It computes the intersection point (-> FIntersect)
// and the distance to this point (-> FDistance)

int CCollision::PolygonIntersect (TVector3 *v, int num_vert, bool onlyface) {
	int type = -1; 
	int i, ii;
	float t0, t1;
	float curr_t, new_t;
	TVector3 coll_point = MakeVec (0,0,0);
	TVector3 cp, v1, v2; 

	float denom = DotVec (FPlaneNml, FVel);
	if (denom > 0) return -1;
	
	float dist = DotVec (FPlaneNml, FPos) + FPlaneConst;
	bool parallel = (fabs(denom) < 0.0001);

	// some preperations: interval where the sphere intersects the plane
	if (parallel) {
		t0 = 0.0;
		t1 = 1.0;
		if (fabs (dist) >= FRad) return -1;	
	} else {
		t0 = (FRad - dist) / denom;
		t1 = (-FRad - dist) / denom;
		if (t0 > t1) PrintStr ("swap t0 and t1 has occured");		
		if (t0 > 1.0 || t1 < 0.0) return -1;
		if (t0 < 0.0) t0 = 0.0;
		if (t1 > 1.0) t1 = 1.0;
	}

	// collision with the face if the polygon
	if (parallel == false) {
		v1 = SubVec (FPos, ScaleVec (FRad, FPlaneNml));
		v2 = ScaleVec (t0, FVel);
		TVector3 plane_intersect = AddVec (v1, v2);
		if (InsidePolygon (v, num_vert, plane_intersect)) {
			FDistance = t0 * FLen;
 			FIntersect = plane_intersect;
			return 1;
		}
	}

	if (onlyface) return -1;

	bool hit = false;
	curr_t = 1.0;

	// collision with the edges
	for (i=0; i<num_vert; i++) {
		if (i == num_vert-1) ii = 0; else ii = i + 1; 			
		new_t = EdgeIntersect (v[i], v[ii], curr_t, &cp);
		if (new_t >= 0) {
			hit = true;
			curr_t = new_t;				
			coll_point = cp;
			type = 2;
		}		
	}	
	
	// collision with the vertices
	for (i=0; i<num_vert; i++) {
		new_t = VertexIntersect (v[i], curr_t); 
		if (new_t >= 0) {
			hit = true;
			curr_t = new_t;
			coll_point = v[i];
			type = 3;
		}
	}		
		
	if (hit) {
		FIntersect = coll_point;
		FDistance = curr_t * FLen;
		return type;
	}		
	return -1;
}

// --------------------------------------------------------------------
//						public
// --------------------------------------------------------------------

void CCollision::Init (TVector3 position, TVector3 velocity, float radius) {
	FRad = radius;
	FPos = position;
 	FVel = velocity;

	FDir = FVel;			
 	FLen = NormVec (FDir);
	FLenlen = FLen * FLen;	
	FMoved = 0.0;
	FTotalLen = FLen;
	FSpeed = FLen;

	FCollided = false;
}


void CCollision::ResetModelPolygons () {
	numModelTriangles = 0;
	numModelQuads = 0;
}

void CCollision::ResetTerrPolygons () {
	numTerrTriangles = 0; 
}

void CCollision::ResetTreePolygons () {
	numTreeTriangles = 0; 
}

// ------------------ CheckCollision ----------------------------------

TCollDetection CCollision::CheckModelCollision () {
	TCollDetection coll;
	int ctype; 
	int i;

	// ----------------------------------------------------------------
	//			collision detection
	// ----------------------------------------------------------------

	coll.hit = false;	
	coll.dist = 1e+10;
	coll.type = -1;
	coll.poly = -1;

	// first the terrain triangles
	// the nearest collision is entered in the coll structure
	for (i=0; i<numTerrTriangles; i++) {
		FPlaneNml = TerrTriangles[i].nml;
		FPlaneConst = TerrTriangles[i].pconst;
		ctype = PolygonIntersect (TerrTriangles[i].vert, 3, false);
		if (ctype > 0 && FDistance < coll.dist) {
			coll.type = ctype;
			coll.hit  = true;
			coll.dist = FDistance;
			coll.intersect = FIntersect;
			coll.nml = FPlaneNml;
			coll.poly = i;
			coll.object = 1;
		} 
	}

	// next the model triangles
	for (i=0; i<numModelTriangles; i++) {
		FPlaneNml = ModelTriangles[i].nml;
		FPlaneConst = ModelTriangles[i].pconst;
		ctype = PolygonIntersect (ModelTriangles[i].vert, 3, false);
		if (ctype > 0 && FDistance < coll.dist) {
			coll.type = ctype;
			coll.hit  = true;
			coll.dist = FDistance;
			coll.intersect = FIntersect;
			coll.nml = FPlaneNml;
			coll.poly = i;
			coll.object = 2;
			coll.itemidx = ModelTriangles[i].idx;
		} 
	}

	// ... and the model quads
	for (i=0; i<numModelQuads; i++) {
		FPlaneNml = ModelQuads[i].nml;
		FPlaneConst = ModelQuads[i].pconst;
		ctype = PolygonIntersect (ModelQuads[i].vert, 4, false);
		if (ctype > 0 && FDistance < coll.dist) {
			coll.type = ctype;
			coll.hit  = true;
			coll.dist = FDistance;
			coll.intersect = FIntersect;
			coll.nml = FPlaneNml;
			coll.poly = i;
			coll.object = 2;
			coll.itemidx = ModelQuads[i].idx;
		} 
	}

	// finally the tree triangles for soft collision
	for (i=0; i<numTreeTriangles; i++) {
		FPlaneNml = TreeTriangles[i].nml;
		FPlaneConst = TreeTriangles[i].pconst;
		ctype = PolygonIntersect (TreeTriangles[i].vert, 3, false);
		if (ctype > 0 && FDistance < coll.dist) {
			coll.type = ctype;
			coll.hit  = true;
			coll.dist = FDistance;
			coll.intersect = FIntersect;
			coll.nml = FPlaneNml;
			coll.poly = i;
			coll.object = 3;
			coll.itemidx = TreeTriangles[i].idx;
		} 
	}

	// ----------------------------------------------------------------
	//			collision response
	// ----------------------------------------------------------------

	if (coll.hit == true) {
		FCollided = true;
		// new position:
		FPos = AddVec (FPos, ScaleVec (coll.dist - 0.0001, FDir));

		// sliding plane:
		TVector3 slide_normal;
		if (coll.type == 1) {
			slide_normal = coll.nml;
		} else {
			slide_normal = SubVec (FPos, coll.intersect);
			NormVec (slide_normal);
		}

		// bounce angle:
		coll.collcos = -DotVec (slide_normal, FDir); 	
		if (coll.collcos < 0.26) coll.bounce = 0;		// < 15°
		else if (coll.collcos < 0.50) coll.bounce = 1;	// < 30°
		else if (coll.collcos < 0.71) coll.bounce = 2;	// < 45°
		else if (coll.collcos < 0.85) coll.bounce = 3;	// < 60°
		else if (coll.collcos < 0.95) coll.bounce = 4;	// < 75°
		else coll.bounce = 5;						// 75°-90°
		
		// special tree collision
		if (coll.object==3) { 
			Models.SetCollEnabled (coll.itemidx, false);
			if (HIDE_COLLIDED_TREES) Models.SetDrawEnabled (coll.itemidx, false);
			coll.pos = AddVec (FPos, FVel);

			float xx = xrandom (MIN_DIRCHANGE, MAX_DIRCHANGE) / FSpeed;
			if (irandom (0, 1) == 0) FDir = AddVec (FDir, MakeVec (xx, 0, 0));
			else FDir = AddVec (FDir, MakeVec (-xx, 0, 0));

			FSpeed *= 0.2;
			coll.ready = true;

		// polygon collision
		} else {
			// new direction (sliding path on sliding plane)		
			TVector3 A = AddVec (coll.intersect, FDir);		
			float dist = PlaneIntersect (coll.intersect, slide_normal, A, slide_normal);
			TVector3 V;

			float reduction;
			// terrain:
			if (coll.object==1) {
				V = ScaleVec (dist * TERR_REFLEXION[coll.bounce], slide_normal);
				reduction = TERR_SPEED_REDUCTION[coll.bounce];
			// models:
			} else {	
				V = ScaleVec (dist * MODEL_REFLEXION[coll.bounce], slide_normal);
				reduction = MODEL_SPEED_REDUCTION[coll.bounce];
			}
	
			TVector3 B = AddVec (A, V);
			FDir = SubVec (B, coll.intersect);
			NormVec (FDir);
	
			// calculate velocity vector and update current params
			FMoved += coll.dist;
			FLen = FTotalLen - FMoved;  // remainder step, Restschritt
			FLen *= reduction;		
			FSpeed *= reduction;
			FTotalLen = FMoved + FLen;		
			FVel = ScaleVec (FLen, FDir);
			FLenlen = FLen * FLen;
	
			coll.pos = FPos;
			coll.ready = false;
		}

	// no collision:
	} else {
		coll.pos = AddVec (FPos, FVel);
		coll.collcos = 0.0;
		coll.bounce = 0;
		coll.ready = true;
	}

	if (coll.object < 1 || coll.object > 3) coll.object = 0;	
	coll.vel = ScaleVec (FSpeed, FDir);
	return coll;
}

// --------------------------------------------------------------------
//					CheckSurface 
// --------------------------------------------------------------------
// this function checks the surface properties at the position
// of the last collision detection. (It's the only function with access to ctrl)

void CCollision::CheckSurface (TControl *ctr) {
	int i, coll;
	ctr->airborne = true;

	// terrain (must be done first):
	if (ctr->airborne) {
		for (i=0; i<numTerrTriangles; i++) {
			FPlaneNml = TerrTriangles[i].nml;
			FPlaneConst = TerrTriangles[i].pconst;
			coll = PolygonIntersect (TerrTriangles[i].vert, 3, true);
			if (coll==1) {
				ctr->airborne = false;
				ctr->surf_nml = FPlaneNml;
				ctr->friction = TerrTriangles[i].frictcoeff;
				ctr->depth = TerrTriangles[i].depth;
				break;
			}
		}
	}

	// models:
	if (ctr->airborne) {
		for (i=0; i<numModelTriangles; i++) {
			FPlaneNml = ModelTriangles[i].nml;
			FPlaneConst = ModelTriangles[i].pconst;
			coll = PolygonIntersect (ModelTriangles[i].vert, 3, true);
			if (coll==1) {
				ctr->airborne = false;
				ctr->surf_nml = FPlaneNml;
				ctr->friction = ModelTriangles[i].frictcoeff;
				ctr->depth = ModelTriangles[i].depth;
				break;
			}
		}
	}
	if (ctr->airborne) {
		for (i=0; i<numModelQuads; i++) {
			FPlaneNml = ModelQuads[i].nml;
			FPlaneConst = ModelQuads[i].pconst;
			coll = PolygonIntersect (ModelQuads[i].vert, 4, true);
			if (coll==1) {
				ctr->airborne = false;
				ctr->surf_nml = FPlaneNml;
				ctr->friction = ModelQuads[i].frictcoeff;
				ctr->depth = ModelQuads[i].depth;
				break;
			}
		}
	}

}

