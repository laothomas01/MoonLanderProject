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
	glm::vec3 gravitationalForce;
	float terrainGravity = 3.711;
	float mass = 1.0;
	float damping = .99;
	//angular physics
	float angularForce = 0;
	float angularVelocity = 0.0;
	float angularAcceleration = 0.0;
	bool bThrust = false;

	//start game or end game
	bool bStart = false;
	bool bOver = false;

	//models
	ofxAssimpModelLoader mars, rover;

	ofLight light;
	//bounds: the bounding box of the rover
	Box roverBounds;
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
	bool bgrounded = false;
	bool bRoverLoaded;
	bool bTerrainSelected;


	float score = 0.0;
	float altitude = 0.0;
	ofVec3f selectedPoint;
	ofVec3f intersectPoint;

	vector<Box> bboxList;

	const float selectionRange = 4.0;

	int startTime;
	int timer;
	
	int i = 0;

	int maxRotations = 4;
	int currentRotations = 0;
	glm::vec3 initialHeading = glm::vec3(0, 0, 1); // heading at start
	//glm::vec3 heading()
	//{
	//	//start the facing direction in the Z axis
	//	//Mrot means matrix rotation
	//	glm::mat4 Mrot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
	//	glm::vec3 h = Mrot * glm::vec4(initialHeading, 1);
	//	return glm::normalize(h);
	//}


	//gui

	ofxPanel gui;
	ofxFloatSlider thrust, camDist, camNearClip, camSetFOV;


	glm::vec3 roverPosition;

	bool rotateX = false;
	bool rotateY = false;
	bool rotateZ = false;
	void ofApp::integrate() {
		// what is ofGetFrameRate dependent upon
		//float framerate = ofGetFrameRate();	// breaking
		float framerate = 60;	// workaround
		float dt = 1.0 / framerate;
		roverPosition = rover.getPosition();
		//cout << "FR: " << framerate << endl;
		//cout << "DT: " << dt << endl;
		//cout << "RP: " << roverPosition << endl;
		//linear motion
		roverPosition += velocity * dt;
		//cout << "RP Update: " << roverPosition << endl;
		//rover.setPosition(roverPosition.x = roverPosition.x + (velocity.x * 1/60), roverPosition.y = roverPosition.y + (velocity.z * 1/60), roverPosition.z = roverPosition.z + (velocity.z * 1/60));
		rover.setPosition(roverPosition.x + (velocity.x * dt),
			roverPosition.y + (velocity.z * dt),
			roverPosition.z + (velocity.z * dt));
		rover.setPosition(roverPosition.x, roverPosition.y, roverPosition.z);

		glm::vec3 accel = acceleration;
		accel += (force * 1.0 / mass);
		velocity += accel * dt;
		velocity *= damping;

		rotation += (angularVelocity * dt);

		rover.setRotation(0, rotation, 1, 0, 0);


		//force += gravitationalForce * mass;
		/*if (rotateY)
		{
			rover.setRotation(1, rotation, 1, 0, 0);
		}
		if (rotateZ)
		{
			rover.setRotation(2, rotation, 0, 0, 1);
		}*/
		float a = angularAcceleration;
		a += (angularForce * 1.0 / mass);
		angularVelocity += a * dt;
		angularVelocity *= damping;
	}
	//for taking multiply key inputs
	map<int, bool> keymap;

	void checkCollisions();
	void drawText();
	void makeLandingZone();

};

