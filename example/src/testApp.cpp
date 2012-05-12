#include "testApp.h"


testApp::~testApp() {
    for(int i = 0; i < bufferPlayers.size(); i++) {
        delete bufferPlayers[i];
    }
    bufferPlayers.clear();
    
    for(int i = 0; i < buffers.size(); i++) {
        delete buffers[i];
    }
    buffers.clear();
}

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(255,255,255);
    ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
    // video setup
    camWidth 		= 320;	// try to grab at this size. 
	camHeight 		= 240;
	
    // ready a grabbery source
	vidGrabber.initGrabber(camWidth,camHeight);
    
    numVideoSources = 3;
    
    // ready a ipcam source
    string uri = "http://148.61.142.228/axis-cgi/mjpg/video.cgi?resolution="+ ofToString(camWidth) + "x" + ofToString(camHeight);
    vidIpCam.setURI(uri);
    vidIpCam.connect();

    // ready a video source
	vidPlayer.loadMovie("fingers.mov");
	vidPlayer.play();
    
    currentBufferPlayer = 0;
    currentVideoSource  = 0;
    
    
    for(int i = 0; i < 4; i++) {
        ofxVideoBuffer* videoBuffer = new ofxVideoBuffer(100);
        buffers.push_back(videoBuffer);
        ofxVideoBufferPlayer* videoPlayer = new ofxVideoBufferPlayer();
        videoPlayer->loadBuffer(videoBuffer);
        bufferPlayers.push_back(videoPlayer);
    }
    
    
    isRecording = false;
	
}

//--------------------------------------------------------------
void testApp::update() {

    vidGrabber.update();
    vidIpCam.update();
    vidPlayer.update();
    
    for(int i = 0; i < bufferPlayers.size(); i++) {
        bufferPlayers[i]->update();;
    }

}

//--------------------------------------------------------------
void testApp::draw(){

    ofEnableAlphaBlending();

    ofBackground(0,0,0);

	ofSetColor(255,255,255,255);
    vidGrabber.draw(20,20,160,120);
    ofDrawBitmapString("LIVE GRABBER", 22,32);
    if(currentVideoSource == 0) {
        
        ofSetColor(isRecording ? 0 : 255,255,0);
        ofNoFill();
        ofRect(20,20,160,120);
    }
    
	ofSetColor(255,255,255,255);
    vidIpCam.draw(20,140,160,120);
    ofDrawBitmapString("LIVE WEB", 22,152);
    if(currentVideoSource == 1) {
        ofSetColor(isRecording ? 0 : 255,255,0);
        ofNoFill();
        ofRect(20,140,160,120);
    }

    
	ofSetColor(255,255,255,255);
    vidPlayer.draw(20,260,160,120);
    ofDrawBitmapString("VID", 22,272);
    if(currentVideoSource == 2) {
        ofSetColor(isRecording ? 0 : 255,255,0);
        ofNoFill();
        ofRect(20,260,160,120);
    }
    
    string keys = "";
    keys += "Keys:\n";
    keys += "Select the Source (ABOVE LEFT)\n";
    keys += "  [1] Next Video Source\n";
    keys += "  [s] Video Grabber Settings\n";
    keys += "\n";
    keys += "Select the Buffer Player (RIGHT)\n";
    keys += "  [`] Next Buffer Player\n";
    keys += "\n";
    keys += "All settings below apply to the\n"; 
    keys += "current player and buffer.\n";
    keys += "  [SPACE] Toggle Recording\n";
    keys += "  [x] Clear Buffer\n";
    keys += "  [?] Load Test Video Into Buffer\n";
    keys += "  [p] Next Buffer Mode\n";
    keys += "  [q] Next Loop Mode\n";
    keys += "\n";
    keys += "  [[] Loop Point (start) (DOWN)\n";
    keys += "  [{] Loop Point (end)   (DOWN)\n";
    keys += "  []] Loop Point (start) (UP)\n";
    keys += "  [}] Loop Point (end)    (UP)\n";
    keys += "  [c] Clear Loop Points\n";
    keys += "\n";
    keys += "  [-] Decrease Speed\n";
    keys += "  [=] Increase Speed\n";
    
    
    ofSetColor(255);
    ofDrawBitmapString(keys,20,400);
    
    if(isRecording) {
        if(currentVideoSource == 0 && vidGrabber.isFrameNew()) {
            bufferPlayers[currentBufferPlayer]->getVideoBuffer()->bufferFrame(vidGrabber.getPixelsRef());
        } else if(currentVideoSource == 1 ) {//&& vidIpCam.isFrameNew()) { // TODO
            bufferPlayers[currentBufferPlayer]->getVideoBuffer()->bufferFrame(vidIpCam.getPixelsRef());
        } else if(currentVideoSource == 2 && vidPlayer.isFrameNew() ) {
            bufferPlayers[currentBufferPlayer]->getVideoBuffer()->bufferFrame(vidPlayer.getPixelsRef());
        }
    }
    
    int x = 320;
    int y = 20;
    
    for(int i = 0; i < bufferPlayers.size(); i++) {

        ofSetColor(255);
        ofFill();
        bufferPlayers[i]->draw(x,y);
        
        if(currentBufferPlayer == i) {
            ofSetColor(255,255,0);
        } else {
            ofSetColor(255,127);
        }

        ofNoFill();
        ofRect(x,y,camWidth,camHeight);
        
        if(!bufferPlayers[i]->getVideoBuffer()->isLoadingMovie()) {
            float p = bufferPlayers[i]->getVideoBuffer()->getPercentFull();
            ofFill();
            ofSetColor(255,255,0,127);
            ofRect(x,y+camHeight-5,camWidth*p,5);
        }

        ofSetColor(255);
        // draw some stats
        string stats = bufferPlayers[i]->toString();
        ofDrawBitmapString(stats, x + 20, y + 20);
        
        
        x += (camWidth + 5);
        if((x + 320)> ofGetWidth()) {
            y+= camHeight + 5;
            x = 320;
        }
    }

    
    
    
    ofDisableAlphaBlending();

}



//--------------------------------------------------------------
void testApp::keyPressed  (int key){
   
    int l0 = bufferPlayers[currentBufferPlayer]->getLoopPointsStart();
    int l1 = bufferPlayers[currentBufferPlayer]->getLoopPointsEnd();
    ofLoopType lt = bufferPlayers[currentBufferPlayer]->getLoopType(); 
    ofVideoBufferType vbt = bufferPlayers[currentBufferPlayer]->getVideoBuffer()->getBufferType();
    
    float speed = bufferPlayers[currentBufferPlayer]->getSpeed();

    
    switch (key) {
        case '`':
            currentBufferPlayer = (currentBufferPlayer + 1) % bufferPlayers.size();
            break;
        case '1':
            currentVideoSource = (currentVideoSource + 1) % 3;
            break;
        case 's':
            vidGrabber.videoSettings();
            break;
        case 'p':
            if(vbt == OFX_VIDEO_BUFFER_NORMAL) vbt = OFX_VIDEO_BUFFER_CIRCULAR; 
            else if(vbt == OFX_VIDEO_BUFFER_CIRCULAR) vbt = OFX_VIDEO_BUFFER_PASSTHROUGH; 
            else if(vbt == OFX_VIDEO_BUFFER_PASSTHROUGH) vbt = OFX_VIDEO_BUFFER_NORMAL; 
            bufferPlayers[currentBufferPlayer]->getVideoBuffer()->setBufferType(vbt);
            break;
        case 'q':
            // cycle through the loop mode
            if(lt == OF_LOOP_NONE) lt = OF_LOOP_NORMAL;
            else if(lt == OF_LOOP_NORMAL) lt = OF_LOOP_PALINDROME;
            else if(lt == OF_LOOP_PALINDROME) lt = OF_LOOP_NONE;
            bufferPlayers[currentBufferPlayer]->setLoopType(lt); 
            break;
        case '[':
            bufferPlayers[currentBufferPlayer]->setLoopPointsStart(l0-1);
            break;
        case '{':
            bufferPlayers[currentBufferPlayer]->setLoopPointsEnd(l1-1);
            break;
        case ']':
            bufferPlayers[currentBufferPlayer]->setLoopPointsStart(l0+1);
            break;
        case '}':
            bufferPlayers[currentBufferPlayer]->setLoopPointsEnd(l1+1);
            break;
        case 'c':
            bufferPlayers[currentBufferPlayer]->clearLoopPoints();
            break;
        case ' ':
            isRecording = !isRecording;
            break;
        case 'x':
            bufferPlayers[currentBufferPlayer]->getVideoBuffer()->clear();
            break;
        case '0':
            bufferPlayers[currentBufferPlayer]->getVideoBuffer()->loadMovie("fingers.mov");
            break;
        case '-':
            speed -= .25;
            bufferPlayers[currentBufferPlayer]->setSpeed(speed);
            break;
        case '=':
            speed += .25;
            bufferPlayers[currentBufferPlayer]->setSpeed(speed);
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
    bufferPlayers[currentBufferPlayer]->setSpeed(speed);
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
