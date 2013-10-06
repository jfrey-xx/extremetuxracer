/* --------------------------------------------------------------------
EXTREME TUXRACER

Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
Copyright (C) 2010 Extreme Tuxracer Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
---------------------------------------------------------------------*/

#ifdef HAVE_CONFIG_H
#include <etr_config.h>
#endif

#include "bh.h"
#include "course.h"
#include "textures.h"
#include "ogl.h"
#include "audio.h"
#include "track_marks.h"
#include "spx.h"
#include "quadtree.h"
#include "env.h"
#include "game_ctrl.h"
#include "font.h"
#include "physics.h"
#include "winsys.h"
#include <cmath>
#include <algorithm>

CCourse Course;

CCourse::CCourse () {
	terrain = NULL;
	elevation = NULL;
	nmls = NULL;
	vnc_array = NULL;
	mirrored = false;

	curr_course = NULL;
}

CCourse::~CCourse() {
	for (size_t i = 0; i < PolyArr.size(); i++) {
		for (size_t j = 0; j < PolyArr[i].num_polygons; j++)
			delete[] PolyArr[i].polygons[j].vertices;
		delete[] PolyArr[i].polygons;
		FreePolyhedron(PolyArr[i]);
	}
	FreeCourseList ();
	ResetCourse ();
}

double CCourse::GetBaseHeight (double distance) const {
	double slope = tan (ANGLES_TO_RADIANS (curr_course->angle));
	double base_height;

	base_height = -slope * distance -
	              base_height_value / 255.0 * curr_course->scale;
	return base_height;
}

double CCourse::GetMaxHeight (double distance) const {
	return GetBaseHeight (distance) + curr_course->scale;
}

void CCourse::GetDivisions (int *x, int *y) const {
	*x = nx;
	*y = ny;
}

const TPolyhedron& CCourse::GetPoly (size_t type) const {
	return PolyArr[ObjTypes[type].poly];
}

size_t CCourse::GetCourseIdx (const string& dir) const {
	return CourseIndex.at(dir);
}

void CCourse::CalcNormals () {
	for (int y=0; y<ny; y++) {
		for (int x=0; x<nx; x++) {
			TVector3d nml(0.0, 0.0, 0.0);
			TVector3d p0 (XCD(x), ELEV(x,y), ZCD(y));

			if ((x + y) % 2 == 0) {
				if (x > 0 && y > 0) {
					TVector3d p1 = NMLPOINT(x,  y-1);
					TVector3d p2 = NMLPOINT(x-1,y-1);
					TVector3d v1 = p1 - p0;
					TVector3d v2 = p2 - p0;
					TVector3d n = CrossProduct (v2, v1);

					n.Norm();
					nml += n;

					p1 = NMLPOINT (x-1, y-1);
					p2 = NMLPOINT (x-1, y);
					v1 = p1 - p0;
					v2 = p2 - p0;
					n = CrossProduct (v2, v1);

					n.Norm();
					nml += n;
				}
				if (x > 0 && y < ny-1) {
					TVector3d p1 = NMLPOINT(x-1,y);
					TVector3d p2 = NMLPOINT(x-1,y+1);
					TVector3d v1 = p1 - p0;
					TVector3d v2 = p2 - p0;
					TVector3d n = CrossProduct (v2, v1);

					n.Norm();
					nml += n;

					p1 = NMLPOINT(x-1,y+1);
					p2 = NMLPOINT(x  ,y+1);
					v1 = p1 - p0;
					v2 = p2 - p0;
					n = CrossProduct (v2, v1);

					n.Norm();
					nml += n;
				}
				if (x < nx-1 && y > 0) {
					TVector3d p1 = NMLPOINT(x+1,y);
					TVector3d p2 = NMLPOINT(x+1,y-1);
					TVector3d v1 = p1 - p0;
					TVector3d v2 = p2 - p0;
					TVector3d n = CrossProduct (v2, v1);

					n.Norm();
					nml += n;

					p1 = NMLPOINT(x+1,y-1);
					p2 = NMLPOINT(x  ,y-1);
					v1 = p1 - p0;
					v2 = p2 - p0;
					n = CrossProduct (v2, v1);

					n.Norm();
					nml += n;
				}
				if (x < nx-1 && y < ny-1) {
					TVector3d p1 = NMLPOINT(x+1,y);
					TVector3d p2 = NMLPOINT(x+1,y+1);
					TVector3d v1 = p1 - p0;
					TVector3d v2 = p2 - p0;
					TVector3d n = CrossProduct (v1, v2);

					n.Norm();
					nml += n;

					p1 = NMLPOINT(x+1,y+1);
					p2 = NMLPOINT(x  ,y+1);
					v1 = p1 - p0;
					v2 = p2 - p0;
					n = CrossProduct (v1, v2);

					n.Norm();
					nml += n;
				}
			} else {
				if (x > 0 && y > 0) {
					TVector3d p1 = NMLPOINT(x,  y-1);
					TVector3d p2 = NMLPOINT(x-1,y);
					TVector3d v1 = p1 - p0;
					TVector3d v2 = p2 - p0;
					TVector3d n = CrossProduct (v2, v1);

					n.Norm();
					nml += n;
				}
				if (x > 0 && y < ny-1) {
					TVector3d p1 = NMLPOINT(x-1,y);
					TVector3d p2 = NMLPOINT(x  ,y+1);
					TVector3d v1 = p1 - p0;
					TVector3d v2 = p2 - p0;
					TVector3d n = CrossProduct (v2, v1);

					n.Norm();
					nml += n;
				}
				if (x < nx-1 && y > 0) {
					TVector3d p1 = NMLPOINT(x+1,y);
					TVector3d p2 = NMLPOINT(x  ,y-1);
					TVector3d v1 = p1 - p0;
					TVector3d v2 = p2 - p0;
					TVector3d n = CrossProduct (v2, v1);

					n.Norm();
					nml += n;
				}
				if (x < nx-1 && y < ny-1) {
					TVector3d p1 = NMLPOINT(x+1,y);
					TVector3d p2 = NMLPOINT(x  ,y+1);
					TVector3d v1 = p1 - p0;
					TVector3d v2 = p2 - p0;
					TVector3d n = CrossProduct (v1, v2);

					n.Norm();
					nml += n;
				}
			}
			nml.Norm();
			nmls [x + nx * y] = nml;
			continue;

		}
	}
}

void CCourse::MakeCourseNormals () {
	if (nmls != NULL) delete[] nmls;
	try {
		nmls = new TVector3d[nx * ny];
	} catch (...) {
		nmls = NULL;
		Message ("Allocation failed in MakeCourseNormals");
	}
	CalcNormals ();
}

// --------------------------------------------------------------------
//					FillGlArrays
// --------------------------------------------------------------------

void CCourse::FillGlArrays() {
	TVector3d *normals = nmls;

	if (vnc_array == NULL)
		vnc_array = new GLubyte[STRIDE_GL_ARRAY * nx * ny];

	for (int x=0; x<nx; x++) {
		for (int y=0; y<ny; y++) {
			int idx = STRIDE_GL_ARRAY * (y * nx + x);

			FLOATVAL(0) = (GLfloat)x / (nx-1.0) * curr_course->size.x;
			FLOATVAL(1) = elevation[(x) + nx*(y)];
			FLOATVAL(2) = -(GLfloat)y / (ny-1.0) * curr_course->size.y;

			const TVector3d& nml = normals[ x + y * nx ];
			FLOATVAL(4) = nml.x;
			FLOATVAL(5) = nml.y;
			FLOATVAL(6) = nml.z;
			FLOATVAL(7) = 1.0f;

			BYTEVAL(0) = 255;
			BYTEVAL(1) = 255;
			BYTEVAL(2) = 255;
			BYTEVAL(3) = 255;
		}
	}
}

void CCourse::MakeStandardPolyhedrons () {
	PolyArr.resize(2);

	// polyhedron "none"
	PolyArr[0].num_vertices = 0;
	PolyArr[0].num_polygons = 0;
	PolyArr[0].vertices = NULL;
	PolyArr[0].polygons = NULL;

	// poyhedron "tree"
	PolyArr[1].num_vertices = 6;
	PolyArr[1].vertices = new TVector3d[6];
	PolyArr[1].vertices[0] = TVector3d(0, 0, 0);
	PolyArr[1].vertices[1] = TVector3d(0, 0.15, 0.5);
	PolyArr[1].vertices[2] = TVector3d(0.5, 0.15, 0);
	PolyArr[1].vertices[3] = TVector3d(0, 0.15, -0.5);
	PolyArr[1].vertices[4] = TVector3d(-0.5, 0.15, 0);
	PolyArr[1].vertices[5] = TVector3d(0, 1, 0);

	PolyArr[1].num_polygons = 8;
	PolyArr[1].polygons = new TPolygon[8];
	for (size_t i=0; i<PolyArr[1].num_polygons; i++) {
		PolyArr[1].polygons[i].num_vertices = 3;
		PolyArr[1].polygons[i].vertices = new int[3];
	}
	PolyArr[1].polygons[0].vertices[0] = 0;
	PolyArr[1].polygons[0].vertices[1] = 1;
	PolyArr[1].polygons[0].vertices[2] = 4;

	PolyArr[1].polygons[1].vertices[0] = 0;
	PolyArr[1].polygons[1].vertices[1] = 2;
	PolyArr[1].polygons[1].vertices[2] = 1;

	PolyArr[1].polygons[2].vertices[0] = 0;
	PolyArr[1].polygons[2].vertices[1] = 3;
	PolyArr[1].polygons[2].vertices[2] = 2;

	PolyArr[1].polygons[3].vertices[0] = 0;
	PolyArr[1].polygons[3].vertices[1] = 4;
	PolyArr[1].polygons[3].vertices[2] = 3;

	PolyArr[1].polygons[4].vertices[0] = 1;
	PolyArr[1].polygons[4].vertices[1] = 5;
	PolyArr[1].polygons[4].vertices[2] = 4;

	PolyArr[1].polygons[5].vertices[0] = 2;
	PolyArr[1].polygons[5].vertices[1] = 5;
	PolyArr[1].polygons[5].vertices[2] = 1;

	PolyArr[1].polygons[6].vertices[0] = 3;
	PolyArr[1].polygons[6].vertices[1] = 5;
	PolyArr[1].polygons[6].vertices[2] = 2;

	PolyArr[1].polygons[7].vertices[0] = 4;
	PolyArr[1].polygons[7].vertices[1] = 5;
	PolyArr[1].polygons[7].vertices[2] = 3;
}

void CCourse::FreeTerrainTextures () {
	for (size_t i=0; i<TerrList.size(); i++) {
		delete TerrList[i].texture;
		TerrList[i].texture = NULL;
	}
}

void CCourse::FreeObjectTextures () {
	for (size_t i=0; i<ObjTypes.size(); i++) {
		delete ObjTypes[i].texture;
		ObjTypes[i].texture = NULL;
	}
}

// --------------------------------------------------------------------
//							LoadElevMap
// --------------------------------------------------------------------

bool CCourse::LoadElevMap () {
	sf::Image img;

	if (!img.loadFromFile(CourseDir + SEP + "elev.png")) {
		Message ("unable to open elev.png");
		return false;
	}
	img.flipVertically();

	nx = img.getSize().x;
	ny = img.getSize().y;
	try {
		elevation = new double[nx * ny];
	} catch (...) {
		Message ("Allocation failed in LoadElevMap");
		return false;
	}

	double slope = tan (ANGLES_TO_RADIANS (curr_course->angle));
	int pad = 0;
	int depth = 4;
	const uint8_t* data = img.getPixelsPtr();
	for (int y=0; y<ny; y++) {
		for (int x=0; x<nx; x++) {
			elevation [(nx-1-x) + nx * (ny-1-y)] =
			    ((data[(x + nx*y) * depth + pad]
			      - base_height_value) / 255.0) * curr_course->scale
			    - (double)(ny-1-y) / ny * curr_course->size.y * slope;
		}
		pad += (nx * depth) % 4;
	}
	return true;
}

// ====================================================================
//						LoadItemList
// ====================================================================

void CCourse::LoadItemList () {
	CSPList list (16000);

	if (!list.Load (CourseDir, "items.lst")) {
		Message ("could not load items list");
		return;
	}

	CollArr.clear();
	NocollArr.clear();
	for (size_t i=0; i<list.Count(); i++) {
		const string& line = list.Line(i);
		int x = SPIntN (line, "x", 0);
		int z = SPIntN (line, "z", 0);
		double height = SPFloatN (line, "height", 1);
		double diam = SPFloatN (line, "diam", 1);
		double xx = (nx - x) / (double)(nx - 1.0) * curr_course->size.x;
		double zz = -(ny - z) / (double)(ny - 1.0) * curr_course->size.y;

		string name = SPStrN (line, "name");
		size_t type = ObjectIndex[name];
		if (ObjTypes[type].texture == NULL && ObjTypes[type].drawable) {
			string terrpath = param.obj_dir + SEP + ObjTypes[type].textureFile;
			ObjTypes[type].texture = new TTexture();
			ObjTypes[type].texture->LoadMipmap(terrpath, false);
		}
		bool coll = ObjTypes[type].collidable;
		if (coll == 1) {
			CollArr.push_back(TCollidable());
			CollArr.back().pt.x = xx;
			CollArr.back().pt.z = zz;
			CollArr.back().pt.y = FindYCoord (xx, zz);
			CollArr.back().height = height;
			CollArr.back().diam = diam;
			CollArr.back().tree_type = type;
		} else if (coll == 0) {
			NocollArr.push_back(TItem());
			NocollArr.back().pt.x = xx;
			NocollArr.back().pt.z = zz;
			NocollArr.back().pt.y = FindYCoord (xx, zz);
			NocollArr.back().height = height;
			NocollArr.back().diam = diam;
			NocollArr.back().item_type = type;
			NocollArr.back().collectable = ObjTypes[type].collectable;
			NocollArr.back().drawable = ObjTypes[type].drawable;
			ObjTypes[type].num_items++;
		}
	}
}

// --------------------	LoadObjectMap ---------------------------------


static int GetObject (const unsigned char* pixel) {
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

#define TREE_MIN 2.0
#define TREE_MAX 5.0
#define BARREN_MIN 4.0
#define BARREN_MAX 8.0
#define SHRUB_MIN 1.0
#define SHRUB_MAX 2.0

const double sizefact[6] = {0.5, 0.5, 0.7, 1.0, 1.4, 2.0};
const double varfact[6] = {1.0, 1.0, 1.22, 1.41, 1.73, 2.0};
const double diamfact = 1.4;

static void CalcRandomTrees (double baseheight, double basediam, double &height, double &diam) {
	double hhh = baseheight * sizefact[g_game.treesize];
	double minsiz = hhh / varfact[g_game.treevar];
	double maxsiz = hhh * varfact[g_game.treevar];
	height = XRandom (minsiz, maxsiz);
	diam = XRandom (height/diamfact, height);
}

bool CCourse::LoadAndConvertObjectMap () {
	sf::Image treeImg;

	if (!treeImg.loadFromFile (CourseDir + SEP + "trees.png")) {
		Message ("unable to open trees.png");
		return false;
	}
	treeImg.flipVertically();

	int pad = 0;
	int cnt = 0;
	int depth = 4;
	const unsigned char* data = (unsigned char*)treeImg.getPixelsPtr();
	double height, diam;
	CSPList savelist (10000);

	CollArr.clear();
	NocollArr.clear();
	for (int y=0; y<ny; y++) {
		for (int x=0; x<nx; x++) {
			int imgidx = (x + nx * y) * depth + pad;
			int type = GetObject (&data[imgidx]);
			if (type >= 0) {
				cnt++;
				double xx = (nx - x) / (double)(nx - 1.0) * curr_course->size.x;
				double zz = -(ny - y) / (double)(ny - 1.0) * curr_course->size.y;
				if (ObjTypes[type].texture == NULL && ObjTypes[type].drawable) {
					string terrpath = param.obj_dir + SEP + ObjTypes[type].textureFile;
					ObjTypes[type].texture = new TTexture();
					ObjTypes[type].texture->LoadMipmap(terrpath, false);
				}

				// set random height and diam - see constants above
				switch (type) {
					case 5:
						CalcRandomTrees (2.5, 2.5, height, diam);
						break;
					case 6:
						CalcRandomTrees (3, 3, height, diam);
						break;
					case 7:
						CalcRandomTrees (1.2, 1.2, height, diam);
						break;

					case 2:
					case 3:
						height = 6.0;
						diam = 9.0;
						break;

					default:
						height = 1;
						diam = 1;
						break;
				}

				bool coll = ObjTypes[type].collidable;
				if (coll == 1) {
					CollArr.push_back(TCollidable());
					CollArr.back().pt.x = xx;
					CollArr.back().pt.z = zz;
					CollArr.back().pt.y = FindYCoord (xx, zz);
					CollArr.back().height = height;
					CollArr.back().diam = diam;
					CollArr.back().tree_type = type;
				} else if (coll == 0) {
					NocollArr.push_back(TItem());
					NocollArr.back().pt.x = xx;
					NocollArr.back().pt.z = zz;
					NocollArr.back().pt.y = FindYCoord (xx, zz);
					NocollArr.back().height = height;
					NocollArr.back().diam = diam;
					NocollArr.back().item_type = type;
					NocollArr.back().collectable = ObjTypes[type].collectable;
					NocollArr.back().drawable = ObjTypes[type].drawable;
					ObjTypes[type].num_items++;
				}

				string line = "*[name]";
				line += ObjTypes[type].name;
				SPSetIntN (line, "x", x);
				SPSetIntN (line, "z", y);
				SPSetFloatN (line, "height", height, 1);
				SPSetFloatN (line, "diam", diam, 1);
				savelist.Add (line);
			}
		}
		pad += (nx * depth) % 4;
	}
	string itemfile = CourseDir + SEP + "items.lst";
	savelist.Save (itemfile); // Convert trees.png to items.lst
	return true;
}

// --------------------------------------------------------------------
//						LoadObjectTypes
// --------------------------------------------------------------------

bool CCourse::LoadObjectTypes () {
	CSPList list (MAX_OBJECT_TYPES+10);

	if (!list.Load (param.obj_dir, "object_types.lst")) {
		Message ("could not load object types");
		return false;
	}

	ObjTypes.resize(list.Count());

	for (size_t i=0; i<list.Count(); i++) {
		const string& line = list.Line(i);
		ObjTypes[i].name = SPStrN (line, "name");
		ObjTypes[i].textureFile = ObjTypes[i].name;
		ObjTypes[i].texture = NULL;

		ObjTypes[i].drawable = SPBoolN (line, "draw", true);
		if (ObjTypes[i].drawable) {
			ObjTypes[i].textureFile = SPStrN (line, "texture");
		}
		ObjTypes[i].collectable = SPBoolN (line, "snap", -1) != 0;
		if (ObjTypes[i].collectable == 0) {
			ObjTypes[i].collectable = -1;
		}

		ObjTypes[i].collidable = SPBoolN (line, "coll", false);
		ObjTypes[i].reset_point = SPBoolN (line, "reset", false);
		ObjTypes[i].use_normal = SPBoolN (line, "usenorm", false);

		if (ObjTypes[i].use_normal) {
			ObjTypes[i].normal = SPVector3(line, "norm", TVector3d(0, 1, 0));
			ObjTypes[i].normal.Norm();
		}
		ObjTypes[i].poly = 1;
	}
	list.MakeIndex (ObjectIndex, "name");
	return true;
}

// ====================================================================
//						Terrain
// ====================================================================

int CCourse::GetTerrain (const unsigned char* pixel) const {
	for (size_t i=0; i<TerrList.size(); i++) {
		if (abs(pixel[0]-(int)(TerrList[i].col.r)) < 30
		        && abs(pixel[1]-(int)(TerrList[i].col.g)) < 30
		        && abs(pixel[2]-(int)(TerrList[i].col.b)) < 30) {
			return (int)i;
		}
	}
	return 0;
}

// --------------------------------------------------------------------
//         				LoadTerrainTypes
// --------------------------------------------------------------------

bool CCourse::LoadTerrainTypes () {
	CSPList list(MAX_TERR_TYPES +10);

	if (!list.Load (param.terr_dir, "terrains.lst")) {
		Message ("could not load terrain types");
		return false;
	}

	TerrList.resize(list.Count());
	for (size_t i=0; i<list.Count(); i++) {
		const string& line = list.Line(i);
		TerrList[i].textureFile = SPStrN (line, "texture");
		TerrList[i].sound = Sound.GetSoundIdx (SPStrN (line, "sound"));
		TerrList[i].starttex = SPIntN (line, "starttex", -1);
		TerrList[i].tracktex = SPIntN (line, "tracktex", -1);
		TerrList[i].stoptex = SPIntN (line, "stoptex", -1);
		TerrList[i].col = SPColor3N (line, "col", TColor3(1, 1, 1));
		TerrList[i].friction = SPFloatN (line, "friction", 0.5);
		TerrList[i].depth = SPFloatN (line, "depth", 0.01);
		TerrList[i].particles = SPBoolN (line, "part", false);
		TerrList[i].trackmarks = SPBoolN (line, "trackmarks", false);
		TerrList[i].texture = NULL;
		TerrList[i].shiny = SPBoolN(line, "shiny", false);
		TerrList[i].vol_type = SPIntN (line, "vol_type", 1);
	}
	return true;
}

// --------------------------------------------------------------------
//					LoadTerrainMap
// --------------------------------------------------------------------

bool CCourse::LoadTerrainMap () {
	sf::Image terrImage;

	if (!terrImage.loadFromFile (CourseDir + SEP + "terrain.png")) {
		Message ("unable to open terrain.png");
		return false;
	}
	terrImage.flipVertically();
	if (nx != terrImage.getSize().x || ny != terrImage.getSize().y) {
		Message ("wrong terrain size");
	}

	try {
		terrain = new char[nx * ny];
	} catch (...) {
		Message ("Allocation failed in LoadTerrainMap");
	}
	int depth = 4;
	const unsigned char* data = (const unsigned char*) terrImage.getPixelsPtr();
	int pad = 0;
	for (int y=0; y<ny; y++) {
		for (int x=0; x<nx; x++) {
			int imgidx = (x+nx*y) * depth + pad;
			int arridx = (nx-1-x) + nx * (ny-1-y);
			int terr = GetTerrain (&data[imgidx]);
			terrain[arridx] = terr;
			if (TerrList[terr].texture == NULL) {
				TerrList[terr].texture = new TTexture();
				TerrList[terr].texture->LoadMipmap(param.terr_dir, TerrList[terr].textureFile, true);
			}
		}
		pad += (nx * depth) % 4;
	}
	return true;
}

// --------------------------------------------------------------------
//					LoadCourseList
// --------------------------------------------------------------------

bool CCourse::LoadCourseList () {
	CSPList list (128);

	if (!list.Load (param.common_course_dir, "courses.lst")) {
		Message ("could not load courses.lst");
		return false;
	}

	CSPList paramlist (48);

	CourseList.resize(list.Count());
	for (size_t i=0; i<list.Count(); i++) {
		const string& line1 = list.Line(i);
		CourseList[i].name = SPStrN (line1, "name", "noname");
		CourseList[i].dir = SPStrN (line1, "dir", "nodir");

		string desc = SPStrN (line1, "desc");
		FT.AutoSizeN (2);
		vector<string> desclist = FT.MakeLineList (desc.c_str(), 335 * Winsys.scale - 16.0);
		size_t cnt = min<size_t>(desclist.size(), MAX_DESCRIPTION_LINES);
		CourseList[i].num_lines = cnt;
		for (size_t ll=0; ll<cnt; ll++) {
			CourseList[i].desc[ll] = desclist[ll];
		}

		string coursepath = param.common_course_dir + SEP + CourseList[i].dir;
		if (DirExists (coursepath.c_str())) {
			// preview
			string previewfile = coursepath + SEP + "preview.png";
			CourseList[i].preview = new TTexture();
			if (!CourseList[i].preview->LoadMipmap(previewfile, false)) {
				Message ("couldn't load previewfile");
//				texid = Tex.TexID (NO_PREVIEW);
			}

			// params
			string paramfile = coursepath + SEP + "course.dim";
			if (!paramlist.Load (paramfile)) {
				Message ("could not load course.dim");
			}

			const string& line2 = paramlist.Line (0);
			CourseList[i].author = SPStrN (line2, "author", "unknown");
			CourseList[i].size.x = SPFloatN (line2, "width", 100);
			CourseList[i].size.y = SPFloatN (line2, "length", 1000);
			CourseList[i].play_size.x = SPFloatN (line2, "play_width", 90);
			CourseList[i].play_size.y = SPFloatN (line2, "play_length", 900);
			CourseList[i].angle = SPFloatN (line2, "angle", 10);
			CourseList[i].scale = SPFloatN (line2, "scale", 10);
			CourseList[i].start.x = SPFloatN (line2, "startx", 50);
			CourseList[i].start.y = SPFloatN (line2, "starty", 5);
			CourseList[i].env = Env.GetEnvIdx (SPStrN (line2, "env", "etr"));
			CourseList[i].music_theme = Music.GetThemeIdx (SPStrN (line2, "theme", "normal"));
			CourseList[i].use_keyframe = SPBoolN (line2, "use_keyframe", false);
			CourseList[i].finish_brake = SPFloatN (line2, "finish_brake", 20);
			paramlist.Clear ();	// the list is used several times
		}
	}
	list.MakeIndex (CourseIndex, "dir");
	return true;
}

void CCourse::FreeCourseList () {
	for (size_t i=0; i<CourseList.size(); i++) {
		delete CourseList[i].preview;
	}
	CourseList.clear();
}

//  ===================================================================
//					LoadCourse
//  ===================================================================

void CCourse::ResetCourse () {
	if (nmls != NULL) {delete[] nmls; nmls = NULL;}
	if (vnc_array != NULL) {delete[] vnc_array; vnc_array = NULL;}
	if (elevation != NULL) {delete[] elevation; elevation = NULL;}
	if (terrain != NULL) {delete[] terrain; terrain = NULL;}

	FreeTerrainTextures ();
	FreeObjectTextures ();
	ResetQuadtree ();
	curr_course = NULL;
	mirrored = false;
}

bool CCourse::LoadCourse (size_t idx) {
	if (idx >= CourseList.size()) {
		Message ("wrong course index");
		curr_course = NULL;
		return false;
	}

	if (&CourseList[idx] != curr_course || g_game.force_treemap) {
		ResetCourse ();
		curr_course = &CourseList[idx];
		CourseDir = param.common_course_dir + SEP + curr_course->dir;

		start_pt.x = CourseList[idx].start.x;
		start_pt.y = -CourseList[idx].start.y;
		base_height_value = 127;

		g_game.use_keyframe = CourseList[idx].use_keyframe;
		g_game.finish_brake = CourseList[idx].finish_brake;

		if (!LoadElevMap ()) {
			Message ("could not load course elev map");
			return false;
		}

		MakeCourseNormals ();
		FillGlArrays ();

		if (!LoadTerrainMap ()) {
			Message ("could not load course terrain map");
			return false;
		}

		// ................................................................
		string itemfile = CourseDir + SEP + "items.lst";
		bool itemsexists = FileExists (itemfile);
		const CControl *ctrl = Players.GetCtrl (g_game.player_id);

		if (itemsexists && !g_game.force_treemap)
			LoadItemList ();
		else
			LoadAndConvertObjectMap ();
		g_game.force_treemap = false;
		// ................................................................

		init_track_marks ();
		InitQuadtree (
		    elevation, nx, ny,
		    curr_course->size.x / (nx - 1.0),
		    -curr_course->size.y / (ny - 1.0),
		    ctrl->viewpos,
		    param.course_detail_level);
	}

	if (g_game.mirror_id != mirrored) {
		MirrorCourse ();
		mirrored = g_game.mirror_id;
	}
	return true;
}

size_t CCourse::GetEnv () const {
	return curr_course->env;
}

// --------------------------------------------------------------------
//				mirror course
// --------------------------------------------------------------------

void CCourse::MirrorCourseData () {
	for (int y=0; y<ny; y++) {
		for (int x=0; x<nx/2; x++) {
			double tmp = ELEV(x,y);
			ELEV(x,y) = ELEV(nx-1-x, y);
			ELEV(nx-1-x,y) = tmp;

			int idx1 = (x+1) + nx*(y);
			int idx2 = (nx-1-x) + nx*(y);
			swap(terrain[idx1], terrain[idx2]);

			idx1 = (x) + nx*(y);
			idx2 = (nx-1-x) + nx*(y);
			swap(nmls[idx1], nmls[idx2]);
			nmls[idx1].x *= -1;
			nmls[idx2].x *= -1;
		}
	}

	for (size_t i=0; i<CollArr.size(); i++) {
		CollArr[i].pt.x = curr_course->size.x - CollArr[i].pt.x;
		CollArr[i].pt.y = FindYCoord (CollArr[i].pt.x, CollArr[i].pt.z);
	}

	for (size_t i=0; i<NocollArr.size(); i++) {
		NocollArr[i].pt.x = curr_course->size.x - NocollArr[i].pt.x;
		NocollArr[i].pt.y = FindYCoord (NocollArr[i].pt.x, NocollArr[i].pt.z);
	}

	FillGlArrays();

	ResetQuadtree ();
	if (nx > 0 && ny > 0) {
		const CControl *ctrl = Players.GetCtrl (g_game.player_id);
		InitQuadtree (elevation, nx, ny, curr_course->size.x/(nx-1),
		              - curr_course->size.y/(ny-1), ctrl->viewpos, param.course_detail_level);
	}

	start_pt.x = curr_course->size.x - start_pt.x;
}

void CCourse::MirrorCourse () {
	MirrorCourseData ();
	init_track_marks ();
}

// ********************************************************************
//				from phys_sim:
// ********************************************************************

void CCourse::GetIndicesForPoint(double x, double z, int *x0, int *y0, int *x1, int *y1) const {

	double xidx = x / curr_course->size.x * ((double) nx - 1.);
	double yidx = -z / curr_course->size.y *  ((double) ny - 1.);

	if (xidx < 0) xidx = 0;
	else if (xidx > nx-1) xidx = nx-1;

	if (yidx < 0) yidx = 0;
	else if (yidx > ny-1) yidx = ny-1;

	*x0 = (int)(xidx);              // floor(xidx)
	*x1 = (int)(xidx + 0.9999);     // ceil(xidx)
	*y0 = (int)(yidx);              // floor(yidx)
	*y1 = (int)(yidx + 0.9999);     // ceil(yidx)

	if (*x0 == *x1) {
		if (*x1 < nx - 1) (*x1)++;
		else (*x0)--;
	}

	if (*y0 == *y1) {
		if (*y1 < ny - 1) (*y1)++;
		else (*y0)--;
	}
}

void CCourse::FindBarycentricCoords(double x, double z, TVector2i *idx0,
                                    TVector2i *idx1, TVector2i *idx2, double *u, double *v) const {
	double xidx, yidx;
	int x0, x1, y0, y1;
	double dx, ex, dz, ez, qx, qz, invdet;

	GetIndicesForPoint (x, z, &x0, &y0, &x1, &y1);
	xidx = x / curr_course->size.x * ((double) nx - 1.);
	yidx = -z / curr_course->size.y * ((double) ny - 1.);

	if ((x0 + y0) % 2 == 0) {
		if (yidx - y0 < xidx - x0) {
			*idx0 = TVector2i(x0, y0);
			*idx1 = TVector2i(x1, y0);
			*idx2 = TVector2i(x1, y1);
		} else {
			*idx0 = TVector2i(x1, y1);
			*idx1 = TVector2i(x0, y1);
			*idx2 = TVector2i(x0, y0);
		}
	} else {
		if (yidx - y0 + xidx - x0 < 1) {
			*idx0 = TVector2i(x0, y0);
			*idx1 = TVector2i(x1, y0);
			*idx2 = TVector2i(x0, y1);
		} else {
			*idx0 = TVector2i(x1, y1);
			*idx1 = TVector2i(x0, y1);
			*idx2 = TVector2i(x1, y0);
		}
	}

	dx = idx0->x - idx2->x;
	dz = idx0->y - idx2->y;
	ex = idx1->x - idx2->x;
	ez = idx1->y - idx2->y;
	qx = xidx - idx2->x;
	qz = yidx - idx2->y;

	invdet = 1 / (dx * ez - dz * ex);
	*u = (qx * ez - qz * ex) * invdet;
	*v = (qz * dx - qx * dz) * invdet;
}

#define COURSE_VERTX(_x, _y) TVector3d ( (double)(_x)/(nx-1.)*curr_course->size.x, \
                       ELEV((_x),(_y)), -(double)(_y)/(ny-1.)*curr_course->size.y )

TVector3d CCourse::FindCourseNormal (double x, double z) const {

	double *elevation = Course.elevation;
	int x0, x1, y0, y1;
	GetIndicesForPoint (x, z, &x0, &y0, &x1, &y1);

	TVector2i idx0, idx1, idx2;
	double u, v;
	FindBarycentricCoords (x, z, &idx0, &idx1, &idx2, &u, &v);

	const TVector3d& n0 = Course.nmls[ idx0.x + nx * idx0.y ];
	const TVector3d& n1 = Course.nmls[ idx1.x + nx * idx1.y ];
	const TVector3d& n2 = Course.nmls[ idx2.x + nx * idx2.y ];

	TVector3d p0 = COURSE_VERTX (idx0.x, idx0.y);
	TVector3d p1 = COURSE_VERTX (idx1.x, idx1.y);
	TVector3d p2 = COURSE_VERTX (idx2.x, idx2.y);

	TVector3d smooth_nml = u * n0 +
	                       v * n1 +
	                       (1.-u-v) * n2;

	TVector3d tri_nml = CrossProduct(p1 - p0, p2 - p0);
	tri_nml.Norm();

	double min_bary = min (u, min (v, 1. - u - v));
	double interp_factor = min (min_bary / NORM_INTERPOL, 1.0);

	TVector3d interp_nml = interp_factor * tri_nml + (1.-interp_factor) * smooth_nml;
	interp_nml.Norm();

	return interp_nml;
}

double CCourse::FindYCoord (double x, double z) const {
	static double last_x, last_z, last_y;
	static bool cache_full = false;

	if (cache_full && last_x == x && last_z == z) return last_y;
	double *elevation = Course.elevation;

	TVector2i idx0, idx1, idx2;
	double u, v;
	FindBarycentricCoords (x, z, &idx0, &idx1, &idx2, &u, &v);

	TVector3d p0 = COURSE_VERTX (idx0.x, idx0.y);
	TVector3d p1 = COURSE_VERTX (idx1.x, idx1.y);
	TVector3d p2 = COURSE_VERTX (idx2.x, idx2.y);

	double ycoord = u * p0.y + v * p1.y +  (1. - u - v) * p2.y;

	last_x = x;
	last_z = z;
	last_y = ycoord;
	cache_full = true;

	return ycoord;
}

void CCourse::GetSurfaceType (double x, double z, double weights[]) const {
	TVector2i idx0, idx1, idx2;
	double u, v;
	FindBarycentricCoords (x, z, &idx0, &idx1, &idx2, &u, &v);

	char *terrain = Course.terrain;
	for (size_t i=0; i<Course.TerrList.size(); i++) {
		weights[i] = 0;
		if (terrain [idx0.x + nx*idx0.y ] == i) weights[i] += u;
		if (terrain [idx1.x + nx*idx1.y ] == i) weights[i] += v;
		if (terrain [idx2.x + nx*idx2.y ] == i) weights[i] += 1.0 - u - v;
	}
}

int CCourse::GetTerrainIdx (double x, double z, double level) const {
	TVector2i idx0, idx1, idx2;
	double u, v;
	FindBarycentricCoords (x, z, &idx0, &idx1, &idx2, &u, &v);
	char *terrain = Course.terrain;

	for (size_t i=0; i<Course.TerrList.size(); i++) {
		double wheight = 0.0;
		if (terrain [idx0.x + nx*idx0.y] == i) wheight += u;
		if (terrain [idx1.x + nx*idx1.y] == i) wheight += v;
		if (terrain [idx2.x + nx*idx2.y] == i) wheight += 1.0 - u - v;
		if (wheight > level) return (int)i;
	}
	return -1;
}

TPlane CCourse::GetLocalCoursePlane (TVector3d pt) const {
	TPlane plane;
	pt.y = FindYCoord (pt.x, pt.z);
	plane.nml = FindCourseNormal (pt.x, pt.z);
	plane.d = -DotProduct(plane.nml, pt);
	return plane;
}
