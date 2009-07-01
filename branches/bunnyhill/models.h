// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#ifndef MODELS_H
#define MODELS_H

#include "bh.h"

#define MAX_OBJECT_TYPES 64
#define MAX_ITEMS 8000

#define MAX_DRAW_PAGES 100
#define MAX_COLL_PAGES 500

// the following structure contains the adjustments in items.lst:
// - position in 3D space
// - trans for position correction
// - rot: the 3 rotation angles (x, y, z axis)
// - scale: the scale factors (3 dimensions)
typedef struct {
	TVector3 pos;
	TVector3 trans;
	TVector3 rot;
	TVector3 scale;
} TPositionParams;

typedef struct {
	short int   numTriangles;
	short int   numQuads;
	TTriangleC *triangles;
	TQuadC     *quads;
} TModelPolygons;

typedef struct {
	TVector2 coord[3];
} TTexCoord3;

typedef struct {
	TVector2 coord[4];
} TTexCoord4;

// TModel contains the geometrical properties
// defined in the particular model files, e.g. "house.lst"
// used for storing the model description
typedef struct {
	int numVertices;
	int numTriangles;
	int numQuads;
	TVector3   *vertices;			
	TVIndex3   *vertidx3;		// for triangles
	TTexCoord3 *texcoord3;
	TVIndex4   *vertidx4;		// for quads
	TTexCoord4 *texcoord4;
} TModel;

// TItem contains the parameters of the objects
// listet in the file "items.lst"
// additionally it gets some properties from TModelType
// for easier and faster access
typedef struct {
	unsigned short int type;	
	unsigned short int mat;		
	unsigned short int collect;		// number of points
	short int model;				// index to model array
	GLuint  texid;
	float radius;					// radius of bounding circle
	float depth;
	float friction;
	bool    collide;				// true if collidable
	bool    draw;					// true if drawable

	TPositionParams ip;
	TModelPolygons *polygons;		// pointer to polygon arrays
} TItem;

// TObjectType contains the common properties
// defined in the file "object_types.lst"
typedef struct {
	char   *name; 		// for string index 
	TModel *model;	    // params of 3D model
	GLuint billboard;	// texture id for billboard 
	GLuint texid;		// object texture id

	// main properties:
	int    type;		// 1 model, 2 tree, 3 sprite 
	bool   draw;		// visible or not		
	int    collect;		// 0 not, 1 number of points
	bool   collide;		// collidable or not

	// values:
	int    mat;			// index of predefined materials
	float radius;	    // radius of bounding circle	
	float friction;		// friction coeff of the faces
	float depth;		// how deep dives Tux in the surface
 } TObjectType;

typedef struct {
	int *idx;
	int numIdx;
	int cnt;
} TPage;

// --------------------------------------------------------------------
//					CModels
// --------------------------------------------------------------------

class CModels {
private:
	// objects and items
	TObjectType ObjectTypes[MAX_OBJECT_TYPES];
	int numObjectTypes;
	string ModelIndex;
	TItem Items[MAX_ITEMS];
	int numItems;

	// paging
	TPage DrawPages[MAX_DRAW_PAGES];
	int LastDrawPage;
	float DrawPageSize;
	TPage CollPages[MAX_COLL_PAGES];
	int LastCollPage;
	float CollPageSize;

	void AddPageItems (int pg, const TVector3 pos, const TVector3 vel);
	void AddItem (TItem &item);
	GLuint CrossTreeList;
	GLuint SpriteList;
	void MakeCrossTreeList ();
	void MakeSpriteList ();
	void DrawSprite (TItem *item, TControl *ctrl);
	void DrawTree (TItem *item, bool crosswise);
	void DrawModel (TItem *item);
	bool isDrawable (const TItem &item, TControl *ctrl);
	void CalcModelPolygons (TModel *mod, int itemidx);
	void CalcPseudoPolygons (int itemidx);
	void LoadModel (const char *filename, TModel *mod);
	void CalcPageIndices ();

	void CalcDrawPages ();
	int  GetDrawPage (float z);
	void CalcCollPages ();
	int  GetCollPage (float x, float z);

	bool LoadTrees (CSPList *list);

public:
	CModels ();
	~CModels ();

	void Init ();
	void DrawObjects (TControl *ctrl);
	bool LoadItems ();
	void ClearItems ();
	void ResetItems ();
	void LoadObjectTypes ();
	void ClearObjectTypes ();
	void AddModels (TControl *ctrl);
	void SetCollEnabled (int itemidx, bool enabled);
	void SetDrawEnabled (int itemidx, bool enabled);

	void ClearDrawPages ();	
	void ClearCollPages ();

};

extern CModels Models;

#endif
