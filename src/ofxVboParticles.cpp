//
//  ofxVboParticles.cpp
//
//  Created by Atsushi Tadokoro on 8/14/14.
//
//

#include "ofxVboParticles.h"

ofxVboParticles::ofxVboParticles(int _maxParticles, float _pointSize){
    maxParticles = _maxParticles;
    pointSize = _pointSize;
    numParticles = 0;
    friction = 0.01;
    fade = 0.999;
    
    positions = boost::circular_buffer<ofVec3f>(maxParticles);
    velocitys = boost::circular_buffer<ofVec3f>(maxParticles);
    //forces = boost::circular_buffer<ofVec3f>(maxParticles);
    colors = boost::circular_buffer<ofColor>(maxParticles);
    
    positions.clear();
    velocitys.clear();
    colors.clear();
    
    static GLfloat distance[] = { 0.0, 0.0, 1.0 };
    glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distance);
    glPointSize(pointSize);
    
    ofDisableArbTex();
    texture.load("Full64.png");
    
    billboards.setUsage(GL_DYNAMIC_DRAW);
    billboards.setMode(OF_PRIMITIVE_POINTS);

    billboards.getVertices().resize(maxParticles);
    billboards.getColors().resize(maxParticles);
    billboards.getNormals().resize(maxParticles, ofVec3f(0));
}

void ofxVboParticles::update(){
        
    for(int i = 0; i < positions.size(); i++){
        //forces[i] = ofVec3f(0, 0, 0);
        //forces[i] -= velocitys[i] * friction;
        //velocitys[i] += forces[i];
        positions[i] += velocitys[i];//+ofVec3f(ofRandom(-0.4,0.4),ofRandom(-0.4,0.4),ofRandom(-0.4,0.4));
        //colors[i].setBrightness(colors[i].getBrightness()*fade);
        colors[i].a = colors[i].a - fade;// + ofRandom(0, 0.5);
        billboards.getVertices()[i].set(positions[i].x, positions[i].y, positions[i].z);
        billboards.getColors()[i].set(colors[i]);
    }
    
}

void ofxVboParticles::reset()
{
    positions.clear();
    velocitys.clear();
    colors.clear();
    billboards.clear();
    billboards.setUsage(GL_DYNAMIC_DRAW);
    billboards.setMode(OF_PRIMITIVE_POINTS);
    billboards.getVertices().resize(maxParticles);
    billboards.getColors().resize(maxParticles);
    billboards.getNormals().resize(maxParticles, ofVec3f(0));
    numParticles = 0;
}

void ofxVboParticles::draw(){
    ofPushStyle();
    glEnable(GL_BLEND);
    ofEnableDepthTest();
    ofEnablePointSprites();
    texture.getTexture().bind();
    billboards.draw();
    ofDisablePointSprites();
    glDisable(GL_BLEND);
    ofPopStyle();
}

void ofxVboParticles::addParticle(ofVec3f _position, ofVec3f _velocity, ofColor _color){
    positions.push_back(_position);
    velocitys.push_back(_velocity);
    colors.push_back(_color);
    //forces.push_back(ofVec3f(0, 0, 0));
    
    billboards.getVertices()[numParticles].set(positions[numParticles].x, positions[numParticles].y, positions[numParticles].z);
    billboards.getColors()[numParticles].set(colors[numParticles]);
    
    if(positions.size() != maxParticles) numParticles++;
    
}