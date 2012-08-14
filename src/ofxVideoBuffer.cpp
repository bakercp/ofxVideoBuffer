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
 
#include "ofxVideoBuffer.h"

/*
//--------------------------------------------------------------
ofxVideoBuffer::ofxVideoBuffer(ofxVideoBuffer& mom) {
    count     = mom.count;
    mode      = mom.mode;
    buffer    = mom.buffer;
    frameRate = mom.frameRate;
    // loader is created here on construction

    // connect to the update event listener
    ofAddListener(ofEvents().update,this,&ofxVideoBuffer::update);
 ofRemoveListener(ofEvents().update,this,&ofxVideoBuffer::update);
} */

//--------------------------------------------------------------
ofxVideoBuffer::ofxVideoBuffer() {
    readOnly = false;
    frameRate = 0.0f;
    count = 0;
    mode    = OFX_VIDEO_BUFFER_FIXED;
    buffer.resize(1, ofxSharedVideoFrame()); // resize buffer
    // connect to the update event listener
//    ofAddListener(ofEvents().update,this,&ofxVideoBuffer::update);
}

//--------------------------------------------------------------
ofxVideoBuffer::ofxVideoBuffer(int _size) {
    readOnly = false;
    frameRate = 0.0f;
    count = 0;
    mode    = OFX_VIDEO_BUFFER_FIXED;
    buffer.resize(_size, ofxSharedVideoFrame()); // resize buffer
    // connect to the update event listener
//    ofAddListener(ofEvents().update,this,&ofxVideoBuffer::update);
}

//--------------------------------------------------------------
ofxVideoBuffer::ofxVideoBuffer(int _size, ofxVideoBufferType _type) {
    readOnly = false;
    frameRate = 0.0f;
    count = 0;
    mode    = _type;
    buffer.resize(_size, ofxSharedVideoFrame()); // resize buffer
    // connect to the update event listener
    //ofAddListener(ofEvents().update,this,&ofxVideoBuffer::update);
}

//--------------------------------------------------------------
ofxVideoBuffer::~ofxVideoBuffer() {
    // disconnect from the update event listener
    //ofRemoveListener(ofEvents().update,this,&ofxVideoBuffer::update);
}    

//--------------------------------------------------------------
void ofxVideoBuffer::update(ofEventArgs & args) {
    update();
}
    
//--------------------------------------------------------------
void ofxVideoBuffer::update() {
        
    if (loader.isComplete()) {
        ofLog(OF_LOG_NOTICE,"ofxVideoBuffer::update - loader complete."); 
        // TODO, upload textures as soon as they are ready
        for(int i = 0; i < (int)buffer.size(); i++) {
            buffer[i]->setUseTexture(true);
            buffer[i]->update(); // load all pixels into opengl textures
        }
        count = getSize();//getSize() - 1;// set it to the last valid
        loader.reset();
        setReadOnly(true); // lock it.
    } 
}


//--------------------------------------------------------------
void ofxVideoBuffer::loadImage(const string& _filename) {
    loader.loadImage(&buffer,_filename);
}

//--------------------------------------------------------------
void ofxVideoBuffer::loadMovie(const string& _filename) {
    loadMovie(_filename,0,INT_MAX);
}

//--------------------------------------------------------------
void ofxVideoBuffer::loadMovie(const string& _filename, int _startFrame, int _endFrame) {
    loader.loadMovie(&buffer,_filename,_startFrame,_endFrame);
}

//--------------------------------------------------------------
bool  ofxVideoBuffer::isLoading() {
    return loader.isLoading();
}

//--------------------------------------------------------------
bool ofxVideoBuffer::bufferFrame(const ofxSharedVideoFrame& frame) {
    if(readOnly) {
        return false;
    }
    
    if(loader.isLoading()) { // no buffering while loading
        count = 0;
        return false;
    }
    
    if(isPassthroughBuffer()) {
        buffer[0] = frame;//setFromPixels(pixels); // the 0th frame is always the passthrough frame
        return true;
    } else if(isFixedBuffer()) {
        if(count < getSize()) {
            buffer[count] = frame;//->setFromPixels(pixels);
            count++;
            return true;
        } else {
            return false; // count
        }
    } else if(isCircularBuffer()) {
        if(count < getSize()) {
            // still adding like a fixed buffer
            buffer[count] = frame;//->setFromPixels(pixels);
            count++;
            return true;
        } else {
            // add a frame to the end
            //ofxSharedVideoFrame newFrame;
            //newFrame->setFromPixels(pixels);
            buffer.push_back(frame);
            
            // TODO: memory management here?  
            // TODO: ring buffer more efficient here?
            // erase the frame from the beginning
            buffer.erase(buffer.begin()); // calls the destructor as on shared ptr
            return true;
        }
    } else {
        // uh no idea
    }
}

//--------------------------------------------------------------
ofxSharedVideoFrame ofxVideoBuffer::operator [](int i) const {
    return at(i);
}

//--------------------------------------------------------------
ofxSharedVideoFrame ofxVideoBuffer::at(int i) const {
    if(isPassthroughBuffer() || isEmpty()) return buffer[0]; // TODO?

    // super modulo
    int r = getCount(); // keep index in range
    if(r != 0) {
        r = i % getCount();
        r = (r < 0) ? (r + getCount()) : (r);
    }
    
    return buffer[r]; // wrap both + and -
}

//--------------------------------------------------------------
float ofxVideoBuffer::getPercentFull() const {
    return float(getCount()) / getSize();
}

//--------------------------------------------------------------
int ofxVideoBuffer::getCount() const {
    return count;
}

//--------------------------------------------------------------
void ofxVideoBuffer::clear() {
    count  = 0;
}

//--------------------------------------------------------------
bool ofxVideoBuffer::isEmpty() const {
    return count == 0;
}

//--------------------------------------------------------------
int ofxVideoBuffer::getSize() const {
    return buffer.size();
}

//--------------------------------------------------------------
bool ofxVideoBuffer::setSize(int _size) {
    // TODO: memory -- are destructors called?
    if(_size > 0 && _size != getSize()) {
        if(_size < getSize()) count = MIN(count,_size); // move head back
        buffer.resize(_size, ofxSharedVideoFrame()); // resize buffer
        return true;
    } else {
        // no change
        return false;
    }
}

//--------------------------------------------------------------
void ofxVideoBuffer::setBufferType(ofxVideoBufferType _mode) {
    mode = _mode;
    // TODO : anything else here?
}

//--------------------------------------------------------------
ofxVideoBufferType ofxVideoBuffer::getBufferType() const {
    return mode;
}

//--------------------------------------------------------------
bool ofxVideoBuffer::isFixedBuffer() const {
    return mode == OFX_VIDEO_BUFFER_FIXED;
}

//--------------------------------------------------------------
bool ofxVideoBuffer::isPassthroughBuffer() const {
    return mode == OFX_VIDEO_BUFFER_PASSTHROUGH;
}

//--------------------------------------------------------------
bool ofxVideoBuffer::isCircularBuffer() const {
    return mode == OFX_VIDEO_BUFFER_CIRCULAR;
}

//--------------------------------------------------------------
float ofxVideoBuffer::getFrameRate() {
    if(frameRate == 0.0f) {
        return 30.0f;
    } else {
        return frameRate;
    }
}

//--------------------------------------------------------------
void  ofxVideoBuffer::setFrameRate(float _frameRate) {
    frameRate = _frameRate;
}

//--------------------------------------------------------------
bool ofxVideoBuffer::isReadOnly() {
    return readOnly;
}

//--------------------------------------------------------------
bool ofxVideoBuffer::isWritable() {
    return !readOnly;
}

//--------------------------------------------------------------
void ofxVideoBuffer::setReadOnly(bool _readOnly) {
    readOnly = _readOnly;
}

//--------------------------------------------------------------
string ofxVideoBuffer::toString() {
    stringstream ss;
    ss << "ofxVideoBuffer:" << endl;
    if(isFixedBuffer()) {
        ss << "\tTYPE=FIXED" << endl;
    } else if(isPassthroughBuffer()) {
        ss << "\tTYPE=PASSTHROUGH" << endl;
    } else if(isCircularBuffer()) {
        ss << "\tTYPE=CIRCULAR" << endl;
    }
    ss << "\tFrameRate="<< getFrameRate() << endl;
    ss << "\t%Full="<< getPercentFull() << endl;
    ss << "\tSize="<< getSize() << endl;
    ss << "\tCount="<< getCount() << endl;
    ss << "\tFrameRate="<< getFrameRate() << endl;
    ss << "\tIsLoading="<< isLoading() << endl;
    ss << "\tIsReadOnly="<< isReadOnly() << endl;
    
    return ss.str();
}

