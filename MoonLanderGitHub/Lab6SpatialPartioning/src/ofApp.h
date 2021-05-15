#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"



class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent2(ofDragInfo dragInfo);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void drawAxis(ofVec3f);
	void initLightingAndMaterials();
	void savePicture();
	void toggleWireframeMode();
	void togglePointsDisplay();
	void toggleSelectTerrain();
	bool doPointSelection();
	void setCameraTarget();

	//way to select points
	bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
	bool raySelectWithOctree(ofVec3f &pointRet);
	glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p, glm::vec3 n);



	ofEasyCam cam;
	//setup cameras later
	ofCamera bottomCam, frontCam, followingCam, topCam, *theCam;

	//NEW CHANGES

	//implement basic physics
	float rotation = 0.0;
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	glm::vec3 force = glm::vec3(0, 0, 0);
	float gravity = 0.0;
	float mass = 1.0;
	float damping = .99;
	//angular physics
	float angularForce = 0;
	float angularVelocity = 0.0;
	float angularAcceleration = 0.0;
	bool bThrust = false;
	//void integrate();


	ofxAssimpModelLoader mars, rover;

	ofLight light;
	//bounds: the bounding box of the rover
	Box roverBounds, boundingBox;
	Box testBox;
	vector<Box> colBoxList;
	bool bRoverSelected = false;
	Octree octree;
	TreeNode selectedNode;
	glm::vec3 mouseDownPos, mouseLastPos;
	bool bInDrag = false;

	//number of octrees that can be generated
	ofxIntSlider numLevels;


	bool bAltKeyDown;
	bool bCtrlKeyDown;
	bool bWireframe;
	bool bDisplayPoints;
	bool bPointSelected;
	bool bHide;
	bool pointSelected = false;
	bool bDisplayLeafNodes = false;
	bool bDisplayOctree = false;
	bool bDisplayBBoxes = false;

	bool bRoverLoaded;
	bool bTerrainSelected;

	ofVec3f selectedPoint;
	ofVec3f intersectPoint;

	vector<Box> bboxList;

	const float selectionRange = 4.0;

	int startTime;

	int i = 0;

	int maxRotations = 4;
	int currentRotations = 0;

	glm::vec3 heading()
	{
		glm::vec3 initialHeading = glm::vec3(0, 1, 0); // heading at start
		//Mrot means matrix rotation
		glm::mat4 Mrot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
		glm::vec3 h = Mrot * glm::vec4(initialHeading, 1);
		return glm::normalize(h);
	}


	//gui

	ofxPanel gui;
	ofxFloatSlider thrust;


	glm::vec3 roverPosition;

	void ofApp::integrate() {
			
	}
};
