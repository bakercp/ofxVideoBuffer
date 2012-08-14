#include "testApp.h"


testApp::~testApp() {
    players.clear();
    buffers.clear();
}

//--------------------------------------------------------------
void testApp::setup(){
	
    // modifiy this
    rows = 50;
    cols = 50;
    useBuffer = true;
    
    //////
    ofBackground(255,255,255);
    ofSetVerticalSync(true);
	ofSetFrameRate(60);

    
    ofxSharedVideoBuffer buffer(new ofxVideoBuffer());
    buffer->loadMovie("fingers.mov");
    buffers.push_back(buffer);
    
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadMovie("bball.mov");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadMovie("countdown.mov");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadMovie("dishes.mov");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadMovie("crashtest.mov");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadMovie("countdown15.mov");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadMovie("dozer.mov");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadMovie("dvkite.mov");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadMovie("dvducks.mov");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadMovie("garbage.mov");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadMovie("oh.mov");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadMovie("ozone.mov");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadMovie("rain.mov");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadImage("IMG_0004.JPG");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadImage("IMG_0003.JPG");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadImage("IMG_0005.JPG");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadImage("IMG_0006.JPG");
    buffers.push_back(buffer);
    buffer = ofxSharedVideoBuffer(new ofxVideoBuffer());
    buffer->loadImage("IMG_0007.JPG");
    buffers.push_back(buffer);
    
    for(int i = 0; i < rows * cols; i++) {
        ofxSharedVideoBufferPlayer player(new ofxVideoBufferPlayer());
        if(useBuffer) {
            player->loadVideoBuffer(buffers[int(ofRandom(buffers.size()))]);
            player->start();
        } else {
            player->loadMovie("fingers_pjpeg.mov");
            player->start();
        }
        player->setLoopType(OF_LOOP_PALINDROME);
        players.push_back(player);
    }
    currentPlayer = 0;
}

//--------------------------------------------------------------
void testApp::update() {
    for(int i = 0; i < buffers.size(); i++) {
        buffers[i]->update();
    }
    
    for(int i = 0; i < rows * cols; i++) {
        players[i]->update();
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0,0,0);

    ofEnableAlphaBlending();
    
    float w = float(ofGetWidth()) / cols;
    float h = float(ofGetHeight()) / rows;
    float r = 0;
    float c = 0;
    
    float mx = ofGetMouseX();
    float my = ofGetMouseY();
    
    float tx,ty,xx,yy,ss,rr;
    
    for(int i = 0; i < rows * cols; i++) {
        ofSetColor(255);
        
        float x = c * w;
        float y = r * h;
        
        float dist = ofDist(x,y,mx,my);
        float scaleScaler = ofMap(dist,0,ofGetWidth(),5,1,true);
        float speedScaler = ofMap(dist,0,ofGetWidth()/4,-2,2,true);
        float rotScaler = ofMap(dist,0,ofGetWidth()/4,360,0,true);
        ofPushMatrix();
        ofTranslate(x + w/2,y + h/2);
        //ofScale(scaleScaler,scaleScaler);
        //ofRotateZ(rotScaler);
        //players[i]->setSpeed(speedScaler);
        players[i]->draw(-w/2,-h /2,w,h);
        
        ofSetColor(255);
        if(rows < 4) {
            ofDrawBitmapString(players[i]->toString(), -w/2 + 10,-h /2 + 20);
        }
        if(i == currentPlayer) {
            xx = -w/2;
            yy = -h/2;
            tx = x + w/2;
            ty = y + h/2;
            ss = speedScaler;
            rr = rotScaler;
        }

        c++;
        if(c >= cols) {
            r++;
            c = 0;
        }
        
        ofPopMatrix();
    }
    
    ofPushMatrix();
    ofTranslate(tx,ty);
    //ofScale(ss,ss);
    //ofRotateZ(rr);
    ofNoFill();
    ofSetColor(255,255,0);
    ofRect(xx,yy,w,h);
    
    ofPopMatrix();

    
    ofDisableAlphaBlending();
}



//--------------------------------------------------------------
void testApp::keyPressed  (int key){
   
    int l0 = players[currentPlayer]->getLoopPointStart();
    int l1 = players[currentPlayer]->getLoopPointEnd();
    ofLoopType lt = players[currentPlayer]->getLoopType(); 
    ofxVideoBufferType vbt = OFX_VIDEO_BUFFER_FIXED;
    if(players[currentPlayer]->isBufferPlayer()) {
       vbt = players[currentPlayer]->getVideoBuffer()->getBufferType();
    }
    
    float speed = players[currentPlayer]->getSpeed();
    int frame = players[currentPlayer]->getFrame();
    int cnt = players[currentPlayer]->getCount();
    
    switch (key) {
        case 'F':
            ofToggleFullscreen();
            break;
        case ' ':
            for(int i = 0; i < rows * cols; i++) {
               players[i]->setFrame(int(ofRandom(players[i]->getCount())));
            }
            break;
        case '`':
            currentPlayer = (currentPlayer + 1) % players.size();
            break;
        case 'q':
            // cycle through the loop mode
            if(lt == OF_LOOP_NONE) lt = OF_LOOP_NORMAL;
            else if(lt == OF_LOOP_NORMAL) lt = OF_LOOP_PALINDROME;
            else if(lt == OF_LOOP_PALINDROME) lt = OF_LOOP_NONE;
            players[currentPlayer]->setLoopType(lt); 
            break;
        case '[':
            players[currentPlayer]->setLoopPointStart(l0-1);
            break;
        case '{':
            players[currentPlayer]->setLoopPointEnd(l1-1);
            break;
        case ']':
            players[currentPlayer]->setLoopPointStart(l0+1);
            break;
        case '}':
            players[currentPlayer]->setLoopPointEnd(l1+1);
            break;
        case 'c':
            players[currentPlayer]->clearLoopPoints();
            break;
        case 'x':
            players[currentPlayer]->getVideoBuffer()->clear();
            break;
        case '-':
            speed -= .05;
            players[currentPlayer]->setSpeed(speed);
            break;
        case '=':
            speed += .05;
            players[currentPlayer]->setSpeed(speed);
            break;
        case '_':
            players[currentPlayer]->setFrame(frame - 1);
            break;
        case '+':
            players[currentPlayer]->setFrame(frame + 1);
            break;
        default:
            break;
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    float speed = ofMap(x, 0, ofGetWidth(), 0, 20);
    players[currentPlayer]->setSpeed(speed);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::resized(int w, int h){

}
