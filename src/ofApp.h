#pragma once

#include "ofMain.h"
#include "ofxOculusDK2.h"
#include "ofxVboParticles.h"
#include "ofxOsc.h"

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
    
    bool show_particle;
    bool color_on;
    bool show_grid;
    bool show_port;
    
    int osc_port;
    
    ofVec3f cursor2D;
    ofVec3f cursor3D;
    
    ofVec3f cursorRift;
    ofVec3f demoRift;
    
    ofVec3f cursorGaze;
    
    //sensor
    vector<string> sensorList;
    vector<float> sensorReading;
    
    //particle
    //void generate_particle_tiles();
    void spawn_particles(float reading, ofColor color, ofxVboParticles *particleTile);
    ofxVboParticles *vboParticles;
    
    vector<ofxVboParticles*> particleTiles;
    float tileWidth, tileHeight;
    float arcAngle;
    float distanceTiles;
    
    //osc
    ofTrueTypeFont font;
    ofxOscReceiver receiver;
    
    float parseMessage(ofxOscMessage & msg);
    
    
    int current_msg_string;
    
    void get_osc_messages();
};
/*
class ParticleTile
{
    ParticleTile(ofVec3f pos, int width, int height, int maxParticles, int particleSize);
    ofVec3f tilePos;
    int width, height;
    int maxParticles, particleSize;
    
    void update();
    void draw();
    
    ofxVboParticles *vboParticles;
};
*/
/*
class Sensor
{
public:
    Sensor();
    void update();
    
    void spawnParticles();
    
    ofxVboParticles *vboParticles;
    float reading;
};

class PythonEEGReceiver
{
public:
    PythonEEGReceiver(int port, vector<Sensor> sensor_list);
    
    int port;
    ofxOscReceiver receiver;
    void parseMessage();
    vector<Sensor> sensors;
    
    vector<String> sensor_list;
    
};

vector<String> sensors{"AF3", "EF"};
PythonEEGReceiver epoc(57100, sensors);
epoc.parseMessage();
*/
