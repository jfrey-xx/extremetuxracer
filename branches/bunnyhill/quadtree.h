// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
// Copyright (C) 1999-2000 Thatcher Ulrich <tu@tulrich.com>
//
// --------------------------------------------------------------------

// Author of the quadtree algorithm is Ulrich Thatcher. To get the code running
// in Tuxracer it was modiefied by Jasmin F. Patry. I've modified the code
// again for Bunny Hill (old version). With this modification it was possible
// to use more terrains than the 3 standard terrains (snow, rock and ice). For
// the current version of Bunny Hill I've only rearranged the modules. The
// quadgeom.cpp ist not required because all needed mathematical functions are
// available in the alglib unit (now called "mathlib"). Furthermore the functions
// of "course_quad.cpp" are integrated in quadtree.cpp. These functions were
// purposed to acess the C++ quadtree from the other modules which were completely
// written in C. The new Bunny Hill code is C++-orientated though not rigorously
// adapted to C++.

// The quadtree algorithm works well but has some disadvantages. One disadvantage
// ist the kind of blurring at adjacent terrain tiles, detailed terrain texturing
// is not possible in this way. Another, more weightly disadvantage is the 
// performance that depends on the count of different terrains. Many terrains on
// a course slow down the race appreciably. It's not urgent but anytime this
// algorithm should be replaced with a more convenient quadtree algorithm.

#ifndef QUADTREE_H
#define QUADTREE_H

#include "bh.h"
#include "view.h"
#include "course.h"

typedef enum {East, South, Center} TVertexLocation;

struct HeightMapInfo {
    float *Data;
    int	XOrigin, ZOrigin;
    int	XSize, ZSize;
    int	RowWidth;
    int	Scale;
    float Sample (int x, int z) const;
};

struct VertInfo { float Y; };
struct Quad; // predeclaration since Corner uses it

class Corner {
public:
    const Corner *Parent;
    Quad *Square;
    int	ChildIndex;
    int	Level;
    int	xorg, zorg;
    VertInfo Verts[4];	
};

struct Quad {
public:
    Quad *Child[4]; 

    VertInfo Vertex[5];
    float Error[6];	
    float MinY, MaxY;	
    unsigned char EnabledFlags;	
    unsigned char SubEnabledCount[2];
    bool Static;
    bool Dirty;	

    bool ForceEastVert;
    bool ForceSouthVert;

    static float ScaleX, ScaleZ;
    static int RowSize, NumRows;
    static char *Terrain;
    static GLuint TexId[MAX_TERR_TYPES];
    static GLuint EnvmapTexId;

    static GLuint *VAindex;
    static GLuint VAcount;
    static GLuint VAmin;
    static GLuint VAmax;

	static void UpdateMinMax (GLuint idx);
    static void MakeTri (int a, int b, int c, int terrain );
    static void MakeSpecialTri (int a, int b, int c, int terrain );
    static void MakeNoBlendTri (int a, int b, int c, int terrain );

    static void DrawTris();
    static void DrawEnvmapTris();

    Quad (Corner* pcd);
	~Quad();

    void	AddHeightMap(const Corner& cd, const HeightMapInfo& hm);
    void	StaticCullData(const Corner& cd, float ThresholdDetail);	
    float	RecomputeError(const Corner& cd);
    int		CountNodes();
    void	Update (const Corner& cd, const float ViewerLocation[3], float Detail);
    void	Render (const Corner& cd);
    float	GetHeight(const Corner& cd, float x, float z);
	
private:
    Quad    *EnableDescendant(int count, int stack[], const Corner& cd);
    Quad    *GetNeighbor(int dir, const Corner &cd);
    TClipRes ClipSquare (const Corner &cd);
    
	void	EnableEdgeVertex(int index, bool IncrementCount, 
			const Corner &cd);
	void	EnableChild (int index, const Corner &cd);
	void	NotifyChildDisable(const Corner& cd, int index);
	void	ResetTree();
	void	StaticCullAux (const Corner &cd, float ThresholdDetail, 
			int TargetLevel);
    void	CreateChild(int index, const Corner &cd);
	void	SetupCorner (Corner *q, const Corner &pd, int ChildIndex);
    void	UpdateAux(const Corner &cd, const float ViewerLocation[3], 
			float CenterError, TClipRes vis);
	void	RenderAux(const Corner &cd, TClipRes vis, int terrain);
	void	SetStatic (const Corner &cd);
	void	InitVert(int i, int x, int z);
    bool	VertexTest(int x, float y, int z, float error, const float Viewer[3], 
			int level, TVertexLocation vertex_loc );
	bool	BoxTest(int x, int z, float size, float miny, float maxy, 
			float error, const float Viewer[3]);
};

// --------------------------------------------------------------------
//				global calls
// --------------------------------------------------------------------

void ResetQuadtree ();
void InitQuadtree (TVector3 start_viewpos); // viewpos related to course
void RenderQuadtree (TVector3 viewpos); 

void ResetTerrain ();
void InitTerrain (TVector3 start_viewpos);
void DrawTerrain (TVector3 viewpos);

#endif 
