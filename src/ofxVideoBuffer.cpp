 
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
ofxVideoBuffer::ofxVideoBuffer(int _size) {
    frameRate = 0.0f;
    count = 0;
    mode    = OFX_VIDEO_BUFFER_FIXED;
    buffer.resize(_size, ofxVideoFrame()); // resize buffer
    // connect to the update event listener
    ofAddListener(ofEvents().update,this,&ofxVideoBuffer::update);
}

//--------------------------------------------------------------
ofxVideoBuffer::~ofxVideoBuffer() {
    // disconnect from the update event listener
    ofRemoveListener(ofEvents().update,this,&ofxVideoBuffer::update);
}    

//--------------------------------------------------------------
void ofxVideoBuffer::update(ofEventArgs & args) {
    if (loader.isComplete()) {
        // TODO, upload textures as soon as they are ready
        for(int i = 0; i < (int)buffer.size(); i++) {
            buffer[i].setUseTexture(true);
            buffer[i].update(); // load all pixels into opengl textures
        }
        count = getSize();//getSize() - 1;// set it to the last valid
        loader.reset();
    } 
}

//--------------------------------------------------------------
void ofxVideoBuffer::loadImage(string _filename) {
    loader.loadImage(&buffer,_filename);
}

//--------------------------------------------------------------
void ofxVideoBuffer::loadMovie(string _filename) {
    loadMovie(_filename,0,INT_MAX);
}

//--------------------------------------------------------------
void ofxVideoBuffer::loadMovie(string _filename, int _startFrame, int _endFrame) {
    loader.loadMovie(&buffer,_filename,_startFrame,_endFrame);
}

//--------------------------------------------------------------
bool  ofxVideoBuffer::isLoading() {
    return loader.isLoading();
}

//--------------------------------------------------------------
bool ofxVideoBuffer::bufferFrame(const ofPixels& pixels) {
    if(loader.isLoading()) { // no buffering while loading
        count = 0;
        return false;
    }
    
    if(isPassthroughBuffer()) {
        buffer[0].setFromPixels(pixels); // the 0th frame is always the passthrough frame
        return true;
    } else if(isFixedBuffer()) {
        if(count < getSize()) {
            buffer[count].setFromPixels(pixels);
            count++;
            return true;
        } else {
            return false; // count
        }
    } else if(isCircularBuffer()) {
        if(count < getSize()) {
            // still adding like a fixed buffer
            buffer[count].setFromPixels(pixels);
            count++;
            return true;
        } else {
            // add a frame to the end
            ofxVideoFrame newFrame;
            newFrame.setFromPixels(pixels);
            buffer.push_back(newFrame);
            
            // TODO: memory management here?  
            // TODO: ring buffer more efficient here?
            // erase the frame from the beginning
            buffer.erase(buffer.begin()); // calls the destructor as long as the item is not a pointer
            return true;
        }
    } else {
        // uh no idea
    }
}

//--------------------------------------------------------------
ofxVideoFrame& ofxVideoBuffer::operator [](int i) {
    return at(i);
}

//--------------------------------------------------------------
ofxVideoFrame& ofxVideoBuffer::at(int i) {
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
bool ofxVideoBuffer::isEmpty() {
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
        buffer.resize(_size, ofxVideoFrame()); // resize buffer
        return true;
    } else {
        // no change
        return false;
    }
}

//--------------------------------------------------------------
void ofxVideoBuffer::setBufferType(ofVideoBufferType _mode) {
    mode = _mode;
    // TODO : anything else here?
}

//--------------------------------------------------------------
ofVideoBufferType ofxVideoBuffer::getBufferType() const {
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


