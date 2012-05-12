#include "ofxVideoBufferPlayer.h"
#include "ofUtils.h"


//--------------------------------------------------------------
ofxVideoBufferPlayer::ofxVideoBufferPlayer() {
    
    buffer = NULL;
    
    lastFrame    = 0;
    currentFrame = 0;
    
    speed = 1;
    fps = 30.0f;

    lastFramePushTime = -1;
    
    lastFrameFraction = 0.0f;

    clearLoopPoints();
    
    loopType = OF_LOOP_NORMAL;
    
}

//--------------------------------------------------------------
ofxVideoBufferPlayer::~ofxVideoBufferPlayer() {}


//--------------------------------------------------------------
void ofxVideoBufferPlayer::loadBuffer(ofxVideoBuffer* _buffer) {
    buffer = _buffer;
    currentFrame = 0;
    lastFramePushTime = -1;
    lastFrameFraction = 0.0f;
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::unloadBuffer() {
    buffer = NULL;
}

//--------------------------------------------------------------
ofxVideoBuffer* ofxVideoBufferPlayer::getVideoBuffer() {
    return buffer;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isBufferLoaded() {
    return buffer != NULL;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isBufferEmpty() {
    return isBufferLoaded() && buffer->isEmpty();
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::update() {
    if(!isBufferLoaded()) return;
    
    if(isBufferEmpty()) return;
    
    float now = ofGetElapsedTimef();

    // if this is the first time
    if(lastFramePushTime == -1) {
        lastFramePushTime = now;
    }
    
    float elapsedTime = (now - lastFramePushTime); // elapsed time, ms
    
    float numFramesToPush = 0; // can be negative
    
    if(getSpeed() != 0) {
        float frameDuration = getFrameDuration();  // 
        numFramesToPush     = elapsedTime / frameDuration; // can be negative
        numFramesToPush    += lastFrameFraction;           // add on any leftovers from last frame
        
        float fNumFramesToPush = floor(numFramesToPush);
        
        lastFrameFraction   =  numFramesToPush - fNumFramesToPush;
        numFramesToPush     = fNumFramesToPush;
        
    } else {
        lastFrameFraction = 0; // 
    }

    ///////////////////////////////////////////
    
    if(numFramesToPush != 0) {
        
        lastFramePushTime = now;
        
        if(loopType == OF_LOOP_NONE) {
            currentFrame += numFramesToPush; // may not be w/in range 
            currentFrame = CLAMP(currentFrame, 0, buffer->getCount() - 1);
        } else {
            
            // get loop points
            int loopStart = getLoopPointsStart();
            int loopEnd   = getLoopPointsEnd();

            // check to see if they are set.  when -1, they are not set, so assum maximums
            if(loopStart < 0) loopStart = 0;
            if(loopEnd < 0)   loopEnd   = buffer->getCount() - 1;
            
            int minFrame      = MIN(loopStart,loopEnd);  // already accounts for loop enabled
            int maxFrame      = MAX(loopStart,loopEnd); // already accounts for loop enabled
            
            int nFramesInLoop = maxFrame - minFrame;

            if(nFramesInLoop <= 0) {
                // there's nothing to do here
                // except stay in the same place
                return;
            }
            
            if(loopType == OF_LOOP_NORMAL) {
                currentFrame += numFramesToPush; 
                // may not be w/in range, could be negative
                // check range and loop appropraitely
                
                if(currentFrame > maxFrame) {
                    currentFrame = minFrame + (currentFrame - maxFrame) % (nFramesInLoop);
                } else if(currentFrame < minFrame) {
                    currentFrame = maxFrame - (minFrame - currentFrame) % (nFramesInLoop);
                }
                
            } else if(loopType == OF_LOOP_PALINDROME) {
                // directionality is controlled not by speed vector but by palindrome state
                int absNumFramesToPush = ABS(numFramesToPush);
                
                currentFrame += (playDirection) ? absNumFramesToPush : -absNumFramesToPush;
                
                // we are outside of the range!
                if(currentFrame < minFrame || currentFrame > maxFrame) {
                    int overshoot = 0;
                    
                    if(currentFrame > maxFrame) {
                        overshoot = (currentFrame - maxFrame);
                    } else {
                        overshoot = (minFrame - currentFrame);
                    }

                    // recursively reduce overshoot by folding

                    while (overshoot > nFramesInLoop) {
                        overshoot -= nFramesInLoop;
                        playDirection = !playDirection;
                    }
                    
                    if(playDirection) {
                        currentFrame = (maxFrame - overshoot);
                    } else {
                        currentFrame = (minFrame + overshoot); 
                    }

                    playDirection = !playDirection;
                    
                } else {
                    // just stick with it!
                }
            }
        
        }
    }
}

//--------------------------------------------------------------
ofxVideoFrame& ofxVideoBufferPlayer::getFrame() {
    return (*buffer)[currentFrame];
}
            
//--------------------------------------------------------------
void ofxVideoBufferPlayer::draw(int x, int y) {
    if(isBufferLoaded()) {
        getFrame().draw(x,y);
    }
}

//--------------------------------------------------------------
// frame based loop control
void ofxVideoBufferPlayer::setLoopPoints(int start, int end) {
    setLoopPointsStart(start);
    setLoopPointsEnd(end);
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::setLoopPointsStart(int frame) {
    loopPointsStart = frame;
    return true;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::setLoopPointsEnd(int frame) {
    loopPointsEnd = frame;
    return true;
}

//--------------------------------------------------------------
int ofxVideoBufferPlayer::getLoopPointsStart() {
    return loopPointsStart;
}

//--------------------------------------------------------------
int ofxVideoBufferPlayer::getLoopPointsEnd() {
    return loopPointsEnd;
}

//--------------------------------------------------------------
// frame based positioning
void ofxVideoBufferPlayer::setPosition(int frame) {
    if(isBufferLoaded()) {
        // super modulo
        int r = buffer->getCount(); // keep index in range
        if(r != 0) {
            r = frame % buffer->getCount();
            r = (r < 0) ? (r + buffer->getCount()) : (r);
        }
        currentFrame = r;
    }
}

//--------------------------------------------------------------
int ofxVideoBufferPlayer::getPosition() {
    if(isBufferLoaded()) {
        return currentFrame;
    } else {
        return 0;
    }
}

//--------------------------------------------------------------
// clear loops!
void ofxVideoBufferPlayer::clearLoopPoints() {
    loopPointsStart = -1;
    loopPointsEnd   = -1;
}

//--------------------------------------------------------------
// loop modes!
void ofxVideoBufferPlayer::setLoopType(ofLoopType _loopType) {
    if(_loopType == OF_LOOP_NONE) {
    } else if(_loopType == OF_LOOP_NORMAL) {
    } else if(_loopType == OF_LOOP_PALINDROME) {
        playDirection = (getSpeed() > 0); // reset
    } else {}
    
    loopType = _loopType;
}

//--------------------------------------------------------------
// loop modes!
ofLoopType ofxVideoBufferPlayer::getLoopType() {
    return loopType;
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::setSpeed(float _speed) {
    speed = _speed;
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getSpeed() {
    // TODO: this speed setting could pull from a more complex speed mapping graph
    return speed;
}

//--------------------------------------------------------------
void  ofxVideoBufferPlayer::setFPS(float _fps) {
    fps = _fps;
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getFPS() {
    return fps;
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getFrameDuration() {
    return 1.0f / (getFPS() * getSpeed());
}

//--------------------------------------------------------------
string ofxVideoBufferPlayer::toString() {
    string stats = "";
    stats+= ("Count, Size   [" + ofToString(getVideoBuffer()->getCount())) + ",";
    stats+= ofToString(getVideoBuffer()->getSize()) + "]\n";
    stats+=  "Loop Points   [" + ofToString(getLoopPointsStart()) + "," + ofToString(getLoopPointsEnd()) + "]\n";
    
    stats+= "Buffer Type   [";
    if(isBufferLoaded()) {
        ofVideoBufferType type = getVideoBuffer()->getBufferType();
        if(type == OFX_VIDEO_BUFFER_NORMAL) {
            stats+="FIXED";
        } else if(type == OFX_VIDEO_BUFFER_CIRCULAR) {
            stats+="CIRCULAR";
        } else if(type == OFX_VIDEO_BUFFER_PASSTHROUGH) {
            stats+="PASSTHROUGH";
        }
    } else {
        stats += "--";
    }
    stats+= "]\n";
    
    string lts = "";
    ofLoopType lt = getLoopType();
    if(lt == OF_LOOP_NONE) {
        lts = "NONE";
    } else if(lt == OF_LOOP_NORMAL) {
        lts = "NORMAL";
    } else if(lt == OF_LOOP_PALINDROME) {
        lts = "PALINDROME";
    }

    stats+= "Loop Mode     [" + lts + "]\n";
    stats+= "Current Frame [" + ofToString(getPosition()) + "]\n";
    stats+= "Speed         [" + ofToString(getSpeed()) + "]";
    return stats;
}


