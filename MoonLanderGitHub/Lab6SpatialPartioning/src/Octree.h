
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Simple Octree Implementation 11/10/2020
// 
//  Copyright (c) by Kevin M. Smith
//  Copying or use without permission is prohibited by law.
//
#pragma once
#include "ofMain.h"
#include "box.h"
#include "ray.h"


//Tree Node
class TreeNode {
public:
	Box box;
	vector<int> points;
	vector<TreeNode> children;
};

class Octree {
public:
	//creates the octree
	void create(const ofMesh & mesh, int numLevels);

	void subdivide(const ofMesh & mesh, TreeNode & node, int numLevels, int level);
	bool intersect(const Ray & ray, const TreeNode & node, TreeNode & nodeRtn);
	//test function with a ray
	bool MouseIntersect(const Ray &, const TreeNode & node, TreeNode & nodeRtn);
	//intersection with a box
	/*

	work on this intersect for the homework project
	*/
	bool BoxIntersect(const Box &, TreeNode & node, vector<Box> & boxListRtn);

	//collisions detection
	bool intersect(const ofVec3f &, const TreeNode & node, TreeNode & nodeRtn);


	//draw hierarchal boxes
	void draw(TreeNode & node, int numLevels, int level);

	//recursive drawing of the octree
	void draw(int numLevels, int level) {
		draw(root, numLevels, level);
	}


	//leaf level drawing
	void drawLeafNodes(TreeNode & node);
	//draw the bounding box
	static void drawBox(const Box &box);

	//drawing bounds around entire mesh
	static Box meshBounds(const ofMesh &);
	//determines which points are in which box
	int getMeshPointsInBox(const ofMesh &mesh, const vector<int> & points, Box & box, vector<int> & pointsRtn);

	int getMeshFacesInBox(const ofMesh &mesh, const vector<int> & faces, Box & box, vector<int> & facesRtn);

	//used to subdivide a box into 8 boxes
	void subDivideBox8(const Box &b, vector<Box> & boxList);

	ofMesh mesh;
	TreeNode root;
	bool bUseFaces = false;

	// debug;
	//
	int strayVerts = 0;
	int numLeaf = 0;
};