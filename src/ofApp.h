#pragma once

#include "ofMain.h"
#include "ofxOculusDK2.h"
#include "ofxVboParticles.h"
#include "ofxOsc.h"


// listen on port 57110
#define PORT 57110
#define NUM_MSG_STRINGS 20

typedef struct{
	ofColor color;
	ofVec3f pos;
	ofVec3f floatPos;
	float radius;
    bool bMouseOver;
    bool bGazeOver;
} DemoSphere;

class ofApp : public ofBaseApp
{
  public:
	
	void setup();
	void update();
	void draw();
	
	void drawScene();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofxOculusDK2		oculusRift;

	ofLight				light;
	ofEasyCam			cam;
    
	bool showOverlay;
	bool predictive;
	vector<DemoSphere> demos;
    
    ofVec3f cursor2D;
    ofVec3f cursor3D;
    
    ofVec3f cursorRift;
    ofVec3f demoRift;
    
    ofVec3f cursorGaze;
    
    //particle
    void spawn_particle(int x, int y, int z, int num, float hue);
    ofxVboParticles *vboPartciles;
    
    //osc
    ofTrueTypeFont font;
    ofxOscReceiver receiver;
    
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];

};
