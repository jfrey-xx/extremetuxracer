// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
//
// --------------------------------------------------------------------

#include "tux.h"
#include "ogl.h"
#include "race.h"
#include "player.h"

// --------------------------------------------------------------------
//					settings
// --------------------------------------------------------------------

#define USE_CHAR_DISPLAY_LIST true
#define MIN_SPHERE_DIV 3
#define MAX_SPHERE_DIV 16

#define MAX_ARM_ANGLE 30.0
#define MAX_PADDLING_ANGLE 35.0
#define MAX_EXT_PADDLING_ANGLE 30.0
#define MAX_KICK_PADDLING_ANGLE 20.0
#define PADDLING_DURATION 0.40  

const float C_BRAKING_ROLL = 50.0;
const float C_NORMAL_ROLL = 30.0;
const float C_AIR_TIME = 0.3;
const float C_GROUND_TIME = 0.12;

// --------------------------------------------------------------------
//					data
// --------------------------------------------------------------------

#define	MAX_CHAR_NODES 128
#define	MAX_MAT2 64

TCharNode2 *CharNodes2 [MAX_CHAR_NODES];
static int  numCharNodes2 = 0;
static string NodeIndex2;

TTuxMaterial2 *CharMaterials2 [MAX_MAT2];
static int    numCharMat = 0;
static string MatIndex2;

TTuxMaterial2 TuxDefMat = {{0.5, 0.5, 0.5, 1.0}, {0.0, 0.0, 0.0, 1.0}, 0.0};

// ********************************************************
void ShowNode2 (int idx) {
    TCharNode2 *node;
	node = (TCharNode2*) CharNodes2[idx];
	Message ("name:  ", node->name);
	if (node->parent != 0) {
		TCharNode2 *par = node->parent;
		Message ("parent:", par->name);
	} else Message ("parent:", "------");
	if (node->child != 0) {
		TCharNode2 *ch = node->child;
		Message ("child: ", ch->name);
	} else Message ("child: ", "------");
	if (node->next != 0) {
		TCharNode2 *next = node->next;
		Message ("next:  ", next->name);
	} else Message ("next:  ", "------");
	PrintMatrix (node->trans);
	PrintStr ("");
}
// ********************************************************

// --------------------------------------------------------------------
//					material
// --------------------------------------------------------------------

bool GetCharMaterial2 (char *mat_name, TTuxMaterial2 **mat) {
	int idx;
	idx = SPIntN (MatIndex2, mat_name, -1);
	if (idx >= 0 && idx < numCharMat) {
		*mat = CharMaterials2[idx];
		return true;
	} else {
		*mat = 0;
		return false;
	}
}

void CreateCharMaterial2 (const char *line) {
	char matName[32];
	TVector3 diff = {0,0,0}; 
	TVector3 spec = {0,0,0};
	float exp = 100;

	string lin = line;	
	SPCharN (lin, "mat", matName);
	diff = SPVector3N (lin, "diff", MakeVec (0,0,0));
	spec = SPVector3N (lin, "spec", MakeVec (0,0,0));
	exp = SPFloatN (lin, "exp", 50);

	TTuxMaterial2 *matPtr = (TTuxMaterial2 *) malloc (sizeof (TTuxMaterial2));

    matPtr->diffuse.r = diff.x;
    matPtr->diffuse.g = diff.y;
    matPtr->diffuse.b = diff.z;
    matPtr->diffuse.a = 1.0;
    matPtr->specular.r = spec.x;
    matPtr->specular.g = spec.y;
    matPtr->specular.b = spec.z;
    matPtr->specular.a = 1.0;
    matPtr->exp = exp;

	TTuxMaterial2 *test;
    if (GetCharMaterial2 (matName, &test)) {
		free (matPtr);
	} else {
		SPAddIntN (MatIndex2, matName, numCharMat);
		CharMaterials2[numCharMat] = matPtr;
		numCharMat++;
	}
}

// --------------------------------------------------------------------
//					draw
// --------------------------------------------------------------------

void DrawCharSphere2 (int num_divisions) {
    GLUquadricObj *qobj;
    qobj = gluNewQuadric();
    gluQuadricDrawStyle (qobj, GLU_FILL);
    gluQuadricOrientation (qobj, GLU_OUTSIDE);
    gluQuadricNormals (qobj, GLU_SMOOTH);
    gluSphere (qobj, 1.0, (GLint)2.0 * num_divisions, num_divisions);
    gluDeleteQuadric (qobj);
}

static GLuint GetTuxDisplayList2 (int divisions) {
    static bool initialized = false;
    static int num_display_lists;
    static GLuint *display_lists = NULL;
    int base_divisions;
    int i, idx;

    if  (!initialized) {
		initialized = true;
		base_divisions = cfg.sphere_divisions;
		num_display_lists = MAX_SPHERE_DIV - MIN_SPHERE_DIV + 1;
		display_lists = (GLuint*) malloc (sizeof(GLuint) * num_display_lists);
		for (i=0; i<num_display_lists; i++) display_lists[i] = 0;
    }

    idx = divisions - MIN_SPHERE_DIV;
    if  (display_lists[idx] == 0) {
		display_lists[idx] = glGenLists (1);
		glNewList (display_lists[idx], GL_COMPILE);
		DrawCharSphere2 (divisions);
		glEndList ();
    }
    return display_lists[idx];
}

void DrawNodes (TCharNode2 *node, TTuxMaterial2 *mat) {
    TCharNode2 *child;
    glPushMatrix();
    glMultMatrixf ((float *) node->trans);

    if  (node->mat != NULL) mat = node->mat;
    if  (node->visible == true) {
        SetMaterial (mat->diffuse, mat->specular, mat->exp);
		if (USE_CHAR_DISPLAY_LIST) {
		    glCallList (GetTuxDisplayList2 (node->divisions));
		} else {
	    	DrawCharSphere2 (node->divisions);
		}
    } 

    child = node->child;
    while (child != NULL) {
        DrawNodes (child, mat);
        child = child->next;
    } 
    glPopMatrix();
} 

void DrawTux () {
    TCharNode2 *nodePtr;
    float dummy_color[]  = {0.0, 0.0, 0.0, 1.0};

    glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, dummy_color);
    SetGLOptions (TUX);

	if (!GetCharNode2 ("root", &nodePtr)) return;
	DrawNodes (nodePtr, &TuxDefMat);
} 

// --------------------------------------------------------------------
//					node functions
// --------------------------------------------------------------------


bool GetCharNode2 (char *node_name, TCharNode2 **node) {
	int entry = SPIntN (NodeIndex2, node_name, -1);
	
	if (entry < 0) { 
		*node = 0;	
		return false;
	} else {
		*node = (TCharNode2*) CharNodes2[entry];
		return true;
	}
}

bool ResetCharNode2 (char *node_name) {  
    TCharNode2 *nodePtr;
    if (!GetCharNode2 (node_name, &nodePtr)) {PrintInt (8888); return false;}
    MakeIdentityMat (nodePtr->trans);
    MakeIdentityMat (nodePtr->invtrans);
	return true;
}

bool TransformCharNode2 (char *node_name, TMatrix mat, TMatrix invmat) {
    TCharNode2 *nodePtr;
    if (!GetCharNode2 (node_name, &nodePtr)) return false;
    MultMat (nodePtr->trans, nodePtr->trans, mat);
    MultMat (nodePtr->invtrans, invmat, nodePtr->invtrans);
    return true;
}


bool CreateCharNode2 (char *parent_name, char *child_name, float vis) {
    TCharNode2 *parent, *child;

    if  (!GetCharNode2 (parent_name, &parent)) return false;
    child = (TCharNode2 *) malloc (sizeof (TCharNode2));

    child->parent = parent;
    child->next  = 0;
    child->child = 0;
    child->mat   = 0;
	child->visible = (vis > 0);
    MakeIdentityMat (child->trans);
    MakeIdentityMat (child->invtrans);

	if (vis > 0) {
	    child->radius = 1.0;
    	child->divisions = 
		MIN (MAX_SPHERE_DIV, MAX (MIN_SPHERE_DIV, 
		    ROUND_TO_NEAREST (cfg.sphere_divisions * vis / 10)));
	} 

    child->name = (char*) malloc (strlen(child_name) + 1);
	strcpy (child->name, child_name);

	if (SPIntN (NodeIndex2, child_name, -1) < 0) { 
		SPAddIntN (NodeIndex2, child_name, numCharNodes2);
		CharNodes2[numCharNodes2] = child;
		numCharNodes2++;
	} else {
		free (child);
		return false;
	}

    if  (parent != 0) {
        if  (parent->child == 0) {
            parent->child = child;
        } else {
			for (parent = parent->child; parent->next != 0; parent = parent->next) {} 
			parent->next = child;
        } 
    } 
    return true;
} 

void CreateRootNode () {
    TCharNode2 *child = (TCharNode2 *) malloc (sizeof (TCharNode2));
    child->parent = 0;
    child->next = 0;
    child->child = 0;
    child->mat = 0;
	child->visible = false;
    MakeIdentityMat (child->trans);
    MakeIdentityMat (child->invtrans);

    child->name = (char*) malloc (strlen ("root") + 1);
	strcpy (child->name, "root");
	NodeIndex2[0] = '\0';
	NodeIndex2 = "[root]0";
	CharNodes2[0] = child;
	numCharNodes2 = 1;
}

bool TranslateCharNode2 (char *node_name, TVector3 vec) {
    TCharNode2 *nodePtr;
    TMatrix TransMatrix;

    if  (!GetCharNode2 (node_name, &nodePtr)) return false;
    MakeTransMat (TransMatrix, vec.x, vec.y, vec.z);
    MultMat (nodePtr->trans, nodePtr->trans, TransMatrix);
    MakeTransMat (TransMatrix, -vec.x, -vec.y, -vec.z);
    MultMat (nodePtr->invtrans, TransMatrix, nodePtr->invtrans);
    return true;
}

void ScaleCharNode2 (char *node_name, TVector3 vec) {
    TCharNode2 *nodePtr;
    TMatrix matrix;

    if  (!GetCharNode2 (node_name, &nodePtr)) return;

	MakeIdentityMat (matrix);
    MultMat (nodePtr->trans, nodePtr->trans, matrix);
	MakeIdentityMat (matrix);
    MultMat (nodePtr->invtrans, matrix, nodePtr->invtrans);

    MakeScaleMat (matrix, vec.x, vec.y, vec.z);
    MultMat (nodePtr->trans, nodePtr->trans, matrix);
    MakeScaleMat (matrix, 1.0 / vec.x, 1.0 / vec.y, 1.0 / vec.z);
    MultMat (nodePtr->invtrans, matrix, nodePtr->invtrans);

	MakeIdentityMat (matrix);
    MultMat (nodePtr->trans, nodePtr->trans, matrix);
	MakeIdentityMat (matrix);
    MultMat (nodePtr->invtrans, matrix, nodePtr->invtrans);
}

bool RotateCharNode2 (char *node_name, float axis, float angle) {
    TCharNode2 *nodePtr;
    TMatrix rotMatrix;
	char caxis = '0';

    if (!GetCharNode2 (node_name, &nodePtr)) return false;

	int a = (int) axis;
	if (axis > 3) return false;
	switch (a) {
		case 1: caxis = 'x'; break;
		case 2: caxis = 'y'; break;
		case 3: caxis = 'z'; break;
	}

    MakeRotMat (rotMatrix, angle, caxis);
    MultMat (nodePtr->trans, nodePtr->trans, rotMatrix);
    MakeRotMat (rotMatrix, -angle, caxis);
    MultMat (nodePtr->invtrans, rotMatrix, nodePtr->invtrans);
    return true;
}

bool MaterialCharNode2 (char *node_name, char *mat_name) {
    TTuxMaterial2 *matPtr;
    TCharNode2 *nodePtr;
    if (!GetCharNode2 (node_name, &nodePtr)) return false;
    if (!GetCharMaterial2 (mat_name, &matPtr)) return false;
	nodePtr->mat = matPtr;
	return true;
}

void LoadCharacter () {
	string commands2 = "[create] 1 [prop] 2 [material] 3";
	char cmd_name[24], par_name[24], node_name[24], mat_name[24];
	CSPList list (500);
	int i, cmd;
	string line;
	TVector3 scale, trans;
	TVector2 rot;
	float axis, angle, res, visible;

	CreateRootNode ();

	list.Load (cfg.character_dir, "char1.lst");
	for (i=0; i<list.Count(); i++) {
		line = list.Line (i);
		SPCharN (line, "cmd", cmd_name);
		cmd = SPIntN (commands2, cmd_name, 0);
		SPCharN (line, "node", node_name);		

		if (cmd == 3) {
			CreateCharMaterial2 (line.c_str());
		} else {
			// new node
			if (cmd == 1) {
				SPCharN (line, "par", par_name);		
				visible = SPFloatN (line, "vis", -1.0);
				CreateCharNode2 (par_name, node_name, visible);					
			}

			// properties in an existing node
			res = SPFloatN (line, "res", 1.0);

			if (SPExistsN (line, "trans")) {
				trans = SPVector3N (line, "trans", MakeVec (0,0,0));
				TranslateCharNode2 (node_name, trans);
			}

			if (SPExistsN (line, "rot")) {
				rot = SPVector2N (line, "rot", MakeVec2 (0,0));
				axis = rot.x;
				angle = rot.y;
				RotateCharNode2 (node_name, axis, angle);
			}

			if (SPExistsN (line, "scale")) {
				scale = SPVector3N (line, "scale", MakeVec (1,1,1));
				ScaleCharNode2 (node_name, scale);
			}
			SPCharN (line, "mat", mat_name);
			MaterialCharNode2 (node_name, mat_name);
		}
	}
}

// --------------------------------------------------------------------
//					reset
// --------------------------------------------------------------------

void ResetTuxRoot () { ResetCharNode2 ("root"); }

void ResetTuxJoints () {
    ResetCharNode2 ("left_shoulder");
    ResetCharNode2 ("right_shoulder");
    ResetCharNode2 ("left_hip");
    ResetCharNode2 ("right_hip");
    ResetCharNode2 ("left_knee");
    ResetCharNode2 ("right_knee");
    ResetCharNode2 ("left_ankle");
    ResetCharNode2 ("right_ankle");
    ResetCharNode2 ("tail");
    ResetCharNode2 ("neck");
    ResetCharNode2 ("head");
}

// --------------------------------------------------------------------
//					orientation
// --------------------------------------------------------------------

static TVector3 AdjustRollAngle2 (TVector3 yvec, TControl *ctrl) {
    TMatrix rot_mat; 
    TVector3 vel = ProjectToPlane (yvec, ctrl->vel);
    NormVec (vel);

	if (!ctrl->final_state) {
		if (ctrl->brake) RotateVecMat (rot_mat, vel, ctrl->turn * C_BRAKING_ROLL);
		else RotateVecMat (rot_mat, vel, ctrl->turn * C_NORMAL_ROLL);
	} else {
		RotateVecMat (rot_mat, vel, 0);
	}
	return TransformVec (rot_mat, yvec);
}

void AdjustTuxOrientation2 (float timestep, TControl *ctrl) {
    TVector3 xvec, zvec, yvec; 
    TMatrix mat, invmat;
    TQuaternion new_orient;
	
	if (ctrl->airborne) {
		// orientation follows vel
		zvec = ctrl->vel;
		NormVec (zvec);
		
		yvec = ProjectToPlane (zvec, MakeVec (0.0, -1.0, 0.0));
		NormVec (yvec);
		yvec = AdjustRollAngle2 (yvec, ctrl);
	} else {
		// orientation is result of vel and surf_plane
		zvec = ProjectToPlane (ctrl->surf_nml, ctrl->vel);
		NormVec (zvec);

		// reverse surf nml
		yvec = ScaleVec (-1.0, ctrl->surf_nml);
		// with rotation
		yvec = AdjustRollAngle2 (yvec, ctrl);
 	}

    xvec = CrossVec (zvec, yvec);
    MakefloatBasisMat (mat, invmat, xvec, zvec, yvec);
    new_orient = MatToQuat (mat);

	// in case of first call: setting orientation to start val
    if (ctrl->firstorient) {
		ctrl->firstorient = false;
		ctrl->orientation = new_orient;
    }

	// interpolation for smooth orientation
	if (ctrl->airborne) {
	    ctrl->orientation = InterpolateQuat (ctrl->orientation, new_orient, 
			MIN (timestep / C_AIR_TIME, 1.0));
	} else {
	    ctrl->orientation = InterpolateQuat (ctrl->orientation, new_orient, 
			MIN (timestep / C_GROUND_TIME, 1.0));
	}

    QuatToMat (mat, ctrl->orientation);
    TransposeMat (mat, invmat);
    TransformCharNode2 ("root", mat, invmat); 
}

// --------------------------------------------------------------------
//					animation
// --------------------------------------------------------------------

void AdjustTuxJoints2 (float turnFact, bool isBraking, 
			float paddling_factor, float speed,
			TVector3 net_force, float flap_factor) {
    float turning_angle[2] = {0, 0};
    float paddling_angle = 0;
    float ext_paddling_angle = 0; 
    float kick_paddling_angle = 0;
    float braking_angle = 0;
    float force_angle = 0;
    float turn_leg_angle = 0;
    float flap_angle = 0;

    if  (isBraking) braking_angle = MAX_ARM_ANGLE;

    paddling_angle = MAX_PADDLING_ANGLE * sin(paddling_factor * M_PI);
    ext_paddling_angle = MAX_EXT_PADDLING_ANGLE * sin(paddling_factor * M_PI);
    kick_paddling_angle = MAX_KICK_PADDLING_ANGLE * sin(paddling_factor * M_PI * 2.0);

    turning_angle[0] = MAX(-turnFact,0.0) * MAX_ARM_ANGLE;
    turning_angle[1] = MAX(turnFact,0.0) * MAX_ARM_ANGLE;

    flap_angle = MAX_ARM_ANGLE * (0.5 + 0.5 * sin (M_PI * flap_factor * 6 - M_PI / 2));

    force_angle = MAX (-20.0, MIN (20.0, -net_force.z / 300.0));
    turn_leg_angle = turnFact * 10;
    
	ResetTuxJoints ();

    RotateCharNode2 ("left_shoulder", 3, 
		    MIN (braking_angle + paddling_angle + turning_angle[0], MAX_ARM_ANGLE) + flap_angle);
    RotateCharNode2 ("right_shoulder", 3,
		    MIN (braking_angle + paddling_angle + turning_angle[1], MAX_ARM_ANGLE) + flap_angle);

    RotateCharNode2 ("left_shoulder", 2, -ext_paddling_angle);
    RotateCharNode2 ("right_shoulder", 2, ext_paddling_angle);

    RotateCharNode2 ("left_hip", 3, -20 + turn_leg_angle + force_angle);
    RotateCharNode2 ("right_hip", 3, -20 - turn_leg_angle + force_angle);
	
    RotateCharNode2 ("left_knee", 3, 
		-10 + turn_leg_angle - MIN (35, speed) + kick_paddling_angle + force_angle);
    RotateCharNode2 ("right_knee", 3, 
		-10 - turn_leg_angle - MIN (35, speed) - kick_paddling_angle + force_angle);

    RotateCharNode2 ("left_ankle", 3, -20 + MIN(50, speed));
    RotateCharNode2 ("right_ankle", 3, -20 + MIN(50, speed));
    RotateCharNode2 ("tail", 3, turnFact * 20);
    RotateCharNode2 ("neck", 3, -50);
    RotateCharNode2 ("head", 3, -30);
    RotateCharNode2 ("head", 2, -turnFact * 70);
}

void AdjustTuxAnimation2 (TControl *ctrl) {
    float paddling_factor; 
    TVector3 local_force;
    float flap_factor;
	float factor = 0;

    TVector3 tmp_vel = ctrl->vel;
	float speed = NormVec (tmp_vel);

    flap_factor = 0;
    if (ctrl->paddle) {
		factor = (Race.time - ctrl->paddle_time) / PADDLING_DURATION;
		if  (ctrl->airborne) {
		    paddling_factor = 0;
		    flap_factor = factor;
		} else {
		    paddling_factor = factor;
		    flap_factor = 0;
		}
    } else paddling_factor = 0.0;

    local_force = RotateVec (ConjugateQuat (ctrl->orientation), ScaleVec (10, ctrl->netforce));

    AdjustTuxJoints2 (
			ctrl->turn_animation, 
			ctrl->brake,
			paddling_factor, 
			speed, 
			local_force, 
			flap_factor);
}


// ********************************************************************
//				experimental (only for testing)
// ********************************************************************

void SetTuxOrientation2 (TVector3 dir, TVector3 up, float angle) {
    TVector3 vec3, upvec, dirvec; 
    TMatrix mat, invmat, rot_mat;

	dirvec = dir;
	upvec = ScaleVec (-1.0, up);
	NormVec (upvec);	
    dirvec = ProjectToPlane (upvec, dirvec);
    NormVec (dirvec);
	RotateVecMat (rot_mat, dirvec, angle);
	upvec = TransformVec (rot_mat, upvec);	
    vec3 = CrossVec (dirvec, upvec); 
    MakefloatBasisMat (mat, invmat, vec3, dirvec, upvec);
    TransposeMat (mat, invmat);
    TransformCharNode2 ("root", mat, invmat); 
}

void SetTuxOrientation2 (TVector3 dir, float angle) {
	TVector3 vec3, dirvec, upvec;
    TMatrix mat, invmat, rot_mat;

	TVector3 groundnml = MakeVec (0, -1, 0);
	TVector3 v1 = ProjectToPlane (groundnml, dir);
	NormVec (v1);
	TVector3 v2 = CrossVec (dir, v1);
	NormVec (v2);
	upvec = CrossVec (dir, v2);
 	NormVec (upvec);
	dirvec = dir;
    dirvec = ProjectToPlane (upvec, dirvec);
    NormVec (dirvec);
	RotateVecMat (rot_mat, dirvec, angle);
	upvec = TransformVec (rot_mat, upvec);	
    vec3 = CrossVec (dirvec, upvec); 
    MakefloatBasisMat (mat, invmat, vec3, dirvec, upvec);
    TransposeMat (mat, invmat);
    TransformCharNode2 ("root", mat, invmat); 
}

void SetTuxStandingOrientation2 (float angle) {
    TVector3 acrossvec, upvec, dirvec; 
    TMatrix mat, invmat, rot_mat;

	dirvec = MakeVec (0, 1, 0);
	upvec = MakeVec (0, 0, 1);
	RotateVecMat (rot_mat, dirvec, angle);
	upvec = TransformVec (rot_mat, upvec);	
    acrossvec = CrossVec (dirvec, upvec); 
    MakefloatBasisMat (mat, invmat, acrossvec, dirvec, upvec);
    TransposeMat (mat, invmat);
    TransformCharNode2 ("root", mat, invmat); 
}

void SetTuxAnimation2 () {
	
	// ---------------- directly ----------------------------------------
	float NeckUpDownAngle = 0;			// positive: forward
	float HeadUpDownAngle = 0;			// positive: forward
 	float HeadRoundAngle = 0;			// positive: to the left
	float TailAngle = 0;				// positive: to the right
	float LeftShoulderAngle_z = 0;		// positive: arm forward
	float RightShoulderAngle_z = 0;		// positive: arm forward
	float LeftShoulderAngle_y = 0;		// negative: arm upward
	float RightShoulderAngle_y = 0;		// positive: arm upward
	float LeftHipAngle = 0;				// positive: forward
	float RightHipAngle = 0;			// positive: forward
	float LeftKneeAngle = 0;			// positive: forward
	float RightKneeAngle = 0;			// positive: forward
	float LeftAnkleAngle = 0;			// positive: downward
	float RightAnkleAngle = 0;			// positive: downward

	ResetTuxJoints ();

	// 2 = 'y'  3 = 'z'
	RotateCharNode2 ("neck", 			3, NeckUpDownAngle);
	RotateCharNode2 ("head",			3, HeadUpDownAngle);
    RotateCharNode2 ("head",			2, HeadRoundAngle);
    RotateCharNode2 ("tail",			3, TailAngle);
    RotateCharNode2 ("left_shoulder",	3, LeftShoulderAngle_z);
    RotateCharNode2 ("right_shoulder",	3, RightShoulderAngle_z);
    RotateCharNode2 ("left_shoulder",	2, LeftShoulderAngle_y);
    RotateCharNode2 ("right_shoulder",	2, RightShoulderAngle_y);
    RotateCharNode2 ("left_hip",		3, LeftHipAngle);
    RotateCharNode2 ("right_hip",		3, RightHipAngle);
    RotateCharNode2 ("left_knee",		3, LeftKneeAngle);
    RotateCharNode2 ("right_knee",		3, RightKneeAngle);
    RotateCharNode2 ("left_ankle",		3, LeftAnkleAngle);
    RotateCharNode2 ("right_ankle",		3, RightAnkleAngle);
	// ---------------- end of directly -----------------------------------

/*
	// ----------------- using motion params: ----------------------
	ResetTuxJoints ();
	SetTuxJoints (
		false,	// braking 
		0,		// paddling               
		0,		// turning
		0,		// flapping
		0,		// force, z-component
		0		// speed
	);
	// -------------------------------------------------------------
*/
}

// --------------------------------------------------------------------


const float DEPTH_INTERPOLATION = 0.5;
static float newdepth = 0;
static float currentdepth = 0;

void SetCharacterPosition (TVector3 pos, float timestep, TControl *ctrl) {
	newdepth = ctrl->depth;
	if (currentdepth < newdepth) {
		currentdepth += timestep * DEPTH_INTERPOLATION;
		if (currentdepth > newdepth) currentdepth = newdepth;
	} else {
		currentdepth -= timestep * DEPTH_INTERPOLATION;
		if (currentdepth < newdepth) currentdepth = newdepth;
	}

	if (game.mode == RACING) pos.y -= 0.4 + currentdepth;
	else pos.y -= 0.4;


	const float TUX_Y_CORR = 0.33;
    float disp_y = pos.y + TUX_Y_CORR; 
    ResetCharNode2 ("root");
    TranslateCharNode2 ("root", MakeVec (pos.x, disp_y, pos.z));
}

void SetCharacterOrientation (float timestep, TControl *ctrl) {
	AdjustTuxOrientation2 (timestep, ctrl);
}

void SetCharacterAnimation (TControl *ctrl) {
	AdjustTuxAnimation2 (ctrl);
}















