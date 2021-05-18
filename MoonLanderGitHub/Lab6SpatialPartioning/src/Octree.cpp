
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Simple Octree Implementation 11/10/2020
// 
//  Copyright (c) by Kevin M. Smith
//  Copying or use without permission is prohibited by law. 
//


#include "Octree.h"



//draw a box from a "Box" class  
//given a box type, draws a box
void Octree::drawBox(const Box &box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
	ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//

//Bounding box for the entire Mesh
Box Octree::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	cout << "vertices: " << n << endl;
	//	cout << "min: " << min << "max: " << max << endl;
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

// getMeshPointsInBox:  return an array of indices to points in mesh that are contained 
//                      inside the Box.  Return count of points found;
//comparison routine: given a mesh, if the points in the array are inside the box, it returns a list of points inside the box 
int Octree::getMeshPointsInBox(const ofMesh & mesh, const vector<int>& points,
	Box & box, vector<int> & pointsRtn)
{
	int count = 0;
	for (int i = 0; i < points.size(); i++) {
		ofVec3f v = mesh.getVertex(points[i]);
		if (box.inside(Vector3(v.x, v.y, v.z))) {
			count++;
			pointsRtn.push_back(points[i]);
		}
	}
	return count;
}

// getMeshFacesInBox:  return an array of indices to Faces in mesh that are contained 
//                      inside the Box.  Return count of faces found;
//
int Octree::getMeshFacesInBox(const ofMesh & mesh, const vector<int>& faces,
	Box & box, vector<int> & facesRtn)
{
	int count = 0;
	for (int i = 0; i < faces.size(); i++) {
		ofMeshFace face = mesh.getFace(faces[i]);
		ofVec3f v[3];
		v[0] = face.getVertex(0);
		v[1] = face.getVertex(1);
		v[2] = face.getVertex(2);
		Vector3 p[3];
		p[0] = Vector3(v[0].x, v[0].y, v[0].z);
		p[1] = Vector3(v[1].x, v[1].y, v[1].z);
		p[2] = Vector3(v[2].x, v[2].y, v[2].z);
		if (box.inside(p, 3)) {
			count++;
			facesRtn.push_back(faces[i]);
		}
	}
	return count;
}

//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void Octree::subDivideBox8(const Box &box, vector<Box> & boxList) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	float xdist = (max.x() - min.x()) / 2;
	float ydist = (max.y() - min.y()) / 2;
	float zdist = (max.z() - min.z()) / 2;
	Vector3 h = Vector3(0, ydist, 0);

	//  generate ground floor
	//
	Box b[8];
	b[0] = Box(min, center);
	b[1] = Box(b[0].min() + Vector3(xdist, 0, 0), b[0].max() + Vector3(xdist, 0, 0));
	b[2] = Box(b[1].min() + Vector3(0, 0, zdist), b[1].max() + Vector3(0, 0, zdist));
	b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0), b[2].max() + Vector3(-xdist, 0, 0));

	boxList.clear();
	for (int i = 0; i < 4; i++)
		boxList.push_back(b[i]);

	// generate second story
	//
	for (int i = 4; i < 8; i++) {
		b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
		boxList.push_back(b[i]);
	}
}

void Octree::create(const ofMesh & geo, int numLevels) {
	// initialize octree structure
	//
	//int actualTime = ofGetElapsedTimeMillis();
	//cout << " CREATING THE TREE:" << endl;
	//cout << "ACUTAL TIME:" << actualTime / 1000 << "SECONDS" << endl;
	mesh = geo;
	int level = 0;
	root.box = meshBounds(mesh);
	if (!bUseFaces) {
		for (int i = 0; i < mesh.getNumVertices(); i++) {
			root.points.push_back(i);
		}
	}
	else {
		// need to load face vertices here
		//
	}

	// recursively buid octree
	//
	level++;
	subdivide(mesh, root, numLevels, level);
	//int endTime = ofGetElapsedTimeMillis();
	//cout << "CREATED THE TREE!" << endl;
	//cout << "END TIME:" << endTime / 1000 << "SECONDS" << endl;
}

//
void Octree::subdivide(const ofMesh & mesh, TreeNode & node, int numLevels, int level) {
	if (level >= numLevels) return;
	vector<Box> boxList;
	subDivideBox8(node.box, boxList);
	level++;
	int pointsInNode = node.points.size();
	int totalPoints = 0;
	for (int i = 0; i < boxList.size(); i++) {
		TreeNode child;
		int count = 0;
		if (!bUseFaces)
			count = getMeshPointsInBox(mesh, node.points, boxList[i], child.points);
		else
			//this case is never supported
			count = getMeshFacesInBox(mesh, node.points, boxList[i], child.points);
		totalPoints += count;

		if (count > 0) {
			child.box = boxList[i];
			node.children.push_back(child);
			if (count > 1) {
				subdivide(mesh, node.children.back(), numLevels, level);
			}
		}


	}
	// debug
	//
	if (pointsInNode != totalPoints) {
		strayVerts += (pointsInNode - totalPoints);
	}
}

// Implement functions below for Homework project
//

//HOMEWORK ASSIGNMENT


/*
What did I just do here: our MouseIntersects takes in a ray, a treenode and whatever noteRtn is. i guess that is the returning node?
we have a flag: bool intersects  = false
if our ray intersects with a our box, which is a node, let's do some recursion to fine tuning getting the point we are clicking in 3D.

we have an array of 8 nodes initialized and is passed in to this method.

that will be our base case: if the array.size == 0, intersects = true and we set the returning node = node.

else

loop through out array of nodes.

if our mouse intersects with a node again, recursively call our function.
*/
//Will be used to check for Altitude

//check if a ray intersects a treenode
bool Octree::intersect(const Ray &ray, const TreeNode & node, TreeNode & nodeRtn)
{
	bool intersects = false;
	if (node.box.intersect(ray, 0, INFINITE))
	{
		if (node.children.size() == 0)
		{
			nodeRtn = node;
			intersects = true;
		}
		else
		{
			for (int i = 0; i < node.children.size(); i++)
			{
				if (intersect(ray, node.children[i], nodeRtn))
				{
					intersects = true;
				}
			}
		}
	}
	return intersects;

}

bool Octree::MouseIntersect(const Ray &ray, const TreeNode & node, TreeNode & nodeRtn) {

	bool intersects = false;
	if (node.box.intersect(ray, 0, INFINITE))
	{
		int startTime = ofGetElapsedTimeMillis();
		/*	cout << "BEGIN RAY INTERSECTION SELECTION\n" << endl;
			cout << "START TIME:" << startTime / 1000 << "SECONDS\n" << endl;*/
		if (node.children.size() == 0)
		{
			nodeRtn = node;
			intersects = true;
			int endTime = ofGetElapsedTimeMillis();
			/*	cout << "END RAY INTERSECTION SELECTION\n" << endl;
				cout << "END TIME:" << endTime / 1000 << "SECONDS\n" << endl;*/
		}
		else {
			for (int i = 0; i < node.children.size(); i++) {
				if (MouseIntersect(ray, node.children[i], nodeRtn))
				{

					intersects = true;

				}
			}
			int endTime = ofGetElapsedTimeMillis();
			/*	cout << "END RAY INTERSECTION SELECTION\n" << endl;
				cout << "END TIME:" << endTime / 1000 << "SECONDS\n" << endl;*/
		}

	}

	return intersects;
}


bool Octree::BoxIntersect(const Box &box, TreeNode & node, vector<Box> & boxListRtn) {
	bool intersects = false;
	if (node.box.overlap(box))
	{
		if (node.children.size() == 0) { boxListRtn.push_back(node.box); intersects = true; }
		else
		{
			for (int i = 0; i < node.children.size(); i++)
			{
				if (BoxIntersect(box, node.children[i], boxListRtn))
				{
					intersects = true;
				}
			}
		}
	}
	return intersects;
}
////Checking collision
//
//bool Octree::intersect(const ofVec3f & vec, const TreeNode & node, TreeNode & nodeRtn)
//{
//	Box temp = node.box;
//	if (temp.inside(Vector3(vec.x, vec.y, vec.z)))
//	{
//		if (node.children.size() == 0)
//		{
//			nodeRtn = node;
//			return true;
//		}
//		else
//		{
//			for (unsigned int i = 0; i < node.children.size(); i++)
//			{
//				if (intersect(vec, node.children[i], nodeRtn))
//				{
//					return true;
//				}
//			}
//		}
//	}
//}

void Octree::draw(TreeNode & node, int numLevels, int level) {
	if (level >= numLevels) return;
	drawBox(node.box);
	level++;
	for (int i = 0; i < node.children.size();i++)
	{

		draw(node.children[i], numLevels, level);
		//maybe make an associative array to color the levels?

	}
}

//HOMEWORK ASSIGNMENT

// Optional
//
void Octree::drawLeafNodes(TreeNode & node) {


}




