#include "ofxVideoBufferPlayer.h"
#include "ofUtils.h"


//--------------------------------------------------------------
ofxVideoBufferPlayer::ofxVideoBufferPlayer() {
    emptyFrame = ofxVideoFrame(new ofImage());
    emptyFrame->allocate(1,1,OF_IMAGE_COLOR);
    
    image      = ofxVideoFrame(new ofImage());
    image->allocate(1,1,OF_IMAGE_COLOR);
    
    
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
ofxVideoBufferPlayer::~ofxVideoBufferPlayer() {
    // the player never owns the buffer or the player
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isFrameNew() {
    return bIsFrameNew;
}
//--------------------------------------------------------------
void ofxVideoBufferPlayer::close() {
    buffer = NULL;
    player.close();
    image->clear();
    sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_NONE;
}

//--------------------------------------------------------------
unsigned char * ofxVideoBufferPlayer::getPixels() {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            ofLog(OF_LOG_WARNING,"ofxVideoBufferPlayer::getPixels() - no source.");
            return emptyFrame->getPixels();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image->getPixels();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            if(isFrameNew()) setFrame(currentFrame);
            return player.getPixels();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->at(currentFrame)->getPixels();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
ofPixelsRef ofxVideoBufferPlayer::getPixelsRef() {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            return emptyFrame->getPixelsRef();
            ofLog(OF_LOG_WARNING,"ofxVideoBufferPlayer::getPixelsRef() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image->getPixelsRef();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            if(isFrameNew()) setFrame(currentFrame);
            return player.getPixelsRef();
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->at(currentFrame)->getPixelsRef();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::draw(float x,float y) {
    if(isEmpty()) {
        emptyFrame->draw(x,y);
        return;
    }
    
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            emptyFrame->draw(x,y);
            ofLog(OF_LOG_WARNING,"ofxVideoBufferPlayer::draw() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            image->draw(x,y);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            if(isFrameNew()) setFrame(currentFrame);
            player.draw(x,y);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            buffer->at(currentFrame)->draw(x,y);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void  ofxVideoBufferPlayer::draw(float x,float y,float w, float h) {
    if(isEmpty()) {
        emptyFrame->draw(x,y,w,y);
        return;
    }
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            emptyFrame->draw(x,y,w,h);
            ofLog(OF_LOG_WARNING,"ofxVideoBufferPlayer::draw() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            image->draw(x,y,w,h);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            if(isFrameNew()) setFrame(currentFrame);
            player.draw(x,y,w,h);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            buffer->at(currentFrame)->draw(x,y,w,h);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getHeight() {
    if(isEmpty()) {
        return emptyFrame->getHeight();
    }

    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            return emptyFrame->getHeight();
            ofLog(OF_LOG_WARNING,"ofxVideoBufferPlayer::getHeight() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image->getHeight();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            if(isFrameNew()) setFrame(currentFrame);
            return player.getHeight();
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->at(currentFrame)->getHeight();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getWidth() {
    if(isEmpty()) {
        return emptyFrame->getWidth();
    }

    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            return emptyFrame->getWidth();
            ofLog(OF_LOG_WARNING,"ofxVideoBufferPlayer::getWidth() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image->getWidth();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            if(isFrameNew()) setFrame(currentFrame);
            return player.getWidth();
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->at(currentFrame)->getWidth();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void  ofxVideoBufferPlayer::setAnchorPercent(float xPct, float yPct) {
    // this will not work because the video buffer is sometimes shared with others
}

//--------------------------------------------------------------
void  ofxVideoBufferPlayer::setAnchorPoint(float x, float y) {
    // this will not work because the video buffer is sometimes shared with others
}

//--------------------------------------------------------------
void  ofxVideoBufferPlayer::resetAnchor() {
    // this will not work because the video buffer is sometimes shared with others
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::loadImage(string filename) {
    if(image->loadImage(filename)) {
        sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE;
        player.play();
        player.setSpeed(0);
        lastFrame = -1;
        currentFrame = bufferMod(currentFrame);
        lastFramePushTime = -1;
        lastFrameFraction = 0.0f;
        lastFrame = -1;
        clearLoopPoints();    
    } else {
        close();
        ofLog(OF_LOG_ERROR, "ofxVideoBufferPlayer::loadImage - failure to load");
    }
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::loadMovie(string filename) {
    if(player.loadMovie(filename)) {
        sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER;
        player.play();
        player.setSpeed(0);
        lastFrame = -1;
        currentFrame = bufferMod(currentFrame);
        lastFramePushTime = -1;
        lastFrameFraction = 0.0f;
        lastFrame = -1;
        clearLoopPoints();    
    } else {
        close();
        ofLog(OF_LOG_ERROR, "ofxVideoBufferPlayer::loadMovie - failure to load");
    }
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::loadVideoBuffer(ofxVideoBuffer* _buffer) {
    if(_buffer != NULL) {
        sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER;
        player.close();
        buffer = _buffer;
        lastFrame = -1;
        currentFrame = bufferMod(currentFrame);
        lastFramePushTime = -1;
        lastFrameFraction = 0.0f;
        clearLoopPoints();
    } else {
        close();
        ofLog(OF_LOG_ERROR, "ofxVideoBufferPlayer::loadVideoBuffer - null buffer loaded, no change.");
    }
}

//--------------------------------------------------------------
ofxVideoFrame&  ofxVideoBufferPlayer::getImagePlayer() {
    return image;
}

//--------------------------------------------------------------
ofxVideoPlayer&  ofxVideoBufferPlayer::getVideoPlayer() {
    return player;
}

//--------------------------------------------------------------
ofxVideoBuffer* ofxVideoBufferPlayer::getVideoBuffer() {
    return buffer;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isImagePlayer() {
    return sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isVideoPlayer() {
    return sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isBufferPlayer() {
    return sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER;
}


//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isLoaded() {
    return sourceType != OFX_VIDEO_PLAYER_SRC_TYPE_NONE;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isEmpty() {
    return getCount() < 1;
}

//--------------------------------------------------------------
int ofxVideoBufferPlayer::getCount() {
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
int ofxVideoBufferPlayer::getSize() {
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
bool ofxVideoBufferPlayer::isFull() {
    return getSize() > getCount();
}       

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getPctFull() {
    if(sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_NONE) {
        return 0;
    } else {
        return float(getCount()) / getSize();
    }
}


//--------------------------------------------------------------

void ofxVideoBufferPlayer::update() {
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

    lastFramePushTime = now;
    
    if(numFramesToPush != 0 && isPlaying() && !isPaused()) {
        
        
        if(loopType == OF_LOOP_NONE) {
            currentFrame += numFramesToPush; // may not be w/in range 
            currentFrame = CLAMP(currentFrame, 0, getCount() - 1);
        } else {
            
            // get loop points
            int loopStart = getLoopPointStart();
            int loopEnd   = getLoopPointEnd();
            
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
ofTexture& ofxVideoBufferPlayer::getTextureReference() {
    if(isEmpty()) {
        return emptyFrame->getTextureReference();
    }

    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            ofLog(OF_LOG_WARNING,"ofxVideoBufferPlayer::getTextureReference() - no source.");
            return emptyFrame->getTextureReference();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image->getTextureReference();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            return player.getTextureReference();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->at(currentFrame)->getTextureReference();
            break;
        default:
            break;
    }    
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::setUseTexture(bool bUseTex) {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            emptyFrame->setUseTexture(bUseTex);
            ofLog(OF_LOG_WARNING,"ofxVideoBufferPlayer::getPixels() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return image->setUseTexture(bUseTex);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            player.setUseTexture(bUseTex);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            buffer->at(currentFrame)->setUseTexture(bUseTex);
            break;
        default:
            break;
    }    
}

//--------------------------------------------------------------
// frame based loop control
void ofxVideoBufferPlayer::setLoopPoints(int start, int end) {
    setLoopPointStart(start);
    setLoopPointEnd(end);
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::setLoopPointStart(int frame) {
    loopPointsStart = frame;
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::setLoopPointEnd(int frame) {
    loopPointsEnd = frame;
}

//--------------------------------------------------------------
int ofxVideoBufferPlayer::getLoopPointStart() {
    return loopPointsStart;
}

//--------------------------------------------------------------
int ofxVideoBufferPlayer::getLoopPointEnd() {
    return loopPointsEnd;
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::setLoopPointsNorm(float start, float end) {
    setLoopPointStartNorm(start);
    setLoopPointEndNorm(end);
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::setLoopPointStartNorm(float frame) {
    setLoopPointStart(frameNormToFrame(frame));
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::setLoopPointEndNorm(float frame) {
    setLoopPointEnd(frameNormToFrame(frame));
}


//--------------------------------------------------------------
float ofxVideoBufferPlayer::getLoopPointStartNorm() {
    return frameToFrameNorm(getLoopPointStart());
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getLoopPointEndNorm() {
    return frameToFrameNorm(getLoopPointEnd());
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
ofLoopType ofxVideoBufferPlayer::getLoopType() {
    return loopType;
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::setFrame(int frame) {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            ofLog(OF_LOG_WARNING,"ofxVideoBufferPlayer::getPixels() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            currentFrame = 0; // always 0
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            currentFrame = bufferMod(frame);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            currentFrame = bufferMod(frame);
            break;
        default:
            break;
    }    
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::setFrameNorm(float n) {
    setFrame(frameNormToFrame(n));
}

//--------------------------------------------------------------
int ofxVideoBufferPlayer::getFrame() {
    return currentFrame;
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getFrameNorm() {
    return frameToFrameNorm(currentFrame);
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::start() {
    bIsPlaying = true;
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::stop() {
    bIsPlaying = false;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isPlaying() {
    return bIsPlaying;
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::setPaused(bool _bIsPaused) {
    bIsPaused = _bIsPaused;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isPaused() {
    return bIsPaused;
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::setSpeed(float _speed) {
    speed = _speed;
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getSpeed() {
    return speed;
}


//--------------------------------------------------------------
float ofxVideoBufferPlayer::getFrameRate() {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_NONE:
            return 0.0f;
            ofLog(OF_LOG_WARNING,"ofxVideoBufferPlayer::getPixels() - no source.");
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return 0.0f;
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            return 30.0f;//player.getFrameRate();
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->getFrameRate();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getFrameDuration() {
    return 1.0f / (getFrameRate() * getSpeed());
}

//--------------------------------------------------------------
string ofxVideoBufferPlayer::toString() {
    string stats = "";
    stats+= ("Count, Size   [" + ofToString(getCount())) + ",";
    stats+= ofToString(getSize()) + "]\n";
    stats+=  "Loop Points   [" + ofToString(getLoopPointStart()) + "," + ofToString(getLoopPointEnd()) + "]\n";
    
    stats+= "Buffer Type   [";
    
    if(!isLoaded()) {
        stats+= "No Source Loaded\n";
    } else if(isImagePlayer()) {
        stats+= "Image Player";
    } else if(isVideoPlayer()) {
        stats+= "Video Player";
    } else if(isBufferPlayer()) {
        // cout << "IN HERE BUFFER PLAYER!! > source type == " << sourceType << endl;
        ofxVideoBufferType type = getVideoBuffer()->getBufferType();
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
int ofxVideoBufferPlayer::bufferMod(int frame) {
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

//--------------------------------------------------------------
int ofxVideoBufferPlayer::frameNormToFrame(float n) {
    return bufferMod(ofMap(n, 0, 1, 0, getCount()));
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::frameToFrameNorm(int frame) {
    return ofMap(bufferMod(frame), 0, getCount(), 0, 1);
}