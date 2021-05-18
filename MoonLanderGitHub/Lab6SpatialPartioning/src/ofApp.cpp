
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
// 
//
//  Student Name:   < Your Name goes Here >
//  Date: <date of last version>


#include "ofApp.h"
#include "Util.h"
#include <glm/gtx/intersect.hpp>



//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup() {
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bRoverLoaded = false;
	bTerrainSelected = true;
	//	ofSetWindowShape(1024, 768);
	gravitationalForce = glm::vec3(0, -terrainGravity, 0);
	//GUI PANEL THIS 
	cam.setDistance(52.48);
	//cam.setNearClip(8.425);
	//cam.setFov(56.44);   // approx equivalent to 28mm in 35mm format
	timer = 0;

	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	rotation = 90.0;

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();
	//load the terrain
	printf("Map loaded. Creating Octree....\n");
	if (mars.loadModel("geo/mars-low-5x-v2.obj"))
	{
		mars.setScaleNormalization(false);
		printf("Map loaded, creating octree...\n");
		//time goes here if you want
		octree.create(mars.getMesh(0), 20);
		printf("Octree created!");
	}
	else
	{
		printf("Map could not be loaded.\n");
		ofExit(0);
	}
	if (rover.loadModel("geo/lander.obj"))
	{
		printf("ROVER HAS BEEN LOADED!");
		rover.setScale(0.005, 0.005, 0.005);
		rover.setRotation(0, 180.0, 1, 0, 0);
		rover.setPosition(0, 5, 0);
		bRoverLoaded = true;
	}
	else
	{
		printf("Vehicle could not be loaded.\n");
		ofExit(0);
	}


	//mars.setRotation(0, 180, 0, 0, 1);


	//  Create Octree for testing.


	//Octree level gui
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));

	bHide = false;




	cout << "Number of Verts: " << mars.getMesh(0).getNumVertices() << endl;

	testBox = Box(Vector3(3, 3, 0), Vector3(5, 5, 2));

	startTime = 0;

	float time = ofGetElapsedTimef();

	//physics initialization


	gui.setup();
	gui.add(thrust.setup("Thrust", 100, 1, 1000));
	//gui.add(camDist.setup("Cam Distance", 52.48, 1, 100));
	gui.add(camNearClip.setup("Cam Near Clip", 8.425, 1, 100));
	gui.add(camSetFOV.setup("Cam FOV", 56.44, 1, 100));



}
//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
	if (bStart)
	{
		checkCollisions();
		if (bgrounded)
		{
			velocity = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			force = glm::vec3(0, 0, 0);
			bOver = true;
			//cout << "GAME OVER" << endl;
		}
		//ray cast on to the surface of the terrain from the position of the rover

		Ray altitudeRay = Ray(Vector3(rover.getPosition().x, rover.getPosition().y, rover.getPosition().z), Vector3(rover.getPosition().x, rover.getPosition().y - 200, rover.getPosition().z));
		TreeNode altNode;
		/*if(octree.intersect())*/
		if (octree.intersect(altitudeRay, octree.root, altNode))
		{
			altitude = glm::length(octree.mesh.getVertex(altNode.points[0]) - rover.getPosition());
		}



		int tempTime = ofGetElapsedTimeMillis() / 1000;
		if (!bOver)
		{
			timer = tempTime - startTime;
		}
		//cout << "CAMERA POSITION:\n" << cam.getPosition() << endl;

		cam.setDistance(camDist);
		cam.setNearClip(camNearClip);
		cam.setFov(camSetFOV);


		//integrate();
		//cout << "ROVER POSITION:" << rover.getPosition() << endl;
		//force = glm::vec3(0, 0, 0);
		//cout << "ROVER POSITION BEFORE INTEGRATE:\n" << rover.getPosition() << endl;
		integrate();

		glm::vec3 roverPosition = rover.getPosition();

		//cout << "ROVER POSITION AFTER: \n" << rover.getPosition() << endl;
		//zero out the forces
		force = glm::vec3(0, 0, 0);
	}


}
//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(ofColor::black);

	glDepthMask(false);
	if (!bHide) gui.draw();
	glDepthMask(true);

	cam.begin();
	//push objects you want to draw on the rover or bounding box onto the object space

	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		mars.drawWireframe();
		if (bRoverLoaded) {
			rover.drawWireframe();
			if (!bTerrainSelected) drawAxis(rover.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		mars.drawFaces();
		ofMesh mesh;
		if (bRoverLoaded) {

			rover.drawFaces();
			if (!bTerrainSelected) drawAxis(rover.getPosition());
			if (bDisplayBBoxes) {
				ofNoFill();
				ofSetColor(ofColor::white);
				for (int i = 0; i < rover.getNumMeshes(); i++) {
					ofPushMatrix();
					ofMultMatrix(rover.getModelMatrix());
					ofRotate(-90, 1, 0, 0);
					Octree::drawBox(bboxList[i]);
					ofPopMatrix();
				}
			}

			if (bRoverSelected) {

				ofVec3f min = rover.getSceneMin() + rover.getPosition();
				ofVec3f max = rover.getSceneMax() + rover.getPosition();

				roverBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

				ofSetColor(ofColor::green);
				ofNoFill();
				Octree::drawBox(roverBounds);

				// draw colliding boxes
				//
				ofSetColor(ofColor::lightBlue);
				for (int i = 0; i < colBoxList.size(); i++) {
					Octree::drawBox(colBoxList[i]);
				}
			}
		}
	}
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));



	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		mars.drawVertices();
	}

	// highlight selected point (draw sphere around selected point)
	//
	if (bPointSelected) {
		ofSetColor(ofColor::blue);
		ofDrawSphere(selectedPoint, .1);
	}


	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();

	if (bDisplayLeafNodes) {
		octree.drawLeafNodes(octree.root);
		cout << "num leaf: " << octree.numLeaf << endl;
	}
	else if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
	}

	// if point selected on the terrain, draw a sphere
	//
	//cout << "POINT SELECTED:\n" << pointSelected << endl;
	if (pointSelected) {
		//if point selected after clicking and intersecting with a box, drawsphere
		//cout << "POINT SELECTED:\n" << pointSelected << endl;
		ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
		ofVec3f d = p - cam.getPosition();
		ofSetColor(ofColor::lightGreen);
		ofDrawSphere(p, .02 * d.length());
	}
	//ofSetColor(ofColor::red);
	//ofDrawLine(rover.getPosition(), rover.getPosition() * 1000);

	ofPopMatrix();
	cam.end();

	if (!bStart)
	{
		ofSetColor(ofColor::white);
		ofDrawBitmapString("Press Spacebar to Start", (ofGetWindowWidth() / 2) - 92, ofGetWindowHeight() / 2 - 5);
	}
	if (bStart && !bOver)
	{
		drawText();
	}

}


// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	//RED
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));


	// Y Axis
	//GREEN
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	//BLUE
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {
	//insert multiply key inputs later
	glm::vec3 roverPosition = rover.getPosition();
	switch (key) {
	case 'B':
	case 'b':
		//this crashes my program for some odd reason
		//bDisplayBBoxes = !bDisplayBBoxes;
		break;
	case 'C':
	case 'c':

		if (cam.getMouseInputEnabled())
			cam.disableMouseInput();

		else
			cam.enableMouseInput();

		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'n':
	case 'N':
		bDisplayLeafNodes = !bDisplayLeafNodes;
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 'r':
		cam.reset();
		break;
	case 's':
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;

	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
		//our movement will be based on the 3D coordinates.
		//up = Y coordinates
		//left, right movement = X coordinates
		//towards the screen = Z coordinates

		//X DIRECTION
	case OF_KEY_UP:
		bThrust = true;
		force = float(thrust) * ofVec3f(1, 0, 0);

		angularVelocity = 0;
		break;
	case OF_KEY_DOWN:
		bThrust = true;
		force = float(thrust) * ofVec3f(-1, 0, 0);
		angularVelocity = 0;
		break;
		//Z DIRECTION
	case OF_KEY_RIGHT:
		bThrust = true;
		force = float(thrust) * ofVec3f(0, 0, 1);
		angularVelocity = 0;
		break;
	case OF_KEY_LEFT:
		bThrust = true;
		force = float(thrust) * ofVec3f(0, 0, -1);
		angularVelocity = 0;
		break;
		//Y DIRECTION
	case ' ':
		if (!bStart)
		{
			bStart = true;
			startTime = ofGetElapsedTimeMillis() / 1000;
		}
		bStart = true;
		force = float(thrust) * ofVec3f(0, 1, 0);
		angularVelocity = 0;
		break;
	case OF_KEY_CONTROL:
		force = float(thrust) * ofVec3f(0, -1, 0);
		angularVelocity = 0;
		break;
		//rotation inputs
	case 'j':
	case 'J':
		bThrust = true;
		angularForce = 100.0;
		break;
	case 'l':
	case 'L':
		bThrust = true;
		angularForce = -100.0;
		break;
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key)
	{

	case 'j':
	case 'J':
		bThrust = false;
		angularForce = 0;
		break;
	case 'l':
	case 'L':
		bThrust = false;
		angularForce = 0;
		break;
		//break;
	//case 'j':
	//case 'J':
	//	angularForce = 0;
	//	bThrust = false;
	//	//break;
	//case 'l':
	//case 'L':
	//	bThrust = 0;
	//	angularForce = 0;
		//break;





	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;

		/*case OF_KEY_CONTROL:
			bCtrlKeyDown = false;
			break;*/
	case OF_KEY_SHIFT:
		break;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {


}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don'ft allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	// if moving camera, don't allow mouse interaction
//
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bRoverLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = rover.getSceneMin() + rover.getPosition();
		ofVec3f max = rover.getSceneMax() + rover.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bRoverSelected = true;
			mouseDownPos = getMousePointOnPlane(rover.getPosition(), cam.getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bRoverSelected = false;
		}
	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
		cout << "POINT SELECTED: " << raySelectWithOctree(p) << endl;
		//cout << "POINT SELECTED: POINT NOT FOUND!" << raySelectWithOctree(p) << endl;
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	//determines 
	pointSelected = octree.MouseIntersect(ray, octree.root, selectedNode);

	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
		cout << "POINT RET:" << pointRet << endl;
	}
	return pointSelected;
}





//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	if (bInDrag) {

		glm::vec3 roverPos = rover.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(roverPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;

		roverPos += delta;
		rover.setPosition(roverPos.x, roverPos.y, roverPos.z);
		rover.setRotation(0, rotation, 1, 0, 0);
		mouseLastPos = mousePos;

		ofVec3f min = rover.getSceneMin() + rover.getPosition();
		ofVec3f max = rover.getSceneMax() + rover.getPosition();

		Box roverBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		//if the octree intersects the rover bounds, 
		//octree intersects, boxes are pushed on to an array of collision boxes
		octree.BoxIntersect(roverBounds, octree.root, colBoxList);



		if (roverBounds.overlap(testBox)) {
			cout << "overlap" << endl;
		}
		else {
			cout << "OK" << endl;
		}


	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{ 5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
}

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent2(ofDragInfo dragInfo) {

	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);
	//loading of the model
	if (rover.loadModel(dragInfo.files[0])) {
		rover.setScaleNormalization(false);
		rover.setScale(.1, .1, .1);
		rover.setPosition(1, 1, 0);

		bRoverLoaded = true;
		for (int i = 0; i < rover.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(rover.getMesh(i)));
		}

		cout << "Mesh Count: " << rover.getMeshCount() << endl;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
//THIS DRAG EVENT IS FOR THE BOUNDING BOX CREATION
void ofApp::dragEvent(ofDragInfo dragInfo) {
	if (rover.loadModel(dragInfo.files[0])) {
		bRoverLoaded = true;
		rover.setScaleNormalization(false);
		//rover.setRotation(0, 180, 0, 0, 1);
		rover.setPosition(0, 0, 0);
		cout << "number of meshes: " << rover.getNumMeshes() << endl;
		bboxList.clear();
		for (int i = 0; i < rover.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(rover.getMesh(i)));
		}

		//rover.setRotation(1, 180, 1, 0, 0);

		// We want to drag and drop a 3D object in space so that the model appears 
		// under the mouse pointer where you drop it !
		//
		// Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
		// once we find the point of intersection, we can position the rover/rover
		// at that location.
		//

		// Setup our rays
		//
		glm::vec3 origin = cam.getPosition();
		glm::vec3 camAxis = cam.getZAxis();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
		float distance;

		bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
		if (hit) {
			// find the point of intersection on the plane using the distance 
			// We use the parameteric line or vector representation of a line to compute
			//
			// p' = p + s * dir;
			//
			glm::vec3 intersectPoint = origin + distance * mouseDir;

			// Now position the rover's origin at that intersection point
			//
			glm::vec3 min = rover.getSceneMin();
			glm::vec3 max = rover.getSceneMax();
			float offset = (max.y - min.y) / 2.0;
			rover.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

			// set up bounding box for rover while we are at it
			//
			roverBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		}
	}


}

//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}


void ofApp::checkCollisions()
{
	ofVec3f min = rover.getSceneMin() + rover.getPosition();
	ofVec3f max = rover.getSceneMax() + rover.getPosition();

	Box roverBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	colBoxList.clear();
	//if the octree intersects the rover bounds, 
	//octree intersects, boxes are pushed on to an array of collision boxes
	if (octree.BoxIntersect(roverBounds, octree.root, colBoxList))
	{
		printf("BOX HAS INTERSECTED OCTREE!");
		bgrounded = true;
		//zero out gravity
		gravitationalForce = glm::vec3(0, 0, 0);

	}



	/*if (roverBounds.overlap(testBox)) {
		cout << "overlap" << endl;
	}
	else {
		cout << "OK" << endl;
	}*/

}
void ofApp::drawText()
{
	ofSetColor(ofColor::white);
	string altitudeText = "Altitude: " + std::to_string(altitude);

	//60 frames per second
	int framerate = ofGetFrameRate();

	string fpsText = "Frame Rate:" + std::to_string(framerate);

	string timerText = "Time: " + std::to_string(timer);

	ofDrawBitmapString(altitudeText, 10, 15);
	ofDrawBitmapString(fpsText, ofGetWindowWidth() - 130, 15);
	ofDrawBitmapString(timerText, 10, 40);
}
//draws landing zones



