#include "ofApp.h"

/*
 TODO:
 
 Get OSC data
 relate Particles to OSC data
 Add Particle behaviour i.e. fade out with time, camera distance
 Add Particle Tile that uses specific Sensordata and spawns Particles
 
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
    
    // ofxVboParticles([max particle number], [particle size]);
    vboPartciles = new ofxVboParticles(60000, 2000);
    // set friction (0.0 - 1.0);
    vboPartciles->friction = 0.00;
    vboPartciles->fade = 0.99;
    
    show_particle = true;
    color_on = true;
    show_grid = true;
    show_port = true;
    osc_port = 57110;
    
    showOverlay = false;
    predictive = true;
    
    sensorList = {"AF3", "AB2"};
    
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
        generate_particle_tiles(m);
        
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

void ofApp::generate_particle_tiles(ofxOscMessage msg)
{
    int x_offset = -25 - sensorList.size()/2;
    
    for(int i = 0; i < sensorList.size(); i++)
    {
        if(msg.getAddress() == sensorList[i])
        {
            for (int arg = 0; arg < msg.getNumArgs(); arg++)
            {
                int reading = msg.getArgAsInt(arg);
                int x_pos = x_offset + i * 50;
                spawn_particle(x_pos, reading, 0, 10, i * 40);
            }
        }
    }
    
}

void ofApp::spawn_particle(int x, int y, int z, int num, float hue)
{
    for (int i = 0; i < num; i++)
    {
        ofVec3f position = ofVec3f(ofRandom(-50, 50), ofRandom(-200, 200), 100);
        ofVec3f velocity = ofVec3f(0, 0, -2);
        ofColor color;
        int sat;
        if(color_on) sat = 200;
        else sat = 0;
        color.setHsb(hue, sat, 255);
        
        position += ofVec3f( x , y, z);
        // add a particle
        vboPartciles->addParticle(position, velocity, color);
    }
}


//--------------------------------------------------------------
void ofApp::update()
{
    
    //particle
    spawn_particle(-175, 0, 0, 10, 0);
    spawn_particle(-125, 0, 0, 10, 40);
    spawn_particle( -25, 0, 0, 10, 80);
    spawn_particle(  25, 0, 0, 10, 120);
    spawn_particle( 125, 0, 0, 10, 160);
    spawn_particle( 175, 0, 0, 10, 220);
    
    
    vboPartciles->update();
    //magically enable mouserotation
    oculusRift.worldToScreen(ofVec3f(), true);

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
            ofDrawBitmapString("particle num = " + ofToString(vboPartciles->numParticles) + "\nPredictive Tracking " + (oculusRift.getUsePredictiveOrientation() ? "YES" : "NO"), 10, 35);
            ofDrawBitmapString("[f] key : toggle fullscreen", 10, 60);
            if (show_port) {
                string buf;
                buf = "listening for osc messages on port" + ofToString(osc_port);
                ofDrawBitmapString(buf, 10, 80);
            }
            
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
        //cam.begin();
        //ofRotate(ofGetElapsedTimef() * 20, 1, 1, 0);
        
        // draw particles
        vboPartciles->draw();
        
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
