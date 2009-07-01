// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "bh.h"
#include "course.h"
#include "textures.h"
#include "ogl.h"
#include "physics.h"
#include "quadtree.h"
#include "models.h"
#include "collision.h"
#include "env.h"
#include "keyframe.h"
#include "trackmarks.h"
#include "info.h"

// --------------------------------------------------------------------
// the terrain is rendered in this way: there are 2 kinds of 
// squares (A, B). a und b are the different triangles.
//
// 		3	2 B	1 A	0
//		+---+---+---+ 0
//		|  /|\ a|a /|
//		| /	| \ | / |
//		|/  |b \|/ b|
//		+---+---+---+ -1
//		|\  |  /|\  |
//		| \	| / | \ |
//		|  \|/  |  \|
//		+---+---+---+ -2
// --------------------------------------------------------------------

CCourseData Course;  // contains all params of a single course

// --------------------------------------------------------------------
CCourseData::CCourseData () {}
CCourseData::~CCourseData () {}

void CCourseData::Init () {
	int i;

	nmls = 0;
	elevation = 0;
	terrain_v = 0;
	terrain_q = 0;
	colors = 0;
	iarr = 0;
	workarr = 0;
	vertices = 0;
	numTerrTypes = 0;
	for (i=0; i<MAX_TERR_TYPES; i++) {
		TerrTypes[i].texid = 0;
		TerrTypes[i].name = "";
		TerrTypes[i].texture = "";
		TerrTypes[i].track_texture = "";
		TerrTypes[i].sound = "";
	}
}

void CCourseData::Reset () {
	// reset arrays
	if (elevation != 0) {delete elevation; elevation = 0;}
	if (terrain_v != 0) {delete terrain_v; terrain_v = 0;}
	if (terrain_q != 0) {delete terrain_q; terrain_q = 0;}
	if (nmls != 0)      {delete nmls; nmls = 0;}
	if (colors != 0)    {delete colors; colors = 0;}
	if (iarr != 0)      {delete iarr; iarr = 0;}
	if (workarr != 0)   {delete workarr; workarr = 0;}
	if (vertices != 0)  {delete vertices; vertices = 0;}

	// reset several strings
	name          = "";
	dir           = "";
	start_frame   = "";
	success_frame = "";
	failure_frame = "";
	final_frame   = "";
	
	Models.ClearDrawPages ();
	Models.ClearCollPages ();

	Trackmarks.ResetLastTrack ();

	// reset terrain textures
	GLuint id;
	for (int i=0; i<numTerrTypes; i++) {
		id = TerrTypes[i].texid;
		if (id > 0) {
			glDeleteTextures (1, &id);
			TerrTypes[i].texid = 0;
		}
		id = TerrTypes[i].tracktexid;
		if (id > 0) {
			glDeleteTextures (1, &id);
			TerrTypes[i].tracktexid = 0;
		}
	}
}

// --------------------------------------------------------------------
//					terrain
// --------------------------------------------------------------------

int CCourseData::GetTerrain (unsigned char pixel[]) {
	int i;
	for (i=0; i<numTerrTypes; i++) {
		if (fabs(pixel[0]-TerrTypes[i].col.r) < 30 
			&& fabs(pixel[1]-TerrTypes[i].col.g) < 30 
			&& fabs(pixel[2]-TerrTypes[i].col.b) < 30) {
			return i;
		}
	}
	return 1;
}

void CCourseData::LoadTerrainTypes () {
	int i;	
	TVector3 col;
	CSPList list(120);
	string line;
	
	for (i=0; i<MAX_TERR_TYPES; i++) TerrTypes[i].texid = 0;
	numTerrTypes = 0;
	if (!list.Load (cfg.terraindir, "terrains.lst")) return;
	for (i=0; i<list.Count(); i++) {
		line = list.Line (i);
		TerrTypes[numTerrTypes].texture = SPStrN (line, "texture", "missed");
		TerrTypes[numTerrTypes].track_texture = SPStrN (line, "track_texture", "missed");
		TerrTypes[numTerrTypes].texid = 0;
		TerrTypes[numTerrTypes].tracktexid = 0;
		col = SPVector3N (line, "col", MakeVec (1,1,1));
		TerrTypes[numTerrTypes].col.r = col.x;
		TerrTypes[numTerrTypes].col.g = col.y;
		TerrTypes[numTerrTypes].col.b = col.z;
		TerrTypes[numTerrTypes].friction = SPFloatN (line, "friction", 0.5);
		TerrTypes[numTerrTypes].depth = SPFloatN (line, "depth", 0.01);

		TerrTypes[numTerrTypes].particles = SPBoolN (line, "part", false);
		TerrTypes[numTerrTypes].trackmarks = SPBoolN (line, "trackmarks", false);
		TerrTypes[numTerrTypes].shiny = SPBoolN (line,"shiny", false);
		numTerrTypes++;
	}
}

bool CCourseData::LoadTerrainMap () {
	string terrpath;
    int arridx, imgidx, terr;
	Image img;
	if (!img.LoadPng (dir.c_str(), "terrain.png")) {
		Message ("terrain map not found");
		return false;
	}
	if (img.nx != nx || img.ny != ny) {
		Message ("maps have not equal size");
		return false;
	}

	terrain_v = new char [nx * ny];
	terrain_q = new char [qnx * qny];
    if (terrain_v == 0) {
		Message ("malloc terrain_v failed");
		return false;
	}
    if (terrain_q == 0) {
		Message ("malloc terrain_q failed");
		return false;
	}

	int pad = 0;
	for (int y=0; y<ny; y++) {
        for (int x=0; x<nx; x++) {
            imgidx = (x + nx * y) * img.depth + pad;
			arridx = (nx-1-x) + nx * (ny-1-y);
			terr = GetTerrain (&img.data[imgidx]);	
			terrain_v [arridx] = terr;	

			// load needed terrain textures
			if (TerrTypes[terr].texid < 1) {
				terrpath = MakePathStr (cfg.terraindir, TerrTypes[terr].texture);
				TerrTypes[terr].texid = LoadTexture (terrpath.c_str(), true);

				if (TerrTypes[terr].trackmarks) {
					terrpath = MakePathStr (cfg.terraindir, TerrTypes[terr].track_texture);
					TerrTypes[terr].tracktexid  = LoadTexture (terrpath.c_str(), true);
					if (TerrTypes[terr].tracktexid < 1) {
						TerrTypes[terr].trackmarks = false;
						Message ("couldn't load track texture");
					}
				}
			}
		} 
        pad += (nx * img.depth) % 4;
    } 

	for (int y=0; y<qny; y++) {
		for (int x=0; x<qnx; x++) {
			terrain_q [x + qnx * y] = terrain_v [x + nx * y];
		}
	} 
	return true;
} 

// --------------------------------------------------------------------
//					calculate arrays
// --------------------------------------------------------------------

void CCourseData::CalcVertices () {
	TVector3 v;

	vertices = new TVector3 [nx * ny];
    for (int x=0; x<nx; x++) {
		for (int y=0; y<ny; y++) {
			v.x = x / (nx-1.0) * width;
			v.y = elevation [x + nx * y]; 
			v.z = -y / (ny-1.0) * length;
			vertices [x + nx * y] = v;
		}
	}
}

void CCourseData::CalcNormals () {
	TVector3 nullvec = {0.0, 0.0, 0.0};
	TVector3 norm, tri[6];
	TIndex2 pp[6];
	int i, x, y;
	int qnx = nx-1;
	int qny = ny-1;

	nmls = new TVector3 [nx * ny];
	for (i=0; i<(nx*ny); i++) nmls[i] = nullvec;

	for (y=0; y<qny; y++) {
        for  (x=0; x<qnx; x++) {
		    if  ((x + y) % 2 == 0) { // even
				pp[0] = MakeIndex2 (x,   y);
				pp[1] = MakeIndex2 (x+1, y);
				pp[2] = MakeIndex2 (x+1, y+1);
				pp[3] = MakeIndex2 (x+1, y+1);
				pp[4] = MakeIndex2 (x,   y+1);
				pp[5] = MakeIndex2 (x,   y);
			} else { // odd
				pp[0] = MakeIndex2 (x,   y);
				pp[1] = MakeIndex2 (x+1, y);
				pp[2] = MakeIndex2 (x,   y+1);
				pp[3] = MakeIndex2 (x+1, y+1);
				pp[4] = MakeIndex2 (x,   y+1);
				pp[5] = MakeIndex2 (x+1, y);
			}

			for (i=0; i<6; i++) tri[i] = GetVertex (pp[i].i, pp[i].j);
			for (i=0; i<6; i++) {
				if (i<3) norm = CrossVec (SubVec (tri[1], tri[0]), SubVec (tri[2], tri[0]));
				else norm = CrossVec (SubVec (tri[4], tri[3]), SubVec (tri[5], tri[3]));
				NormVec (norm);
				nmls[pp[i].i + nx * pp[i].j] = 
				AddVec (nmls[pp[i].i + nx * pp[i].j], norm);			
			}
		}
	}
	for (i=0; i<(nx*ny); i++) NormVec (nmls[i]);
}

void CCourseData::CalcColors () {
    int x,y;
	TIntColorGL col = {255, 255, 255, 255};

    glDisableClientState (GL_VERTEX_ARRAY);
    glDisableClientState (GL_NORMAL_ARRAY);
    glDisableClientState (GL_COLOR_ARRAY);

	colors = new TIntColorGL [nx * ny];
    for (x=0; x<nx; x++) {
		for (y=0; y<ny; y++) colors [x+y*nx] = col;	
    }

    glEnableClientState (GL_VERTEX_ARRAY);
    glVertexPointer (3, GL_FLOAT, 0, vertices);

    glEnableClientState (GL_NORMAL_ARRAY);
	glNormalPointer (GL_FLOAT, 0, nmls);

    glEnableClientState (GL_COLOR_ARRAY);
    glColorPointer (4, GL_UNSIGNED_BYTE, 0, colors);
}

void CCourseData::CalcIndices () {
	iarr = new TQuadIndex [qnx * qny];
	workarr = new TQuadIndex [qnx * qny];
	for (int y=0; y<(qny); y++) {
        for  (int x=0; x<qnx; x++) {
		    if  ((x + y) % 2 == 0) {		// gerade
				iarr[x+qnx*y].a = VertIdx (x,   y); 
				iarr[x+qnx*y].b = VertIdx (x+1, y); 
				iarr[x+qnx*y].c = VertIdx (x+1, y+1); 
				iarr[x+qnx*y].d = VertIdx (x+1, y+1); 
				iarr[x+qnx*y].e = VertIdx (x,   y+1); 
				iarr[x+qnx*y].f = VertIdx (x,   y); 
			} else {
				iarr[x+qnx*y].a = VertIdx (x,   y); 
				iarr[x+qnx*y].b = VertIdx (x+1, y); 
				iarr[x+qnx*y].c = VertIdx (x,   y+1); 
				iarr[x+qnx*y].d = VertIdx (x+1, y+1); 
				iarr[x+qnx*y].e = VertIdx (x,   y+1); 
				iarr[x+qnx*y].f = VertIdx (x+1, y); 
			}
		}
	}
}

// --------------------------------------------------------------------
//					elevation
// --------------------------------------------------------------------

bool CCourseData::LoadElevMap () {
	int elev;
	int imgidx;
	Image img;
	if (!img.LoadPng (dir.c_str(), "elev.png")) return false;

    nx = img.nx;
    ny = img.ny;
	qnx = nx - 1;
	qny = ny - 1;
	elevation = new float [nx * ny];
    if (elevation == 0) Message ("malloc elevation failed", "");

    int pad = 0;   
    for (int y=0; y<ny; y++) {
        for (int x=0; x<nx; x++) {
			elev = (qnx-x) + nx * (qny-y);
			imgidx = (x+nx*y) * img.depth + pad;
			elevation [elev] = ((img.data [imgidx] - 
				CBASE_HEIGHT) / 255.0) * scale - (float)(qny-y)/ny * length * slope;
   	     } 
        pad += (nx * img.depth) % 4;
    } 

	return true;
} 

// --------------------------------------------------------------------
//					access to the course
// --------------------------------------------------------------------

TVector3 CCourseData::GetVertex (int x, int y) {
	return (vertices [x + nx * y]);
}

GLuint CCourseData::VertIdx (int x, int y) {
	return x + nx * y;
}

// ----------------------- GetIndices ---------------------------------
void CCourseData::GetIndices (const TVector3 pos, int &x0, int &y0, int &x1, int &y1) {
    float xidx, yidx;
    
    xidx = pos.x / width * ((float) nx - 1.0);
    yidx = -pos.z / length *  ((float) ny - 1.0);

    if (xidx < 0) xidx = 0;
    else if (xidx > nx-1) xidx = nx-1;

    if (yidx < 0) yidx = 0;
    else if (yidx > ny-1) yidx = ny-1;

/*  hm, don't know why Tuxracer does this:
	x0 = (int)(xidx);  
    x1 = (int)(xidx + 0.9999);  
    y0 = (int)(yidx);           
    y1 = (int)(yidx + 0.9999); */

	x0 = (int)(xidx);
	x1 = x0 + 1;
	y0 = (int)(yidx);
	y1 = y0 + 1;

    if (x0 == x1) {
		if (x1 < nx - 1) x1++; else x0--;
    } 

    if (y0 == y1) {
		if (y1 < ny - 1) y1++; else y0--;
    } 
}

void CCourseData::GetQuadIndex (const TVector3 pos, int &qx, int &qy) {
    float xidx, yidx;
    
    xidx = pos.x / width * ((float) nx - 1.0);
    yidx = -pos.z / length *  ((float) ny - 1.0);

    if (xidx < 0) xidx = 0;
    else if (xidx > nx-1) xidx = nx-1;

    if (yidx < 0) yidx = 0;
    else if (yidx > ny-1) yidx = ny-1;

	qx = (int)(xidx);
	qy = (int)(yidx);
}

// -------------------- GetBarycentrics -------------------------------

void CCourseData::GetBarycentrics (const TVector3 pos, 
		TIndex2 &idx0, TIndex2 &idx1, TIndex2 &idx2, float &u, float &v) {
    float xidx, yidx;
    int x0, x1, y0, y1;
    float dx, ex, dz, ez, qx, qz, invdet; 

    GetIndices (pos, x0, y0, x1, y1);
    xidx = pos.x / width * ((float) nx - 1.0);
    yidx = -pos.z / length * ((float) ny - 1.0);

    if  ((x0 + y0) % 2 == 0) {			
		if  (yidx - y0 < xidx - x0) {	
		    idx0 = MakeIndex2 (x0, y0); 
	    	idx1 = MakeIndex2 (x1, y0); 
		    idx2 = MakeIndex2 (x1, y1); 
		} else {						
		    idx0 = MakeIndex2 (x1, y1); 
		    idx1 = MakeIndex2 (x0, y1); 
	    	idx2 = MakeIndex2 (x0, y0); 
		} 
    } else {							
		if  (yidx - y0 + xidx - x0 < 1) {
		    idx0 = MakeIndex2 (x0, y0); 
	    	idx1 = MakeIndex2 (x1, y0); 
		    idx2 = MakeIndex2 (x0, y1); 
		} else {
	    	idx0 = MakeIndex2 (x1, y1); 
		    idx1 = MakeIndex2 (x0, y1); 
		    idx2 = MakeIndex2 (x1, y0); 
		} 
    }

    dx = idx0.i - idx2.i;
    dz = idx0.j - idx2.j;
    ex = idx1.i - idx2.i;
    ez = idx1.j - idx2.j;
    qx = xidx - idx2.i;
    qz = yidx - idx2.j;

    invdet = 1 / (dx * ez - dz * ex);
    u = (qx * ez - qz * ex) * invdet;
    v = (qz * dx - qx * dz) * invdet;
}

// --------------------- GetTriangleIndices ---------------------------
// the function returns the indices of the 3 triangle vertices 
// at position x, z

void CCourseData::GetTriangleIndices (TVector3 pos,
		TIndex2 &idx0, TIndex2 &idx1, TIndex2 &idx2) {
    float xidx, yidx;
    int x0, x1, y0, y1;

    GetIndices (pos, x0, y0, x1, y1);
    xidx = pos.x / width * ((float) nx - 1.0);
    yidx = -pos.z / length * ((float) ny - 1.0);

    if  ((x0 + y0) % 2 == 0) {				// quadrant A	
		if  (yidx - y0 < xidx - x0) {		// closer to y: triangle a
		    idx0 = MakeIndex2 (x0, y0); 
	    	idx1 = MakeIndex2 (x1, y0); 
		    idx2 = MakeIndex2 (x1, y1); 
		} else {							// triangle b
		    idx0 = MakeIndex2 (x1, y1); 
		    idx1 = MakeIndex2 (x0, y1); 
	    	idx2 = MakeIndex2 (x0, y0); 
		} 
    } else {								// quadrant B
		if  (yidx - y0 + xidx - x0 < 1) {
		    idx0 = MakeIndex2 (x0, y0); 
	    	idx1 = MakeIndex2 (x1, y0); 
		    idx2 = MakeIndex2 (x0, y1); 
		} else {
	    	idx0 = MakeIndex2 (x1, y1); 
		    idx1 = MakeIndex2 (x0, y1); 
		    idx2 = MakeIndex2 (x1, y0); 
		} 
    }
}

// ------------------ GetCourseNormal ---------------------------------

TVector3 CCourseData::GetCourseNormal (const TVector3 pos) {
    TIndex2 idx0, idx1, idx2;

    GetTriangleIndices (pos, idx0, idx1, idx2);
    TVector3 p0 = GetVertex (idx0.i, idx0.j);
    TVector3 p1 = GetVertex (idx1.i, idx1.j);
	TVector3 p2 = GetVertex (idx2.i, idx2.j);

    TVector3 tri_nml = CrossVec (SubVec (p1, p0), SubVec (p2, p0));
    NormVec (tri_nml);
	return tri_nml;
}

// later as private fields
static float lastx, lastz, lasty;
static bool cachefull = false;

float CCourseData::GetYCoord (const TVector3 pos) {
	TVector3 temppos = pos;
	if (-temppos.z >= length - 0.1) temppos.z = -(length - 0.1);

    TIndex2 idx0, idx1, idx2;
    float u, v;

    if  (cachefull && lastx == temppos.x && lastz == temppos.z) return lasty;

    GetBarycentrics (temppos, idx0, idx1, idx2, u, v);
    TVector3 p0 = GetVertex (idx0.i, idx0.j);
    TVector3 p1 = GetVertex (idx1.i, idx1.j);
    TVector3 p2 = GetVertex (idx2.i, idx2.j);
    float res = u * p0.y + v * p1.y +  (1.0 - u - v) * p2.y;

    lastx = temppos.x;
    lastz = temppos.z;
    lasty = res;
    cachefull = true;

    return res;
} 

void CCourseData::GetSurfaceWeights (const TVector3 pos, float weights[]) {
    TIndex2 idx0, idx1, idx2;
    float u, v;

    GetBarycentrics (pos, idx0, idx1, idx2, u, v);
    for (int i=0; i<numTerrTypes; i++) {
		weights[i] = 0;
		if (terrain_v [idx0.i + nx*idx0.j] == i) weights[i] += u;
		if (terrain_v [idx1.i + nx*idx1.j] == i) weights[i] += v;
		if (terrain_v [idx2.i + nx*idx2.j] == i) weights[i] += 1.0 - u - v;
    }
} 

TPlane CCourseData::GetCoursePlane (TVector3 pos) {
    TPlane res;
	pos.y = GetYCoord (pos);
	res.nml = GetCourseNormal (pos);
    res.d = - (res.nml.x * pos.x +  res.nml.y * pos.y +  res.nml.z * pos.z);
    return res;
}

int CCourseData::GetTerrainIdx (TVector3 pos, float level) {
	float weights[MAX_TERR_TYPES];
    TIndex2 idx0, idx1, idx2;
    float u, v;
    int i;

    GetBarycentrics (pos, idx0, idx1, idx2, u, v);
	for (i=0; i<numTerrTypes; i++) {
		weights[i] = 0;
		if (terrain_v [idx0.i + nx * idx0.j] == i) weights[i] += u;
		if (terrain_v [idx1.i + nx * idx1.j] == i) weights[i] += v;
		if (terrain_v [idx2.i + nx * idx2.j] == i) weights[i] += 1.0 - u - v;
     	if (weights[i] > level) return (i);
	}
	return -1;
} 

int CCourseData::GetTerrainIdx (TVector3 pos) {
	int qx, qy;
	GetQuadIndex (pos, qx, qy);
	int idx = terrain_q[qx + (nx-1) * qy];
	return idx;
} 

// this version is adapted to the special conditions of trackmarks
int CCourseData::GetTrackTerrainIdx (TVector3 pos) {
	int qx, qy;
	GetQuadIndex (pos, qx, qy);
	int idx = qx + qnx * (qy+1);
	if (idx >= qnx * qny) return -1;
	else return terrain_q[idx];
} 

float CCourseData::GetTerrainFriction (TVector3 pos) {
	int idx = GetTerrainIdx (pos);
	if (idx>=0 && idx<numTerrTypes) return TerrTypes[idx].friction;
	else return (0.5); 
}

void CCourseData::GetTrianglePair (int x, int y, TTriangleC &tri1, TTriangleC &tri2) {
	// ConvertVec: TVector3 to TVector3
	TVector3 v1 = GetVertex (x, y);
	TVector3 v2 = GetVertex (x+1, y);
	TVector3 v3 = GetVertex (x, y+1);
	TVector3 v4 = GetVertex (x+1, y+1);

	if ((x + y) % 2 == 0) {
		tri1.vert[0] = v1;
		tri1.vert[1] = v2;
		tri1.vert[2] = v4;
		tri2.vert[0] = v4;
		tri2.vert[1] = v3;
		tri2.vert[2] = v1;
	} else {
		tri1.vert[0] = v1;
		tri1.vert[1] = v2;
		tri1.vert[2] = v3;
		tri2.vert[0] = v3;
		tri2.vert[1] = v2;
		tri2.vert[2] = v4;
	}

	tri1.nml = PlaneNormal (tri1.vert[0], tri1.vert[1], tri1.vert[2]); 
	tri1.pconst = -DotVec (tri1.vert[0], tri1.nml);

 	tri2.nml = PlaneNormal (tri2.vert[0], tri2.vert[1], tri2.vert[2]); 
	tri2.pconst = -DotVec (tri2.vert[0], tri2.nml);

	int terridx = terrain_q[x + (nx-1) * y];
	if (terridx >= 0 && terridx < numTerrTypes) {
		tri1.frictcoeff = TerrTypes[terridx].friction;
		tri2.frictcoeff = TerrTypes[terridx].friction;
		tri1.depth = TerrTypes[terridx].depth;
		tri2.depth = TerrTypes[terridx].depth;
	} else {
		tri1.frictcoeff = 0.5;
		tri2.frictcoeff = 0.5;
		tri1.depth = 0;
		tri2.depth = 0;
	}
}

bool CCourseData::ParticleTerrain (TVector3 pos) {
	int terr = GetTerrainIdx (pos);
	return TerrTypes[terr].particles;
}

void CCourseData::AddPolygons (TControl *ctrl) {
	int minx, miny, maxx, maxy;
	TTriangleC tri1, tri2;

	float reach = VecLen (ctrl->vel) + adj.radius;
	float minxpos = ctrl->pos.x - reach; 
	float maxxpos = ctrl->pos.x + reach; 
	float minypos = ctrl->pos.z + reach; 
	float maxypos = ctrl->pos.z - reach; 

	TVector3 startpos = MakeVec (minxpos, 0, minypos);
	TVector3 destpos = MakeVec (maxxpos, 0, maxypos);
	GetQuadIndex (startpos, minx, miny);
	GetQuadIndex (destpos, maxx, maxy);

	Collision.ResetTerrPolygons ();
	for (int x=minx; x<=maxx; x++) {
		for (int y=miny; y<=maxy; y++) {
			Course.GetTrianglePair (x, y, tri1, tri2);
			Collision.AddTerrTriangle (tri1);			
			Collision.AddTerrTriangle (tri2);			
		}
	}
}

bool CCourseData::LoadCourseParams () {
	CSPList list(120);
	string line;

	if (!list.Load (dir, "course.lst")) {
		Message ("could not load course.lst", "");
		return false;
	}
		for (int i=0; i<list.Count(); i++) {
			line = list.Line(i);
			width = SPFloatN (line, "width", 100);
			length = SPFloatN (line, "length", 500);
			play_width = SPFloatN (line, "play_width", 90);
			play_length = SPFloatN (line, "play_length", 450);
			angle = SPFloatN (line, "angle", 20.0);
			scale = SPFloatN (line, "scale", 10.0);
			speed = SPFloatN (line, "speed", 1.0);
			startpoint = SPVector2N (line, "startpoint", MakeVec2 (0,0));
			start_frame = SPStrN (line, "start_keyframe", "start"); 
			success_frame = SPStrN (line, "success_keyframe", "success"); 
			failure_frame = SPStrN (line, "failure_keyframe", "failure"); 
			final_frame = SPStrN (line, "final_keyframe", "final"); 
		}
	return true;
}

bool CCourseData::Load (int id) {
	name = CourseInfo.GetDir (id);
	dir = MakePathStr (cfg.common_coursedir, CourseInfo.GetDir (id));
	if (!LoadCourseParams ()) {
		Message ("error: course params");
		return false;
	}
	descent = tan (RADIANS (angle)) * length;
	slope = tan (RADIANS (angle));
	baseheight = -CBASE_HEIGHT / 255.0 * scale;
	maxheight = baseheight + scale;

	if (!LoadElevMap()) {
		Message ("error: course elev map");
		return false;
	}
	CalcVertices ();
	CalcNormals ();
    CalcColors ();
	CalcIndices ();
	if (!LoadTerrainMap()) {
		Message ("error: course terrain map");
		return false;
	}

	// keyframes
	TuxStart.Load (start_frame);
	TuxSuccess.Load (success_frame);
	TuxFailure.Load (failure_frame);
	TuxFinal.Load (final_frame);

	return true;
}

// --------------------------------------------------------------------
//					Load Course (global)
// --------------------------------------------------------------------

static int CourseID = -1;

void LoadCourse () {
	if (game.course_id < 0) {
		Message ("not valid course id", "");
		WinExit (0);	
	}

	if (game.course_id != CourseID) { 
		if (CourseID >= 0) {
			Course.Reset ();
			ResetTerrain ();
		}
		if (Course.Load (game.course_id)) {
			Models.LoadItems ();
			TVector3 start_view = MakeVec (0, 0, 0); // evtl. Startposition ??
			InitTerrain (start_view);
			CourseID = game.course_id;
		} else {
			Message ("could not load course");
			WinExit (0);
		}
	}
}

