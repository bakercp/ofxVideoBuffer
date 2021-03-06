#include "testApp.h"


testApp::~testApp() {
    cout << "CLEANING UP" << endl;
    vidPlayer.close();
    vidGrabber.close();
    vidIpCam.close();

    
    bufferPlayers.clear();
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
    
    
    for(int i = 0; i < 6; i++) {
        ofxSharedVideoBuffer videoBuffer(new ofxVideoBuffer(100));
        buffers.push_back(videoBuffer);
        ofxSharedVideoBufferPlayer videoPlayer(new ofxVideoBufferPlayer());
        videoPlayer->loadVideoBuffer(videoBuffer);
        bufferPlayers.push_back(videoPlayer);
        
        videoPlayer->start();
    }
    
    
    isRecording = false;
	
}

//--------------------------------------------------------------
void testApp::update() {

    vidGrabber.update();
    vidIpCam.update();
    vidPlayer.update();
    
    for(size_t i = 0; i < buffers.size(); i++) {
        buffers[i]->update();
    }
    
    for(size_t i = 0; i < bufferPlayers.size(); i++) {
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
    keys += "  [}] Loop Point (end)   (UP)\n";
    keys += "  [c] Clear Loop Points\n";
    keys += "\n";
    keys += "  [-] Decrease Speed\n";
    keys += "  [=] Increase Speed\n";
    keys += "\n";
    keys += "  [_] Decrease Position\n";
    keys += "  [+] Increase Position\n";

    
    ofSetColor(255);
    ofDrawBitmapString(keys,20,400);
    
    if(isRecording) {
        
        
        if(currentVideoSource == 0 && vidGrabber.isFrameNew()) {
            ofxSharedVideoFrame frame(new ofImage());
            frame->setFromPixels(vidGrabber.getPixelsRef());
            bufferPlayers[currentBufferPlayer]->getVideoBuffer()->bufferFrame(frame);
        } else if(currentVideoSource == 1 ) {//&& vidIpCam.isFrameNew()) { // TODO
            ofxSharedVideoFrame frame(new ofImage());
            frame->setFromPixels(vidIpCam.getPixelsRef());
            bufferPlayers[currentBufferPlayer]->getVideoBuffer()->bufferFrame(frame);
        } else if(currentVideoSource == 2 && vidPlayer.isFrameNew() ) {
            ofxSharedVideoFrame frame(new ofImage());
            frame->setFromPixels(vidPlayer.getPixelsRef());
            bufferPlayers[currentBufferPlayer]->getVideoBuffer()->bufferFrame(frame);
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
        
        if(!bufferPlayers[i]->getVideoBuffer()->isLoading()) {
            float p = bufferPlayers[i]->getVideoBuffer()->getPercentFull();
            ofFill();
            ofSetColor(255,255,0,127);
            ofRect(x,y+camHeight-5,camWidth*p,5);
        }

        {
            ofPushStyle();
            float ff = bufferPlayers[i]->getFrame() / (float)bufferPlayers[i]->getSize();
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofSetColor(0,255,0);
            ofRect(x + camWidth*ff,y+camHeight-2.5,3,10);
            ofPopStyle();
        }
        
        {
            ofPushStyle();
            float ff = bufferPlayers[i]->getLoopPointStart() / (float)bufferPlayers[i]->getSize();
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofSetColor(0,0,255);
            ofRect(x + camWidth*ff,y+camHeight-2.5,3,10);
            ofPopStyle();
        }
        
        {
            ofPushStyle();
            float ff = bufferPlayers[i]->getLoopPointEnd() / (float)bufferPlayers[i]->getSize();
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofSetColor(0,0,255);
            ofRect(x + camWidth*ff,y+camHeight-2.5,3,10);
            ofPopStyle();
        }

        
        ofSetColor(255);
        // draw some stats
        string stats = bufferPlayers[i]->toString();
        ofDrawBitmapString(stats, x + 20, y + 20);
        
        
        x += (camWidth + 0);
        if((x + 320)> ofGetWidth()) {
            y+= camHeight + 0;
            x = 320;
        }
    }

    
    
    
    ofDisableAlphaBlending();

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
   
    int l0 = bufferPlayers[currentBufferPlayer]->getLoopPointStart();
    int l1 = bufferPlayers[currentBufferPlayer]->getLoopPointEnd();
    ofLoopType lt = bufferPlayers[currentBufferPlayer]->getLoopType(); 
    ofxVideoBufferType vbt = bufferPlayers[currentBufferPlayer]->getVideoBuffer()->getBufferType();
    
    float speed = bufferPlayers[currentBufferPlayer]->getSpeed();

    int position = bufferPlayers[currentBufferPlayer]->getFrame();
    
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
            if(vbt == OFX_VIDEO_BUFFER_FIXED) vbt = OFX_VIDEO_BUFFER_CIRCULAR; 
            else if(vbt == OFX_VIDEO_BUFFER_CIRCULAR) vbt = OFX_VIDEO_BUFFER_PASSTHROUGH; 
            else if(vbt == OFX_VIDEO_BUFFER_PASSTHROUGH) vbt = OFX_VIDEO_BUFFER_FIXED; 
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
            bufferPlayers[currentBufferPlayer]->setLoopPointStart(l0-1);
            break;
        case '{':
            bufferPlayers[currentBufferPlayer]->setLoopPointEnd(l1-1);
            break;
        case ']':
            bufferPlayers[currentBufferPlayer]->setLoopPointStart(l0+1);
            break;
        case '}':
            bufferPlayers[currentBufferPlayer]->setLoopPointEnd(l1+1);
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
        case '?':
            bufferPlayers[currentBufferPlayer]->getVideoBuffer()->loadMovieAsync("fingers.mov");
            break;
        case '-':
            speed -= .05;
            bufferPlayers[currentBufferPlayer]->setSpeed(speed);
            break;
        case '=':
            speed += .05;
            bufferPlayers[currentBufferPlayer]->setSpeed(speed);
            break;
        case '_':
            bufferPlayers[currentBufferPlayer]->setFrame(position - 1);
            break;
        case '+':
            bufferPlayers[currentBufferPlayer]->setFrame(position + 1);
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
