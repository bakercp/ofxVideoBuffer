/*==============================================================================
 
 Copyright (c) 2010, 2011, 2012 Christopher Baker <http://christopherbaker.net>
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 ==============================================================================*/

#include "ofxVideoBufferLoader.h"

//--------------------------------------------------------------
ofxVideoBufferLoader::ofxVideoBufferLoader() {
    reset();
}

//--------------------------------------------------------------
ofxVideoBufferLoader::~ofxVideoBufferLoader() {}

//--------------------------------------------------------------
void ofxVideoBufferLoader::loadImage(ofxVideoBufferData* _buffer, string _filename) {
    
    image = ofPtr<ofImage>(new ofImage());
    
    if(image->loadImage(_filename)) {
        loadType = OFX_VID_BUFFER_LOAD_IMAGE;
        buffer = _buffer;
        image->setUseTexture(false); // important
        startFrame = 0;
        endFrame   = 0;
        currentFrame = 0;
        buffer->clear(); // erase all of the items
        startThread(true,false);
    } else {
        reset();
        ofLog(OF_LOG_ERROR, "ofxVideoBufferLoader::loadImage() - image load error.");
    }    
}

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
    
    if(player.loadMovie(_filename)) {
        loadType = OFX_VID_BUFFER_LOAD_VIDEO;
        buffer = _buffer;
        player.setUseTexture(false); // important
        int frameCount = player.getTotalNumFrames();
        startFrame     = ofClamp(_startFrame,        0, frameCount - 1);
        endFrame       = ofClamp(_endFrame, _startFrame, frameCount - 1);
        currentFrame   = startFrame;
        buffer->clear(); // erase all of the items
        startThread(true,false);
    } else {
        reset();
        ofLog(OF_LOG_ERROR, "ofxVideoBufferLoader::loadMovie() - image load error.");
    }
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
    
    if(loadType == OFX_VID_BUFFER_LOAD_IMAGE) {
        if(lock()) {
            buffer->push_back(image);
            currentFrame++;
            unlock();
        }
    } else if(loadType == OFX_VID_BUFFER_LOAD_VIDEO) {
        while (currentFrame <= endFrame) {
            if( lock() ){
                ofxVideoFrame frame = ofPtr<ofImage>(new ofImage());
                frame->setUseTexture(false);
                // TODO: there are strange sitatuations with set frame.
                // setFrame(0) is equal to setFrame(1) in most cases.
                // is this normal?
                
                player.setFrame(currentFrame);
                frame->setFromPixels(player.getPixelsRef());
                buffer->push_back(frame);
                currentFrame++;
                unlock();
            }
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




