#include "ofApp.h"

/*
 TODO:
 
 Get OSC data
 relate Particles to OSC data
 Add Particle behaviour i.e. fade out with time, camera distance
 Add Particle Tile that uses specific Sensordata and spawns Particles
 Fix Camera reposition viewpoint
 
 */

//--------------------------------------------------------------
void ofApp::setup()
{
    ofBackground(0);
    ofSetLogLevel( OF_LOG_VERBOSE );
    ofSetVerticalSync( true );
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    cam.setFov(80);
    
    oculusRift.baseCamera = &cam;
    oculusRift.setup();
    
    ofHideCursor();
    
    sensorList = {"AF3", "AB2", "bla", "bla", "bla", "bla"};
    sensorReading = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    
    // ofxVboParticles([max particle number], [particle size]);
    //vboParticles = new ofxVboParticles(60000, 2000);
    // set friction (0.0 - 1.0);

    for (int i = 0; i < sensorList.size(); i++)
    {
        vboParticles = new ofxVboParticles(10000, 5000);
        vboParticles->friction = 0.00;
        vboParticles->fade = 0.99;
        particleTiles.push_back(vboParticles);
    }
    
    tileWidth = 400.0;
    tileHeight = 600.0;
    arcAngle = 180.0;
    distanceTiles= 600.0;
    
    particleSpread = 2.0;
    particleSpeed = 10.0;

    ofLog()<<"sensorList size:"<< sensorList.size();
    ofLog()<<"particleTiles size:"<< particleTiles.size();
    
    show_particle = true;
    color_on = true;
    show_grid = true;
    show_port = true;
    osc_port = 57110;
    
    showOverlay = false;
    predictive = true;
    
    //osc
    cout << "listening for osc messages on port " << osc_port << "\n";
    receiver.setup(osc_port);
    current_msg_string = 0;
    
    //enable mouse;
    cam.begin();
    cam.end();
    
}

float ofApp::parseMessage(ofxOscMessage &msg)
{
}


void ofApp::get_osc_messages()
{
    //osc
    /*
     // hide old messages
     for (int i= 0; i < NUM_MSG_STRINGS; i++) {
     if (timers[i] < ofGetElapsedTimef()) {
     msg_strings[i] = "";
     }
     }
     */
    //check for waiting messages
    while (receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        
        //check for incoming messages
        if (m.getAddress() == "/AF3") {
            string msg_string;
            msg_string = m.getAddress();
            msg_string += ": ";
            
            for (int i = 0; i < m.getNumArgs(); i++) {
                msg_string += m.getArgTypeName(i);
                msg_string += ":";
                
                if(m.getArgType(i) == OFXOSC_TYPE_INT32)  {
                    msg_string += ofToString(m.getArgAsInt32(i));
                }
                
                else if (m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                    msg_string += ofToString(m.getArgAsFloat(i));
                }
                else if (m.getArgType(i) == OFXOSC_TYPE_STRING){
                    msg_string += m.getArgAsString(i);
                }
                else{
                    msg_string += "sensors";
                }
            }
        }
    }
}

void ofApp::spawn_particles(float reading, ofColor color, ofxVboParticles *particleTile)
{
    //mapping Sensorreadings to Spawningpoint in VR
    float y = ofMap(reading, 0.0, 800.0, -tileHeight, tileHeight);
    ofVec3f position;
    ofVec3f velocity;

    int numParticlesSpawned = 10;
    for (int i = 0; i < numParticlesSpawned; i++)
    {
        position = ofVec3f(ofRandom(-tileWidth, tileWidth), y, 0.0);
        velocity = ofVec3f(ofRandom(-particleSpread, particleSpread), 0, particleSpeed);
        // add a particle
        particleTile->addParticle(position, velocity, color);
    }
}


//--------------------------------------------------------------
void ofApp::update()
{
    
    for (int i = 0; i < sensorList.size(); i++)
    {
        sensorReading[i] = ofRandom(800.0);
    }
    
    //particle
    
    for (int i = 0; i < particleTiles.size(); i++)
    {
        ofColor color(255,0,255);
        color.setHue((255/particleTiles.size())*i);
        if(!color_on) color.setSaturation(0);
        spawn_particles(sensorReading[i], color, particleTiles[i]);
    }

    for( auto tile: particleTiles)
    {
        tile->update();
    }
    
    
    //set cameraposition to 0,0,0
    cam.setPosition(ofVec3f());
    
}

//--------------------------------------------------------------
void ofApp::drawScene()
{
    
    if(show_grid)
    {
        ofPushMatrix();
        ofRotate(90, 0, 0, -1);
        
        ofDrawGridPlane(500.0f, 10.0f, false );
        ofPopMatrix();
    }
    
    
    ofPushStyle();
    ofNoFill();
    
    if (show_particle)
    {
        
        for (int i = 0; i < particleTiles.size(); i++)
        {
            int numTiles = particleTiles.size();
            float angleStep = arcAngle/numTiles;
            
            float angle = angleStep * i;
            //create position in arc
            ofVec3f pos(sin(i*ofDegToRad(angleStep))*distanceTiles, 0.0, cos(i*ofDegToRad(angleStep))*distanceTiles);
            
            ofPushMatrix();
            //look into the arc
            ofRotateY(180-arcAngle/2+angleStep/2);
            ofPushMatrix();
            ofTranslate(pos);
            ofRotateY(angle);
            particleTiles[i]->draw();
            ofPopMatrix();
            ofPopMatrix();
        }
        
        //cam.begin();
        //ofRotate(ofGetElapsedTimef() * 20, 1, 1, 0);
        
        /*
        // draw particles
        ofPushMatrix();
        ofRotate(45, 0, 1, 0);
        ofTranslate(-300, 0);
        vboParticles->draw();
        ofPopMatrix();
        ofPushMatrix();
        ofRotate(-45, 0, 1, 0);
        ofTranslate(300, 0);
        vboParticles->draw();
        ofPopMatrix();
        
        */
        
        //cam.end();
    }
    
    //billboard and draw the mouse
    if(oculusRift.isSetup()){
        
        ofPushMatrix();
        oculusRift.multBillboardMatrix();
        ofSetColor(255, 0, 0);
        ofCircle(0,0,.5);
        ofPopMatrix();
        
    }
    
    ofPopStyle();
    
}

//--------------------------------------------------------------
void ofApp::draw()
{
    if(oculusRift.isSetup()){
        
        if(showOverlay){
            
            oculusRift.beginOverlay(-230, 320,240);
            ofRectangle overlayRect = oculusRift.getOverlayRectangle();
            
            ofPushStyle();
            ofEnableAlphaBlending();
            ofFill();
            ofSetColor(255, 40, 10, 200);
            
            ofRect(overlayRect);
            
            ofSetColor(255,255);
            ofFill();
            ofDrawBitmapString(ofToString(ofGetFrameRate(), 4) + "fps", 10, 20);
            ofDrawBitmapString("particle num = " + ofToString(vboParticles->numParticles) + "\nPredictive Tracking " + (oculusRift.getUsePredictiveOrientation() ? "YES" : "NO"), 10, 35);
            ofDrawBitmapString("[f] key : toggle fullscreen", 10, 60);
            if (show_port) {
                string buf;
                buf = "listening for osc messages on port" + ofToString(osc_port);
                ofDrawBitmapString(buf, 10, 80);
            }
            ofDrawBitmapString( "Camposition:" + ofToString(cam.getPosition()), 10, 100);
            
            ofSetColor(0, 255, 0);
            ofNoFill();
            ofCircle(overlayRect.getCenter(), 20);
            
            ofPopStyle();
            oculusRift.endOverlay();
        }
        
        ofSetColor(255);
        glEnable(GL_DEPTH_TEST);
        
        oculusRift.beginLeftEye();
        drawScene();
        oculusRift.endLeftEye();
        
        oculusRift.beginRightEye();
        drawScene();
        oculusRift.endRightEye();
        
        oculusRift.draw();
        
        glDisable(GL_DEPTH_TEST);
        
    }
    else{
        cam.begin();
        drawScene();
        cam.end();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if( key == 'f' )
    {
        //gotta toggle full screen for it to be right
        ofToggleFullscreen();
    }
    
    if (key == 'g')
    {
        //toggle grid
        show_grid = !show_grid;
    }
    
    if (key == 'c')
        
    {
        color_on = !color_on;
    }
    
    if(key == 's'){
        oculusRift.reloadShader();
    }
    
    if(key == 'l'){
        oculusRift.lockView = !oculusRift.lockView;
    }
    
    if(key == 'o'){
        showOverlay = !showOverlay;
    }
    if(key == 'r'){
        oculusRift.reset();
        
    }
    if(key == 'h'){
        ofHideCursor();
    }
    if(key == 'H'){
        ofShowCursor();
    }
    
    if(key == 'p'){
        predictive = !predictive;
        oculusRift.setUsePredictedOrientation(predictive);
    }
    if(key == '1')
    {
        show_particle= !show_particle;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
    //   cursor2D.set(x, y, cursor2D.z);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
    //    cursor2D.set(x, y, cursor2D.z);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
    
}

/*
 
 ParticleTile::ParticleTile(int _x, int _y, int _z, int _maxP, int _sizeP)
 : x(_x), y(_y), z(_z), maxParticles(_maxP), particleSize(_sizeP)
 {
 }
 */
