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
    
    //define Epoc Sensors
    sensorList = {"/AF3", "/F7", "/F4","/F3", "/F8", "/AF4"};
    
    //define min and max inputvalues
    readingMin = 10.0;
    readingMax = 75.0;
    
    //initialize array of Sensorvalues
    sensorReading = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    
    //start Colors for Sensors
    startColors = {ofColor(0, 0, 200), ofColor(170, 50, 170), ofColor(200, 200, 200),
        ofColor(200, 200, 200), ofColor(170, 50, 170), ofColor(0, 0, 200)};
    //end Colors for sensors;
    endColors = {ofColor(0, 0, 200), ofColor(200, 0, 200), ofColor(200, 200, 200),
        ofColor(200, 200, 200), ofColor(200, 0, 200), ofColor(0, 0, 200)};
    
    
    colorStart.set(200, 50, 200);
    colorEnd.set( 0, 50, 200);
    
    
    //setup Particletiles depending on entries in sensorList
    for (int i = 0; i < sensorList.size(); i++)
    {
        vboParticles = new ofxVboParticles(5000, 2000);
        //friction is not used
        //vboParticles->friction = 0.00;
        //fadoutspeed of particles
        vboParticles->fade = 0.001;
        particleTiles.push_back(vboParticles);
    }
    
    //attributes of Particletiles
    numParticlesSpawned = 8;
    
    tileWidth = 60.0;
    tileHeight = 500.0;
    arcAngle = 40.0;
    distanceTiles= 600;
    //particle attributes
    particleSpread = 0.5;
    particleSpeed = 3;
    
    //cam
    rotX = 0;
    posY = -80;
    posZ = -260;
    
    ofLog()<<"sensorList size:"<< sensorList.size();
    ofLog()<<"particleTiles size:"<< particleTiles.size();
    
    render_oculus = true;
    show_particle = true;
    color_on = true;
    show_port = true;
    osc_port = 57140;
    
    showOverlay = false;
    predictive = true;
    
    //osc
    receiver.setup(osc_port);
    
    //enable mouse;
    cam.begin();
    cam.end();
    
}


void ofApp::get_osc_messages()
{
    
    //check for waiting messages
    while (receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        //take the all Sensoradresses from sensorList
        for (int i = 0; i < sensorList.size(); i++)
        {
            //compare it to the OSC message adress
            if(m.getAddress() == sensorList[i])
            {
                //save the readings in sensorReadings
                sensorReading[i] = m.getArgAsFloat(0);
            }
        }
    }
    
}

void ofApp::spawn_particles(float reading, ofColor color, ofxVboParticles *particleTile)
{
    //mapping Sensorreadings to Spawningpoint in VR
    float normalize = ofMap(reading, readingMin, readingMax, 0.0, 1.0);
    float y = tileHeight * normalize - tileHeight/2;
    ofVec3f position;
    ofVec3f velocity;
    color = colorStart.getLerped(colorEnd, normalize);
    
    int numParticlesSpawned = 10;
    for (int i = 0; i < numParticlesSpawned; i++)
    {
        position = ofVec3f(ofRandom(-tileWidth, tileWidth), y, 0.0);
        velocity = ofVec3f(ofRandom(-particleSpread, particleSpread), 0, particleSpeed);
        // add a particle
        particleTile->addParticle(position, velocity, color);
    }
}

void ofApp::spawn_particles()
{
    for(int i = 0; i < particleTiles.size(); i++)
    {
        
        //update all particles
        particleTiles[i]->update();
        
        //mapping Sensorreadings to Spawningpoint in VR
        float normalize = ofMap(sensorReading[i], readingMin, readingMax, 0.0, 1.0);
        float y = tileHeight * normalize - tileHeight/2;
        ofVec3f position;
        ofVec3f velocity;
        ofColor color(startColors[i].getLerped(endColors[i], normalize));
        color.setBrightness(color.getBrightness() + ofRandom(-20, 50));
        
        //ofLog() << "Sensor"+ofToString(i)+" :"+ofToString(sensorReading[i]);
        
        for (int j = 0; j < numParticlesSpawned; j++)
        {
            ofVec3f oldPosition = position;
            position = ofVec3f(ofRandom(-tileWidth, tileWidth), ofRandom(-2,2)+y, 0.0);
            velocity = ofVec3f(ofRandom(-particleSpread, particleSpread), 0, particleSpeed);
            // add a particle
            particleTiles[i]->addParticle(position, velocity, color);
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::update()
{
    //osc
    if(true) get_osc_messages();
    else
        for (int i = 0; i < sensorReading.size(); i++)
        {
            sensorReading[i] = ofRandom(readingMin, readingMax);
        }
    
    //particle
    spawn_particles();
    
    
    //set cameraposition to 0,0,0
    cam.setPosition(ofVec3f(0, 0, posZ));
    
    ofLog() << "posZ: "+ofToString(posZ)+" posY: "+ofToString(posY)+" rotX: "+ofToString(rotX)+" tileWidth: "+ofToString(tileWidth)+" tileHeight: "+ofToString(tileHeight);
    
}

//--------------------------------------------------------------
void ofApp::drawScene()
{
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
            ofVec3f pos(sin(i*ofDegToRad(angleStep))*distanceTiles, posY, cos(i*ofDegToRad(angleStep))*distanceTiles);
            
            ofPushMatrix();
            ofRotateX(rotX);
            //look into the arc
            ofRotateY(180-arcAngle/2+angleStep/2);
            ofPushMatrix();
            ofTranslate(pos);
            ofRotateY(angle);
            particleTiles[i]->draw();
            ofPopMatrix();
            ofPopMatrix();
        }
        
    }
    
    //billboard and draw the mouse
    if(oculusRift.isSetup()){
        
        ofPushMatrix();
        oculusRift.multBillboardMatrix();
        ofSetColor(200, 0, 0);
        ofCircle(0,0,.5);
        ofPopMatrix();
        
    }
    ofPopStyle();
    
}

//--------------------------------------------------------------
void ofApp::draw()
{
    if(render_oculus && oculusRift.isSetup()){
        
        if(showOverlay){
            
            oculusRift.beginOverlay(-230, 320,240);
            ofRectangle overlayRect = oculusRift.getOverlayRectangle();
            
            ofPushStyle();
            //ofEnableAlphaBlending();
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
            for (int i = 0; i < sensorReading.size(); i++)
            {
                ofDrawBitmapString( "sensorReading"+ ofToString(i) +": "+ofToString(sensorReading[i]), 10, 110+(10*i));
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
void ofApp::keyPressed(int key)
{
    if( key == 'f' )
    {
        //gotta toggle full screen for it to be right
        ofToggleFullscreen();
    }
    
    if (key == 'c')
        
    {
        color_on = !color_on;
    }
    
    if(key == 's'){
        oculusRift.reloadShader();
    }
    
    if(key == 'l'){
        //oculusRift.lockView = !oculusRift.lockView;
    }
    
    if(key == 'o'){
        //showOverlay = !showOverlay;
    }
    if(key == 'r'){
        oculusRift.reset();
        for (int i = 0; i < particleTiles.size(); i++)
        {
            particleTiles[i]->reset();
        }
        
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
    if(key == '+') rotX +=1;
    if(key == '#') rotX -=1;
    if(key == 'o') posY +=5;
    if(key == 'l') posY -=5;
    if(key == 'O') posZ +=5;
    if(key == 'L') posZ -=5;
    if(key == 'i') tileWidth+=5;
    if(key == 'k') tileWidth-=5;
    if(key == 'I') tileHeight+=5;
    if(key == 'K') tileHeight-=5;
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
