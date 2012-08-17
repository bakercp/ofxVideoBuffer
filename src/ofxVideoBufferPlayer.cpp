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

#include "ofxVideoBufferPlayer.h"
#include "ofUtils.h"

bool fequals(float a, float b) {
    return std::fabs(a - b) < std::numeric_limits<float>::epsilon();
}

//--------------------------------------------------------------
ofxVideoBufferPlayer::ofxVideoBufferPlayer() {
    reset();
}


//--------------------------------------------------------------
ofxVideoBufferPlayer::~ofxVideoBufferPlayer() {
    // the player never owns the buffer or the player
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::reset() {
    
    emptyFrame = ofxSharedVideoFrame(new ofImage());
    emptyFrame->allocate(1,1,OF_IMAGE_COLOR);
    
    sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_NONE;
    
    speed           = 1.0f;
    playDirection   = true;
    
    // frame indicies, times, etc
    bIsPlaying   = false;
    bIsPaused    = false;
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
bool ofxVideoBufferPlayer::isFrameNew() {
    return bIsFrameNew;
}
//--------------------------------------------------------------
void ofxVideoBufferPlayer::close() {
    emptyFrame.reset();
    buffer.reset();
    player.reset();
    image.reset();
    
    reset();
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
            return player->getPixels();
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
            return player->getPixelsRef();
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
            cout << "video player!" << endl;
            player->draw(x,y);
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
        {
        //    cout << "video buffer! "<< buffer->getSize() << "/" << buffer->getCount() << "| currentFrame=" << currentFrame << endl;
            
            
           // cout << "current frame = " << currentFrame << " isTexLoaded->" << buffer->at(currentFrame)->isUsingTexture()<< endl;
//            cout << buffer->toString() << endl;
//            float w = buffer->at(currentFrame)->getWidth();
//            float h = buffer->at(currentFrame)->getHeight();
//            cout << "              [" << w << "/" << h << "]"   << endl;
//            cout << "              [" << buffer->at(currentFrame)->getColor(w/2,h/2) << "]" << endl;
            
      //      cout << "NULL?=" << (buffer->at(currentFrame) == NULL) << endl;
            
            buffer->at(currentFrame)->draw(x,y);
        }
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
            player->draw(x,y,w,h);
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
            return player->getHeight();
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
            return player->getWidth();
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
void ofxVideoBufferPlayer::loadImage(const string& filename) {
    close();
    image = ofxSharedVideoFrame(new ofImage());
    if(image->loadImage(filename)) {
        sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE;
    } else {
        close();
        ofLog(OF_LOG_ERROR, "ofxVideoBufferPlayer::loadImage - failure to load");
    }
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::loadMovie(const string& filename) {
    close();
    player = ofxSharedVideoPlayer(new ofxVideoPlayer());
    if(player->loadMovie(filename)) {
        sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER;
        player->play();
        player->setSpeed(0);
    } else {
        close();
        ofLog(OF_LOG_ERROR, "ofxVideoBufferPlayer::loadMovie - failure to load");
    }
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::replaceMovieWithBuffer(ofxSharedVideoBuffer _buffer) {
    if(_buffer != NULL) {
        if(isVideoPlayer()) {
            // trust that this is a copy
            if(getSize() == _buffer->getSize()) {
                // jump right into things without skipping a beat (fingers crossed)
                sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER;
                player.reset();
                buffer = _buffer;
                // keep all current settings
                
                bIsFrameNew = true;
                
                cout << "DIDIT!!" << endl;
                
                
                return;
            } else {
                cout << "HHHH" << endl;
                ofLogWarning() << "ofxVideoBufferPlayer::replaceMovieWithBuffered: incomingn buffer does not match the size of the current video, so loading as new buffer.";
            }
        } else {
            cout << "1111" << endl;
            ofLogWarning() << "ofxVideoBufferPlayer::replaceMovieWithBuffered: this is not a video player, so loading as a buffer.";
        }
    } else {
        cout << "qqqq" << endl;
        ofLogError() << "ofxVideoBufferPlayer::replaceMovieWithBuffered: buffer was NULL.";
        close();
        return;
    }
    cout << "GGGG" << endl;
    
    // loading as a normal buffer
    loadVideoBuffer(_buffer);

    cout << "QQQQ" << endl;
}

//--------------------------------------------------------------
void ofxVideoBufferPlayer::loadVideoBuffer(ofxSharedVideoBuffer _buffer) {
    close();
    if(_buffer != NULL) {
        sourceType = OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER;
        buffer = _buffer;
    } else {
        close();
        ofLog(OF_LOG_ERROR, "ofxVideoBufferPlayer::loadVideoBuffer - null buffer loaded, no change.");
    }
}

//--------------------------------------------------------------
ofxSharedVideoFrame  ofxVideoBufferPlayer::getImagePlayer() {
    return image;
}

//--------------------------------------------------------------
ofxSharedVideoPlayer ofxVideoBufferPlayer::getVideoPlayer() {
    return player;
}

//--------------------------------------------------------------
ofxSharedVideoBuffer ofxVideoBufferPlayer::getVideoBuffer() {
    return buffer;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isImagePlayer() const {
    return sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isVideoPlayer() const {
    return sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isBufferPlayer() const {
    return sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER;
}


//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isLoaded() const {
    return sourceType != OFX_VIDEO_PLAYER_SRC_TYPE_NONE;
}

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isEmpty() /*const of is not const correct */ {
    return getCount() < 1;
}

//--------------------------------------------------------------
int ofxVideoBufferPlayer::getCount() /*const of is not const correct */ {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return 1;
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            return player->getTotalNumFrames();
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
int ofxVideoBufferPlayer::getSize() /*const of is not const correct */ {
    switch (sourceType) {
        case OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE:
            return 1;
            break;
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER:
            return player->getTotalNumFrames();
        case OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER:
            return buffer->getSize();
            break;
        default:
            return 0;
            break;
    }
}      

//--------------------------------------------------------------
bool ofxVideoBufferPlayer::isFull() /*const of is not const correct */ {
    return getSize() > getCount();
}       

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getPctFull() /*const of is not const correct */ {
    if(sourceType == OFX_VIDEO_PLAYER_SRC_TYPE_NONE) {
        return 0;
    } else {
        return float(getCount()) / getSize();
    }
}


//--------------------------------------------------------------

void ofxVideoBufferPlayer::update() {
    
    
    if(!isLoaded()) {
        cout << "failed to update - not loaded" << endl;
        
        return;
    }
    
    if(isEmpty()) {
        //cout << "failed to update - is empty" << endl;
        
        return;
    }
    
    if(!isPlaying()) {
        cout << "failed to update - is not playing" << endl;
        
        return;
    }
    
    if(isPaused()) {
        cout << "failed to update - is paused" << endl;
        
        return;
    }
        
    float now = ofGetElapsedTimef();
        
    // if this is the first time
    if(lastFramePushTime < -1) {
        lastFramePushTime = now;
    }
    
    float elapsedTime = (now - lastFramePushTime); // elapsed time, ms
    
    float numFramesToPush = 0; // can be negative
    
    if(!fequals(getSpeed(), 0.0f)) {
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
    
    if( !fequals(numFramesToPush,0.0f) ) {
        
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
                int absNumFramesToPush = std::fabs(numFramesToPush);
                
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
            player->setFrame(currentFrame);
            player->update();
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
            return player->getTextureReference();
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
            player->setUseTexture(bUseTex);
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
int ofxVideoBufferPlayer::getLoopPointStart() /*const of is not const correct */ {
    return loopPointsStart;
}

//--------------------------------------------------------------
int ofxVideoBufferPlayer::getLoopPointEnd() /*const of is not const correct */ {
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
float ofxVideoBufferPlayer::getLoopPointStartNorm() /*const of is not const correct */ {
    return frameToFrameNorm(getLoopPointStart());
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getLoopPointEndNorm() /*const of is not const correct */ {
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
ofLoopType ofxVideoBufferPlayer::getLoopType() const {
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
int ofxVideoBufferPlayer::getFrame() const {
    return currentFrame;
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::getFrameNorm() /*const of is not const correct */ {
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
float ofxVideoBufferPlayer::getFrameRate() const {
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
float ofxVideoBufferPlayer::getFrameDuration() /*const of is not const correct */ {
    return 1.0f / (getFrameRate() * getSpeed());
}

//--------------------------------------------------------------
string ofxVideoBufferPlayer::toString() /*const of is not const correct */{
    string stats = "";
    stats+= ("Count, Size   [" + ofToString(getCount())) + "," + ofToString(getSize()) + "]\n";
    stats+=  "IsEmpty       [" + ofToString(isEmpty()) + "]\n";
    stats+=  "Loop Points   [" + ofToString(getLoopPointStart()) + "," + ofToString(getLoopPointEnd()) + "]\n";
    
    stats+= "Buffer Type   [";
    
    if(!isLoaded()) {
        stats+= "No Source Loaded";
    } else if(isImagePlayer()) {
        stats+= "Image Player";
    } else if(isVideoPlayer()) {
        stats+= "Video Player";
    } else if(isBufferPlayer()) {
        // cout << "IN HERE BUFFER PLAYER!! > source type == " << sourceType << endl;
        ofxVideoBufferType type = getVideoBuffer()->getBufferType();
        if(type == OFX_VIDEO_BUFFER_FIXED) {
            stats+="Buffer Player FIXED";
        } else if(type == OFX_VIDEO_BUFFER_CIRCULAR) {
            stats+="Buffer Player CIRCULAR";
        } else if(type == OFX_VIDEO_BUFFER_PASSTHROUGH) {
            stats+="Buffer Player PASSTHROUGH";
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
    stats+= "Is Playing    [" + ofToString(isPlaying()) + "]\n";
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
int ofxVideoBufferPlayer::frameNormToFrame(float n) /*const of is not const correct */{
    return bufferMod(ofMap(n, 0, 1, 0, getCount()));
}

//--------------------------------------------------------------
float ofxVideoBufferPlayer::frameToFrameNorm(int frame) /*const of is not const correct */ {
    return ofMap(bufferMod(frame), 0, getCount(), 0, 1);
}