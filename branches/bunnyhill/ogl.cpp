#include "ogl.h"
// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
// Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
//
// --------------------------------------------------------------------

/*
// --------------------------------------------------------------------
// 					GL Extensions
// --------------------------------------------------------------------

typedef void (*(*TProcPtr) (const GLubyte *)) (); 

PFNGLLOCKARRAYSEXTPROC glLockArraysEXT_p = 0;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT_p = 0;

void InitGLExtensions () {
	TProcPtr glproc = 0;

   	glproc = (TProcPtr) SDL_GL_GetProcAddress;
	if (glproc) {
		glLockArraysEXT_p = (PFNGLLOCKARRAYSEXTPROC) 
		    (*glproc) ((GLubyte*) "glLockArraysEXT");
		glUnlockArraysEXT_p = (PFNGLUNLOCKARRAYSEXTPROC) 
		    (*glproc) ((GLubyte*) "glUnlockArraysEXT");
	
		if (glLockArraysEXT_p == 0 || glUnlockArraysEXT_p == 0) {
 		    printf ("GL_EXT_compiled_vertex_array extension not supported");
	    	glLockArraysEXT_p = 0;
		    glUnlockArraysEXT_p = 0;
		}
    } else {
		Message ("No function available for obtaining GL proc addresses");
    }
}
*/

// --------------------------------------------------------------------
//				GL-Settings
// --------------------------------------------------------------------

void InitGL () {
	glShadeModel (GL_SMOOTH);
	glClearColor (0.0, 0.0, 0.2, 1.0);
    glClearDepth (1.0);
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);
    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDisable (GL_CULL_FACE);
//	InitGLExtensions ();		
}

void SetViewport (int width, int height) {
	float near = cfg.nearclip;
	float far =  cfg.forwardclip + 5;

	glViewport (0, 0, (GLint)width, (GLint)height);
	glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
	gluPerspective ((float)cfg.fov, (float)width / (float)height, near, far);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
}


void SetViewport () {
	SetViewport (cfg.scrwidth, cfg.scrheight);
}

void ClearRenderContext (){
    glDepthMask (GL_TRUE);
    glClearColor (colMBackgr.r, colMBackgr.g, colMBackgr.b, colMBackgr.a);
    glClearStencil (0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void SetupGuiDisplay () {
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho (0, cfg.scrwidth, 0, cfg.scrheight, -1.0, 1.0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glColor4f (1.0, 1.0, 1.0, 1.0);
}

void SetupTexGen (float scale) {
    static float xplane[4] = {1.0 / scale, 0.0, 0.0, 0.0 };
    static float zplane[4] = {0.0, 0.0, 1.0 / scale, 0.0 };
    glTexGenfv (GL_S, GL_OBJECT_PLANE, xplane);
    glTexGenfv (GL_T, GL_OBJECT_PLANE, zplane);
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

// --------------------------------------------------------------------
//					GL-Options
// --------------------------------------------------------------------
// When setting an option the mode is stored in GLOption
// You can read this variable to find out if the right option
// is set (GLReadOption). The settings are the same as in Tuxracer
// Probably it makes sense to store the settings in display lists
// I haven't tested yet if that increases the performance

static int GLOption;

void SetGLOptions (TRenderMode mode) {
	GLOption = mode;
	switch (mode) {
    case GUI:
        glEnable (GL_TEXTURE_2D);
        glDisable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
		glDisable (GL_LIGHTING);
		glDisable (GL_NORMALIZE);
		glDisable (GL_ALPHA_TEST);
        glEnable (GL_BLEND);
		glDisable (GL_STENCIL_TEST);
		glDisable (GL_COLOR_MATERIAL);
		glDepthMask (GL_TRUE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LESS);
		glDisable (GL_FOG);
		
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
		break;
    
	case GAUGE_BARS:
        glEnable (GL_TEXTURE_2D);
        glDisable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
		glDisable (GL_LIGHTING);
		glDisable (GL_NORMALIZE);
		glDisable (GL_ALPHA_TEST);
        glEnable (GL_BLEND);
		glDisable (GL_STENCIL_TEST);
		glDisable (GL_COLOR_MATERIAL);
		glDepthMask (GL_TRUE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LESS);

		glEnable (GL_TEXTURE_GEN_S);
		glEnable (GL_TEXTURE_GEN_T);
		glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        break;

    case TEXFONT:
        glEnable (GL_TEXTURE_2D);
        glDisable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
		glDisable (GL_LIGHTING);
		glDisable (GL_NORMALIZE);
		glDisable (GL_ALPHA_TEST);
        glEnable (GL_BLEND);
		glDisable (GL_STENCIL_TEST);
		glDisable (GL_COLOR_MATERIAL);
		glDepthMask (GL_TRUE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LESS);
        
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
		break;
    
	case COURSE:
		glEnable (GL_TEXTURE_2D);
		glEnable (GL_DEPTH_TEST);
		glEnable (GL_CULL_FACE);
		glEnable (GL_LIGHTING);
		glDisable (GL_NORMALIZE);
		glDisable (GL_ALPHA_TEST);
		glEnable (GL_BLEND);
		glDisable (GL_STENCIL_TEST);
		glEnable (GL_COLOR_MATERIAL);
		glDepthMask (GL_TRUE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LEQUAL);
		
		// autogenerating textures
		glEnable (GL_TEXTURE_GEN_S);
		glEnable (GL_TEXTURE_GEN_T);
		glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		break;

    case TREES:
		glEnable (GL_TEXTURE_2D);
		glEnable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
		glEnable (GL_LIGHTING);
		glDisable (GL_NORMALIZE);
        glEnable (GL_ALPHA_TEST);
		glEnable (GL_BLEND);
		glDisable (GL_STENCIL_TEST);
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
		glDisable (GL_COLOR_MATERIAL);
		glDepthMask (GL_TRUE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LESS);

        glAlphaFunc (GL_GEQUAL, 0.5);
        break;

    case MODELS:
		glEnable (GL_TEXTURE_2D);
		glEnable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
		glEnable (GL_LIGHTING);
		glDisable (GL_NORMALIZE);
        glEnable (GL_ALPHA_TEST);
		glEnable (GL_BLEND);
		glDisable (GL_STENCIL_TEST);
		glEnable (GL_COLOR_MATERIAL);
		glDepthMask (GL_TRUE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LESS);
        glAlphaFunc (GL_GEQUAL, 0.5);
		
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
		break;
        
    case PARTICLES:
        glEnable (GL_TEXTURE_2D);
		glEnable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
		glDisable (GL_LIGHTING);
		glDisable (GL_NORMALIZE);
		glEnable (GL_ALPHA_TEST);
        glEnable (GL_BLEND);
		glDisable (GL_STENCIL_TEST);
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
		glDisable (GL_COLOR_MATERIAL);
		glDepthMask (GL_TRUE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LESS);
        glAlphaFunc (GL_GEQUAL, 0.5);
		
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
		break;
    
	case SKY:
		glEnable (GL_TEXTURE_2D);
		glDisable (GL_DEPTH_TEST);
		glDisable (GL_CULL_FACE); 
		glDisable (GL_LIGHTING);
		glDisable (GL_NORMALIZE);
		glDisable (GL_ALPHA_TEST);
		glEnable (GL_BLEND);
		glDisable (GL_STENCIL_TEST);
		glDisable (GL_COLOR_MATERIAL);
		glDepthMask (GL_FALSE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LESS);
		
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
		break;
 	
    case FOG_PLANE:
		glDisable (GL_TEXTURE_2D);
		glEnable (GL_DEPTH_TEST);
		glDisable (GL_CULL_FACE); 
		glDisable (GL_LIGHTING);
		glDisable (GL_NORMALIZE);
		glDisable (GL_ALPHA_TEST);
		glEnable (GL_BLEND);
		glDisable (GL_STENCIL_TEST);
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
		glDisable (GL_COLOR_MATERIAL);
		glDepthMask (GL_TRUE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LESS);
		
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
		break;

    case TUX:
	    glDisable (GL_TEXTURE_2D);
		glEnable (GL_DEPTH_TEST);
		glEnable (GL_CULL_FACE);
    	glEnable (GL_LIGHTING);
		glEnable (GL_NORMALIZE);
		glDisable (GL_ALPHA_TEST);
		glEnable (GL_BLEND);
		glDisable (GL_STENCIL_TEST);
		glDisable (GL_COLOR_MATERIAL);
		glDepthMask (GL_TRUE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LESS);
		
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
		break;

    case TUX_SHADOW:
	#ifdef USE_STENCIL_BUFFER
		glDisable (GL_TEXTURE_2D);
		glEnable (GL_DEPTH_TEST);
		glDisable (GL_CULL_FACE);
		glDisable (GL_LIGHTING);
		glDisable (GL_NORMALIZE);
		glDisable (GL_ALPHA_TEST);
		glEnable (GL_BLEND);
		glEnable (GL_STENCIL_TEST);
		glDisable (GL_COLOR_MATERIAL);
		glDepthMask (GL_FALSE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LESS);
		glStencilFunc (GL_EQUAL, 0, ~0);
		glStencilOp (GL_KEEP, GL_KEEP, GL_INCR);
	#else
		glDisable (GL_TEXTURE_2D);
		glEnable (GL_DEPTH_TEST);
		glEnable (GL_CULL_FACE);
		glDisable (GL_LIGHTING);
		glDisable (GL_NORMALIZE);
		glDisable (GL_ALPHA_TEST);
		glEnable (GL_BLEND);
		glDisable (GL_STENCIL_TEST);
		glDisable (GL_COLOR_MATERIAL);
		glDepthMask (GL_TRUE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LESS);
	#endif
	break;

    case TRACK_MARKS:
		glEnable (GL_TEXTURE_2D);
		glEnable (GL_DEPTH_TEST);
		glDisable (GL_CULL_FACE);
		glEnable (GL_LIGHTING);
		glDisable (GL_NORMALIZE);
		glDisable (GL_ALPHA_TEST);
		glEnable (GL_BLEND);
		glDisable (GL_STENCIL_TEST);
		glDisable (GL_COLOR_MATERIAL);
		glDepthMask (GL_FALSE);
		glShadeModel (GL_SMOOTH);
		glDepthFunc (GL_LEQUAL);
		
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
		break;
    
	default: break;
    } 
} 

int ReadGLOption () {return GLOption; }

// --------------------------------------------------------------------
//					GL-Info
// --------------------------------------------------------------------

typedef struct {
    char *name;
    GLenum value;
    GLenum type;
} gl_value_t;

gl_value_t gl_values[] = {
    { "maximum lights", GL_MAX_LIGHTS, GL_INT },
    { "modelview stack depth", GL_MAX_MODELVIEW_STACK_DEPTH, GL_INT },
    { "projection stack depth", GL_MAX_PROJECTION_STACK_DEPTH, GL_INT },
    { "max texture size", GL_MAX_TEXTURE_SIZE, GL_INT },
    { "float buffering", GL_DOUBLEBUFFER, GL_UNSIGNED_BYTE },
    { "red bits", GL_RED_BITS, GL_INT },
    { "green bits", GL_GREEN_BITS, GL_INT },
    { "blue bits", GL_BLUE_BITS, GL_INT },
    { "alpha bits", GL_ALPHA_BITS, GL_INT },
    { "depth bits", GL_DEPTH_BITS, GL_INT },
    { "stencil bits", GL_STENCIL_BITS, GL_INT } };

void PrintGLInfo (){
    char *extensions;
    char *p, *oldp;
    int i;
    GLint int_val;
    float float_val;
    GLboolean boolean_val;
	string ss;

	Message ("");
    Message ("Gl vendor: ", (char*)glGetString (GL_VENDOR));
    Message ("Gl renderer: ", (char*)glGetString (GL_RENDERER));
    Message ("Gl version: ", (char*)glGetString (GL_VERSION));
    extensions = NewStrN ((char*) glGetString (GL_EXTENSIONS));
    Message ("");
	Message ("Gl extensions:", "");
	Message ("");
	
    oldp = extensions;
    while ((p=strchr(oldp,' '))) {
		*p='\0';
		Message (oldp,"");
		oldp = p+1;
    }
    if (*oldp) Message (oldp,"");

    free (extensions);
	Message ("");

    for (i=0; i<int(sizeof(gl_values)/sizeof(gl_values[0])); i++) {
		switch (gl_values[i].type) {
			case GL_INT:
	    	glGetIntegerv (gl_values[i].value, &int_val);
			ss = Int_StrN (int_val);
			MessageN (gl_values[i].name, ss);
		    break;

			case GL_FLOAT:
		    glGetFloatv (gl_values[i].value, &float_val);
			ss = Float_StrN (float_val, 2);
			MessageN (gl_values[i].name, ss);
		    break;

			case GL_UNSIGNED_BYTE:
	    	glGetBooleanv (gl_values[i].value, &boolean_val);
			ss = Int_StrN (boolean_val);
			MessageN (gl_values[i].name, ss);
		    break;

			default:
			Message ("");
		}
    }
	Message ("");
}

void CheckGLError () {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
		Message ("OpenGL Error: ", (char*)gluErrorString (error));
	}
}

void SetMaterial (TColor diff, TColor spec, float exp) {
	float mat_amb_diff[4];
	float mat_specular[4];

	mat_amb_diff[0] = diff.r;
	mat_amb_diff[1] = diff.g;
	mat_amb_diff[2] = diff.b;
 	mat_amb_diff[3] = diff.a; 

	mat_specular[0] = spec.r;
	mat_specular[1] = spec.g;
	mat_specular[2] = spec.b;
	mat_specular[3] = spec.a;

	glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
	glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, exp);

	glColor4f (diff.r, diff.g, diff.b, diff.a);
} 

void SetMaterial (TColor diff, TColor amb, TColor spec, float exp) {
	float mat_diff[4];
	float mat_amb[4];
	float mat_spec[4];

	mat_diff[0] = diff.r;
	mat_diff[1] = diff.g;
	mat_diff[2] = diff.b;
 	mat_diff[3] = diff.a; 

	mat_amb[0] = amb.r;
	mat_amb[1] = amb.g;
	mat_amb[2] = amb.b;
 	mat_amb[3] = amb.a; 

	mat_spec[0] = spec.r;
	mat_spec[1] = spec.g;
	mat_spec[2] = spec.b;
	mat_spec[3] = spec.a;

	glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diff);
	glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, mat_amb);
	glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, mat_spec);
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, exp);

	glColor4f (1, 1, 1, 1);
} 

