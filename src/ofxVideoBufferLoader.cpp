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
    
    ofAddListener(ofEvents().exit,this,&ofxVideoBufferLoader::exit);
}

//--------------------------------------------------------------
ofxVideoBufferLoader::~ofxVideoBufferLoader() {
    waitForThread(true); // close it all down (politely)
    ofRemoveListener(ofEvents().exit,this,&ofxVideoBufferLoader::exit);
}

void ofxVideoBufferLoader::exit(ofEventArgs& a) {
    ofLogVerbose("ofxVideoBufferLoader") << "exit() called.  Cleaning up and exiting.";
    waitForThread(true); // close it all down (politely)
    ofRemoveListener(ofEvents().exit,this,&ofxVideoBufferLoader::exit);
}


//--------------------------------------------------------------
void ofxVideoBufferLoader::loadImageAsync(ofxVideoBufferData* _buffer, const string& _filename) {
    
    if(isLoading()) {
        ofLogError("ofxVideoBufferLoader") << "is already loading a video.  You must wait.";
        return;
    }
    
    image = ofxSharedVideoFrame(new ofxVideoFrame());
    
    if(image->loadImage(_filename)) {
        loadType = OFX_VID_BUFFER_LOAD_IMAGE;
        buffer = _buffer;
        image->setUseTexture(false); // important
        frameCount = 1;
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
void ofxVideoBufferLoader::loadMovieAsync(ofxVideoBufferData* _buffer,
                                     const string& _filename) {
    loadMovieAsync(_buffer,_filename,0,INT_MAX);
}

//--------------------------------------------------------------
void ofxVideoBufferLoader::loadMovieAsync(ofxVideoBufferData* _buffer,
                                     const string& _filename,
                                     int _startFrame, 
                                     int _endFrame) {

    if(isLoading()) {
        ofLogError("ofxVideoBufferLoader") << "is already loading a video.  You must wait.";
        return;
    }

    if(player.loadMovie(_filename)) {
        loadType = OFX_VID_BUFFER_LOAD_VIDEO;
        buffer = _buffer;
        player.setUseTexture(false); // important
        frameCount = player.getTotalNumFrames();
        // TODO : fancy modulo these start / end frames
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

////--------------------------------------------------------------
//void ofxVideoBufferLoader::cancelLoad(){
//    stopThread();
//    
//}

//--------------------------------------------------------------
float ofxVideoBufferLoader::getPercentLoaded() const {
    return float(currentFrame) / (endFrame - startFrame);
}

//--------------------------------------------------------------
bool ofxVideoBufferLoader::isIdle() const {
    return state == OFX_VID_BUFFER_IDLE;
}

//--------------------------------------------------------------
bool ofxVideoBufferLoader::isLoading() const {
    return state == OFX_VID_BUFFER_LOADING;
}

//--------------------------------------------------------------
bool ofxVideoBufferLoader::isComplete() const {
    return state == OFX_VID_BUFFER_COMPLETE;
}

//--------------------------------------------------------------
void ofxVideoBufferLoader::reset() {
    setState(OFX_VID_BUFFER_IDLE);
    frameCount   = 0;
    currentFrame = 0;
    startFrame   = 0;
    endFrame     = INT_MAX;

	buffer = NULL;
    player.close();
    image.reset();
    
    loadType = OFX_VID_BUFFER_LOAD_VIDEO;
}

//--------------------------------------------------------------
ofxVideoBufferLoaderState ofxVideoBufferLoader::getState() const {
    return state;
}

//--------------------------------------------------------------
void ofxVideoBufferLoader::threadedFunction(){
    
    setState(OFX_VID_BUFFER_LOADING);
    
    if(loadType == OFX_VID_BUFFER_LOAD_IMAGE) {
        if(lock()) {
            buffer->push_back(image);
            currentFrame++;
            unlock();
        }
    } else if(loadType == OFX_VID_BUFFER_LOAD_VIDEO) {
        
        while (currentFrame <= endFrame) {
            if( lock() ){

                ofxSharedVideoFrame frame = ofxSharedVideoFrame(new ofxVideoFrame());
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
    
    setState(OFX_VID_BUFFER_COMPLETE);
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

//--------------------------------------------------------------
int ofxVideoBufferLoader::getFrameCount() const {
    if(buffer != NULL) {
        return frameCount;
    } else {
        ofLogWarning("ofxVideoBufferLoader") << "attempted to get the target number of frames, but the buffer is NULL.";
        return 0;
    }
}


//--------------------------------------------------------------
void ofxVideoBufferLoader::setState(ofxVideoBufferLoaderState _state) {
    state = _state;
}




