// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "models.h"
#include "common.h"
#include "ogl.h"
#include "course.h"
#include "collision.h"
#include "textures.h"
#include "race.h"
#include "sound.h"

// --------------------------------------------------------------------
//				settings
// --------------------------------------------------------------------

const float NORMAL_TREE_SIZE = 6; 
const bool SHOW_TREEPOLY = false;
#define MIN_PAGE_SIZE 10

// for reading trees.png
#define TREE_MIN_SIZE 2
#define TREE_MAX_SIZE 3
#define SHRUB_MIN_SIZE 0.5
#define SHRUB_MAX_SIZE 2
#define BARREN_MIN_SIZE 4
#define BARREN_MAX_SIZE 6
const bool use_snowy_tree = true;

/* Instruction ---------------------------------------------------------
	after program start in main() you have to
	- init the CModels class (perhaps in constructor?)
	- load the object types (LoadObjectTypes)

	when loading the first course you have to
	- load the items (LoadItems)

	when loading another course you have to
	- dispose the items of the old course (ClearItems)
	- load the items of the new cours (LoadItems)

	before starting a new race on the same course you have to
	- reset the items (ResetItems)

	when terminating the program you should
	- dispose the object types (ClearObjectTypes, perhaps in destructor?)

	-------------------------------------------------------------------
	
	The objects which are collected (herrings) are checked in this module
	and not in collision.cpp, because this special collision detection
	is done by the bounding check and not by complete polygon check.
	You can adjust the hit distance with the param "radius" in 
	object_types.lst even if the object is a sprite. Remember that the
	default radius of sprites is 0.6.

	The collisions are collected in Race.herring. You can read and 
	output the value in loop.cpp or hud.cpp. Remember that you can 
	freely define the points with [collect] in object_types.lst.
	0 implicates a normal collision.
*/

//	class CModels:

CModels Models; // external declared

CModels::CModels () {
}

CModels::~CModels () {
}

void CModels::Init () {
	numObjectTypes = 0;
	numItems = 0;
	int i;

	for (i=0; i<MAX_OBJECT_TYPES; i++) {
		ObjectTypes[i].name = 0;
		ObjectTypes[i].model = 0;
	}
	for (i=0; i<MAX_ITEMS; i++) Items[i].polygons = 0;

	CrossTreeList = 0;
	MakeCrossTreeList ();
	SpriteList = 0;
	MakeSpriteList ();
//	numColliders = 0;
}

// material: 

typedef struct {
	float amb[4];
	float diff[4];
	float spec[4];	
} TMat;

static int curr_mat = 1;
static int numMaterials = 5;
static TMat ogl_material[5] = {
	{{0.2, 0.2, 0.2, 1.0}, {0.8, 0.8, 0.8, 1.0}, {0.0, 0.0, 0.0, 1.0}}, // 0
	{{0.5, 0.1, 0.1, 1.0}, {0.9, 0.3, 0.1, 1.0}, {0.0, 0.0, 0.0, 1.0}}, // 1
	{{0.5, 0.5, 0.5, 1.0}, {0.5, 0.5, 0.5, 1.0}, {0.0, 0.0, 0.0, 1.0}}, // 2
	{{0.2, 0.2, 0.6, 1.0}, {0.1, 0.3, 0.8, 1.0}, {0.9, 0.9, 0.9, 1.0}}, // 3
	{{0.1, 0.5, 0.1, 1.0}, {0.3, 0.8, 0.2, 1.0}, {0.0, 0.0, 0.0, 1.0}}  // 4
};
void SetDefMaterial (int nr) {
	if (nr != curr_mat && nr < numMaterials) {
		glMaterialfv (GL_FRONT, GL_AMBIENT, ogl_material[nr].amb);
		glMaterialfv (GL_FRONT, GL_DIFFUSE, ogl_material[nr].diff);
		glMaterialfv (GL_FRONT, GL_SPECULAR, ogl_material[nr].spec);
		curr_mat = nr;
	}
}

// Pages for drawing:

#define DRAW_FORWARD 70
#define DRAW_BACKWARD 20

int CModels::GetDrawPage (float z) {
	return -(int)(z / DrawPageSize);
}

void CModels::ClearDrawPages () {
	if (LastDrawPage < 0) return;
	for (int pg=0; pg<=LastDrawPage; pg++) {
		if (DrawPages[pg].numIdx > 0) free (DrawPages[pg].idx);
		DrawPages[pg].numIdx = 0;
	}
}

void CModels::CalcDrawPages () {
	int i, pg;
	float zpos, back, front;

	if (numItems < 1) return;

	if (Course.length < 5000) DrawPageSize = 50;
	else DrawPageSize = 100;
	LastDrawPage = (int)(Course.length / DrawPageSize);

	for (i=0; i<=LastDrawPage; i++) DrawPages[i].numIdx = 0;

	// pass one
	for (i=0; i<numItems; i++) {
		if (Items[i].draw) {
			zpos = -Items[i].ip.pos.z;
			for (pg=0; pg<=LastDrawPage; pg++) {
				back = pg * DrawPageSize;
				front = back + DrawPageSize;
				if ((zpos + Items[i].radius > back - DRAW_BACKWARD ) && 
					(zpos - Items[i].radius < front + DRAW_FORWARD)) {
					DrawPages[pg].numIdx++;
				}
			}
		}
	}

	// alocate memory
	for (i=0; i<=LastDrawPage; i++) {
		if (DrawPages[i].numIdx > 0) {
			DrawPages[i].idx = (int*) malloc (sizeof(int) * (DrawPages[i].numIdx + 1));			
			if (DrawPages[i].idx==0) Message ("malloc error", "CalcPageIndices for drawing");
		}
	}

	for (i=0; i<=LastDrawPage; i++) DrawPages[i].numIdx = 0;

	// pass two
	for (i=0; i<numItems; i++) {
		if (Items[i].draw) {
			zpos = -Items[i].ip.pos.z;
			for (pg=0; pg<=LastDrawPage; pg++) {
				back = pg * DrawPageSize;
				front = back + DrawPageSize;
				if ((zpos + Items[i].radius > back - DRAW_BACKWARD ) && 
					(zpos - Items[i].radius < front + DRAW_FORWARD)) {
					DrawPages[pg].idx[DrawPages[pg].numIdx] = i;
					DrawPages[pg].numIdx++;
				}
			}
		}
	}
}

//	Pages for collision:

#define COLL_FORWARD 1
#define COLL_BACKWARD 3

int CModels::GetCollPage (float x, float z) {
	return -(int)(z / CollPageSize);
}

void CModels::ClearCollPages () {
	if (LastCollPage < 0) return;
	for (int pg=0; pg<=LastCollPage; pg++) {
		if (CollPages[pg].numIdx > 0) free (CollPages[pg].idx);
		CollPages[pg].numIdx = 0;
	}
}

void CModels::CalcCollPages () {
	int i, pg;
	float zpos, back, front;

	if (numItems < 1) return;

	if (Course.length < 5000) CollPageSize = 10;
	else CollPageSize = 20;
	LastCollPage = (int)(Course.length / CollPageSize);

	for (i=0; i<=LastCollPage; i++) CollPages[i].numIdx = 0;

	// pass one
	for (i=0; i<numItems; i++) {
		if (Items[i].collide) {
			zpos = -Items[i].ip.pos.z;
			for (pg=0; pg<=LastCollPage; pg++) {
				back = pg * CollPageSize;
				front = back + CollPageSize;
				if ((zpos + Items[i].radius > back - COLL_BACKWARD ) && 
					(zpos - Items[i].radius < front + COLL_FORWARD)) {
					CollPages[pg].numIdx++;
				}
			}
		}
	}

	// alocate memory
	for (i=0; i<=LastCollPage; i++) {
		if (CollPages[i].numIdx > 0) {
			CollPages[i].idx = (int*) malloc (sizeof(int) * (CollPages[i].numIdx + 1));			
			if (CollPages[i].idx==0) Message ("malloc error", "CalcPageIndices for collection");
		}
	}

	for (i=0; i<=LastCollPage; i++) CollPages[i].numIdx = 0;

	// pass two
	for (i=0; i<numItems; i++) {
		if (Items[i].collide) {
			zpos = -Items[i].ip.pos.z;
			for (pg=0; pg<=LastCollPage; pg++) {
				back = pg * CollPageSize;
				front = back + CollPageSize;
				if ((zpos + Items[i].radius > back - COLL_BACKWARD ) && 
					(zpos - Items[i].radius < front + COLL_FORWARD)) {
					CollPages[pg].idx[CollPages[pg].numIdx] = i;
					CollPages[pg].numIdx++;
				}
			}
		}
	}
}

// drawing:

void CModels::MakeCrossTreeList () {
// create display list
	CrossTreeList = glGenLists (1);
	glNewList (CrossTreeList, GL_COMPILE);
	glBegin (GL_QUADS);
	    glTexCoord2f (0, 0); glVertex3f (-0.5, 0, 0);
    	glTexCoord2f (1, 0); glVertex3f (+0.5, 0, 0);
	    glTexCoord2f (1, 1); glVertex3f (+0.5, 1.0, 0);
 	    glTexCoord2f (0, 1); glVertex3f (-0.5, 1.0, 0);

	    glTexCoord2f (0, 0); glVertex3f (0, 0, +0.5);
	    glTexCoord2f (1, 0); glVertex3f (0, 0, -0.5);
	   	glTexCoord2f (1, 1); glVertex3f (0, 1.0, -0.5);
	    glTexCoord2f (0, 1); glVertex3f (0, 1.0, +0.5);		
	glEnd ();
	glEndList ();
}

void CModels::MakeSpriteList () {
	SpriteList = glGenLists (1);
	glNewList (SpriteList, GL_COMPILE);
	glBegin (GL_QUADS);
	    glTexCoord2f (0, 0); glVertex3f (-0.5, 0, 0);
    	glTexCoord2f (1, 0); glVertex3f (+0.5, 0, 0);
	    glTexCoord2f (1, 1); glVertex3f (+0.5, 1.0, 0);
 	    glTexCoord2f (0, 1); glVertex3f (-0.5, 1.0, 0);
	glEnd ();
	glEndList ();
}

void CModels::DrawModel (TItem *item) {
	int i, j;
	TVector3 nn, vv;
	TVector2 tt;

	SetGLOptions (MODELS);
	SetDefMaterial (1);
	glColor3f (1,1,1);

	if (item->draw == false) return;
	TModelPolygons *inst = item->polygons;

	glBindTexture (GL_TEXTURE_2D, item->texid);
	if (inst->numTriangles > 0) {
		glBegin (GL_TRIANGLES);
		for (i=0; i<inst->numTriangles; i++) {
			nn = inst->triangles[i].nml;
			glNormal3f (nn.x, nn.y, nn.z);	
			for (j=0; j<3; j++) {
 				vv = inst->triangles[i].vert[j];
				tt = inst->triangles[i].coeff[j];
				glTexCoord2f (tt.x, tt.y);
				glVertex3f (vv.x, vv.y, vv.z);
			}
		} 
		glEnd ();
	}

	if (inst->numQuads > 0) {
		glBegin (GL_QUADS);
		for (i=0; i<inst->numQuads; i++) {
			nn = inst->quads[i].nml;
			glNormal3f (nn.x, nn.y, nn.z);	
			for (j=0; j<4; j++) {
 				vv = inst->quads[i].vert[j];
				tt = inst->quads[i].coeff[j];
				glTexCoord2f (tt.x, tt.y);
				glVertex3f (vv.x, vv.y, vv.z);
			}
		} 
		glEnd ();
	}
}

void CModels::DrawTree (TItem *item, bool crosswise) { 
	if (item->draw == false) return;

	float treeRadius = item->ip.scale.x / 2;
	float treeHeight = item->ip.scale.y;
/*
	TVector3 normal = SubVec (ctrl[0].viewpos, item->ip.pos);
	NormVec (normal);
	glNormal3f (normal.x, normal.y, normal.z);
*/
	glBindTexture (GL_TEXTURE_2D, item->texid);
	glPushMatrix ();
		glTranslatef (item->ip.pos.x, item->ip.pos.y, item->ip.pos.z);	
		glRotatef    (item->ip.rot.x, 1, 0, 0);
		glRotatef    (item->ip.rot.y, 0, 1, 0);
		glRotatef    (item->ip.rot.z, 0, 0, 1);

		glBegin (GL_QUADS);
			glTexCoord2f (0.0, 0.0);
    	    glVertex3f (-treeRadius, 0.0, 0.0);
    	    glTexCoord2f (1.0, 0.0);
    	    glVertex3f (treeRadius, 0.0, 0.0);
    	    glTexCoord2f (1.0, 1.0);
    	    glVertex3f (treeRadius, treeHeight, 0.0);
    	    glTexCoord2f (0.0, 1.0);
    	    glVertex3f (-treeRadius, treeHeight, 0.0);
			    glTexCoord2f  (0.0, 0.0);
			    glVertex3f  (0.0, 0.0, -treeRadius);
			    glTexCoord2f  (1.0, 0.0);
			    glVertex3f  (0.0, 0.0, treeRadius);
			    glTexCoord2f  (1.0, 1.0);
			    glVertex3f  (0.0, treeHeight, treeRadius);
			    glTexCoord2f  (0.0, 1.0);
			    glVertex3f  (0.0, treeHeight, -treeRadius);
		glEnd();
	glPopMatrix ();
}

void CModels::DrawSprite (TItem *item, TControl *ctrl) {
	TVector3 normal;
	if (item->draw == false) return;

	float treeRadius = item->ip.scale.x / 2;
	float treeHeight = item->ip.scale.y;

	if (item->type == 4) {	
		normal = SubVec (ctrl->viewpos, item->ip.pos);
		normal.y = 0;
		NormVec (normal);
	}

	glBindTexture (GL_TEXTURE_2D, item->texid);
	glColor4f (1,1,1,1);
	glPushMatrix ();
		glTranslatef (item->ip.pos.x, item->ip.pos.y, item->ip.pos.z);	
		glRotatef    (item->ip.rot.x, 1, 0, 0);
		glRotatef    (item->ip.rot.y, 0, 1, 0);
		glRotatef    (item->ip.rot.z, 0, 0, 1);

		if (item->type == 4) {	// sprite, automatically orientated
			glBegin (GL_QUADS);
				glTexCoord2f (0.0, 0.0);
				glVertex3f (-treeRadius * normal.z, 0.0, treeRadius * normal.x);
				glTexCoord2f (1.0, 0.0);
				glVertex3f (treeRadius * normal.z, 0.0, -treeRadius * normal.x);
				glTexCoord2f (1.0, 1.0);
				glVertex3f (treeRadius * normal.z, treeHeight, -treeRadius * normal.x);
				glTexCoord2f (0.0, 1.0);
				glVertex3f (-treeRadius * normal.z, treeHeight, treeRadius * normal.x);
			glEnd ();
		} else { // sprite with solid orientation and adjusted rotation
			glBegin (GL_QUADS);
				glTexCoord2f (0.0, 0.0);
				glVertex3f (-treeRadius, 0.0, 0.0);
				glTexCoord2f (1.0, 0.0);
				glVertex3f (treeRadius, 0.0, 0.0);
				glTexCoord2f (1.0, 1.0);
				glVertex3f (treeRadius, treeHeight, 0.0);
				glTexCoord2f (0.0, 1.0);
				glVertex3f (-treeRadius, treeHeight, 0.0);
			glEnd ();
		}
	glPopMatrix ();
}

bool CModels::isDrawable (const TItem &item, TControl *ctrl) {
	float zpos = item.ip.pos.z;
	if (zpos > ctrl->pos.z - cfg.forwardclip 
		&& zpos < ctrl->pos.z + cfg.backwardclip
		&& (item.collect == 0 || cfg.show_herring)) return true;	
	else return false;
}

void CModels::DrawObjects (TControl *ctrl) { 
	int i, idx;

	// pass 1: draw models
	SetGLOptions (MODELS);

	int pg = GetDrawPage (ctrl->pos.z);
	for (i=0; i<DrawPages[pg].numIdx; i++)  {
		idx = DrawPages[pg].idx[i];
		if (isDrawable (Items[idx], ctrl)) {
			if (Items[idx].type == 1) DrawModel (&Items[idx]);
		}
	}

	// pass 2: draw trees and sprites
	SetGLOptions (TREES);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	SetMaterial (colWhite, colBlack, 1.0);
	glColor4f (1, 1, 1, 1);

	TVector3 normal = SubVec (ctrl->viewpos, ctrl->pos);
	NormVec (normal);
	glNormal3f (normal.x, normal.y, normal.z);

	for (i=0; i<DrawPages[pg].numIdx; i++)  {
		idx = DrawPages[pg].idx[i];
		if (isDrawable (Items[idx], ctrl)) {
			if (Items[idx].type == 2) DrawTree (&Items[idx], true);
			if (Items[idx].type == 3) DrawSprite (&Items[idx], ctrl);
			if (Items[idx].type == 4) DrawSprite (&Items[idx], ctrl);
		}
	}
}

// --------------------------------------------------------------------
//					items
// --------------------------------------------------------------------

void CModels::ClearItems () {
	int i;
	for (i=0; i<numItems; i++) {
		if (Items[i].polygons != 0) {
			if (Items[i].polygons->triangles != 0) {
				free (Items[i].polygons->triangles);
				Items[i].polygons->triangles = 0;
				Items[i].polygons->numTriangles = 0;
			}
			if (Items[i].polygons->quads != 0) {
				free (Items[i].polygons->quads);
				Items[i].polygons->quads = 0;
				Items[i].polygons->numQuads = 0;
			}
			free (Items[i].polygons);
			Items[i].polygons = 0;
		}	
	}
	numItems = 0;
}

void CModels::ResetItems () {
	TObjectType *objtype;

	for (int n=0; n<numItems; n++) {
		objtype = &ObjectTypes[Items[n].model];
		Items[n].collide   = objtype->collide; 
		Items[n].draw      = objtype->draw;
	}
}

void CModels::CalcModelPolygons (TModel *mod, int itemidx) {
// params: 0 pos, 1 trans, 2 rot, 3 scale
 	TMatrix matrix;
	TVector3 nnn;
	int i, j;
	int cnt = 0;
	int vidx[4];	
	TVector3 vvec[4];

	TModelPolygons *poly = Items[itemidx].polygons;
	TPositionParams ip = Items[itemidx].ip;
	
	if (mod == NULL || poly == NULL) {
		printf ("Error: Can't make item, pointer is 0");
		return;
	}

	poly->numTriangles = mod->numTriangles;
	poly->numQuads = mod->numQuads;
		
	if (poly->numTriangles > 0) {
		poly->triangles = (TTriangleC*) malloc (sizeof (TTriangleC) *poly->numTriangles);
	} else poly->triangles = 0;
	if (poly->numQuads > 0) { 
		poly->quads = (TQuadC*) malloc (sizeof (TQuadC) * poly->numQuads);
	} else poly->quads = 0;
	
	float xpos = ip.pos.x;
	float zpos = ip.pos.z;
	float ypos = ip.pos.y;
	glPushMatrix ();
		glLoadIdentity ();
		glTranslatef (xpos, ypos, zpos);	
		glRotatef (ip.rot.x, 1, 0, 0);
		glRotatef (ip.rot.y, 0, 1, 0);
		glRotatef (ip.rot.z, 0, 0, 1);
		glScalef (ip.scale.x, ip.scale.y, ip.scale.z);
		GetMatrix (matrix);
	glPopMatrix ();

	
	for (i=0; i<poly->numTriangles; i++) {
		for (j=0; j<3; j++) vidx[j] = mod->vertidx3[i][j];
		for (j=0; j<3; j++) vvec[j] = VectorMatrixProduct (matrix, mod->vertices[vidx[j]]);
		for (j=0; j<3; j++) poly->triangles[i].vert[j] = vvec[j];

		// texture coords:
		for (j=0; j<3; j++) poly->triangles[i].coeff[j] = mod->texcoord3[i].coord[j];
	
		nnn = CrossVec (SubVec (vvec[1], vvec[0]), SubVec (vvec[2], vvec[0]));
		NormVec (nnn);
		poly->triangles[i].nml = nnn;
		poly->triangles[i].pconst = -DotVec (vvec[0], nnn);
		poly->triangles[i].idx = itemidx;
		poly->triangles[i].frictcoeff = Items[itemidx].friction;
		poly->triangles[i].depth = Items[itemidx].depth;

		/* This is for optimizing: the planes perpendicular on the polygon edges
		are precalculated. That might result in a faster InsidePolygonCheck.
		TVector3 temp;
		temp = CrossVec (SubVec (vvec[0], vvec[1]), nnn);
		inst->triangles[i].nml[0] = temp;
		inst->triangles[i].pln[0] = -DotVec (vvec[0], temp);
	
		temp = CrossVec (SubVec (vvec[1], vvec[2]), nnn);
		inst->triangles[i].nml[1] = temp;
		inst->triangles[i].pln[1] = -DotVec (vvec[1], temp);
	
		temp = CrossVec (SubVec (vvec[2], vvec[0]), nnn);
		inst->triangles[i].nml[2] = temp;
		inst->triangles[i].pln[2] = -DotVec (vvec[2], temp); */		
		cnt++;
	}

	for (i=0; i<poly->numQuads; i++) {
		for (j=0; j<4; j++) vidx[j] = mod->vertidx4[i][j];
		for (j=0; j<4; j++) vvec[j] = VectorMatrixProduct (matrix, mod->vertices[vidx[j]]);
		for (j=0; j<4; j++) poly->quads[i].vert[j] = vvec[j];

		// texture coords:
		for (j=0; j<4; j++) poly->quads[i].coeff[j] = mod->texcoord4[i].coord[j];
	
		nnn = CrossVec (SubVec (vvec[1], vvec[0]), SubVec (vvec[2], vvec[0]));
		NormVec (nnn);
		poly->quads[i].nml = nnn;
		poly->quads[i].pconst = -DotVec (vvec[0], nnn);
		poly->quads[i].idx = itemidx;
		poly->quads[i].frictcoeff = Items[itemidx].friction;
		poly->quads[i].depth = Items[itemidx].depth;
	
		/* temp = CrossVec (SubVec (vvec[0], vvec[1]), nnn);
		inst->quads[i].nml[0] = temp;
		inst->quads[i].pln[0] = -DotVec (vvec[0], temp);
	
		temp = CrossVec (SubVec (vvec[1], vvec[2]), nnn);
		inst->quads[i].nml[1] = temp;
		inst->quads[i].pln[1] = -DotVec (vvec[1], temp);
	
		temp = CrossVec (SubVec (vvec[2], vvec[3]), nnn);
		inst->quads[i].nml[2] = temp;
		inst->quads[i].pln[2] = -DotVec (vvec[2], temp);
	
		temp = CrossVec (SubVec (vvec[3], vvec[0]), nnn);
		inst->quads[i].nml[3] = temp;
		inst->quads[i].pln[3] = -DotVec (vvec[3], temp); */	
		cnt++;
	}
}

void CModels::CalcPseudoPolygons (int itemidx) {
	TMatrix matrix;
	TVector3 vvec[4];
	TVector3 nvec[4];
	TVector3 nnn;
	int i;

	TModelPolygons *poly = Items[itemidx].polygons;
	TPositionParams ip = Items[itemidx].ip;
	if (poly == NULL) {
		printf ("Error: Can't calc pseudo polygons, pointer is 0");
		return;
	}

	glPushMatrix ();
		glLoadIdentity ();
		glTranslatef (ip.pos.x, ip.pos.y, ip.pos.z);	
		glRotatef (ip.rot.x, 1, 0, 0);
		glRotatef (ip.rot.z, 0, 0, 1);
		glScalef (ip.scale.x, ip.scale.y, ip.scale.z);
		GetMatrix (matrix);
	glPopMatrix ();

	// ----------------------------------------------------------------
	// calculate pseudo quads for the trunc
	// ----------------------------------------------------------------
	const float truncheight = 1.0;
	const float truncwidth = 0.025;

	poly->numQuads = 1;
	poly->quads = (TQuadC*) malloc (sizeof (TQuadC) * poly->numQuads);

	vvec[0] = MakeVec (-truncwidth, 0.0, 0.0);
	vvec[1] = MakeVec (+truncwidth, 0.0, 0.0);
	vvec[2] = MakeVec (+truncwidth, truncheight, 0.0);
	vvec[3] = MakeVec (-truncwidth, truncheight, 0.0);

	for (i=0; i<4; i++) nvec[i] = VectorMatrixProduct (matrix, vvec[i]);
	for (i=0; i<4; i++) poly->quads[0].vert[i] = nvec[i];

	nnn = CrossVec (SubVec (nvec[1], nvec[0]), SubVec (nvec[2], nvec[0]));
	NormVec (nnn);
	poly->quads[0].nml = nnn;
	poly->quads[0].pconst = -DotVec (nvec[0], nnn);
	poly->quads[0].idx = itemidx;
	poly->quads[0].depth = 0;
	poly->quads[0].frictcoeff = 0.5;

	// ----------------------------------------------------------------
	// calculate triangles for the treetop
	// ----------------------------------------------------------------
	const float topbottom = 0.35;  // lower border of tree trunc

	poly->numTriangles = 2;
	poly->triangles = (TTriangleC*) malloc (sizeof (TQuadC) * poly->numTriangles);

	vvec[0] = MakeVec (0.0, topbottom, 0.0);
	vvec[1] = MakeVec (0.0, 1.0, 0.0);
	vvec[2] = MakeVec (-0.5, topbottom, -0.5);

	for (i=0; i<3; i++) nvec[i] = VectorMatrixProduct (matrix, vvec[i]);
	for (i=0; i<3; i++) poly->triangles[0].vert[i] = nvec[i];
	nnn = CrossVec (SubVec (nvec[1], nvec[0]), SubVec (nvec[2], nvec[0]));
	NormVec (nnn);
	poly->triangles[0].nml = nnn;
	poly->triangles[0].pconst = -DotVec (nvec[0], nnn);
	poly->triangles[0].idx = itemidx;

	// ----------------------------------------------------------------

	vvec[0] = MakeVec (0.0, topbottom, 0.0);
	vvec[1] = MakeVec (0.5, topbottom, -0.5);
	vvec[2] = MakeVec (0.0, 1.0, 0.0);

	for (i=0; i<3; i++) nvec[i] = VectorMatrixProduct (matrix, vvec[i]);
	for (i=0; i<3; i++) poly->triangles[1].vert[i] = nvec[i];
	nnn = CrossVec (SubVec (nvec[1], nvec[0]), SubVec (nvec[2], nvec[0]));
	NormVec (nnn);
	poly->triangles[1].nml = nnn;
	poly->triangles[1].pconst = -DotVec (nvec[0], nnn);
	poly->triangles[1].idx = itemidx;
}

int GetObject (unsigned char pixel[]) {
	int r = pixel[0];
	int g = pixel[1];
	int b = pixel[2];

	if (r<150 && b>200) return 0;
	if (abs(r-194)<10 && abs(g-40)<10 && abs(b-40)<10) return 1;
	if (abs(r-128)<10 && abs(g-128)<10 && b<10) return 2;
	if (r>220 && g>220 && b<20) return 3;
	if (r>220 && abs(g-128)<10 && b>220) return 4;
	if (r>220 && g>220 && b>220) return 5;
	if (r>220 && abs(g-96)<10 && b<40) return 6;
	if (r<40 && g >220 && b<80) return 7;
	return -1;	
}


bool CModels::LoadTrees (CSPList *list) {
    Image treeImg;
    int x,y;
	int type, imgidx;
	string line;
	TVector3 pos, scale, rot;		
	float fact;
	int nx = Course.nx;
	int ny = Course.ny;
	if (!treeImg.LoadPng (Course.dir.c_str(), "trees.png")) return false;
	
	int pad = 0;
	for (y=0; y<ny; y++) {
		for (x=0; x<nx; x++) {
            imgidx = (x + nx * y) * treeImg.depth + pad;
			type = GetObject (&treeImg.data[imgidx]);


			if (type==0 || type==1 || type==5 || type==6 || type==7) {
				pos.x = (nx - x) / (float)(nx - 1.0) * Course.width;
				pos.z = -(ny - y) / (float)(ny - 1.0) * Course.length;
				pos.y = 0;
				scale = MakeVec (1, 1, 1);
				rot = MakeVec (0, 0, 0);
	
				switch (type) {
					case 0: line = "*[name] herring"; break;
					case 1: line = "*[name] flag"; break;
					case 5: 
						if (use_snowy_tree) line = "*[name] snowy_tree"; 
						else line = "*[name] green_tree"; 
						fact = xrandom (TREE_MIN_SIZE, TREE_MAX_SIZE);
						scale = ScaleVec (fact, scale);	
						scale.y = scale.y * xrandom (0.8, 1.3);
						rot.y = xrandom (0, 90);
						rot.z = xrandom (-5, 5);
						break;
					case 6: 
						line = "*[name] tree_barren"; 
						fact = xrandom (BARREN_MIN_SIZE, BARREN_MAX_SIZE);
						rot.y = xrandom (0, 90);
						scale = ScaleVec (fact, scale);
						break;
					case 7: 
						line = "*[name] shrub"; 
						fact = xrandom (SHRUB_MIN_SIZE, SHRUB_MAX_SIZE);
						rot.y = xrandom (0, 90);
						scale = ScaleVec (fact, scale);
						break;
				}
	
				SPAddVec3N (line, "pos", pos, 1);
				SPAddVec3N (line, "scale", scale, 1);
				SPAddVec3N (line, "rotation", rot, 0);
				list->Add (line);
			}
		} 
        pad += (nx * treeImg.depth) % 4;
	} 
	if (cfg.save_itemslist) {
		list->Save (Course.dir.c_str(), "items.lst");
		Message ("create and save items.lst","");
	}
	return true;
}

bool CModels::LoadItems () {  
	CSPList list(MAX_ITEMS);
	string line;
	int i, n, idx;
	string item_name;
	TObjectType *objtype;
	TPositionParams *ip;

	ClearItems ();	

	// first try to read items.lst. If not available read trees.png first,
	// generate a items.lst and store it immediately
	if (!list.Load (Course.dir.c_str(), "items.lst")) {
		Message ("cannot load items.lst", "- try to read trees.png");
		if (!LoadTrees (&list)) {
			Message ("cannot load", "trees.png");
			return false;
		}
	}

	// now parsing the list:
	for (i=0; i<list.Count(); i++) {
		line = list.Line (i);
		item_name = SPStrN (line, "name", "");	
		idx = SPIntN (ModelIndex, item_name, -1); // index to model array

		if (idx>=0 && idx<numObjectTypes) {
			n = numItems;

			objtype = &ObjectTypes[idx];
			Items[n].model     = idx;
			Items[n].texid     = objtype->texid;
				
			// position params from item file
			Items[n].ip.pos    = SPVector3N (line, "pos");
			Items[n].ip.trans  = SPVector3N (line, "translation");
			Items[n].ip.rot    = SPVector3N (line, "rotation");
			Items[n].ip.scale  = SPVector3N (line, "scale");

			// main props from objtype:
			Items[n].type      = objtype->type;
			Items[n].collide   = objtype->collide; 
			Items[n].draw      = objtype->draw;
			Items[n].collect   = objtype->collect;
			
			// values from objtype:
			Items[n].mat       = objtype->mat;
			Items[n].depth     = objtype->depth;
			Items[n].friction  = objtype->friction;
			Items[n].polygons  = 0;

//			if (Items[n].type == 2) Items[n].ip.scale = 
//				ScaleVec (NORMAL_TREE_SIZE, Items[n].ip.scale);
			
			if (Items[n].ip.scale.x > Items[n].ip.scale.z) {
				Items[n].radius = objtype->radius * Items[n].ip.scale.x;
			} else Items[n].radius = objtype->radius * Items[n].ip.scale.z;
			
			// compute the real position
			ip = &Items[n].ip;
			ip->pos.x += ip->trans.x;
			ip->pos.z += ip->trans.z;
			ip->pos.y =	Course.GetYCoord (ip->pos) + ip->trans.y;

			// for 3d models all vertices are calculated
			if (Items[n].type==1) {
				Items[n].polygons = (TModelPolygons*) malloc (sizeof (TModelPolygons));
				CalcModelPolygons (objtype->model, n);
			} 

			if (Items[n].type==2) {
				Items[n].polygons = (TModelPolygons*) malloc (sizeof (TModelPolygons));
				CalcPseudoPolygons (n);
			}
			numItems++;
		}
	}
	CalcDrawPages ();
	CalcCollPages ();
	ResetItems ();	
	return true;
}

// object types:

void CModels::ClearObjectTypes () { 
	int i;
	for (i=0; i<numObjectTypes; i++) {
		free (ObjectTypes[i].name);
		if (ObjectTypes[i].model != 0) {
			free (ObjectTypes[i].model->vertices);
			ObjectTypes[i].model->vertices = 0;
			ObjectTypes[i].model->numVertices = 0;

			free (ObjectTypes[i].model->vertidx3);
			ObjectTypes[i].model->vertidx3 = 0;
			free (ObjectTypes[i].model->texcoord3);
			ObjectTypes[i].model->texcoord3 = 0;
			ObjectTypes[i].model->numTriangles = 0;

			free (ObjectTypes[i].model->vertidx4);
			ObjectTypes[i].model->vertidx4 = 0;
			free (ObjectTypes[i].model->texcoord4);
			ObjectTypes[i].model->texcoord4 = 0;
			ObjectTypes[i].model->numQuads = 0;

			free (ObjectTypes[i].model);
		}
	}
	numObjectTypes = 0;
}

void CModels::LoadModel (const char *filename, TModel *mod) {
	CSPList list (1000);
	string line;
	int type, i, n;
	TVector3 tri;
	TVector4 quad;
	int usedVertices=0, usedTriangles=0, usedQuads=0;
		
	if (mod == NULL) {
		printf ("Can't load model, pointer is NULL");
		return;
	}
	
	if (!list.Load (cfg.objectdir, filename)) {
		Message ("Couldn't load model list");
		return;
	}

	for (i=0; i<list.Count(); i++) {
		line = list.Line (i);
		type = SPIntN (line, "S", 99);
			
		// read header to get the size for malloc
		if (type==0) {
			usedVertices = SPIntN (line, "vertices", 0);
			usedTriangles = SPIntN (line, "triangles", 0);
			usedQuads = SPIntN (line, "quads", 0);
			break;
		}
	}

	// allocate memory
	if (usedVertices > 0) {
		mod->vertices = (TVector3*) malloc (sizeof (TVector3) * usedVertices);		
	}
	if (usedTriangles > 0) {
		mod->vertidx3 = (TVIndex3*) malloc (sizeof (TVIndex3) * usedTriangles);		
		mod->texcoord3 = (TTexCoord3*) malloc (sizeof (TTexCoord3) * usedTriangles);
	}
	if (usedQuads > 0) {
		mod->vertidx4 = (TVIndex4*) malloc (sizeof (TVIndex4) * usedQuads);		
		mod->texcoord4 = (TTexCoord4*) malloc (sizeof (TTexCoord4) * usedQuads);
	}

	// read data
	for (i=0; i<list.Count(); i++) {
		line = list.Line (i);
		type = SPIntN (line, "S", 99);
						
		// read vertices
		if (type==1) {	
			if (mod->numVertices < usedVertices) {
				mod->vertices[mod->numVertices] = SPVector3N (line, "v");
				mod->numVertices++;
			}		
		// read triangles
		} else if (type==2) {  
			n = mod->numTriangles;
			if (n < usedTriangles) {
				tri = SPVector3N (line, "v");
				mod->vertidx3[n][0] = (int)tri.x;
				mod->vertidx3[n][1] = (int)tri.y;
				mod->vertidx3[n][2] = (int)tri.z;
				mod->texcoord3[n].coord[0] = SPVector2N (line, "t1");
				mod->texcoord3[n].coord[1] = SPVector2N (line, "t2");
				mod->texcoord3[n].coord[2] = SPVector2N (line, "t3");
				mod->numTriangles++;
			}
		// read quads
		} else if (type == 3) {
			n = mod->numQuads;
			if (n < usedQuads) {
				quad = SPVector4N (line, "v", MakeVec4 (0,0,0,0));
				mod->vertidx4[n][0] = (int)quad.x;
				mod->vertidx4[n][1] = (int)quad.y;
				mod->vertidx4[n][2] = (int)quad.z;
				mod->vertidx4[n][3] = (int)quad.w;
				mod->texcoord4[n].coord[0] = SPVector2N (line, "t1");
				mod->texcoord4[n].coord[1] = SPVector2N (line, "t2");
				mod->texcoord4[n].coord[2] = SPVector2N (line, "t3");
				mod->texcoord4[n].coord[3] = SPVector2N (line, "t4");
				mod->numQuads++;
			}				
		}
	}
}

void CModels::LoadObjectTypes () { 
	string EnumStr
	("[no]0[none]0[false]0[true]1[yes]1[model]1[tree]2[sprite]3[autosprite]4[0]0[1]1");
	SetEnum (EnumStr);
	CSPList list(MAX_OBJECT_TYPES);
	string objfile;
	string texfile;
	string billfile;
	string line;
	int i=0;

	//	ClearObjectTypes ();
	if (list.Load (cfg.objectdir, "object_types.lst")) {
		for (i=0; i<list.Count(); i++) {
			line = list.Line (i);

			// main properties
			ObjectTypes[i].type = SPEnumN (line, "type", 0);
			ObjectTypes[i].draw = SPEnumN (line, "visible", 1);	
			ObjectTypes[i].collect = SPIntN (line, "collect", 0);		
			ObjectTypes[i].collide = SPEnumN (line, "collide", 1);
			if (ObjectTypes[i].type == 3) ObjectTypes[i].collide = 0; // sprites
			if (ObjectTypes[i].collect > 0) ObjectTypes[i].collide = true;

			// values
			ObjectTypes[i].mat = SPIntN (line, "mat", 0);	
			ObjectTypes[i].radius = SPFloatN (line, "radius", 0.6);
			ObjectTypes[i].friction = SPFloatN (line, "friction", 0.5);
			ObjectTypes[i].depth = SPFloatN (line, "depth", 0.0);

			ObjectTypes[i].name = SPNewCharN (line, "name");						
			ObjectTypes[i].texid = 0;
			texfile = SPStrN (line, "texture", "");
			if (texfile.size() > 0)	
				ObjectTypes[i].texid = 
				LoadTexture (cfg.objectdir.c_str(), texfile.c_str(), false);		

			// billboard is used for models drawn with polygons
			ObjectTypes[i].billboard = 0;
			if (ObjectTypes[i].draw==1) {
				billfile = SPStrN (line, "billboard", "");
				if (billfile.size() > 0)	
					ObjectTypes[i].billboard = 
					LoadTexture (cfg.objectdir.c_str(), billfile.c_str(), false);		
			}

			// load the geometrical description	if type = 3d model	
			if (ObjectTypes[i].type==1) {
				ObjectTypes[i].model = (TModel*) malloc (sizeof (TModel));
				ObjectTypes[i].model->numVertices = 0;				
				ObjectTypes[i].model->numTriangles = 0;				
				ObjectTypes[i].model->numQuads = 0;				
			
				objfile = SPStrN (line, "modeldesc", "");						
				LoadModel (objfile.c_str(), ObjectTypes[i].model);
			} else ObjectTypes[i].model = 0;
		}
	} else Message ("Couldn't load object types");

	numObjectTypes = i;
	list.MakeIndex (ModelIndex, "name");
}

// ----------------------- collision: ---------------------------------

void CModels::SetCollEnabled (int itemidx, bool enabled) {
	if (itemidx < 0 || itemidx >= numItems) return;
	Items[itemidx].collide = enabled;
}

void CModels::SetDrawEnabled (int itemidx, bool enabled) {
	if (itemidx < 0 || itemidx >= numItems) return;
	Items[itemidx].draw = enabled;
}

void CModels::AddItem (TItem &item) {
	TModelPolygons *poly;
	int p;

	poly = item.polygons;
	if (poly!=0) {
		// collison with polygons:
		if (item.type==1) {
			for (p=0; p<poly->numTriangles; p++) {
				Collision.AddModelTriangle (poly->triangles[p]);			
			}
			for (p=0; p<poly->numQuads; p++) {
				Collision.AddModelQuad (poly->quads[p]);			
			}
		}
		// tree collision:
		if (item.type==2) { 
			Collision.AddModelQuad (poly->quads[0]);	
			Collision.AddTreeTriangle (poly->triangles[0]);	
			Collision.AddTreeTriangle (poly->triangles[1]);	
		}
	}
}

void CModels::AddPageItems (int pg, const TVector3 pos, const TVector3 vel) {
	int idx;
	TVector2 aa;
	float colldist;

	int num = CollPages[pg].numIdx;
	if (num < 1) return;

	int numcoll = 0;
	for (int i=0; i<num; i++) {
		idx = CollPages[pg].idx[i];

		if (Items[idx].collide) {
			aa.x = Items[idx].ip.pos.x - pos.x;
			aa.y = Items[idx].ip.pos.z - pos.z;
			float dist = sqrt (aa.x * aa.x + aa.y * aa.y);
			float vellng = sqrt (vel.x * vel.x + vel.z * vel.z);

			bool collpossible = (vellng + Items[idx].radius + adj.radius) > dist;
			if (collpossible) {
				// special collision with collected objects
				// now we respect the direction of vel
				if (Items[idx].collect > 0) {
					colldist = aa.x * vel.x + aa.y * vel.z;
					if (colldist + Items[idx].radius + adj.radius > dist) {
						Sound.Play ("pickup", 0);
						Items[idx].draw = false;
						Items[idx].collide = false;
						Race.herring += Items[idx].collect;
					}
				} else {
					// normal collision
					AddItem (Items[idx]);
					numcoll++;
				}
			}
		}
	}
}

void CModels::AddModels (TControl *ctrl) {
	Collision.ResetModelPolygons ();
	Collision.ResetTreePolygons ();
	int pg = GetCollPage (1, ctrl->pos.z);
	AddPageItems (pg, ctrl->pos, ctrl->vel);
}









