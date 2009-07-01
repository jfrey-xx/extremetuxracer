#include "treegen.h"
#include "textures.h"
#include "ogl.h"
#include "course.h"

// --------------------------------------------------------------------
//					settings
// --------------------------------------------------------------------

const int MIN_BRANCHES = 4;		// variation of number per bundle
const int MAX_BRANCHES = 6;	
const float MIN_ZANGLE = -5.0;	// variation of up-down-angle
const float MAX_ZANGLE = 5.0;
const float MIN_LENGTH = -0.1;	// variation of length of branches
const float MAX_LENGTH = 0.0;
const float MIN_HEIGHT = -0.0;	// variation of height of branches
const float MAX_HEIGHT = 0.2;
const float MIN_WIDTH = -0.0;	// variation of width of branches
const float MAX_WIDTH = 0.2;

typedef struct {
	TVector3 vert[3];
	TVector2 tex[3];
	TVector3 nml;
	float pconst;
	float frictcoeff;
	float depth;
	int idx;
} TTriangleCX;

typedef struct {
	TTriangleCX tri[2];
} TBranch;

static TBranch branch[36];
static int numBranches = 0;
static GLuint treetex;
static TBranch br;
static TVector3 vert[400];
static int numVert = 0;
static TIndex3 idx[200];
static int numIdx = 0;
static TIndex4 idx4[40];
static int numIdx4 = 0;

void DrawBranch (TBranch branch) {
	TVector3 vv;
	TVector2 tt;
	int j;

	glColor3f (1, 1, 1);

	glBegin (GL_TRIANGLES);
		for (j=0; j<3; j++) {
			vv = branch.tri[0].vert[j];
			tt = branch.tri[0].tex[j];
			glTexCoord2f (tt.x, tt.y); glVertex3f (vv.x, vv.y, vv.z);
		}

		for (j=0; j<3; j++) {
			vv = branch.tri[1].vert[j];
			tt = branch.tri[1].tex[j];
			glTexCoord2f (tt.x, tt.y); glVertex3f (vv.x, vv.y, vv.z);
		}
	glEnd ();	
}


void DrawTreegen () {
	TVector3 pos = MakeVec (45, 0, -465);
	pos.y = Course.GetYCoord (pos)-0.5;

	glBindTexture (GL_TEXTURE_2D, treetex);

	glPushMatrix ();
	glTranslatef (pos.x, pos.y, pos.z);	

	SetGLOptions (MODELS);
	glColor3f (1, 1, 1);
	glNormal3f (0, 0, 1);	
	glBegin (GL_QUADS);
		glTexCoord2f (0, 1); glVertex3f   (-0.1, 0, 0.1);
		glTexCoord2f (0, 0.9); glVertex3f (+0.1, 0, 0.1);
		glTexCoord2f (1, 0.9); glVertex3f (+0.03, 5, 0.1);
		glTexCoord2f (1, 1); glVertex3f   (-0.03, 5, 0.1);
	glEnd ();


	SetGLOptions (MODELS);
 	glScalef (3, 3, 3);
	for (int i=0; i<numBranches; i++) DrawBranch (branch[i]);
	glPopMatrix ();
}

void InitTreegen () {
	treetex = LoadTexture ("data", "treegen2.png", false);
	GenerateTree ();
}

void GenerateBranch (float xrot, float yrot, float zrot, float height, 
	float xscale, float yscale, float zscale) {
	TMatrix matrix;
	int j;

	int num = numBranches;
	glPushMatrix ();
		glLoadIdentity ();
		glTranslatef (0, height, 0);	
		glRotatef (xrot, 1, 0, 0);
		glRotatef (yrot, 0, 1, 0);
		glRotatef (zrot, 0, 0, 1);
		glScalef (xscale, yscale, zscale);
		GetMatrix (matrix);
	glPopMatrix ();
	for (j=0; j<3; j++) branch[num].tri[0].vert[j] = 
		VectorMatrixProduct (matrix, br.tri[0].vert[j]);
	for (j=0; j<3; j++) branch[num].tri[1].vert[j] = 
		VectorMatrixProduct (matrix, br.tri[1].vert[j]);
	for (j=0; j<3; j++) branch[num].tri[0].tex[j] = br.tri[0].tex[j];
	for (j=0; j<3; j++) branch[num].tri[1].tex[j] = br.tri[1].tex[j];


	int n[4];
	vert[numVert] = branch[num].tri[0].vert[0];
	n[0] = numVert;	numVert++;

	vert[numVert] = branch[num].tri[0].vert[1];
	n[1] = numVert;	numVert++;

	vert[numVert] = branch[num].tri[0].vert[2];
	n[2] = numVert;	numVert++;

	vert[numVert] = branch[num].tri[1].vert[2];
	n[3] = numVert;	numVert++;

	idx[numIdx].i = n[0];
	idx[numIdx].j = n[1];
	idx[numIdx].k = n[2];
	numIdx++;

	idx[numIdx].i = n[0];
	idx[numIdx].j = n[2];
	idx[numIdx].k = n[3];
	numIdx++;

	numBranches++;
}

void GenerateBundle (float height, float width, float angle) {
	int i;
	float yangle;
	float zangle;
	float hh, wwx, wwy, wwz;
	
	int num = irandom (MIN_BRANCHES, MAX_BRANCHES);
	float baseangle = xrandom (0, 70);
	for (i=0; i<num; i++) {
		yangle = baseangle + 360 / num * i;
		zangle = angle + xrandom (MIN_ZANGLE, MAX_ZANGLE);
		hh = height + xrandom (-0.0, 0.0);
		wwx = width + xrandom (MIN_LENGTH, MAX_LENGTH);
		wwy = width + xrandom (MIN_HEIGHT, MAX_HEIGHT);
		wwz = width + xrandom (MIN_WIDTH, MAX_WIDTH);
		GenerateBranch (0, yangle, -zangle, hh, wwx, wwy, wwz);
	}
}

void GenerateTrunc () {
	int n[16];

	vert[numVert] = MakeVec (-0.04, 0.0, 0.04);
	n[0] = numVert;	numVert++;
	vert[numVert] = MakeVec ( 0.04, 0.0, 0.04);
	n[1] = numVert;	numVert++;

	vert[numVert] = MakeVec ( 0.01, 1.7, 0.01);
	n[2] = numVert;	numVert++;
	vert[numVert] = MakeVec (-0.01, 1.7, 0.01);
	n[3] = numVert;	numVert++;

	vert[numVert] = MakeVec (-0.04, 0.0, -0.04);
	n[4] = numVert;	numVert++;
	vert[numVert] = MakeVec ( 0.04, 0.0, -0.04);
	n[5] = numVert;	numVert++;

	vert[numVert] = MakeVec ( 0.01, 1.7, -0.01);
	n[6] = numVert;	numVert++;
	vert[numVert] = MakeVec (-0.01, 1.7, -0.01);
	n[7] = numVert;	numVert++;

	idx4[numIdx4].i = n[0];
	idx4[numIdx4].j = n[1];
	idx4[numIdx4].k = n[2];
	idx4[numIdx4].l = n[3];
	numIdx4++;	
	idx4[numIdx4].i = n[1];
	idx4[numIdx4].j = n[5];
	idx4[numIdx4].k = n[6];
	idx4[numIdx4].l = n[2];
	numIdx4++;	
	idx4[numIdx4].i = n[5];
	idx4[numIdx4].j = n[4];
	idx4[numIdx4].k = n[7];
	idx4[numIdx4].l = n[6];
	numIdx4++;	
	idx4[numIdx4].i = n[4];
	idx4[numIdx4].j = n[0];
	idx4[numIdx4].k = n[3];
	idx4[numIdx4].l = n[7];
	numIdx4++;	
}


void GenerateTree () {
	br.tri[0].vert[0] = MakeVec (0, 0, 0);
	br.tri[0].vert[1] = MakeVec (0.25, -0.175, 0.175);
	br.tri[0].vert[2] = MakeVec (1, 0, 0);

	br.tri[1].vert[0] = MakeVec (0, 0, 0);
	br.tri[1].vert[1] = MakeVec (1, 0, 0);
	br.tri[1].vert[2] = MakeVec (0.25, -0.175, -0.175);

	br.tri[0].tex[0] = MakeVec2 (0, 0.25);
	br.tri[0].tex[1] = MakeVec2 (0.25, 0);
	br.tri[0].tex[2] = MakeVec2 (1, 0.25);

	br.tri[1].tex[0] = MakeVec2 (0, 0.25);
	br.tri[1].tex[1] = MakeVec2 (1, 0.25);
	br.tri[1].tex[2] = MakeVec2 (0.25, 0.5);

	GenerateBundle (0.75, 0.75, 30);
	GenerateBundle (1.05, 0.70, 35);
	GenerateBundle (1.30, 0.5, 35);
	GenerateBundle (1.45, 0.35, 35);
	GenerateBundle (1.60, 0.25, 50);

	GenerateTrunc ();
}

void SaveTreegen () {
	string line;
	string item;
	int i;

	PrintInt (numVert);
	PrintInt (numIdx);

	CSPList list (1000);
	list.Add ("# tree description generated by treegen");
	list.Add ("");

	line = "*[S] 0 [vertices] ";
	line += Int_StrN (numVert);
	line += " [triangles] ";
	line += Int_StrN (numIdx);
	line += " [quads] ";
	line += Int_StrN (numIdx4);
	line += " [tex] 1 [mat] 1 [smooth] 0";
	list.Add (line);
	list.Add ("");
	list.Add ("# vertices:");
	for (i=0; i<numVert; i++) {
		line = "*[S] 1";
		SPAddVec3N (line, "v", vert[i], 2);
		list.Add (line);
	}

	list.Add ("");
	list.Add ("# triangles:");
	for (i=0; i<numIdx; i++) {
		line = "*[S] 2";
		SPAddIndx3N (line, "v", idx[i]);
	    if  (i % 2 == 0) {				
			line += " [t1] 0.00 0.25 [t2] 0.25 0.0 [t3] 1.00 0.25";
		} else {
			line += " [t1] 0.00 0.25 [t2] 1.00 0.25 [t3] 0.25 0.50";
		}
		list.Add (line);
	}

	list.Add ("");
	list.Add ("# quads:");
	for (i=0; i<numIdx4; i++) {
		line = "*[S] 3";
		SPAddIndx4N (line, "v", idx4[i]);
		line += " [t1] 0.0 1.0 [t2] 0.0 0.9 [t3] 1.0 0.9 [t4] 1.0 1.0";
		list.Add (line);
	}
	list.Save ("data", "treegen.lst");
}





















