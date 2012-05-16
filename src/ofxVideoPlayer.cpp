#include "ofxVideoPlayer.h"
#include "ofUtils.h"


//--------------------------------------------------------------
ofxVideoPlayer::ofxVideoPlayer() {
    emptyFrame.allocate(1,1,OF_IMAGE_COLOR);
    
    buffer = NULL;
    
    sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_NONE;
    
    speed           = 1.0f;
    playDirection   = 1.0f;
    
    // frame indicies, times, etc
    bIsFrameNew  = true;
    lastFrame    = -1;
    currentFrame = 0;
    
    lastFramePushTime = -1;
    lastFrameFraction = 0.0f;
    
	loopPointsStart = -1;
	loopPointsEnd   = -1;

    loopType = OF_LOOP_NORMAL;
}

//--------------------------------------------------------------
ofxVideoPlayer::~ofxVideoPlayer() {
    // the player never owns the buffer or the player
}

//--------------------------------------------------------------
bool ofxVideoPlayer::isFrameNew() {
    return bIsFrameNew;
}
//--------------------------------------------------------------
void ofxVideoPlayer::close() {
    buffer = NULL;
    player.close();
    image.clear();
    sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_NONE;
}

//--------------------------------------------------------------
unsigned char * ofxVideoPlayer::getPixels() {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            return emptyFrame.getPixels();
            ofLog(OF_LOG_WARNING,"ofxVideoPlayer::getPixels() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image.getPixels();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            if(isFrameNew()) setFrame(currentFrame);
            return player.getPixels();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->at(currentFrame).getPixels();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
ofPixelsRef ofxVideoPlayer::getPixelsRef() {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            return emptyFrame.getPixelsRef();
            ofLog(OF_LOG_WARNING,"ofxVideoPlayer::getPixelsRef() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image.getPixelsRef();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            if(isFrameNew()) setFrame(currentFrame);
            return player.getPixelsRef();
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->at(currentFrame).getPixelsRef();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofxVideoPlayer::draw(float x,float y) {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            return emptyFrame.draw(x,y);
            ofLog(OF_LOG_WARNING,"ofxVideoPlayer::draw() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image.draw(x,y);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            if(isFrameNew()) setFrame(currentFrame);
            return player.draw(x,y);
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->at(currentFrame).draw(x,y);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void  ofxVideoPlayer::draw(float x,float y,float w, float h) {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            return emptyFrame.draw(x,y,w,h);
            ofLog(OF_LOG_WARNING,"ofxVideoPlayer::draw() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image.draw(x,y,w,h);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            if(isFrameNew()) setFrame(currentFrame);
            return player.draw(x,y,w,h);
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->at(currentFrame).draw(x,y,w,h);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
float ofxVideoPlayer::getHeight() {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            return emptyFrame.getHeight();
            ofLog(OF_LOG_WARNING,"ofxVideoPlayer::getHeight() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image.getHeight();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            if(isFrameNew()) setFrame(currentFrame);
            return player.getHeight();
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->at(currentFrame).getHeight();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
float ofxVideoPlayer::getWidth() {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            return emptyFrame.getWidth();
            ofLog(OF_LOG_WARNING,"ofxVideoPlayer::getWidth() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image.getWidth();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            if(isFrameNew()) setFrame(currentFrame);
            return player.getWidth();
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->at(currentFrame).getWidth();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void  ofxVideoPlayer::setAnchorPercent(float xPct, float yPct) {
    // this will not work because the video buffer is sometimes shared with others
}

//--------------------------------------------------------------
void  ofxVideoPlayer::setAnchorPoint(float x, float y) {
    // this will not work because the video buffer is sometimes shared with others
}

//--------------------------------------------------------------
void  ofxVideoPlayer::resetAnchor() {
    // this will not work because the video buffer is sometimes shared with others
}

//--------------------------------------------------------------
void ofxVideoPlayer::loadImage(string filename) {
    if(image.loadImage(filename)) {
        sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE;
        player.play();
        player.setSpeed(0);
        lastFrame = -1;
        currentFrame = mod(currentFrame);
        lastFramePushTime = -1;
        lastFrameFraction = 0.0f;
        lastFrame = -1;
        clearLoopPoints();    
    } else {
        close();
        ofLog(OF_LOG_ERROR, "ofxVideoPlayer::loadImage - failure to load");
    }
}

//--------------------------------------------------------------
void ofxVideoPlayer::loadMovie(string filename) {
    if(player.loadMovie(filename)) {
        sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER;
        player.play();
        player.setSpeed(0);
        lastFrame = -1;
        currentFrame = mod(currentFrame);
        lastFramePushTime = -1;
        lastFrameFraction = 0.0f;
        lastFrame = -1;
        clearLoopPoints();    
    } else {
        close();
        ofLog(OF_LOG_ERROR, "ofxVideoPlayer::loadMovie - failure to load");
    }
}

//--------------------------------------------------------------
void ofxVideoPlayer::loadVideoBuffer(ofxVideoBuffer* _buffer) {
    if(_buffer != NULL) {
        sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER;
        player.close();
        buffer = _buffer;
        lastFrame = -1;
        currentFrame = mod(currentFrame);
        lastFramePushTime = -1;
        lastFrameFraction = 0.0f;
        clearLoopPoints();
    } else {
        close();
        ofLog(OF_LOG_ERROR, "ofxVideoPlayer::loadVideoBuffer - null buffer loaded, no change.");
    }
}

//--------------------------------------------------------------
ofImage&  ofxVideoPlayer::getImagePlayer() {
    return image;
}

//--------------------------------------------------------------
ofVideoPlayer&  ofxVideoPlayer::getVideoPlayer() {
    return player;
}

//--------------------------------------------------------------
ofxVideoBuffer* ofxVideoPlayer::getVideoBuffer() {
    return buffer;
}

//--------------------------------------------------------------
bool ofxVideoPlayer::isImagePlayer() {
    return sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE;
}

//--------------------------------------------------------------
bool ofxVideoPlayer::isVideoPlayer() {
    return sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER;
}

//--------------------------------------------------------------
bool ofxVideoPlayer::isBufferPlayer() {
    return sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER;
}


//--------------------------------------------------------------
bool ofxVideoPlayer::isLoaded() {
    return sourceType != OFX_VIDEO_PLAYER_SRC_TYPE_NONE;
}

//--------------------------------------------------------------
bool ofxVideoPlayer::isEmpty() {
    return getCount() < 1;
}

//--------------------------------------------------------------
int ofxVideoPlayer::getCount() {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return 1;
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            return player.getTotalNumFrames();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->getCount();
            break;
        default:
            return 0;
            break;
    }
}

//--------------------------------------------------------------
int ofxVideoPlayer::getSize() {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return 1;
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            return player.getTotalNumFrames();
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->getSize();
            break;
        default:
            return 0;
            break;
    }
}      

//--------------------------------------------------------------
bool ofxVideoPlayer::isFull() {
    return (sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_NONE) || (getCount() != getSize());
}       

//--------------------------------------------------------------
float ofxVideoPlayer::getPctFull() {
    if(sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_NONE) {
        return 0;
    } else {
        return float(getCount()) / getSize();
    }
}


//--------------------------------------------------------------

void ofxVideoPlayer::update() {
    if(!isLoaded()) return;
    if(isEmpty()) return;
    
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
            currentFrame = CLAMP(currentFrame, 0, getCount() - 1);
        } else {
            
            // get loop points
            int loopStart = getLoopPointsStart();
            int loopEnd   = getLoopPointsEnd();

            // check to see if they are set.  when -1, they are not set, so assum maximums
            if(loopStart < 0) loopStart = 0;
            if(loopEnd < 0)   loopEnd   = getCount() - 1;
            
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
    
    bIsFrameNew = (lastFrame != currentFrame);
    lastFrame = currentFrame;
    
    if(bIsFrameNew) {
        if(sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER) {
            //cout << "updating player." << endl;
            player.setFrame(currentFrame);
            player.update();
        } else if(sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER) {
            // the video buffer updates itself
        }
    }
    
    
}

//--------------------------------------------------------------
ofTexture& ofxVideoPlayer::getTextureReference() {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            emptyFrame.getTextureReference();
            ofLog(OF_LOG_WARNING,"ofxVideoPlayer::getPixels() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image.getTextureReference();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            player.getTextureReference();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            buffer->at(currentFrame).getTextureReference();
            break;
        default:
            break;
    }    
}

//--------------------------------------------------------------
void ofxVideoPlayer::setUseTexture(bool bUseTex) {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            emptyFrame.setUseTexture(bUseTex);
            ofLog(OF_LOG_WARNING,"ofxVideoPlayer::getPixels() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image.setUseTexture(bUseTex);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            player.setUseTexture(bUseTex);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            buffer->at(currentFrame).setUseTexture(bUseTex);
            break;
        default:
            break;
    }    
}

//--------------------------------------------------------------
// frame based loop control
void ofxVideoPlayer::setLoopPoints(int start, int end) {
    setLoopPointsStart(start);
    setLoopPointsEnd(end);
}

//--------------------------------------------------------------
bool ofxVideoPlayer::setLoopPointsStart(int frame) {
    loopPointsStart = frame;
    return true;
}

//--------------------------------------------------------------
bool ofxVideoPlayer::setLoopPointsEnd(int frame) {
    loopPointsEnd = frame;
    return true;
}

//--------------------------------------------------------------
int ofxVideoPlayer::getLoopPointsStart() {
    return loopPointsStart;
}

//--------------------------------------------------------------
int ofxVideoPlayer::getLoopPointsEnd() {
    return loopPointsEnd;
}


//--------------------------------------------------------------
// clear loops!
void ofxVideoPlayer::clearLoopPoints() {
    loopPointsStart = -1;
    loopPointsEnd   = -1;
}

//--------------------------------------------------------------
// loop modes!
void ofxVideoPlayer::setLoopType(ofLoopType _loopType) {
    if(_loopType == OF_LOOP_NONE) {
    } else if(_loopType == OF_LOOP_NORMAL) {
    } else if(_loopType == OF_LOOP_PALINDROME) {
        playDirection = (getSpeed() > 0); // reset
    } else {}
    
    loopType = _loopType;
}

//--------------------------------------------------------------
ofLoopType ofxVideoPlayer::getLoopType() {
    return loopType;
}

//--------------------------------------------------------------
void ofxVideoPlayer::setFrame(int frame) {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            ofLog(OF_LOG_WARNING,"ofxVideoPlayer::getPixels() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            currentFrame = 0; // always 0
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            currentFrame = mod(frame);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            currentFrame = mod(frame);
            break;
        default:
            break;
    }    
}

//--------------------------------------------------------------
int ofxVideoPlayer::getFrame() {
    return currentFrame;
}

//--------------------------------------------------------------
void ofxVideoPlayer::setSpeed(float _speed) {
    speed = _speed;
}

//--------------------------------------------------------------
float ofxVideoPlayer::getSpeed() {
    return speed;
}


//--------------------------------------------------------------
float ofxVideoPlayer::getFrameRate() {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            return 0.0f;
            ofLog(OF_LOG_WARNING,"ofxVideoPlayer::getPixels() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return 0.0f;
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            return player.getFrameRate();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->getFrameRate();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
float ofxVideoPlayer::getFrameDuration() {
    return 1.0f / (getFrameRate() * getSpeed());
}

//--------------------------------------------------------------
string ofxVideoPlayer::toString() {
    string stats = "";
    stats+= ("Count, Size   [" + ofToString(getCount())) + ",";
    stats+= ofToString(getSize()) + "]\n";
    stats+=  "Loop Points   [" + ofToString(getLoopPointsStart()) + "," + ofToString(getLoopPointsEnd()) + "]\n";
    
    stats+= "Buffer Type   [";

    if(!isLoaded()) {
        stats+= "No Source Loaded\n";
    } else if(isImagePlayer()) {
        stats+= "Image Player";
    } else if(isVideoPlayer()) {
        stats+= "Video Player";
    } else if(isBufferPlayer()) {
       // cout << "IN HERE BUFFER PLAYER!! > source type == " << sourceType << endl;
        ofVideoBufferType type = getVideoBuffer()->getBufferType();
        if(type == OFX_VIDEO_BUFFER_FIXED) {
            stats+="FIXED";
        } else if(type == OFX_VIDEO_BUFFER_CIRCULAR) {
            stats+="CIRCULAR";
        } else if(type == OFX_VIDEO_BUFFER_PASSTHROUGH) {
            stats+="PASSTHROUGH";
        }
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
    stats+= "Current Frame [" + ofToString(getFrame()) + "]\n";
    stats+= "Speed         [" + ofToString(getSpeed()) + "]";
    return stats;
}

//--------------------------------------------------------------
int ofxVideoPlayer::mod(int frame) {
    int cnt = getCount();
    int r   = cnt;
    if(r != 0) {
        r = frame % cnt;
        if(r < 0) {
            r += cnt; 
        }
    }
    return r;
}

