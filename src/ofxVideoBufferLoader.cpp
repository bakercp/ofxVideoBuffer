
#include "ofxVideoBufferLoader.h"

//--------------------------------------------------------------
ofxVideoBufferLoader::ofxVideoBufferLoader() {
    reset();
}

//--------------------------------------------------------------
ofxVideoBufferLoader::~ofxVideoBufferLoader() {}

//--------------------------------------------------------------
void ofxVideoBufferLoader::loadMovie(ofxVideoBufferData* _buffer, 
                                     string _filename) {
    loadMovie(_buffer,_filename,0,INT_MAX);
}

//--------------------------------------------------------------
void ofxVideoBufferLoader::loadMovie(ofxVideoBufferData* _buffer, 
                                     string _filename, 
                                     int _startFrame, 
                                     int _endFrame) {

    buffer = _buffer;

    player.setUseTexture(false); // important
    player.loadMovie(_filename);
    
    int frameCount = player.getTotalNumFrames();
    
    startFrame     = ofClamp(_startFrame,        0, frameCount - 1);
    endFrame       = ofClamp(_endFrame, _startFrame, frameCount - 1);
    currentFrame   = startFrame;

    
    buffer->clear(); // erase all of the items
    
    startThread(true,false);
}

//--------------------------------------------------------------
void ofxVideoBufferLoader::cancelLoad(){
    stopThread();
}

//--------------------------------------------------------------
float ofxVideoBufferLoader::getPercentLoaded() {
    return float(currentFrame) / (endFrame - startFrame);
}

//--------------------------------------------------------------
bool ofxVideoBufferLoader::isIdle() {
    return state == OFX_VID_BUFFER_IDLE;
}

//--------------------------------------------------------------
bool ofxVideoBufferLoader::isLoading() {
    return state == OFX_VID_BUFFER_LOADING;
}

//--------------------------------------------------------------
bool ofxVideoBufferLoader::isComplete() {
    return state == OFX_VID_BUFFER_COMPLETE;
}

//--------------------------------------------------------------
void ofxVideoBufferLoader::reset() {
    state       = OFX_VID_BUFFER_IDLE;
    startFrame   = 0;
    endFrame     = INT_MAX;
}

//--------------------------------------------------------------
ofxVideoBufferLoaderState ofxVideoBufferLoader::getState() {
    return state;
}

//--------------------------------------------------------------
void ofxVideoBufferLoader::threadedFunction(){
    state = OFX_VID_BUFFER_LOADING;
    while (currentFrame <= endFrame) {
        if( lock() ){
            ofImage frame;
            frame.setUseTexture(false);
            // TODO: there are strange sitatuations with set frame.
            // setFrame(0) is equal to setFrame(1) in most cases.
            // is this normal?

            player.setFrame(currentFrame);
            frame.setFromPixels(player.getPixelsRef());
            buffer->push_back(frame);
            currentFrame++;
            unlock();
        }
    }
    state = OFX_VID_BUFFER_COMPLETE;
}

//--------------------------------------------------------------
int ofxVideoBufferLoader::getStartFrame() const {
    return startFrame; 
}

//--------------------------------------------------------------
int ofxVideoBufferLoader::getEndFrame() const {
    return endFrame;
}

//--------------------------------------------------------------
int ofxVideoBufferLoader::getCurrentFrame() const {
    return currentFrame;
}




